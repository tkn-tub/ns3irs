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
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <iomanip>
#include <ostream>
#include <string>

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
    void RateCallback(std::string path, uint64_t oldRate, uint64_t newRate);
    void SNRCallback(std::string context,
                     Ptr<const Packet> p,
                     uint16_t channelFreqMhz,
                     WifiTxVector txVector,
                     MpduInfo aMpdu,
                     SignalNoiseDbm signalNoise,
                     uint16_t staId);

  private:
    std::string m_scenarioName;
    uint32_t m_bytesTotal;
    double m_throughput;
    double m_dataRate;
    double m_snr;
};

ScenarioStatistics::ScenarioStatistics(std::string scenarioName)
    : m_scenarioName(scenarioName),
      m_bytesTotal(0),
      m_throughput(0),
      m_dataRate(0),
      m_snr(0)
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
ScenarioStatistics::GetSNR() const
{
    return m_snr;
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
    m_snr = (m_snr + signalNoise.signal - signalNoise.noise) / 2;
}

void
RunScenario(Vector irsPosition,
            std::string scenario,
            std::string wifiManager,
            std::string lookuptable)
{
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
    onoff.SetConstantRate(DataRate("400Mbps"), 1500);
    ApplicationContainer sourceApp = onoff.Install(wifiApNode.Get(0));

    sinkApp.Start(Seconds(0));
    sourceApp.Start(Seconds(1));

    // Statistics
    ScenarioStatistics stats(scenario);
    Config::Connect("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx",
                    MakeCallback(&ScenarioStatistics::RxCallback, &stats));
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/$" +
                        wifiManager + "/Rate",
                    MakeCallback(&ScenarioStatistics::RateCallback, &stats));
    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx",
        MakeCallback(&ScenarioStatistics::SNRCallback, &stats));

    // Run simulation
    Simulator::Stop(Seconds(10));
    Simulator::Run();

    std::cout << std::fixed << std::setprecision(1) << "Scenario: " << stats.GetScenarioName()
              << ", Throughput: " << stats.GetThroughput() << " Mbps" << ", SNR: " << stats.GetSNR()
              << " dB" << ", Data Rate: " << stats.GetDataRate() << " Mbps" << std::endl;

    Simulator::Destroy();
}

int
main(int argc, char* argv[])
{
    // ns3::LogComponentEnable("IrsValidation", ns3::LOG_LEVEL_ALL);
    // ns3::LogComponentEnable("IrsPropagationLossModel", ns3::LOG_LEVEL_ALL);

    std::string wifiManager = "ns3::MinstrelHtWifiManager";

    // Scenario 1: only LOS
    Vector irs(0, 0, 0);
    RunScenario(irs, "LOS", wifiManager, "");

    // Scenario 2: IRS Constructive
    Vector irs2(1.35, -1.35, 0);
    RunScenario(
        irs2,
        "IRS",
        wifiManager,
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT2_FREQ5.21GHz_constructive.csv");

    // Scenario 3: IRS Constructive
    RunScenario(
        irs2,
        "IRS Constructive",
        wifiManager,
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT2_FREQ5.21GHz_constructive.csv");

    // Scenario 4: IRS Destructive
    Vector irs3(20, -7.6141, 0);
    RunScenario(
        irs3,
        "IRS Destructive",
        wifiManager,
        "contrib/irs/examples/lookuptables/IRS_400_IN159_OUT21_FREQ5.21GHz_destructive.csv");

    return 0;
}
