/*
 * Copyright (c) 2025 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "ns3/boolean.h"
#include "ns3/bulk-send-helper.h"
#include "ns3/command-line.h"
#include "ns3/config-store-module.h"
#include "ns3/config.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/double.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/ipv4.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs-spectrum-model.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/queue-size.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/timestamp-tag.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
// #include <ns3/nlohmann_json.hpp> // if not installed system wide

#include <cmath>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/types.h>

using namespace ns3;
using json = nlohmann::json;

NS_LOG_COMPONENT_DEFINE("IRS-MultiUser");

struct NodeStats
{
    uint32_t rxPackets = 0; // at AP
    uint32_t rxBytes = 0;   // at AP
    uint32_t txPackets = 0;
    uint32_t txBytes = 0;
    uint64_t dataRate = 0;

    uint32_t phyRxDropPackets = 0;
    uint32_t macRxDropPackets = 0;

    uint32_t phyTxDropPackets = 0;
    uint32_t macTxDropPackets = 0;

    std::map<WifiPhyRxfailureReason, uint32_t> failureReasons;
    // time packet takes from STA to AP at Mac Layer in microseconds
    std::vector<uint64_t> latencies;
    std::map<uint32_t, std::tuple<uint32_t, double>> packetCountPhy; // nodeId: (count, snr)
};

std::map<uint32_t, NodeStats> nodeStatistics;

class SenderIdTag : public Tag
{
  public:
    SenderIdTag()
        : m_senderId(0xFFFFFFFF)
    {
    }

    void SetSenderId(uint32_t id)
    {
        m_senderId = id;
    }

    uint32_t GetSenderId() const
    {
        return m_senderId;
    }

    static TypeId GetTypeId()
    {
        static TypeId tid = TypeId("SenderIdTag").SetParent<Tag>().AddConstructor<SenderIdTag>();
        return tid;
    }

    TypeId GetInstanceTypeId() const override
    {
        return GetTypeId();
    }

    uint32_t GetSerializedSize() const override
    {
        return sizeof(uint32_t);
    }

    void Serialize(TagBuffer i) const override
    {
        i.WriteU32(m_senderId);
    }

    void Deserialize(TagBuffer i) override
    {
        m_senderId = i.ReadU32();
    }

    void Print(std::ostream& os) const override
    {
        os << "SenderId=" << m_senderId;
    }

  private:
    uint32_t m_senderId;
};

/**
 * Get the Node Id From Context.
 * From examples/tcp/tcp-variants-comparison.cc
 *
 * @param context The context.
 * @return the node ID.
 */
static uint32_t
GetNodeIdFromContext(std::string context)
{
    const std::size_t n1 = context.find_first_of('/', 1);
    const std::size_t n2 = context.find_first_of('/', n1 + 1);
    return std::stoul(context.substr(n1 + 1, n2 - n1 - 1));
}

void
PhyRxCallback(std::string context,
              Ptr<const Packet> packet,
              uint16_t channelFreqMhz,
              WifiTxVector txVector,
              MpduInfo aMpdu,
              SignalNoiseDbm signalNoise,
              uint16_t staId)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    uint32_t rxNodeId = GetNodeIdFromContext(context);

    SenderIdTag tag;
    if (packet->PeekPacketTag(tag))
    {
        uint32_t txNodeId = tag.GetSenderId();
        if (txNodeId != rxNodeId && txNodeId != 0)
        {
            std::get<0>(nodeStatistics[rxNodeId].packetCountPhy[txNodeId])++;
            std::get<1>(nodeStatistics[rxNodeId].packetCountPhy[txNodeId]) +=
                signalNoise.signal - signalNoise.noise;
        }
    }
}

void
MacTxCallback(std::string context, Ptr<const Packet> packet)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    uint32_t nodeId = GetNodeIdFromContext(context);

    TimestampTag timestamp;
    timestamp.SetTimestamp(Simulator::Now());
    packet->AddByteTag(timestamp);

    SenderIdTag tag;
    tag.SetSenderId(nodeId);
    packet->AddPacketTag(tag);

    nodeStatistics[nodeId].txBytes += packet->GetSize();
    nodeStatistics[nodeId].txPackets++;
}

void
MacRxCallback(std::string context, Ptr<const Packet> packet)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    TimestampTag timestamp;
    SenderIdTag sender;
    if (packet->FindFirstMatchingByteTag(timestamp) && packet->PeekPacketTag(sender))
    {
        uint32_t txNodeId = sender.GetSenderId();
        Time delay = Simulator::Now() - timestamp.GetTimestamp();

        nodeStatistics[txNodeId].latencies.push_back(delay.GetMicroSeconds());
        nodeStatistics[txNodeId].rxBytes += packet->GetSize();
        nodeStatistics[txNodeId].rxPackets++;
    }
}

void
PhyRxDropCallback(std::string context, Ptr<const Packet> packet, WifiPhyRxfailureReason reason)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    SenderIdTag tag;
    if (packet->PeekPacketTag(tag))
    {
        uint32_t txNodeId = tag.GetSenderId();
        nodeStatistics[txNodeId].phyRxDropPackets++;
        nodeStatistics[txNodeId].failureReasons[reason]++;
    }
}

void
MacRxDropCallback(std::string context, Ptr<const Packet> packet)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    SenderIdTag tag;
    if (packet->PeekPacketTag(tag))
    {
        uint32_t txNodeId = tag.GetSenderId();
        nodeStatistics[txNodeId].macRxDropPackets++;
    }
}

void
MacTxDropCallback(std::string context, Ptr<const Packet> packet)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    uint32_t nodeId = GetNodeIdFromContext(context);
    nodeStatistics[nodeId].macTxDropPackets++;
}

void
PhyTxDropCallback(std::string context, Ptr<const Packet> packet)
{
    if (Simulator::Now() < Seconds(65))
    {
        return;
    }

    uint32_t nodeId = GetNodeIdFromContext(context);
    nodeStatistics[nodeId].phyTxDropPackets++;
}

void
RateCallback(std::string context, uint64_t oldRate, uint64_t newRate)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    nodeStatistics[nodeId].dataRate = newRate;
}

json
GetScenarioStatistics(uint16_t run,
                      uint16_t numElements,
                      uint16_t numUsers,
                      uint16_t numStaElements,
                      Time time,
                      bool noirs,
                      bool rtsCts,
                      bool mesh,
                      bool staIrs,
                      NodeContainer staNodes)
{
    json nodeStats = json::object();
    double totalThroughput = 0.0;
    double totalLatency = 0.0;
    double totalJitter = 0.0;
    uint16_t countLatency = 0;
    double sumThroughputSquared = 0.0;
    double totalTxPackets = 0.0;
    double totalRxPackets = 0.0;

    for (const auto& node : nodeStatistics)
    {
        uint32_t nodeId = node.first;
        std::string nodeIdStr = std::to_string(node.first);
        if (nodeId > 0)
        {
            double nodeThroughput = (node.second.rxBytes * 8.0) / (time.GetSeconds() * 1e6);
            totalThroughput += nodeThroughput;
            sumThroughputSquared += nodeThroughput * nodeThroughput;
            totalTxPackets += node.second.txPackets;
            totalRxPackets += node.second.rxPackets;
            Vector nodePos = staNodes.Get(nodeId - 2)->GetObject<MobilityModel>()->GetPosition();

            uint64_t minLatency = 0;
            uint64_t maxLatency = 0;
            double meanLatency = 0;
            double jitter = 0;
            if (!node.second.latencies.empty())
            {
                // Compute Min, Max, and Mean Latency
                minLatency =
                    *std::min_element(node.second.latencies.begin(), node.second.latencies.end());
                maxLatency =
                    *std::max_element(node.second.latencies.begin(), node.second.latencies.end());
                meanLatency =
                    std::reduce(node.second.latencies.begin(), node.second.latencies.end(), 0LL) /
                    static_cast<double>(node.second.latencies.size());

                // Compute Jitter
                std::vector<uint64_t> jitters;
                for (size_t i = 1; i < node.second.latencies.size(); ++i)
                {
                    jitters.push_back(std::abs(static_cast<int64_t>(node.second.latencies[i]) -
                                               static_cast<int64_t>(node.second.latencies[i - 1])));
                }
                jitter = jitters.empty() ? 0.0
                                         : std::accumulate(jitters.begin(), jitters.end(), 0ULL) /
                                               static_cast<double>(jitters.size());
            }

            if (meanLatency > 0)
            {
                totalLatency += meanLatency;
                totalJitter += jitter;
                countLatency++;
            }

            nodeStats[nodeIdStr] = {
                {"Position", {{"x", nodePos.x}, {"y", nodePos.y}, {"z", nodePos.z}}},
                {"TxPackets", node.second.txPackets},
                {"TxBytes", node.second.txBytes},
                {"TxOffered", (node.second.txBytes * 8.0) / (time.GetSeconds() * 1e6)},
                {"RxPackets", node.second.rxPackets},
                {"RxBytes", node.second.rxBytes},
                {"Throughput", (node.second.rxBytes * 8.0) / (time.GetSeconds() * 1e6)},
                {"DataRate", node.second.dataRate / 1e6},
                {"PhyRxDropPackets", node.second.phyRxDropPackets},
                {"MacRxDropPackets", node.second.macRxDropPackets},
                {"PhyTxDropPackets", node.second.phyTxDropPackets},
                {"MacTxDropPackets", node.second.macTxDropPackets},
                {"MeanLatency", meanLatency},
                {"MaxLatency", maxLatency},
                {"MinLatency", minLatency},
                {"Jitter", jitter},
                {"PacketDeliveryRatio",
                 static_cast<double>(node.second.rxPackets) / node.second.txPackets},
                {"PacketLossRatio",
                 static_cast<double>(node.second.txPackets - node.second.rxPackets) /
                     node.second.txPackets}};
            for (const auto& entry : node.second.failureReasons)
            {
                std::ostringstream oss;
                oss << entry.first;
                nodeStats[nodeIdStr]["FailureReasons"][oss.str()] = entry.second;
            }
        }
        for (const auto& entry : node.second.packetCountPhy)
        {
            if (std::get<0>(entry.second) > 1)
            {
                nodeStats[nodeIdStr]["ReceivedFrom"][std::to_string(entry.first)]["Packets"] =
                    std::get<0>(entry.second);
                nodeStats[nodeIdStr]["ReceivedFrom"][std::to_string(entry.first)]["SNR"] =
                    std::get<1>(entry.second) / std::get<0>(entry.second);
            }
        }
        if (nodeStats[nodeIdStr].find("ReceivedFrom") == nodeStats[nodeIdStr].end())
        {
            nodeStats[nodeIdStr]["ReceivedFrom"] = nullptr;
        }
    }

    double fairnessIndex = (totalThroughput * totalThroughput) / (numUsers * sumThroughputSquared);

    json scenario = {{"Run", run},
                     {"Time", time.GetSeconds()},
                     {"Users", numUsers},
                     {"RtsCts", rtsCts},
                     {"Mesh", mesh},
                     {"StaIrs", staIrs},
                     {"Elements", numElements},
                     {"StaElements", numStaElements},
                     {"AvgThroughput", totalThroughput / numUsers},
                     {"AvgJitter", totalJitter / countLatency},
                     {"AvgLatency", totalLatency / countLatency},
                     {"TotalThroughput", totalThroughput},
                     {"PacketLoss", (totalTxPackets - totalRxPackets) / totalTxPackets},
                     {"FairnessIndex", (std::isnan(fairnessIndex) ? 0.0 : fairnessIndex)},
                     {"Nodes", nodeStats}};
    return scenario;
}

Eigen::VectorXcd
calcReflectionCoefficients(Ptr<IrsSpectrumModel> irs,
                           const std::optional<Angles>& in_angle,
                           const std::optional<Angles>& out_angle,
                           double shift,
                           double lambda,
                           const Eigen::MatrixX3d& slice)
{
    NS_ABORT_UNLESS(in_angle.has_value());
    NS_ABORT_UNLESS(out_angle.has_value());

    Eigen::VectorXcd stv_in =
        irs->CalcSteeringvector(in_angle.value(), lambda, slice).array().arg();
    Eigen::VectorXcd stv_out =
        irs->CalcSteeringvector(out_angle.value(), lambda, slice).array().arg();

    uint32_t segmentSize = slice.rows();
    Eigen::VectorXcd rcoeffs_segment(segmentSize);

    if (shift != 0)
    {
        rcoeffs_segment = (std::complex<double>(0, 1) *
                           (Eigen::VectorXd::Constant(segmentSize, shift) - stv_in - stv_out))
                              .array()
                              .exp();
    }
    else
    {
        rcoeffs_segment = (std::complex<double>(0, 1) * (-stv_in - stv_out)).array().exp();
    }

    return rcoeffs_segment;
}

double
calculatePhaseShift(Ptr<IrsSpectrumModel> irs,
                    Ptr<Node> source,
                    Ptr<Node> reflector,
                    Ptr<Node> destination)
{
    double directDistance = source->GetObject<MobilityModel>()->GetDistanceFrom(
        destination->GetObject<MobilityModel>());

    double reflectedDistance =
        source->GetObject<MobilityModel>()->GetDistanceFrom(reflector->GetObject<MobilityModel>()) +
        reflector->GetObject<MobilityModel>()->GetDistanceFrom(
            destination->GetObject<MobilityModel>());

    return irs->CalcPhaseShift(directDistance, reflectedDistance, 0);
}

struct Tile
{
    int startRow;
    int startCol;
    int numRows;
    int numCols;
};

std::vector<Tile>
optimizedGridPartition(int N, int numTiles)
{
    // Find p and q that minimize the aspect ratio
    int best_p = 1, best_q = numTiles;
    double best_ratio = std::numeric_limits<double>::max();

    for (int p = 1; p <= std::sqrt(numTiles); ++p)
    {
        if (numTiles % p == 0)
        {
            int q = numTiles / p;
            double ratio = std::max(static_cast<double>(N) / p, static_cast<double>(N) / q);
            if (ratio < best_ratio)
            {
                best_ratio = ratio;
                best_p = p;
                best_q = q;
            }
        }
    }

    int p = best_p;
    int q = best_q;

    std::vector<Tile> tiles;
    tiles.reserve(p * q);

    int baseTileRows = N / p;
    int baseTileCols = N / q;
    int extraRows = N % p;
    int extraCols = N % q;

    int currentRow = 0;
    for (int i = 0; i < p; i++)
    {
        int tileRows = baseTileRows + (i < extraRows ? 1 : 0);
        int currentCol = 0;
        for (int j = 0; j < q; j++)
        {
            if (tiles.size() >= static_cast<size_t>(numTiles))
            {
                break;
            }
            int tileCols = baseTileCols + (j < extraCols ? 1 : 0);
            tiles.push_back({currentRow, currentCol, tileRows, tileCols});
            currentCol += tileCols;
        }
        currentRow += tileRows;
    }
    return tiles;
}

// Process IRS Tiles & Compute Reflection Coefficients
void
processIRSTiles(Ptr<IrsSpectrumModel> irs,
                Ptr<NodeContainer> staNodes,
                Ptr<NodeContainer> apNode,
                Ptr<NodeContainer> irsNode,
                Vector pos_irs,
                Eigen::MatrixX3d& elementPos,
                double lambda,
                Eigen::VectorXcd& rcoeffs,
                uint32_t numUsers,
                std::optional<Angles> in_angle)
{
    uint32_t numTiles = numUsers + (numUsers * (numUsers - 1)) / 2;
    uint16_t N = std::sqrt(elementPos.rows()); // Assuming a square IRS

    std::vector<Tile> tiles = optimizedGridPartition(N, numTiles);

#ifdef NS3_BUILD_PROFILE_DEBUG
    NS_LOG_DEBUG("TileID | StartRow | StartCol | NumRows | NumCols");
    NS_LOG_DEBUG("----------------------------------------------");

    for (size_t i = 0; i < tiles.size(); ++i)
    {
        NS_LOG_DEBUG(i << " | " << tiles[i].startRow << " | " << tiles[i].startCol << " | "
                       << tiles[i].numRows << " | " << tiles[i].numCols);
    }

    NS_LOG_DEBUG("----------------------------------------------");
#endif

    int index = 0;

    // Process Direct Reflection for Each User
    for (size_t i = 0; i < numUsers; ++i)
    {
        Vector posSta = staNodes->Get(i)->GetObject<MobilityModel>()->GetPosition();

        const Tile& tile = tiles[i]; // Assign tile to this user
        Eigen::MatrixX3d slice(tile.numRows * tile.numCols, 3);
        size_t sliceIndex = 0;
        for (int col = 0; col < tile.numCols; ++col)
        {
            for (int row = 0; row < tile.numRows; ++row)
            {
                // Column-major global indexing
                size_t globalRow = tile.startRow + row;
                size_t globalCol = tile.startCol + col;
                size_t globalIndex = globalCol * N + globalRow;

                // Extract element position for this global index
                slice.row(sliceIndex++) = elementPos.row(globalIndex);
            }
        }

        std::optional<Angles> out_angle =
            IrsPropagationLossModel::CalcAngles3D(posSta, pos_irs, {0, 0, -1});

        double shift = calculatePhaseShift(irs, apNode->Get(0), irsNode->Get(0), staNodes->Get(i));

        Eigen::VectorXcd rcoeffs_col =
            calcReflectionCoefficients(irs, in_angle, out_angle, shift, lambda, slice);

        for (int col = 0; col < tile.numCols; ++col)
        {
            for (int row = 0; row < tile.numRows; ++row)
            {
                size_t globalCol = tile.startCol + col;
                size_t globalRow = tile.startRow + row;
                size_t rcoeffsIndex = globalCol * N + globalRow;
                rcoeffs(rcoeffsIndex) = rcoeffs_col(col * tile.numRows + row);
            }
        }
    }

    // Process Reflection Between User Pairs
    for (size_t i = 0; i < numUsers; ++i)
    {
        for (size_t j = i + 1; j < numUsers; ++j)
        {
            Vector posStaA = staNodes->Get(i)->GetObject<MobilityModel>()->GetPosition();
            Vector posStaB = staNodes->Get(j)->GetObject<MobilityModel>()->GetPosition();

            const Tile& tile = tiles[numUsers + index];
            Eigen::MatrixX3d slice(tile.numRows * tile.numCols, 3);
            size_t sliceIndex = 0;
            for (int col = 0; col < tile.numCols; ++col)
            {
                for (int row = 0; row < tile.numRows; ++row)
                {
                    // Column-major global indexing
                    size_t globalRow = tile.startRow + row;
                    size_t globalCol = tile.startCol + col;
                    size_t globalIndex = globalCol * N + globalRow;

                    slice.row(sliceIndex++) = elementPos.row(globalIndex);
                }
            }

            std::optional<Angles> in_angle =
                IrsPropagationLossModel::CalcAngles3D(posStaA, pos_irs, {0, 0, -1});
            std::optional<Angles> out_angle =
                IrsPropagationLossModel::CalcAngles3D(posStaB, pos_irs, {0, 0, -1});
            Eigen::VectorXcd rcoeffs_col =
                calcReflectionCoefficients(irs, in_angle, out_angle, 0, lambda, slice);

            // Store results
            for (int col = 0; col < tile.numCols; ++col)
            {
                for (int row = 0; row < tile.numRows; ++row)
                {
                    size_t globalCol = tile.startCol + col;
                    size_t globalRow = tile.startRow + row;
                    size_t rcoeffsIndex = globalCol * N + globalRow;
                    rcoeffs(rcoeffsIndex) = rcoeffs_col(col * tile.numRows + row);
                }
            }
            index++;
        }
    }
}

json
RunScenario(uint16_t numUsers,
            uint16_t run,
            Time time,
            bool noirs,
            uint16_t numElements,
            bool rtsCts,
            bool pcap,
            bool mesh,
            bool staIrs,
            uint16_t staElements)
{
    RngSeedManager::SetSeed(2025);
    RngSeedManager::SetRun(run);

    Ptr<ns3::UniformRandomVariable> random = ns3::CreateObject<ns3::UniformRandomVariable>();

    // Create nodes
    NodeContainer apNode;
    NodeContainer irsNode;
    NodeContainer staNodes;
    NodeContainer irsStaNodes;

    apNode.Create(1);
    irsNode.Create(1);
    staNodes.Create(numUsers);
    if (staIrs)
    {
        irsStaNodes.Create(numUsers - 1);
    }

    if (rtsCts)
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(0));
    }
    else
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(99999));
    }

    // Mobility setup
    MobilityHelper mobilitySta;
    mobilitySta.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    // Configure grid position allocator for stations
    Ptr<GridPositionAllocator> staGrid = CreateObject<GridPositionAllocator>();
    staGrid->SetMinX(0.0);
    staGrid->SetMinY(0.0);
    staGrid->SetDeltaX(10.0);
    staGrid->SetDeltaY(10.0);
    staGrid->SetN(3);
    mobilitySta.SetPositionAllocator(staGrid);
    mobilitySta.Install(staNodes);

    // IRS stations setup (only if enabled)
    if (staIrs)
    {
        MobilityHelper mobilityStaIrs;
        mobilityStaIrs.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        Ptr<GridPositionAllocator> irsGrid = CreateObject<GridPositionAllocator>();
        *irsGrid = *staGrid;
        irsGrid->SetZ(2.5);
        mobilityStaIrs.SetPositionAllocator(irsGrid);
        mobilityStaIrs.Install(irsStaNodes);
    }

    // AP and IRS node positioning
    Ptr<ListPositionAllocator> apIrsList = CreateObject<ListPositionAllocator>();
    Vector pos_ap = {10, -1, 7};
    Vector pos_irs = {10, 0, 8};
    apIrsList->Add(pos_ap);
    apIrsList->Add(pos_irs);
    MobilityHelper mobilityApIrs;
    mobilityApIrs.SetPositionAllocator(apIrsList);
    mobilityApIrs.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityApIrs.Install(apNode);
    mobilityApIrs.Install(irsNode);

    // Apply random position offsets to stations
    Ptr<UniformRandomVariable> randOffset = CreateObject<UniformRandomVariable>();
    randOffset->SetAttribute("Min", DoubleValue(-4.0));
    randOffset->SetAttribute("Max", DoubleValue(4.0));

    for (uint32_t i = 0; i < staNodes.GetN(); ++i)
    {
        double offsetX = randOffset->GetValue();
        double offsetY = randOffset->GetValue();

        // Apply offsets to station
        Ptr<MobilityModel> mobStaT = staNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mobStaT->GetPosition();
        pos.x += offsetX;
        pos.y += offsetY;
        mobStaT->SetPosition(pos);

        // Apply same offsets to IRS station if enabled
        if (staIrs && i < irsStaNodes.GetN())
        {
            Ptr<MobilityModel> mobIrsT = irsStaNodes.Get(i)->GetObject<MobilityModel>();
            Vector posIrs = mobIrsT->GetPosition();
            posIrs.x += offsetX;
            posIrs.y += offsetY;
            mobIrsT->SetPosition(posIrs);
        }
    }

    // Set up WiFi
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    WifiHelper wifi;
    WifiMacHelper wifiMac;

    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    if (noirs)
    {
        wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
                                       "Exponent",
                                       DoubleValue(5));
    }
    else
    {
        // assuming an center frequency of 5.21 GHz
        double freq = 5.21e9;
        double c = 299792458.0; // speed of light in vacuum
        double lambda = c / freq;
        double spacing = lambda * 0.5; // half wavelength spacing

        Ptr<IrsSpectrumModel> irs;
        uint16_t Nr;
        uint16_t Nc;

        Nr = Nc = sqrt(numElements);
        irs = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({0, 0, -1}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({Nr, Nc}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({spacing, spacing}),
            "Frequency",
            DoubleValue(freq));

        irs->SetElementPos(irs->CalcElementPositions());
        Eigen::MatrixX3d elementPos = irs->GetElementPos();

        std::optional<Angles> in_angle =
            IrsPropagationLossModel::CalcAngles3D(pos_ap, pos_irs, {0, 0, -1});

        Eigen::VectorXcd rcoeffs(Nr * Nc);
        // no mesh -> only devide Irs in n sections
        if (!mesh)
        {
            NS_ABORT_UNLESS(Nc >= numUsers);
            uint16_t numCols = Nc / numUsers;
            uint16_t rest = Nc % numUsers;

            std::vector<uint16_t> userCols(numUsers, numCols);
            std::vector<uint16_t> userIndices(numUsers);

            for (uint16_t i = 0; i < numUsers; i++)
            {
                userIndices[i] = i;
            }
            Shuffle(userIndices.begin(), userIndices.end(), random);

            // Distribute the extra columns randomly
            for (uint16_t i = 0; i < rest; i++)
            {
                userCols[userIndices[i]] += 1;
            }

            uint16_t colOffset = 0;
            for (int i = 0; i < numUsers; ++i)
            {
                uint16_t colsForUser = userCols[i];

                Vector posSta = staNodes.Get(i)->GetObject<MobilityModel>()->GetPosition();

                Eigen::MatrixX3d slice(Nr * colsForUser, 3);
                for (int j = 0; j < Nr * colsForUser; ++j)
                {
                    slice.row(j) = elementPos.row(colOffset * Nr + j);
                }
                std::optional<Angles> out_angle =
                    IrsPropagationLossModel::CalcAngles3D(posSta, pos_irs, {0, 0, -1});

                double shift =
                    calculatePhaseShift(irs, apNode.Get(0), irsNode.Get(0), staNodes.Get(i));

                Eigen::VectorXcd rcoeffs_col =
                    calcReflectionCoefficients(irs, in_angle, out_angle, shift, lambda, slice);

                rcoeffs.segment(colOffset * Nr, colsForUser * Nr) = rcoeffs_col;
                colOffset += colsForUser;
            }
        }
        // Mesh -> split Irs in n + n(n-1)/2 sections
        else
        {
            processIRSTiles(irs,
                            &staNodes,
                            &apNode,
                            &irsNode,
                            pos_irs,
                            elementPos,
                            lambda,
                            rcoeffs,
                            numUsers,
                            in_angle);
        }

        irs->SetRcoeffs(rcoeffs);
        irsNode.Get(0)->AggregateObject(irs);

        // Irs per station
        if (staIrs)
        {
            struct IrsConnectionDebug
            {
                uint32_t irsNodeId;
                std::vector<std::tuple<uint32_t, uint32_t, uint16_t>> connections;
                uint16_t totalElementsUsed;
                uint16_t totalConnectionsMade;
            };

            std::vector<IrsConnectionDebug> irsConnectionDetails;
            uint16_t totalConnections = (numUsers * (numUsers - 1)) / 2;
            uint16_t connectionsPerIrs =
                std::ceil(static_cast<double>(totalConnections) / irsStaNodes.GetN());
            std::vector<std::vector<bool>> connectionMatrix(numUsers,
                                                            std::vector<bool>(numUsers, false));
            irsConnectionDetails.resize(irsStaNodes.GetN());

            // For each IRS node
            for (uint32_t irsIndex = 0; irsIndex < irsStaNodes.GetN(); ++irsIndex)
            {
                // Initialize debug info for this IRS
                IrsConnectionDebug& debugInfo = irsConnectionDetails[irsIndex];
                debugInfo.irsNodeId = irsIndex;
                debugInfo.totalElementsUsed = 0;
                debugInfo.totalConnectionsMade = 0;

                Ptr<Node> currIrsNode = irsStaNodes.Get(irsIndex);

                // Configure IRS parameters
                uint16_t Nr = Nc = sqrt(staElements);
                Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
                    "Direction",
                    VectorValue({0, 0, -1}),
                    "N",
                    TupleValue<UintegerValue, UintegerValue>({Nr, Nc}),
                    "Spacing",
                    TupleValue<DoubleValue, DoubleValue>({spacing, spacing}),
                    "Samples",
                    UintegerValue(1000),
                    "Frequency",
                    DoubleValue(freq));

                Eigen::MatrixX3d elementPos = irs->CalcElementPositions();
                irs->SetElementPos(elementPos);

                // Calculate column distribution
                uint16_t columnsPerConnection = Nc / connectionsPerIrs;
                uint16_t remainingColumns = Nc % connectionsPerIrs;

                Eigen::VectorXcd rcoeffs(Nr * Nc);
                rcoeffs.setZero();

                uint32_t currentColumnOffset = 0;
                uint16_t connectionsMade = 0;

                // Attempt to create new connections
                for (uint32_t srcUser = 0; srcUser < numUsers; ++srcUser)
                {
                    for (uint32_t destUser = srcUser + 1; destUser < numUsers; ++destUser)
                    {
                        // Skip if this connection already exists
                        if (connectionMatrix[srcUser][destUser] ||
                            connectionMatrix[destUser][srcUser])
                        {
                            continue;
                        }

                        // Check if this IRS can handle another connection
                        if (connectionsMade >= connectionsPerIrs)
                        {
                            break;
                        }

                        // Positions of users and IRS
                        Vector posSrcSta =
                            staNodes.Get(srcUser)->GetObject<MobilityModel>()->GetPosition();
                        Vector posDestSta =
                            staNodes.Get(destUser)->GetObject<MobilityModel>()->GetPosition();
                        Vector posIrs = currIrsNode->GetObject<MobilityModel>()->GetPosition();

                        // Determine columns for this connection
                        uint16_t columnsForThisConnection =
                            columnsPerConnection + (remainingColumns > 0 ? 1 : 0);
                        remainingColumns = std::max(0, remainingColumns - 1);

                        // Calculate angles
                        std::optional<Angles> in_angle =
                            IrsPropagationLossModel::CalcAngles3D(posSrcSta, posIrs, {0, 0, -1});
                        std::optional<Angles> out_angle =
                            IrsPropagationLossModel::CalcAngles3D(posDestSta, posIrs, {0, 0, -1});

                        // Select slice of IRS for this connection
                        Eigen::MatrixX3d slice(Nr * columnsForThisConnection, 3);
                        for (int k = 0; k < Nr * columnsForThisConnection; ++k)
                        {
                            slice.row(k) = elementPos.row(currentColumnOffset * Nr + k);
                        }

                        // Calculate phase shift and reflection coefficients
                        double shift = calculatePhaseShift(irs,
                                                           staNodes.Get(srcUser),
                                                           currIrsNode,
                                                           staNodes.Get(destUser));

                        Eigen::VectorXcd rcoeffs_col = calcReflectionCoefficients(irs,
                                                                                  in_angle,
                                                                                  out_angle,
                                                                                  shift,
                                                                                  lambda,
                                                                                  slice);

                        // Update reflection coefficients
                        rcoeffs.segment(currentColumnOffset * Nr, columnsForThisConnection * Nr) =
                            rcoeffs_col;

                        // Mark connection as established
                        connectionMatrix[srcUser][destUser] = true;
                        connectionMatrix[destUser][srcUser] = true;

                        // Log connection details
                        debugInfo.connections.emplace_back(srcUser,
                                                           destUser,
                                                           columnsForThisConnection * Nr);
                        debugInfo.totalElementsUsed += columnsForThisConnection * Nr;

                        currentColumnOffset += columnsForThisConnection;
                        connectionsMade++;
                        debugInfo.totalConnectionsMade++;
                    }

                    // Break if all connections are made
                    if (connectionsMade >= totalConnections)
                    {
                        break;
                    }
                }

                // Set reflection coefficients for the IRS
                irs->SetRcoeffs(rcoeffs);
                currIrsNode->AggregateObject(irs);
            }

            // Debug output
            NS_LOG_DEBUG("IRS Connectivity Debug Information:");
            NS_LOG_DEBUG("Total Unique Connections Expected: " << totalConnections);

            for (const auto& irsDebug : irsConnectionDetails)
            {
                NS_LOG_DEBUG("IRS Node " << irsDebug.irsNodeId << " Details:");
                NS_LOG_DEBUG("  Total Connections Made: " << irsDebug.totalConnectionsMade);
                NS_LOG_DEBUG("  Total Elements Used: " << irsDebug.totalElementsUsed);
                NS_LOG_DEBUG("  Connection Details:");

                for (const auto& conn : irsDebug.connections)
                {
                    NS_LOG_DEBUG("    - Connection between User "
                                 << std::get<0>(conn) << " and User " << std::get<1>(conn) << ": "
                                 << std::get<2>(conn) << " elements");
                }
            }

            // Verify all connections are established
            bool allConnectionsEstablished = true;
            for (uint32_t srcUser = 0; srcUser < numUsers; ++srcUser)
            {
                for (uint32_t destUser = srcUser + 1; destUser < numUsers; ++destUser)
                {
                    if (!connectionMatrix[srcUser][destUser])
                    {
                        allConnectionsEstablished = false;
                        NS_LOG_ERROR("Missing connection between User " << srcUser << " and User "
                                                                        << destUser);
                    }
                }
            }

            NS_ABORT_MSG_UNLESS(allConnectionsEstablished,
                                "Not all user combinations are connected");
        }

        Ptr<LogDistancePropagationLossModel> lossModel =
            CreateObject<LogDistancePropagationLossModel>();
        lossModel->SetPathLossExponent(2);
        Ptr<LogDistancePropagationLossModel> losLossModel =
            CreateObject<LogDistancePropagationLossModel>();
        losLossModel->SetPathLossExponent(5);

        if (staIrs)
        {
            irsNode.Add(irsStaNodes);
        }
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irsNode),
                                       "IrsLossModel",
                                       PointerValue(lossModel),
                                       "LosLossModel",
                                       PointerValue(losLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }

    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiPhy.SetInterferenceHelper("ns3::InterferenceHelper");

    uint32_t nMpdus = 1;
    uint32_t payloadSize = 1500;
    uint32_t maxAmpduSize = nMpdus * (payloadSize + 200);

    wifi.SetStandard(WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

    Ssid ssid = Ssid("ns3-wifi");

    NetDeviceContainer apDevice;
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevice = wifi.Install(wifiPhy, wifiMac, apNode);

    NetDeviceContainer staDevice;
    wifiMac.SetType("ns3::StaWifiMac",
                    "Ssid",
                    SsidValue(ssid),
                    "ActiveProbing",
                    BooleanValue(true));
    staDevice = wifi.Install(wifiPhy, wifiMac, staNodes);

    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/BE_MaxAmpduSize",
                UintegerValue(maxAmpduSize));

    // reduce queue size from 500 (default) to 1 to reduce queuing delay
    Config::Set("/$ns3::NodeListPriv/NodeList/*/$ns3::Node/DeviceList/*/$ns3::WifiNetDevice/Mac/"
                "$ns3::StaWifiMac/VI_Txop/$ns3::QosTxop/Queue/$ns3::WifiMacQueue/MaxSize",
                QueueSizeValue(QueueSize("1p")));
    Config::Set("/$ns3::NodeListPriv/NodeList/*/$ns3::Node/DeviceList/*/$ns3::WifiNetDevice/Mac/"
                "$ns3::StaWifiMac/BE_Txop/$ns3::QosTxop/Queue/$ns3::WifiMacQueue/MaxSize",
                QueueSizeValue(QueueSize("1p")));
    Config::Set("/$ns3::NodeListPriv/NodeList/*/$ns3::Node/DeviceList/*/$ns3::WifiNetDevice/Mac/"
                "$ns3::StaWifiMac/BK_Txop/$ns3::QosTxop/Queue/$ns3::WifiMacQueue/MaxSize",
                QueueSizeValue(QueueSize("1p")));
    Config::Set("/$ns3::NodeListPriv/NodeList/*/$ns3::Node/DeviceList/*/$ns3::WifiNetDevice/Mac/"
                "$ns3::StaWifiMac/VO_Txop/$ns3::QosTxop/Queue/$ns3::WifiMacQueue/MaxSize",
                QueueSizeValue(QueueSize("1p")));

    // Internet stack
    InternetStackHelper stack;
    stack.Install(apNode);
    stack.Install(staNodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(NetDeviceContainer(apDevice, staDevice));

    Ptr<UniformRandomVariable> randStart = CreateObject<UniformRandomVariable>();
    randStart->SetAttribute("Min", DoubleValue(0.0));
    randStart->SetAttribute("Max", DoubleValue(5.0));

    uint16_t port = 4711;
    Ipv4Address apAddr = apNode.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(apAddr, port)));
    sink.Install(apNode.Get(0));

    //  Traffic
    for (uint32_t i = 0; i < numUsers; ++i)
    {
        // Uplink traffic: STA to AP
        OnOffHelper uplink("ns3::UdpSocketFactory", Address(InetSocketAddress(apAddr, port)));
        uplink.SetAttribute("DataRate", StringValue("50Mbps"));
        uplink.SetAttribute("PacketSize", UintegerValue(1500));
        uplink.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        uplink.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        ApplicationContainer app = uplink.Install(staNodes.Get(i));

        // Start application
        app.Start(Seconds(60.0) + Seconds(randStart->GetValue()));

        app.Stop(time + Seconds(65));
    }

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/MonitorSnifferRx",
                    MakeCallback(&PhyRxCallback));
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                    MakeCallback(&MacTxCallback));
    Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                    MakeCallback(&MacRxCallback));
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/"
                    "$ns3::MinstrelHtWifiManager/Rate",
                    MakeCallback(&RateCallback));
    Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop",
                    MakeCallback(&PhyRxDropCallback));
    Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop",
                    MakeCallback(&MacRxDropCallback));
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop",
                    MakeCallback(&PhyTxDropCallback));
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop",
                    MakeCallback(&MacTxDropCallback));

    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/TxPowerStart",
                DoubleValue(17));
    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/TxPowerEnd",
                DoubleValue(17));

    if (pcap)
    {
        std::ostringstream oss;
        oss << numUsers << "_" << numElements << "_" << run << "_" << rtsCts << "_" << mesh << "_"
            << staIrs;
        wifiPhy.EnablePcapAll(oss.str());
    }

    // Run simulation
    Simulator::Stop(time + Seconds(65));

    // NOTE: uncomment to debug attributes
    //
    // Config::SetDefault("ns3::ConfigStore::Filename", StringValue("output-attributes.txt"));
    // Config::SetDefault("ns3::ConfigStore::FileFormat", StringValue("RawText"));
    // Config::SetDefault("ns3::ConfigStore::Mode", StringValue("Save"));
    // ConfigStore outputConfig;
    // outputConfig.ConfigureAttributes();

    Simulator::Run();

    Simulator::Destroy();

    return GetScenarioStatistics(run,
                                 numElements,
                                 numUsers,
                                 staElements,
                                 time,
                                 noirs,
                                 rtsCts,
                                 mesh,
                                 staIrs,
                                 staNodes);
}

int
main(int argc, char* argv[])
{
    bool verbose = false;
    bool debug = false;
    bool noirs = false;
    bool pretty = false;
    bool rtsCts = false;
    bool pcap = false;
    bool mesh = false;
    bool staIrs = false;
    uint16_t run = 1;
    uint16_t numUsers = 5;
    Time time{"15s"};
    uint16_t elements = 1024;
    uint16_t staElements = 100;

    CommandLine cmd(__FILE__);
    cmd.AddValue("debug", "IRS Debug Info", debug);
    cmd.AddValue("verbose", "Verbose Data Rate Logging", verbose);
    cmd.AddValue("run", "Run number for different randomness seed", run);
    cmd.AddValue("users", "Amount of users to simulate", numUsers);
    cmd.AddValue("pretty", "Pretty print the JSON", pretty);
    cmd.AddValue("pcap", "Generate Pcap files", pcap);
    cmd.AddValue("mesh", "Fully meshed nodes", mesh);
    cmd.AddValue("time", "Simulation Time", time);
    cmd.AddValue("stairs", "IRS per STA", staIrs);
    cmd.AddValue("no-irs", "No IRS and LOS connection", noirs);
    cmd.AddValue("rts-cts", "Enable Rts/Cts for all packets", rtsCts);
    cmd.AddValue("elements", "Total amount of elements", elements);
    cmd.AddValue("sta-elements", "Amount of elements for StaIrs", staElements);

    cmd.Parse(argc, argv);

    if (debug)
    {
        LogComponentEnable("IrsPropagationLossModel", ns3::LOG_LEVEL_ALL);
    }
    if (verbose)
    {
        LogComponentEnable("IRS-MultiUser", ns3::LOG_LEVEL_ALL);
    }

    json scenarioResults =
        RunScenario(numUsers, run, time, noirs, elements, rtsCts, pcap, mesh, staIrs, staElements);

    if (pretty)
    {
        std::cout << scenarioResults.dump(4) << std::endl;
    }
    else
    {
        std::cout << scenarioResults.dump() << std::endl;
    }

    return 0;
}
