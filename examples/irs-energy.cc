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
#include "ns3/energy-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/irs-lookup-helper.h"
#include "ns3/irs-propagation-loss-model.h"
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
#include "ns3/wifi-radio-energy-model-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/types.h>

using namespace ns3;
using namespace ns3::energy;

NS_LOG_COMPONENT_DEFINE("IrsValidation");

class ScenarioStatistics
{
  public:
    ScenarioStatistics(std::string scenarioName, double interval = 1.0);
    double GetThroughput() const;
    std::string GetScenarioName() const;
    double GetDataRate() const;
    double GetSNR() const;
    double GetSuccessRate() const;
    uint32_t GetTotalRxPackets() const;
    uint32_t GetTotalTxPackets() const;
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
    void SetEnergySources(Ptr<EnergySource> apSource, Ptr<EnergySource> staSource);
    void PeriodicUpdate();
    void SaveDataPoint();

  private:
    std::string m_scenarioName;
    uint64_t m_bytesTotal;
    double m_throughput;
    double m_dataRate;
    uint64_t m_rxpackets;
    uint64_t m_txpackets;
    uint64_t m_snrSum;
    Ptr<EnergySource> m_apEnergySource;
    Ptr<EnergySource> m_staEnergySource;
    std::ofstream m_outputFile;
    double m_interval;
    Time m_startTime;
};

ScenarioStatistics::ScenarioStatistics(std::string scenarioName, double interval)
    : m_scenarioName(scenarioName),
      m_bytesTotal(0),
      m_dataRate(0),
      m_rxpackets(0),
      m_txpackets(0),
      m_snrSum(0),
      m_interval(interval),
      m_startTime(Simulator::Now())
{
    std::string filename = "/scratch/ruehlow/" + scenarioName + "_data.csv";
    m_outputFile.open(filename.c_str(), std::ios::out | std::ios::trunc);
    m_outputFile << "Time,Throughput,DataRate,SNR,SuccessRate,APEnergyLevel,STAEnergyLevel,"
                    "TotalTxPackets,TotalRxPackets"
                 << std::endl;

    Simulator::Schedule(Seconds(m_interval), &ScenarioStatistics::PeriodicUpdate, this);
}

void
ScenarioStatistics::SetEnergySources(Ptr<EnergySource> apSource, Ptr<EnergySource> staSource)
{
    m_apEnergySource = apSource;
    m_staEnergySource = staSource;
}

void
ScenarioStatistics::SaveDataPoint()
{
    double time = Simulator::Now().GetSeconds();
    double throughput = GetThroughput();
    double dataRate = GetDataRate();
    double snr = GetSNR();
    double successRate = GetSuccessRate();
    double apEnergyLevel = m_apEnergySource ? m_apEnergySource->GetRemainingEnergy() : 0.0;
    double staEnergyLevel = m_staEnergySource ? m_staEnergySource->GetRemainingEnergy() : 0.0;

    m_outputFile << std::fixed << std::setprecision(4) << time << "," << throughput << ","
                 << dataRate << "," << snr << "," << successRate << "," << apEnergyLevel << ","
                 << staEnergyLevel << "," << m_txpackets << "," << m_rxpackets << std::endl;
}

void
ScenarioStatistics::PeriodicUpdate()
{
    SaveDataPoint();
    Simulator::Schedule(Seconds(m_interval), &ScenarioStatistics::PeriodicUpdate, this);
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
ScenarioStatistics::GetThroughput() const
{
    Time now = Simulator::Now();
    double totalTime = (now - m_startTime).GetSeconds();
    if (totalTime > 0)
    {
        return (m_bytesTotal * 8.0) / (totalTime * 1e6); // Mbps
    }
    return 0.0;
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
ScenarioStatistics::RxCallback(std::string context,
                               Ptr<const Packet> p,
                               uint16_t channelFreqMhz,
                               WifiTxVector txVector,
                               MpduInfo aMpdu,
                               SignalNoiseDbm signalNoise,
                               uint16_t staId)
{
    m_snrSum += signalNoise.signal - signalNoise.noise;
    m_bytesTotal += p->GetSize();
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
        IrsLookupHelper irsHelper;
        irsHelper.SetDirection(Vector(0, 1, 0));
        irsHelper.SetLookupTable(lookuptable);
        irsHelper.Install(irsNode);

        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        irsLossModel->SetFrequency(5.21e9);

        if (scenario == "IRS+LOS")
        {
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
        }
        else
        {
            wifiChannel.AddPropagationLoss("ns3::IrsPropagationLossModel",
                                           "IrsNodes",
                                           PointerValue(&irsNode),
                                           "IrsLossModel",
                                           PointerValue(irsLossModel),
                                           "Frequency",
                                           DoubleValue(5.21e9));
        }
    }
    if (scenario == "LOS")
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

    // Use a preset PANASONIC Li-Ion batteries arranged in a cell pack (2 series, 2 parallel)
    GenericBatteryModelHelper batteryHelper;
    EnergySourceContainer energySourceContainerAp =
        batteryHelper.Install(wifiApNode, PANASONIC_CGR18650DA_LION);
    EnergySourceContainer energySourceContainerSta =
        batteryHelper.Install(wifiStaNode, PANASONIC_CGR18650DA_LION);
    batteryHelper.SetCellPack(energySourceContainerAp, 2, 2);
    batteryHelper.SetCellPack(energySourceContainerSta, 2, 2);

    Ptr<GenericBatteryModel> batteryAp =
        DynamicCast<GenericBatteryModel>(energySourceContainerAp.Get(0));
    Ptr<GenericBatteryModel> batterySta =
        DynamicCast<GenericBatteryModel>(energySourceContainerSta.Get(0));
    // Energy consumption quantities have been exaggerated for
    // demonstration purposes, real consumption values are much smaller.
    WifiRadioEnergyModelHelper radioEnergyHelper;

    DeviceEnergyModelContainer deviceModelsAp =
        radioEnergyHelper.Install(apDevice, energySourceContainerAp);

    DeviceEnergyModelContainer deviceModelsSta =
        radioEnergyHelper.Install(staDevice, energySourceContainerSta);

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
    onoff.SetConstantRate(DataRate("15Mbps"), 1200);
    ApplicationContainer sourceApp = onoff.Install(wifiApNode.Get(0));

    sinkApp.Start(Seconds(0.5));
    sourceApp.Start(Seconds(1.0));

    // Statistics
    ScenarioStatistics stats(scenario, 10.0);
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

    // Set energy sources
    stats.SetEnergySources(DynamicCast<EnergySource>(energySourceContainerAp.Get(0)),
                           DynamicCast<EnergySource>(energySourceContainerSta.Get(0)));

    // Run simulation
    Simulator::Stop(Hours(3));
    Simulator::Run();

    stats.SaveDataPoint();

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
        RunScenario(
            "IRS",
            wifiManager,
            runNumber,
            irs,
            "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv");
    }
    else if (scenario == "IRS+LOS")
    {
        // Scenario 3: IRS Constructive
        Vector irs(1.35, -1.35, 0);
        RunScenario(
            "IRS+LOS",
            wifiManager,
            runNumber,
            irs,
            "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv");
    }
    return 0;
}
