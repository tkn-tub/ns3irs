/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/irs-lookup-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/spectrum-module.h"
#include <ns3/buildings-helper.h>

using namespace ns3;

void
RunScenario(std::string scenario)
{
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // Create Nodes: eNodeB and UE
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    NodeContainer irsNodes;
    enbNodes.Create(1);
    ueNodes.Create(1);
    irsNodes.Create(1);

    IrsLookupHelper irsHelper;
    irsHelper.SetDirection(Vector(1, 1, 0));
    irsHelper.SetLookupTable(
        "contrib/irs/examples/lookuptables/IRS_400_IN153_OUT27_FREQ1.50GHz_rem.csv");
    irsHelper.Install(irsNodes);

    if (scenario == "LOS")
    {
        lteHelper->SetAttribute("PathlossModel",
                                StringValue("ns3::LogDistancePropagationLossModel"));
        lteHelper->SetPathlossModelAttribute("ReferenceLoss", DoubleValue(35.9696));
    }
    else if (scenario == "IRS")
    {
        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        irsLossModel->SetFrequency(1.5e9);
        lteHelper->SetAttribute("PathlossModel", StringValue("ns3::IrsPropagationLossModel"));
        lteHelper->SetPathlossModelAttribute("IrsNodes", PointerValue(&irsNodes));
        lteHelper->SetPathlossModelAttribute("IrsLossModel", PointerValue(irsLossModel));
        lteHelper->SetPathlossModelAttribute("Frequency", DoubleValue(5.21e9));
    }
    else if (scenario == "IRS+LOS")
    {
        Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
        irsLossModel->SetFrequency(1.5e9);
        Ptr<LogDistancePropagationLossModel> losLossModel =
            CreateObject<LogDistancePropagationLossModel>();
        losLossModel->SetReference(1.0, 35.9696);
        losLossModel->SetPathLossExponent(3);
        lteHelper->SetAttribute("PathlossModel", StringValue("ns3::IrsPropagationLossModel"));
        lteHelper->SetPathlossModelAttribute("IrsNodes", PointerValue(&irsNodes));
        lteHelper->SetPathlossModelAttribute("IrsLossModel", PointerValue(irsLossModel));
        lteHelper->SetPathlossModelAttribute("LosLossModel", PointerValue(losLossModel));
        lteHelper->SetPathlossModelAttribute("Frequency", DoubleValue(5.21e9));
    }
    // Install Mobility Model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));  // Position for eNodeB
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));  // Position for UE
    positionAlloc->Add(Vector(0.0, -2.0, 0.0)); // Position for IRS

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
    remHelper->SetAttribute("OutputFile", StringValue("rem_" + scenario + ".out"));
    remHelper->SetAttribute("XMin", DoubleValue(-15.0));
    remHelper->SetAttribute("XMax", DoubleValue(25.0));
    remHelper->SetAttribute("YMin", DoubleValue(-15.0));
    remHelper->SetAttribute("YMax", DoubleValue(25.0));
    remHelper->SetAttribute("XRes", UintegerValue(1000));
    remHelper->SetAttribute("YRes", UintegerValue(1000));
    remHelper->SetAttribute("Z", DoubleValue(0.0));
    remHelper->Install();

    Simulator::Run();

    Simulator::Destroy();
}

int
main(int argc, char* argv[])
{
    std::string scenario = "LOS";

    CommandLine cmd(__FILE__);
    cmd.AddValue("scenario", "Scenario to run: LOS, IRS, IRS+LOS", scenario);

    cmd.Parse(argc, argv);

    if (scenario == "LOS")
    {
        RunScenario("LOS");
    }
    else if (scenario == "IRS")
    {
        RunScenario("IRS");
    }
    else if (scenario == "IRS+LOS")
    {
        RunScenario("IRS+LOS");
    }
    return 0;
}
