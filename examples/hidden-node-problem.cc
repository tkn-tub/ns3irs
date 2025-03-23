/*
 * Original Copyright (c) 2010 IITP RAS.  Authors: Pavel Boyko <boyko@iitp.ru>
 *
 * Copyright (c) 2024-2025 Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Description: Classical hidden node problem and solution using IRS.
 * This program is derived from Pavel Boyko's work, but has been significantly modified to show the
 * impact of IRS in such scenarios.
 *
 * See Also: examples/wireless/wifi-hidden-terminal.cc
 */

#include "ns3/boolean.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/gnuplot.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/irs-lookup-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/string.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/uinteger.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <cstdint>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HiddenNodeProblem");

void
CalculateThroughput(Ptr<FlowMonitor> monitor,
                    std::ofstream* csvFile,
                    double time,
                    const std::string& scenario)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    double throughput1 = 0.0, throughput2 = 0.0;

    auto iter = stats.begin();
    ++iter;
    ++iter;
    if (iter != stats.end())
    {
        throughput1 = iter->second.rxBytes * 8.0 / (time * 1e6); // Convert to Mbps
        ++iter;
    }
    if (iter != stats.end())
    {
        throughput2 = iter->second.rxBytes * 8.0 / (time * 1e6); // Convert to Mbps
    }

    *csvFile << time << "," << scenario << "," << throughput1 << "," << throughput2 << "\n";
}

void
RunSimulation(bool useIRS,
              bool useRtsCts,
              double simTime,
              std::ofstream& csvFile,
              uint16_t runNumber,
              const std::string& scenario)
{
    RngSeedManager::SetSeed(2025);
    RngSeedManager::SetRun(runNumber);

    // Disable CTS/RTS
    if (useRtsCts)
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(100));
    }
    else
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(2200));
    }
    // Create 3 nodes
    NodeContainer nodes;
    nodes.Create(3);
    // Create Irs
    NodeContainer irsNode;
    irsNode.Create(1);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(50.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.7, -0.7, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    mobility.Install(irsNode);

    // Create propagation loss matrix
    Ptr<LogDistancePropagationLossModel> lossModel =
        CreateObject<LogDistancePropagationLossModel>();
    lossModel->SetPathLossExponent(3);

    // Create & setup wifi channel
    Ptr<YansWifiChannel> wifiChannel = CreateObject<YansWifiChannel>();
    if (useIRS)
    {
        IrsLookupHelper irsHelper;
        irsHelper.SetDirection(Vector(0, 1, 0));
        irsHelper.SetLookupTable(
            "contrib/irs/examples/lookuptables/IRS_625_IN135_OUT90_FREQ5.15GHz_hidden_node.csv");
        irsHelper.Install(irsNode);

        Ptr<LogDistancePropagationLossModel> irsModel =
            CreateObject<LogDistancePropagationLossModel>();
        irsModel->SetPathLossExponent(3);

        Ptr<IrsPropagationLossModel> irsLossModel = CreateObject<IrsPropagationLossModel>();
        irsLossModel->SetIrsNodes(&irsNode);
        irsLossModel->SetIrsPropagationModel(irsModel);
        irsLossModel->SetLosPropagationModel(lossModel);
        irsLossModel->SetFrequency(5.15e9);

        wifiChannel->SetPropagationLossModel(irsLossModel);
    }
    else
    {
        wifiChannel->SetPropagationLossModel(lossModel);
    }
    wifiChannel->SetPropagationDelayModel(CreateObject<ConstantSpeedPropagationDelayModel>());

    // Install wireless devices
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel);
    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);

    // Install TCP/IP stack & assign IP addresses
    InternetStackHelper internet;
    internet.Install(nodes);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.0.0.0");
    ipv4.Assign(devices);

    // Install applications: two CBR streams each saturating the channel
    ApplicationContainer cbrApps;
    uint16_t cbrPort = 12345;
    OnOffHelper onOffHelper("ns3::UdpSocketFactory",
                            InetSocketAddress(Ipv4Address("10.0.0.2"), cbrPort));
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1400));
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    // flow 1:  node 0 -> node 1
    onOffHelper.SetAttribute("DataRate", StringValue("10Mbps"));
    onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
    cbrApps.Add(onOffHelper.Install(nodes.Get(0)));

    // flow 2:  node 2 -> node 1
    onOffHelper.SetAttribute("DataRate", StringValue("10Mbps"));
    onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
    cbrApps.Add(onOffHelper.Install(nodes.Get(2)));

    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper(Ipv4Address("10.0.0.2"), echoPort);
    echoClientHelper.SetAttribute("MaxPackets", UintegerValue(1));
    echoClientHelper.SetAttribute("Interval", TimeValue(Seconds(0.1)));
    echoClientHelper.SetAttribute("PacketSize", UintegerValue(10));
    ApplicationContainer pingApps;

    echoClientHelper.SetAttribute("StartTime", TimeValue(Seconds(0.001)));
    pingApps.Add(echoClientHelper.Install(nodes.Get(0)));
    pingApps.Add(echoClientHelper.Install(nodes.Get(2)));

    // 8. Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Simulation time
    Simulator::Stop(Seconds(simTime));

    // Schedule throughput calculation
    for (double t = 1.0; t <= simTime; t += 0.05)
    {
        Simulator::Schedule(Seconds(t), &CalculateThroughput, monitor, &csvFile, t, scenario);
    }

    Simulator::Run();

    // 10. Print per flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    for (auto i = stats.begin(); i != stats.end(); ++i)
    {
        // first 2 FlowIds are for ECHO apps, we don't want to display them
        if (i->first > 2)
        {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
            NS_LOG_INFO("Flow " << i->first - 2 << " (" << t.sourceAddress << " -> "
                                << t.destinationAddress << ")\n"
                                << "  Tx Packets: " << i->second.txPackets << "\n"
                                << "  Tx Bytes:   " << i->second.txBytes << "\n"
                                << "  TxOffered:  "
                                << (i->second.txBytes * 8.0) / ((simTime - 1) * 1e6) << " Mbps\n"
                                << "  Rx Packets: " << i->second.rxPackets << "\n"
                                << "  Rx Bytes:   " << i->second.rxBytes << "\n"
                                << "  Throughput: "
                                << (i->second.rxBytes * 8.0) / ((simTime - 1) * 1e6) << " Mbps\n");
        }
    }

    // 11. Cleanup
    Simulator::Destroy();
}

int
main(int argc, char** argv)
{
    double simTime = 15.0;
    bool verbose = false;
    uint16_t run = 1;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Verbose Logging", verbose);
    cmd.AddValue("run", "Seed run number", run);
    cmd.AddValue("simTime", "Simulation Time (in seconds)", simTime);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        ns3::LogComponentEnable("HiddenNodeProblem", ns3::LOG_LEVEL_INFO);
    }

    std::ostringstream filename;
    filename << "contrib/irs/results_and_scripts/hidden-node-problem/hidden-node-problem_" << run
             << ".csv";
    std::ofstream csvFile(filename.str());
    csvFile << "Time,Scenario,Tx1_Throughput,Tx2_Throughput\n";

    NS_LOG_INFO("Hidden node problem without IRS:");
    RunSimulation(false, false, simTime, csvFile, run, "Baseline");
    NS_LOG_INFO("------------------------------------------------");
    NS_LOG_INFO("Hidden node problem with IRS:");
    RunSimulation(true, false, simTime, csvFile, run, "IRS");
    NS_LOG_INFO("------------------------------------------------");
    NS_LOG_INFO("Hidden node problem with RTS-CTS:");
    RunSimulation(false, true, simTime, csvFile, run, "RTS/CTS");

    csvFile.close();

    return 0;
}
