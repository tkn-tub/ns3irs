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

#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/double.h"
#include "ns3/irs-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include <cmath>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsOptimalRisPlacement");

struct SimulationResult
{
    double irs_x;
    double only_irs;
    double irs_los;
};

std::vector<SimulationResult> simulationResults;

const std::string irs_lookup_table[31] = {
    "IRS_400_IN180_OUT86_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN153_OUT86_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN135_OUT86_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN124_OUT86_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN117_OUT86_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN112_OUT85_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN108_OUT85_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN106_OUT85_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN104_OUT85_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN103_OUT85_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN101_OUT84_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN100_OUT84_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN99_OUT84_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN99_OUT83_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN98_OUT83_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN98_OUT82_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN97_OUT82_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN97_OUT81_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN96_OUT81_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN96_OUT80_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN96_OUT79_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN95_OUT77_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN95_OUT76_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN95_OUT74_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN95_OUT72_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN95_OUT68_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN94_OUT63_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN94_OUT56_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN94_OUT45_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN94_OUT27_FREQ5.21GHz_constructive_changeRisPos.csv",
    "IRS_400_IN94_OUT0_FREQ5.21GHz_constructive_changeRisPos.csv"};

void
writeResultsToCSV(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write header
    file << "ris_x,only_irs,irs_los\n";

    // Write data
    for (const auto& result : simulationResults)
    {
        file << result.irs_x << "," << result.only_irs << "," << result.irs_los << "\n";
    }

    file.close();
}

int
main(int argc, char* argv[])
{
    double frequency = 5.21e9;
    double txPowerDbm = 17;

    RngSeedManager::SetSeed(2024);

    NodeContainer irsNode;
    irsNode.Create(1);

    IrsHelper irsHelper;
    irsHelper.SetDirection(Vector(0, 1, 0));

    Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel>();
    a->SetPosition(Vector(0, 0, 0));
    Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel>();
    b->SetPosition(Vector(15, 0, 0));
    Ptr<MobilityModel> irs = CreateObject<ConstantPositionMobilityModel>();
    irsNode.Get(0)->AggregateObject(irs);

    Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
    irsLossModel->SetFrequency(frequency);
    irsLossModel->SetSystemLoss(1);

    int i = 0;
    for (double pos_irs = 0; pos_irs <= 15; pos_irs += 0.5)
    {
        irs->SetPosition(Vector(pos_irs, -1, 0));
        irsHelper.SetLookupTable("contrib/irs/examples/lookuptables/changeRisPos/" +
                                 irs_lookup_table[i]);
        irsHelper.Install(irsNode);

        Ptr<IrsPropagationLossModel> lossModel = CreateObject<IrsPropagationLossModel>();
        lossModel->SetFrequency(frequency);
        lossModel->SetIrsNodes(&irsNode);
        lossModel->SetPropagationModel(irsLossModel);

        SimulationResult result;

        result.irs_x = pos_irs;
        result.only_irs = lossModel->CalcRxPower(txPowerDbm, a, b);

        lossModel->SetNext(irsLossModel);
        result.irs_los = lossModel->CalcRxPower(txPowerDbm, a, b);
        simulationResults.push_back(result);

        i++;
    }

    writeResultsToCSV("contrib/irs/matlab/optimal_ris_placement_ns3.csv");
}
