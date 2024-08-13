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

NS_LOG_COMPONENT_DEFINE("IrsValidation");

class ScenarioStatistics
{
  public:
    ScenarioStatistics(std::string scenarioName);
    void RxCallback(std::string path, Ptr<const Packet> packet, const Address& from);
    double GetThroughput();
    std::string GetScenarioName();
    double GetDataRate() const;
    double GetSNR() const;
    double GetSuccessRate() const;
    uint32_t GetTotalRxPackets() const;
    uint32_t GetTotalTxPackets() const;
    void RateCallback(std::string path, uint64_t oldRate, uint64_t newRate);
    void SNRCallback(std::string context,
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

  private:
    std::string m_scenarioName;
    uint64_t m_bytesTotal;
    double m_throughput;
    double m_dataRate;
    uint32_t m_rxpackets;
    uint32_t m_txpackets;
    uint64_t m_snrSum;
};

ScenarioStatistics::ScenarioStatistics(std::string scenarioName)
    : m_scenarioName(scenarioName),
      m_bytesTotal(0),
      m_throughput(0),
      m_dataRate(0),
      m_rxpackets(0),
      m_txpackets(0),
      m_snrSum(0)
{
}

void
ScenarioStatistics::RxCallback(std::string path, Ptr<const Packet> packet, const Address& from)
{
    m_bytesTotal += packet->GetSize();
}

double
ScenarioStatistics::GetThroughput()
{
    m_throughput = (m_bytesTotal * 8.0) / (1e6 * 10); // Mbps over 10 seconds
    return m_throughput;
}

std::string
ScenarioStatistics::GetScenarioName()
{
    return m_scenarioName;
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

uint32_t
ScenarioStatistics::GetTotalRxPackets() const
{
    return m_rxpackets;
}

uint32_t
ScenarioStatistics::GetTotalTxPackets() const
{
    return m_txpackets;
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
ScenarioStatistics::SNRCallback(std::string context,
                                Ptr<const Packet> p,
                                uint16_t channelFreqMhz,
                                WifiTxVector txVector,
                                MpduInfo aMpdu,
                                SignalNoiseDbm signalNoise,
                                uint16_t staId)
{
    m_snrSum += signalNoise.signal - signalNoise.noise;
    m_rxpackets++;
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
RunScenario(std::string scenario,
            std::string wifiManager,
            uint16_t runNumber = 1,
            Vector irsPosition = Vector(0, 0, 0),
            std::string lookuptable = "")
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

    if (scenario != "LOS")
    {
        IrsHelper irsHelper;
        irsHelper.SetDirection(Vector(1, 0, 0));
        irsHelper.SetLookupTable(lookuptable);
        irsHelper.Install(irsNode);

        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        irsLossModel->SetFrequency(5.21e9);
        wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                       "IrsNodes",
                                       PointerValue(&irsNode),
                                       "LossModel",
                                       PointerValue(irsLossModel),
                                       "Frequency",
                                       DoubleValue(5.21e9));
    }
    if (scenario != "IRS")
    {
        wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
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

    // Mobility
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(40.0, 0.0, 0.0));
    positionAlloc->Add(irsPosition);
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(wifiStaNode);
    mobility.Install(irsNode);

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
    onoff.SetConstantRate(DataRate("400Mbps"), 1200);
    ApplicationContainer sourceApp = onoff.Install(wifiApNode.Get(0));

    sinkApp.Start(Seconds(0));
    sourceApp.Start(Seconds(1));

    // Statistics
    ScenarioStatistics stats(scenario);
    // STA: total recieved bytes
    Config::Connect("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx",
                    MakeCallback(&ScenarioStatistics::RxCallback, &stats));
    // STA: mean SNR + count recieved packages at phy level
    Config::Connect(
        "/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx",
        MakeCallback(&ScenarioStatistics::SNRCallback, &stats));
    // AP: DataRate
    Config::Connect("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$" +
                        wifiManager + "/Rate",
                    MakeCallback(&ScenarioStatistics::RateCallback, &stats));
    // AP: count sent packages at phy level
    Config::Connect(
        "/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferTx",
        MakeCallback(&ScenarioStatistics::TxCallback, &stats));

    // Run simulation
    Simulator::Stop(Seconds(10));
    Simulator::Run();

    std::cout << std::fixed << std::setprecision(2) << "Scenario: " << stats.GetScenarioName()
              << ", Throughput: " << stats.GetThroughput() << " Mbps" << ", SNR: " << stats.GetSNR()
              << " dBm" << ", Data Rate: " << stats.GetDataRate() << " Mbps"
              << ", Success Rate:" << stats.GetSuccessRate()
              << "%, Transmitted Packets:" << stats.GetTotalTxPackets()
              << ", Recieved Packets:" << stats.GetTotalRxPackets() << std::endl;

    Simulator::Destroy();
}

int
main(int argc, char* argv[])
{
    std::string scenario = "LOS";
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
        ns3::LogComponentEnable("IrsValidation", ns3::LOG_LEVEL_ALL);
    }

    if (scenario == "LOS")
    {
        // Scenario 1: only LOS
        RunScenario("LOS", wifiManager, runNumber);
    }
    else if (scenario == "IRS")
    {
        // Scenario 2: IRS Constructive
        Vector irs(1.35, -1.35, 0);
        RunScenario("IRS",
                    wifiManager,
                    runNumber,
                    irs,
                    "contrib/examples/lookuptables/IRS_400_IN135_OUT2_FREQ5.21GHz_constructive.csv");
    }
    else if (scenario == "IrsConstructive")
    {
        // Scenario 3: IRS Constructive
        Vector irs(1.35, -1.35, 0);
        RunScenario("IRS Constructive",
                    wifiManager,
                    runNumber,
                    irs,
                    "contrib/examples/lookuptables/IRS_400_IN135_OUT2_FREQ5.21GHz_constructive.csv");
    }
    else if (scenario == "IrsDestructive")
    {
        // // Scenario 4: IRS Destructive
        Vector irs(20.5186, -7.6093, 0);
        RunScenario("IRS Destructive",
                    wifiManager,
                    runNumber,
                    irs,
                    "contrib/examples/lookuptables/IRS_400_IN160_OUT21_FREQ5.21GHz_destructive.csv");
    }
    return 0;
}
