/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "ns3/boolean.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/double.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs-spectrum-model.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/pointer.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/types.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsMobility");

class ScenarioStatistics
{
  public:
    ScenarioStatistics(std::string scenarioName, double interval = 1.0);
    double GetThroughput() const;
    double GetSNR() const;
    double GetSuccessRate() const;
    double GetDataRate() const;
    void RateCallback(std::string path, uint64_t oldRate, uint64_t newRate);
    void RxCallback(std::string context,
                    Ptr<const Packet> p,
                    uint16_t channelFreqMhz,
                    WifiTxVector txVector,
                    MpduInfo aMpdu,
                    SignalNoiseDbm signalNoise,
                    uint16_t staId);
    void TxCallback(std::string context,
                    const Ptr<const Packet> packet,
                    uint16_t channelFreqMhz,
                    WifiTxVector txVector,
                    MpduInfo aMpdu,
                    uint16_t staId);
    void PeriodicUpdate();
    void SaveDataPoint();
    void SetNodes(Ptr<NodeContainer> sta, Ptr<NodeContainer> irs, Ptr<NodeContainer> ap);

  private:
    std::string m_scenarioName;
    uint64_t m_bytesTotal;
    double m_dataRate;
    uint64_t m_rxpackets;
    uint64_t m_txpackets;
    uint64_t m_snrSum;
    std::ofstream m_outputFile;
    double m_interval;
    double m_startTime;
    Ptr<NodeContainer> m_sta;
    Ptr<NodeContainer> m_irs;
    Ptr<NodeContainer> m_ap;
    uint8_t m_skip;
};

ScenarioStatistics::ScenarioStatistics(std::string scenarioName, double interval)
    : m_scenarioName(scenarioName),
      m_bytesTotal(0),
      m_dataRate(0),
      m_rxpackets(0),
      m_txpackets(0),
      m_snrSum(0),
      m_interval(interval),
      m_startTime(1),
      m_skip(0)
{
    std::string filename = scenarioName + "_mobility.csv";
    m_outputFile.open(filename.c_str(), std::ios::out | std::ios::trunc);
    m_outputFile
        << "Time,Throughput,DataRate,SNR,SuccessRate,TxPackets,RxPackets,PosAp,PosIrs,PosSta"
        << std::endl;

    Simulator::Schedule(Seconds(m_startTime), &ScenarioStatistics::PeriodicUpdate, this);
}

void
ScenarioStatistics::SaveDataPoint()
{
    if (m_skip < 2)
    {
        m_bytesTotal = 0;
        m_snrSum = 0;
        m_rxpackets = 0;
        m_txpackets = 0;
        m_skip++;
        return;
    }

    double time = Simulator::Now().GetSeconds();

    Ptr<MobilityModel> pos = m_sta->Get(0)->GetObject<MobilityModel>();
    Ptr<MobilityModel> irs = m_irs->Get(0)->GetObject<MobilityModel>();
    Ptr<MobilityModel> ap = m_ap->Get(0)->GetObject<MobilityModel>();

    m_outputFile << std::fixed << std::setprecision(4) << time << "," << GetThroughput() << ","
                 << GetDataRate() << "," << GetSNR() << "," << GetSuccessRate() << ","
                 << m_txpackets << "," << m_rxpackets << "," << ap->GetPosition() << ","
                 << irs->GetPosition() << "," << pos->GetPosition() << std::endl;
    m_bytesTotal = 0;
    m_snrSum = 0;
    m_rxpackets = 0;
    m_txpackets = 0;
}

void
ScenarioStatistics::PeriodicUpdate()
{
    SaveDataPoint();
    Simulator::Schedule(Seconds(m_interval), &ScenarioStatistics::PeriodicUpdate, this);
}

void
ScenarioStatistics::SetNodes(Ptr<NodeContainer> sta, Ptr<NodeContainer> irs, Ptr<NodeContainer> ap)
{
    m_sta = sta;
    m_irs = irs;
    m_ap = ap;
}

std::optional<Angles>
CalcAngles3D(ns3::Vector node, ns3::Vector irs, ns3::Vector irsNormal)
{
    auto cross = [](const ns3::Vector& a, const ns3::Vector& b) -> ns3::Vector {
        return ns3::Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    };

    auto normalize = [](const ns3::Vector& vec) -> ns3::Vector {
        double length = vec.GetLength();
        return ns3::Vector(vec.x / length, vec.y / length, vec.z / length);
    };

    // Calculate incident vector (from node to IRS) and normalize
    ns3::Vector incident = normalize(node - irs);

    // Check if the node is on the correct side
    double dotProduct = incident * irsNormal;
    if (dotProduct < std::numeric_limits<double>::epsilon())
    {
        std::cout << dotProduct << std::endl;
        // Node is on the wrong side of the IRS
        return std::nullopt;
    }

    // Create a local coordinate system for the IRS
    ns3::Vector z_axis = irsNormal;

    ns3::Vector reference =
        (std::abs(irsNormal.z) > 0.9) ? ns3::Vector(1, 0, 0) : ns3::Vector(0, 0, 1);
    ns3::Vector x_axis = normalize(cross(reference, irsNormal));
    ns3::Vector y_axis = cross(z_axis, x_axis);

    // Project incident vector onto local coordinate system
    double x = incident * x_axis;
    double y = incident * y_axis;
    double z = incident * z_axis;

    // Calculate angles
    double azimuth = std::atan2(y, x);
    double inclination = std::acos(z); // Inclination: [0, π]
    return Angles(azimuth, inclination);
}

double
ScenarioStatistics::GetThroughput() const
{
    return (m_bytesTotal * 8.0) / (m_interval * 1e6); // Mbps
}

double
ScenarioStatistics::GetDataRate() const
{
    return m_dataRate / 1e6;
}

double
ScenarioStatistics::GetSuccessRate() const
{
    return (m_txpackets == 0) ? 0.0 : ((double)m_rxpackets / (double)m_txpackets) * 100.0;
}

double
ScenarioStatistics::GetSNR() const
{
    return (m_rxpackets == 0) ? 0.0 : m_snrSum / m_rxpackets;
}

void
ScenarioStatistics::RateCallback(std::string path, uint64_t oldRate, uint64_t newRate)
{
    NS_LOG_INFO(m_scenarioName << " | " << (Simulator::Now()).GetSeconds() << " | Old rate: "
                               << oldRate / 1e6 << "Mbps New rate: " << newRate / 1e6 << "Mbps");
    m_dataRate = newRate;
}

void
ScenarioStatistics::RxCallback(std::string context,
                               Ptr<const Packet> p,
                               uint16_t channelFreqMhz,
                               WifiTxVector txVector,
                               MpduInfo aMpdu,
                               SignalNoiseDbm signalNoise,
                               uint16_t staId)
{
    m_snrSum += signalNoise.signal - signalNoise.noise;
    m_rxpackets++;
    m_bytesTotal += p->GetSize();
}

void
ScenarioStatistics::TxCallback(std::string context,
                               const Ptr<const Packet> packet,
                               uint16_t channelFreqMhz,
                               WifiTxVector txVector,
                               MpduInfo aMpdu,
                               uint16_t staId)
{
    m_txpackets++;
}

void
RunScenario(std::string scenario, std::string wifiManager, uint16_t runNumber = 1)
{
    RngSeedManager::SetSeed(2024);
    RngSeedManager::SetRun(runNumber);

    // Create nodes
    NodeContainer wifiApNode;
    NodeContainer wifiStaNode;
    NodeContainer irsNode;
    wifiApNode.Create(1);
    wifiStaNode.Create(1);

    Vector pos_ap = {2, 0, 10};
    Vector pos_irs = {0, 2, 10};
    Vector pos_sta_opt = {10, 10, 0};

    MobilityHelper moving;
    moving.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    moving.Install(wifiStaNode);
    // moving along y axis in 1 m/s
    wifiStaNode.Get(0)->GetObject<MobilityModel>()->SetPosition({10, 1, 0});
    wifiStaNode.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity({0, 1, 0});

    // Set up WiFi
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;

    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    if (scenario == "irs" || scenario == "los")
    {
        irsNode.Create(1);
        MobilityHelper mobility;
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        positionAlloc->Add(pos_ap);
        positionAlloc->Add(pos_irs);
        mobility.SetPositionAllocator(positionAlloc);
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(wifiApNode);
        mobility.Install(irsNode);
        Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({1, 0, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({20, 20}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Frequency",
            DoubleValue(5.21e9));

        std::optional<Angles> in_angle = CalcAngles3D(pos_ap, pos_irs, {1, 0, 0});
        std::optional<Angles> out_angle = CalcAngles3D(pos_sta_opt, pos_irs, {1, 0, 0});
        NS_ASSERT(in_angle.has_value());
        NS_ASSERT(out_angle.has_value());
        std::cout << in_angle.value() << std::endl;
        std::cout << out_angle.value() << std::endl;
        irs->CalcRCoeffs(in_angle.value(), out_angle.value());

        irsNode.Get(0)->AggregateObject(irs);

        Ptr<LogDistancePropagationLossModel> lossModel =
            CreateObject<LogDistancePropagationLossModel>();
        lossModel->SetPathLossExponent(2);

        if (scenario == "los")
        {
            Ptr<LogDistancePropagationLossModel> losLossModel =
                CreateObject<LogDistancePropagationLossModel>();
            lossModel->SetPathLossExponent(3);
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
        else
        {
            wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                           "IrsNodes",
                                           PointerValue(&irsNode),
                                           "IrsLossModel",
                                           PointerValue(lossModel),
                                           "Frequency",
                                           DoubleValue(5.21e9));
        }
    }
    else if (scenario == "subsections")
    {
        irsNode.Create(4);
        MobilityHelper mobility;
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        positionAlloc->Add(pos_ap);
        positionAlloc->Add(pos_irs);
        positionAlloc->Add(pos_irs);
        positionAlloc->Add(pos_irs);
        positionAlloc->Add(pos_irs);
        mobility.SetPositionAllocator(positionAlloc);
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(wifiApNode);
        mobility.Install(irsNode);

        Ptr<IrsSpectrumModel> irs1 = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({1, 0, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({5, 5}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Frequency",
            DoubleValue(5.21e9));
        std::optional<Angles> in_angle = CalcAngles3D(pos_ap, pos_irs, {1, 0, 0});
        std::optional<Angles> out_angle = CalcAngles3D({10, 4, 0}, pos_irs, {1, 0, 0});
        NS_ASSERT(in_angle.has_value());
        NS_ASSERT(out_angle.has_value());
        irs1->CalcRCoeffs(in_angle.value(), out_angle.value());
        irsNode.Get(0)->AggregateObject(irs1);

        Ptr<IrsSpectrumModel> irs2 = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({1, 0, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({5, 5}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Frequency",
            DoubleValue(5.21e9));
        in_angle = CalcAngles3D(pos_ap, pos_irs, {1, 0, 0});
        out_angle = CalcAngles3D({10, 8, 0}, pos_irs, {1, 0, 0});
        NS_ASSERT(in_angle.has_value());
        NS_ASSERT(out_angle.has_value());
        irs2->CalcRCoeffs(in_angle.value(), out_angle.value());
        irsNode.Get(1)->AggregateObject(irs2);

        Ptr<IrsSpectrumModel> irs3 = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({1, 0, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({5, 5}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Frequency",
            DoubleValue(5.21e9));
        in_angle = CalcAngles3D(pos_ap, pos_irs, {1, 0, 0});
        out_angle = CalcAngles3D({10, 12, 0}, pos_irs, {1, 0, 0});
        NS_ASSERT(in_angle.has_value());
        NS_ASSERT(out_angle.has_value());
        irs3->CalcRCoeffs(in_angle.value(), out_angle.value());
        irsNode.Get(2)->AggregateObject(irs3);

        Ptr<IrsSpectrumModel> irs4 = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({1, 0, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({5, 5}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Frequency",
            DoubleValue(5.21e9));
        in_angle = CalcAngles3D(pos_ap, pos_irs, {1, 0, 0});
        out_angle = CalcAngles3D({10, 16, 0}, pos_irs, {1, 0, 0});
        NS_ASSERT(in_angle.has_value());
        NS_ASSERT(out_angle.has_value());
        irs4->CalcRCoeffs(in_angle.value(), out_angle.value());
        irsNode.Get(3)->AggregateObject(irs4);

        Ptr<LogDistancePropagationLossModel> lossModel =
            CreateObject<LogDistancePropagationLossModel>();
        lossModel->SetPathLossExponent(2);
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irsNode),
                                       "IrsLossModel",
                                       PointerValue(lossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }

    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiPhy.SetInterferenceHelper("ns3::InterferenceHelper");

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager(wifiManager);

    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("ComparisonScenario");
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer staDevice;
    staDevice = wifi.Install(wifiPhy, wifiMac, wifiStaNode);

    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevice;
    apDevice = wifi.Install(wifiPhy, wifiMac, wifiApNode);

    // Internet stack
    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNode);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(NetDeviceContainer(apDevice, staDevice));

    // Application
    uint16_t port = 9;
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          InetSocketAddress(interfaces.GetAddress(1), port));
    ApplicationContainer sinkApp = sink.Install(wifiStaNode.Get(0));

    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(1), port));
    onoff.SetConstantRate(DataRate("400Mbps"), 1024);
    ApplicationContainer sourceApp = onoff.Install(wifiApNode.Get(0));

    sinkApp.Start(Seconds(0));
    sourceApp.Start(Seconds(1));

    // Statistics
    ScenarioStatistics stats(scenario, 0.25);
    stats.SetNodes(&wifiStaNode, &irsNode, &wifiApNode);
    // STA: mean SNR + count recieved packages at phy level
    Config::Connect(
        "/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx",
        MakeCallback(&ScenarioStatistics::RxCallback, &stats));
    // AP: DataRate
    Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$" +
                        wifiManager + "/Rate",
                    MakeCallback(&ScenarioStatistics::RateCallback, &stats));
    // AP: count sent packages at phy level
    Config::Connect(
        "/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferTx",
        MakeCallback(&ScenarioStatistics::TxCallback, &stats));

    // Run simulation
    Simulator::Stop(Seconds(20));
    Simulator::Run();

    stats.SaveDataPoint();

    Simulator::Destroy();
}

int
main(int argc, char* argv[])
{
    std::string scenario = "irs";
    std::string wifiManager = "ns3::MinstrelHtWifiManager";
    bool verbose = false;
    bool debug = false;
    uint16_t runNumber = 1;

    CommandLine cmd(__FILE__);
    cmd.AddValue("scenario",
                 "Scenario to run: LOS, IRS, IrsConstructive, IrsDestructive",
                 scenario);
    cmd.AddValue("wifiManager", "wifi manager", wifiManager);
    cmd.AddValue("debug", "IRS Debug Info", debug);
    cmd.AddValue("verbose", "Verbose Data Rate Logging", verbose);
    cmd.AddValue("run", "run number for different randomness seed", runNumber);

    cmd.Parse(argc, argv);

    if (debug)
    {
        ns3::LogComponentEnable("IrsPropagationLossModel", ns3::LOG_LEVEL_ALL);
    }
    if (verbose)
    {
        ns3::LogComponentEnable("IrsMobility", ns3::LOG_LEVEL_ALL);
    }

    if (scenario == "irs")
    {
        RunScenario("irs", wifiManager, runNumber);
    }
    else if (scenario == "subsections")
    {
        RunScenario("subsections", wifiManager, runNumber);
    }
    else if (scenario == "los")
    {
        RunScenario("los", wifiManager, runNumber);
    }

    return 0;
}
