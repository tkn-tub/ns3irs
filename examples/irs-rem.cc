#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/irs-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/spectrum-module.h"
#include <ns3/buildings-helper.h>

using namespace ns3;

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults();

    // Parse again so you can override default values from the command line
    cmd.Parse(argc, argv);

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // ns3::LogComponentEnable("IrsPropagationLossModel", ns3::LOG_LEVEL_ALL);
    // Create Nodes: eNodeB and UE
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    NodeContainer irsNodes;
    enbNodes.Create(1);
    ueNodes.Create(1);
    irsNodes.Create(1);

    IrsHelper irsHelper;
    irsHelper.SetDirection(Vector(0, 1, 0));
    irsHelper.SetLookupTable(
        "contrib/irs/examples/lookuptables/IRS_400_IN153_OUT27_FREQ1.50GHz_rem.csv");
    irsHelper.Install(irsNodes);

    Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
    irsLossModel->SetFrequency(1.5e9);
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::IrsPropagationLossModel"));
    lteHelper->SetPathlossModelAttribute("IrsNodes", PointerValue(&irsNodes));
    lteHelper->SetPathlossModelAttribute("LossModel", PointerValue(irsLossModel));
    lteHelper->SetPathlossModelAttribute("Frequency", DoubleValue(5.21e9));

    // Install Mobility Model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));    // Position for eNodeB
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));    // Position for UE
    positionAlloc->Add(Vector(5, -10, 0.0)); // Position for UE

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    mobility.Install(ueNodes);
    mobility.Install(irsNodes);

    BuildingsHelper::Install(enbNodes);
    BuildingsHelper::Install(ueNodes);
    BuildingsHelper::Install(irsNodes);

    // Create Devices and install them in the Nodes (eNB and UE)
    NetDeviceContainer enbDevs;
    NetDeviceContainer ueDevs;

    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    ueDevs = lteHelper->InstallUeDevice(ueNodes);

    // Attach a UE to a eNB
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    // Activate an EPS bearer
    EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // Configure Radio Environment Map (REM) output
    // for LTE-only simulations always use /ChannelList/0 which is the downlink channel
    Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
    remHelper->SetAttribute("ChannelPath", StringValue("/ChannelList/0"));
    remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
    remHelper->SetAttribute("XMin", DoubleValue(-300.0));
    remHelper->SetAttribute("XMax", DoubleValue(300.0));
    remHelper->SetAttribute("YMin", DoubleValue(-50.0));
    remHelper->SetAttribute("YMax", DoubleValue(300.0));
    remHelper->SetAttribute("XRes", UintegerValue(800));
    remHelper->SetAttribute("YRes", UintegerValue(600));
    remHelper->SetAttribute("Z", DoubleValue(0.0));
    remHelper->Install();

    Simulator::Run();

    Simulator::Destroy();
    return 0;
}
