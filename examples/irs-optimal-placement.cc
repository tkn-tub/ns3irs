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
#include "ns3/command-line.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/double.h"
#include "ns3/irs-lookup-helper.h"
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

std::vector<std::vector<SimulationResult>> allSimulationResults;

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
writeResultsToCSV(const std::string& filename, const std::vector<SimulationResult>& averageResults)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write header
    file << "irs_x,only_irs,irs_los\n";

    // Write data
    for (const auto& result : averageResults)
    {
        file << result.irs_x << "," << result.only_irs << "," << result.irs_los << "\n";
    }

    file.close();
}

std::vector<SimulationResult>
calculateAverageResults(const std::vector<std::vector<SimulationResult>>& allResults)
{
    std::vector<SimulationResult> averageResults;

    if (allResults.empty())
    {
        return averageResults;
    }

    size_t numPositions = allResults[0].size();
    size_t numSimulations = allResults.size();

    for (size_t i = 0; i < numPositions; ++i)
    {
        SimulationResult avgResult;
        avgResult.irs_x = allResults[0][i].irs_x;

        double sum_only_irs = 0.0;
        double sum_irs_los = 0.0;

        for (const auto& simulation : allResults)
        {
            sum_only_irs += simulation[i].only_irs;
            sum_irs_los += simulation[i].irs_los;
        }

        avgResult.only_irs = sum_only_irs / numSimulations;
        avgResult.irs_los = sum_irs_los / numSimulations;

        averageResults.push_back(avgResult);
    }

    return averageResults;
}

int
main(int argc, char* argv[])
{
    uint16_t n = 1;
    double frequency = 5.21e9;
    double txPowerDbm = 17;

    CommandLine cmd(__FILE__);
    cmd.AddValue("freq", "Frequency the nodes are communicating on", frequency);
    cmd.AddValue("txPower", "Power the transmitter is sending with. (in dBm)", txPowerDbm);
    cmd.AddValue("n", "Amount of times to run the simulation.", n);

    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(2024);

    NodeContainer irsNode;
    irsNode.Create(1);

    IrsLookupHelper irsHelper;
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

    for (int sim = 0; sim < n; ++sim)
    {
        std::vector<SimulationResult> simulationResults;
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
            lossModel->SetIrsPropagationModel(irsLossModel);

            SimulationResult result;

            result.irs_x = pos_irs;
            result.only_irs = lossModel->CalcRxPower(txPowerDbm, a, b);

            lossModel->SetLosPropagationModel(irsLossModel);
            result.irs_los = lossModel->CalcRxPower(txPowerDbm, a, b);
            simulationResults.push_back(result);

            i++;
        }
        allSimulationResults.push_back(simulationResults);
    }

    std::vector<SimulationResult> averageResults = calculateAverageResults(allSimulationResults);
    writeResultsToCSV("optimal_ris_placement_ns3.csv", averageResults);
}
