/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/interference-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/irs-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/node-container.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-psdu.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/yans-wifi-phy.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsValidateChangingSizes");

struct SimulationResult
{
    double ueDistance;
    std::map<std::string, double> rxPower;
};

std::vector<SimulationResult> simulationResults;
std::string currentSimulationContext;

void
ReceivePacket(Ptr<Socket> socket)
{
    while (socket->Recv())
    {
        std::cout << "Received one packet!" << std::endl;
    }
}

static void
GenerateTraffic(Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
{
    if (pktCount > 0)
    {
        socket->Send(Create<Packet>(pktSize));
        Simulator::Schedule(pktInterval,
                            &GenerateTraffic,
                            socket,
                            pktSize,
                            pktCount - 1,
                            pktInterval);
    }
    else
    {
        socket->Close();
    }
}

void
RCallback(std::string context,
          Ptr<const Packet> p,
          uint16_t channelFreqMhz,
          WifiTxVector txVector,
          MpduInfo aMpdu,
          SignalNoiseDbm signalNoise,
          uint16_t staId)
{
    // std::cout << "SNR: " << signalNoise.signal - signalNoise.noise << std::endl;
    // std::cout << "signal: " << signalNoise.signal << " noise: " << signalNoise.noise <<
    // std::endl; Store the received signal power
    simulationResults.back().rxPower[currentSimulationContext] = signalNoise.signal;
    std::cout << "DEBUG: rx_power: " << signalNoise.signal << std::endl;
    std::cout << "DEBUG: context" << currentSimulationContext << std::endl;
}

void
Receive(Ptr<const WifiPsdu> psdu,
        RxSignalInfo rxSignalInfo,
        WifiTxVector txVector,
        std::vector<bool> statusPerMpdu)
{
    std::cout << "SNR(linear) " << rxSignalInfo.snr << std::endl;
}

void
RunSimulation(std::string lookupTable, bool useRisOnly, double ueDistance)
{
    std::string phyMode("OfdmRate6Mbps");
    uint32_t packetSize = 1000; // bytes
    uint32_t numPackets = 1;
    Time interPacketInterval = Seconds(1.0);

    NodeContainer nodes;
    nodes.Create(2);

    NodeContainer irs;
    irs.Create(1);

    IrsHelper irsHelper;
    irsHelper.SetDirection(Vector(1, 0, 0));
    irsHelper.SetLookupTable("/home/jrueh/Studium/tkn_thesis/ns-3-dev/contrib/irs/matlab/" +
                             lookupTable);
    irsHelper.Install(irs);

    WifiHelper wifi;
    WifiHelper::EnableLogComponents();
    wifi.SetStandard(WIFI_STANDARD_80211ac);

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper();
    wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
    irsLossModel->SetFrequency(5.21e9);
    if (useRisOnly)
    {
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irs),
                                       "IrsLossModel",
                                       PointerValue(irsLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }
    else
    {
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irs),
                                       "IrsLossModel",
                                       PointerValue(irsLossModel),
                                       "LosLossModel",
                                       PointerValue(irsLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }

    wifiPhy.SetChannel(wifiChannel.Create());

    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(phyMode),
                                 "ControlMode",
                                 StringValue(phyMode));
    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // RIS position (always at 0,0,0)
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));

    // UE position
    double risToUeAngle = 10.0; // You can change this if needed
    double ueX = ueDistance * std::cos(risToUeAngle * M_PI / 180.0);
    double ueY = ueDistance * std::sin(risToUeAngle * M_PI / 180.0);
    positionAlloc->Add(Vector(ueX, ueY, 0.0));

    // AP position
    double apDistance = 20.0;
    double apToRisAngle = 170.0; // You can change this if needed
    double apX = apDistance * std::cos(apToRisAngle * M_PI / 180.0);
    double apY = apDistance * std::sin(apToRisAngle * M_PI / 180.0);
    positionAlloc->Add(Vector(apX, apY, 0.0));

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(irs);
    mobility.Install(nodes);

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO("Assign IP Addresses.");
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devices);

    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> recvSink = Socket::CreateSocket(nodes.Get(0), tid);
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
    recvSink->Bind(local);
    recvSink->SetRecvCallback(MakeCallback(&ReceivePacket));

    Ptr<Socket> source = Socket::CreateSocket(nodes.Get(1), tid);
    InetSocketAddress remote = InetSocketAddress(Ipv4Address("255.255.255.255"), 80);
    source->SetAllowBroadcast(true);
    source->Connect(remote);

    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx",
        MakeCallback(&RCallback));

    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/TxPowerStart",
                DoubleValue(17)); // dBm
    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/TxPowerEnd",
                DoubleValue(17)); // dBm

    Simulator::ScheduleWithContext(source->GetNode()->GetId(),
                                   Seconds(1.0),
                                   &GenerateTraffic,
                                   source,
                                   packetSize,
                                   numPackets,
                                   interPacketInterval);

    Simulator::Run();
    Simulator::Destroy();
}

void
writeResultsToCSV(const std::string& filename, std::vector<std::string> lookupTables)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write header
    file << "ueDistance";
    for (const auto& lookupTable : lookupTables)
    {
        file << "," << lookupTable << "_ris," << lookupTable << "_ris_los";
    }
    file << "\n";

    // Write data
    for (const auto& result : simulationResults)
    {
        file << result.ueDistance;
        for (const auto& lookupTable : lookupTables)
        {
            std::string risKey = lookupTable + "_ris";
            std::string risLosKey = lookupTable + "_ris_los";
            file << "," << (result.rxPower.count(risKey) ? result.rxPower.at(risKey) : 0) << ","
                 << (result.rxPower.count(risLosKey) ? result.rxPower.at(risLosKey) : 0);
        }
        file << "\n";
    }

    file.close();
}

int
main(int argc, char* argv[])
{
    std::vector<std::string> lookupTables = {"IRS_100_IN170_OUT10_FREQ5.21GHz.csv",
                                             "IRS_200_IN170_OUT10_FREQ5.21GHz.csv",
                                             "IRS_300_IN170_OUT10_FREQ5.21GHz.csv",
                                             "IRS_400_IN170_OUT10_FREQ5.21GHz.csv"};
    std::vector<bool> risOnlyScenarios = {true, false};
    std::vector<double> ueDistances;
    for (double d = 0.25; d <= 10.0; d += 0.1)
    {
        ueDistances.push_back(d);
    }

    for (double ueDistance : ueDistances)
    {
        SimulationResult result;
        result.ueDistance = ueDistance;
        simulationResults.push_back(result);

        for (const auto& lookupTable : lookupTables)
        {
            for (bool useRisOnly : risOnlyScenarios)
            {
                std::cout << "Running simulation with:" << std::endl;
                std::cout << "Lookup table: " << lookupTable << std::endl;
                std::cout << "RIS only: " << (useRisOnly ? "Yes" : "No") << std::endl;
                std::cout << "UE distance: " << ueDistance << std::endl;

                currentSimulationContext = lookupTable + (useRisOnly ? "_ris" : "_ris_los");
                RunSimulation(lookupTable, useRisOnly, ueDistance);

                std::cout << "-----------------------------------" << std::endl;
            }
        }
    }

    writeResultsToCSV("simulation_results.csv", lookupTables);

    return 0;
}
