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

#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsCompareMatlab");

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
    std::cout << "SNR: " << signalNoise.signal - signalNoise.noise << std::endl;
    std::cout << "signal: " << signalNoise.signal << " noise: " << signalNoise.noise << std::endl;
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
RunSimulation(std::string scenario)
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
    irsHelper.SetLookupTable(
        "/home/jrueh/Studium/tkn_thesis/ns-3-dev/contrib/irs/matlab/lookuptable.csv");
    irsHelper.Install(irs);

    WifiHelper wifi;
    WifiHelper::EnableLogComponents();
    wifi.SetStandard(WIFI_STANDARD_80211ac);

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper();
    wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    if (scenario == "IRS")
    {
        Ptr<FriisPropagationLossModel> losLossModel = CreateObject<FriisPropagationLossModel>();
        losLossModel->SetFrequency(5.21e9);
        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        irsLossModel->SetFrequency(5.21e9);
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irs),
                                       "IrsLossModel",
                                       PointerValue(irsLossModel),
                                       "LosLossModel",
                                       PointerValue(losLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }
    else if (scenario == "IRSnlos")
    {
        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irs),
                                       "LossModel",
                                       PointerValue(irsLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }
    else if (scenario == "los")
    {
        wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
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
    positionAlloc->Add(Vector(1.0607, 1.0607, 0.0));  // RX
    positionAlloc->Add(Vector(-14.1421, 14.1421, 0)); // TX
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    mobility.Install(irs);

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
                DoubleValue(17)); // dBm$

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

int
main(int argc, char* argv[])
{
    std::vector<std::string> scenarios = {"IRS", "IRSnlos", "los"};

    for (const auto& model : scenarios)
    {
        std::cout << "Running simulation with " << model << " propagation model" << std::endl;
        RunSimulation(model);
        std::cout << "-----------------------------------" << std::endl;
    }

    return 0;
}
