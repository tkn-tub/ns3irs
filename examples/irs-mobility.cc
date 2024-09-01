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
 *
 * Author: Jakob Rühlow <j.ruehlow@campus.tu-berlin.de>
 */

#include "ns3/boolean.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/double.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/irs-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs.h"
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
    std::string filename = scenarioName + "_data.csv";
    m_outputFile.open(filename.c_str(), std::ios::out | std::ios::trunc);
    m_outputFile
        << "Time,Throughput,DataRate,SNR,SuccessRate,TxPackets,RxPackets,DistanceIrs,DistanceAp"
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
                 << m_txpackets << "," << m_rxpackets << "," << pos->GetDistanceFrom(irs) << ","
                 << pos->GetDistanceFrom(ap) << std::endl;
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
    irsNode.Create(1);

    // Set up WiFi
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;

    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    IrsHelper irsHelper;
    irsHelper.SetDirection(Vector(0, 1, 0));
    irsHelper.SetLookupTable(
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT6_FREQ5.21GHz_mobility.csv");
    irsHelper.Install(irsNode);

    Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
    irsLossModel->SetFrequency(5.21e9);

    Ptr<LogDistancePropagationLossModel> losLossModel =
        CreateObject<LogDistancePropagationLossModel>();

    wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                   "IrsNodes",
                                   PointerValue(&irsNode),
                                   "IrsLossModel",
                                   PointerValue(irsLossModel),
                                   "LosLossModel",
                                   PointerValue(losLossModel),
                                   "Frequency",
                                   DoubleValue(5.21e9));

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

    // Mobility
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 1.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(irsNode);

    MobilityHelper moving;
    moving.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    moving.Install(wifiStaNode);
    if (scenario == "1")
    {
        // 30s distance to optimal position
        // moving along x axis in walking speed (1.4 m/s)
        wifiStaNode.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(-38, -30, 0));
        wifiStaNode.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(
            Vector(1.4, 0, 0));
    }
    else
    {
        // 30s distance to optimal position
        // moving away from IRS on optimal angle in walking speed (1.4 m/s)
        wifiStaNode.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(1.3029, -2.1302, 0));
        wifiStaNode.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(
            Vector(0.1349, -1.3935, 0));
    }

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
    Simulator::Stop(Seconds(61));
    Simulator::Run();

    stats.SaveDataPoint();

    Simulator::Destroy();
}

int
main(int argc, char* argv[])
{
    std::string scenario = "1";
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

    if (scenario == "1")
    {
        // along x axis
        RunScenario("1", wifiManager, runNumber);
    }
    else if (scenario == "2")
    {
        // on diagonal axis in beamform away from irs
        RunScenario("2", wifiManager, runNumber);
    }
    return 0;
}
