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

using namespace ns3;

/**
 * Run single 10 seconds experiment
 *
 * \param enableCtsRts if true, enable RTS/CTS for packets larger than 100 bytes.
 * \param wifiManager WiFi manager to use.
 */
void
experiment(bool useIRS, std::string wifiManager)
{
    // 0. Enable or disable CTS/RTS
    // UintegerValue ctsThr = (enableCtsRts ? UintegerValue(100) : UintegerValue(2200));
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue(2200));

    // 1. Create 3 nodes
    NodeContainer nodes;
    nodes.Create(3);
    NodeContainer irsNode;
    irsNode.Create(1);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(50.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, -1.75, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    mobility.Install(irsNode);

    // 3. Create propagation loss matrix
    Ptr<LogDistancePropagationLossModel> lossModel =
        CreateObject<LogDistancePropagationLossModel>();
    // 4. Create & setup wifi channel
    Ptr<YansWifiChannel> wifiChannel = CreateObject<YansWifiChannel>();
    if (useIRS)
    {
        IrsHelper irsHelper;
        irsHelper.SetDirection(Vector(0, 1, 0));
        irsHelper.SetLookupTable("contrib/irs/examples/lookuptables/IRS_400_IN150_OUT89_FREQ5.21GHz_hidden_node.csv");
        irsHelper.Install(irsNode);

        Ptr<FriisPropagationLossModel> irsModel = CreateObject<FriisPropagationLossModel>();
        irsModel->SetFrequency(5.21e9);

        Ptr<IrsPropagationLossModel> irsLossModel = CreateObject<IrsPropagationLossModel>();
        irsLossModel->SetIrsNodes(&irsNode);
        irsLossModel->SetPropagationModel(irsModel);
        irsLossModel->SetFrequency(5.21e9);

        irsLossModel->SetNext(lossModel);
        wifiChannel->SetPropagationLossModel(irsLossModel);
    }
    else
    {
        wifiChannel->SetPropagationLossModel(lossModel);
    }
    wifiChannel->SetPropagationDelayModel(CreateObject<ConstantSpeedPropagationDelayModel>());

    // 5. Install wireless devices
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::" + wifiManager + "WifiManager");
    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel);
    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac"); // use ad-hoc MAC
    NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);

    // uncomment the following to have athstats output
    // AthstatsHelper athstats;
    // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" ,
    // nodes);

    // uncomment the following to have pcap output
    // wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
    // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);

    // 6. Install TCP/IP stack & assign IP addresses
    InternetStackHelper internet;
    internet.Install(nodes);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.0.0.0");
    ipv4.Assign(devices);

    // 7. Install applications: two CBR streams each saturating the channel
    ApplicationContainer cbrApps;
    uint16_t cbrPort = 12345;
    OnOffHelper onOffHelper("ns3::UdpSocketFactory",
                            InetSocketAddress(Ipv4Address("10.0.0.2"), cbrPort));
    onOffHelper.SetAttribute("PacketSize", UintegerValue(1400));
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    // flow 1:  node 0 -> node 1
    onOffHelper.SetAttribute("DataRate", StringValue("3000000bps"));
    onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(1.000000)));
    cbrApps.Add(onOffHelper.Install(nodes.Get(0)));

    // flow 2:  node 2 -> node 1
    /** \internal
     * The slightly different start times and data rates are a workaround
     * for \bugid{388} and \bugid{912}
     */
    onOffHelper.SetAttribute("DataRate", StringValue("3001100bps"));
    onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(1.001)));
    cbrApps.Add(onOffHelper.Install(nodes.Get(2)));

    /** \internal
     * We also use separate UDP applications that will send a single
     * packet before the CBR flows start.
     * This is a workaround for the lack of perfect ARP, see \bugid{187}
     */
    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper(Ipv4Address("10.0.0.2"), echoPort);
    echoClientHelper.SetAttribute("MaxPackets", UintegerValue(1));
    echoClientHelper.SetAttribute("Interval", TimeValue(Seconds(0.1)));
    echoClientHelper.SetAttribute("PacketSize", UintegerValue(10));
    ApplicationContainer pingApps;

    // again using different start times to workaround Bug 388 and Bug 912
    echoClientHelper.SetAttribute("StartTime", TimeValue(Seconds(0.001)));
    pingApps.Add(echoClientHelper.Install(nodes.Get(0)));
    echoClientHelper.SetAttribute("StartTime", TimeValue(Seconds(0.006)));
    pingApps.Add(echoClientHelper.Install(nodes.Get(2)));

    // 8. Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // 9. Run simulation for 10 seconds
    Simulator::Stop(Seconds(10));
    Simulator::Run();

    // 10. Print per flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    for (auto i = stats.begin(); i != stats.end(); ++i)
    {
        // first 2 FlowIds are for ECHO apps, we don't want to display them
        //
        // Duration for throughput measurement is 9.0 seconds, since
        //   StartTime of the OnOffApplication is at about "second 1"
        // and
        //   Simulator::Stops at "second 10".
        if (i->first > 2)
        {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
            std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> "
                      << t.destinationAddress << ")\n";
            std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
            std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
            std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000
                      << " Mbps\n";
            std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
            std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000
                      << " Mbps\n";
        }
    }

    // 11. Cleanup
    Simulator::Destroy();
}

int
main(int argc, char** argv)
{
    std::string wifiManager("Aarf");
    CommandLine cmd(__FILE__);
    cmd.AddValue(
        "wifiManager",
        "Set wifi rate manager (Aarf, Aarfcd, Amrr, Arf, Cara, Ideal, Minstrel, Onoe, Rraa)",
        wifiManager);
    cmd.Parse(argc, argv);

    std::cout << "Hidden node problem without IRS:\n" << std::flush;
    experiment(false, wifiManager);
    std::cout << "------------------------------------------------\n";
    std::cout << "Hidden node problem with IRS:\n";
    experiment(true, wifiManager);

    return 0;
}
