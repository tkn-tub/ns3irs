/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * This program is based on the work by Pavel Boyko <boyko@iitp.ru> (original copyright details
 * below).
 *
 * Original Copyright (c) 2010 IITP RAS
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
 *
 * Authors: Pavel Boyko <boyko@iitp.ru> (original author)
 *          Jakob Rühlow <j.ruehlow@campus.tu-berlin.de> (modifications)
 *
 * Description: Classical hidden node problem and solution using IRS
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
#include "ns3/irs-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
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
                    Gnuplot2dDataset* data1,
                    Gnuplot2dDataset* data2,
                    double time)
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

    // Add points to datasets
    data1->Add(time, throughput1);
    data2->Add(time, throughput2);
}

void
RunSimulation(bool useIRS,
              bool useRtsCts,
              double simTime,
              Gnuplot2dDataset& data1,
              Gnuplot2dDataset& data2)
{
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
    positionAlloc->Add(Vector(1.0, -1.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    mobility.Install(irsNode);

    // Create propagation loss matrix
    Ptr<LogDistancePropagationLossModel> lossModel =
        CreateObject<LogDistancePropagationLossModel>();

    // Create & setup wifi channel
    Ptr<YansWifiChannel> wifiChannel = CreateObject<YansWifiChannel>();
    if (useIRS)
    {
        IrsHelper irsHelper;
        irsHelper.SetDirection(Vector(0, 1, 0));
        irsHelper.SetLookupTable(
            "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT89_FREQ5.18GHz_hidden_node.csv");
        irsHelper.Install(irsNode);

        Ptr<FriisPropagationLossModel> irsModel = CreateObject<FriisPropagationLossModel>();
        irsModel->SetFrequency(5.18e9);

        Ptr<IrsPropagationLossModel> irsLossModel = CreateObject<IrsPropagationLossModel>();
        irsLossModel->SetIrsNodes(&irsNode);
        irsLossModel->SetPropagationModel(irsModel);
        irsLossModel->SetFrequency(5.18e9);

        irsLossModel->SetNext(lossModel);
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
    for (double t = 1.0; t <= simTime; t += 0.5)
    {
        Simulator::Schedule(Seconds(t), &CalculateThroughput, monitor, &data1, &data2, t);
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
    bool rts_cts = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Verbose Logging", verbose);
    cmd.AddValue("simTime", "Simulation Time (in seconds)", simTime);
    cmd.AddValue("rts-cts", "Enable Rts/Cts simulation", rts_cts);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        ns3::LogComponentEnable("HiddenNodeProblem", ns3::LOG_LEVEL_INFO);
    }

    // Create Gnuplot object
    Gnuplot plot("hidden_node_problem_throughput.svg");
    plot.SetTitle("Throughput: With and Without IRS");
    plot.SetLegend("Time (s)", "Throughput (Mbps)");
    plot.SetExtra("set key right center box 3");
    plot.SetTerminal("svg");

    // Create datasets
    Gnuplot2dDataset dataNoIrs1, dataNoIrs2, dataIrs1, dataIrs2;
    Gnuplot2dDataset dataRts1, dataRts2;
    dataNoIrs1.SetTitle("Tx(1) without IRS");
    dataNoIrs2.SetTitle("Tx(2) without IRS");
    dataIrs1.SetTitle("Tx(1) with IRS");
    dataIrs2.SetTitle("Tx(2) with IRS");

    dataNoIrs1.SetStyle(Gnuplot2dDataset::LINES);
    dataNoIrs2.SetStyle(Gnuplot2dDataset::LINES);
    dataIrs1.SetStyle(Gnuplot2dDataset::LINES);
    dataIrs2.SetStyle(Gnuplot2dDataset::LINES);

    NS_LOG_INFO("Hidden node problem without IRS:");
    RunSimulation(false, false, simTime, dataNoIrs1, dataNoIrs2);
    NS_LOG_INFO("------------------------------------------------");
    NS_LOG_INFO("Hidden node problem with IRS:");
    RunSimulation(true, false, simTime, dataIrs1, dataIrs2);

    if (rts_cts)
    {
        dataRts1.SetTitle("Tx(1) RTS/CTS");
        dataRts2.SetTitle("Tx(2) RTS/CTS");
        dataRts1.SetStyle(Gnuplot2dDataset::LINES);
        dataRts2.SetStyle(Gnuplot2dDataset::LINES);
        RunSimulation(false, true, simTime, dataRts1, dataRts2);
    }

    // Add datasets to plot
    plot.AddDataset(dataNoIrs1);
    plot.AddDataset(dataNoIrs2);
    plot.AddDataset(dataIrs1);
    plot.AddDataset(dataIrs2);
    if (rts_cts)
    {
        plot.AddDataset(dataRts1);
        plot.AddDataset(dataRts2);
    }

    // Open plot file
    std::ofstream plotFile("hidden_node_problem_throughput.plt");
    plot.GenerateOutput(plotFile);
    plotFile.close();

    return 0;
}
