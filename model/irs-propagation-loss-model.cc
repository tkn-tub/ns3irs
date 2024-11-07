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

#include "irs-propagation-loss-model.h"

#include "irs-model.h"

#include "ns3/angles.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/irs-lookup-table.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/object-base.h"
#include "ns3/object.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stddef.h>
#include <utility>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IrsPropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED(IrsPropagationLossModel);

TypeId
IrsPropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsPropagationLossModel")
            .SetParent<PropagationLossModel>()
            .SetGroupName("Propagation")
            .AddConstructor<IrsPropagationLossModel>()
            .AddAttribute("IrsNodes",
                          "The IRS nodes in the network.",
                          PointerValue(),
                          MakePointerAccessor(&IrsPropagationLossModel::SetIrsNodes,
                                              &IrsPropagationLossModel::GetIrsNodes),
                          MakePointerChecker<NodeContainer>())
            .AddAttribute("IrsLossModel",
                          "The propagation loss model for the path over the IRS.",
                          PointerValue(),
                          MakePointerAccessor(&IrsPropagationLossModel::SetIrsPropagationModel,
                                              &IrsPropagationLossModel::GetIrsPropagatioModel),
                          MakePointerChecker<PropagationLossModel>())
            .AddAttribute("LosLossModel",
                          "The propagation loss model for the line-of-sight path.",
                          PointerValue(),
                          MakePointerAccessor(&IrsPropagationLossModel::SetLosPropagationModel,
                                              &IrsPropagationLossModel::GetLosPropagatioModel),
                          MakePointerChecker<PropagationLossModel>())
            .AddAttribute(
                "Frequency",
                "The carrier frequency (in Hz) at which propagation occurs (default is 5.21 GHz).",
                DoubleValue(5.21e9),
                MakeDoubleAccessor(&IrsPropagationLossModel::SetFrequency,
                                   &IrsPropagationLossModel::GetFrequency),
                MakeDoubleChecker<double>());
    return tid;
}

IrsPropagationLossModel::IrsPropagationLossModel()
    : PropagationLossModel()
{
    m_rng = CreateObject<NormalRandomVariable>();
    m_rng->SetAttribute("Mean", DoubleValue(0.0));
    m_rng->SetAttribute("Variance", DoubleValue(0.1));
}

IrsPropagationLossModel::~IrsPropagationLossModel()
{
}

// from FriisPropagationLossModel
void
IrsPropagationLossModel::SetFrequency(double frequency)
{
    NS_ASSERT_MSG(frequency > 0, "Frequency should be greater zero (in Hz)");
    m_frequency = frequency;
    static const double c = 299792458.0; // speed of light in vacuum
    m_lambda = c / frequency;
}

double
IrsPropagationLossModel::GetFrequency() const
{
    return m_frequency;
}

void
IrsPropagationLossModel::SetIrsNodes(Ptr<NodeContainer> nodes)
{
    NS_ASSERT_MSG(nodes && nodes->GetN() > 0, "IRS nodes are not set or the container is empty");
    m_irsNodes = nodes;
    CalcIrsPaths();
}

Ptr<NodeContainer>
IrsPropagationLossModel::GetIrsNodes() const
{
    return m_irsNodes;
}

void
IrsPropagationLossModel::SetIrsPropagationModel(Ptr<PropagationLossModel> model)
{
    NS_ASSERT_MSG(model, "Cannot set a null propagation model");
    m_irsLossModel = model;
}

Ptr<PropagationLossModel>
IrsPropagationLossModel::GetIrsPropagatioModel() const
{
    return m_irsLossModel;
}

void
IrsPropagationLossModel::SetLosPropagationModel(Ptr<PropagationLossModel> model)
{
    NS_ASSERT_MSG(model, "Cannot set a null propagation model");
    m_losLossModel = model;
}

Ptr<PropagationLossModel>
IrsPropagationLossModel::GetLosPropagatioModel() const
{
    return m_losLossModel;
}

// from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmToW(double dbm) const
{
    double mw = std::pow(10.0, dbm / 10.0);
    return mw / 1000.0;
}

// from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmFromW(double w) const
{
    double dbm = std::log10(w * 1000.0) * 10.0;
    return dbm;
}

std::ostream&
operator<<(std::ostream& os, const std::vector<IrsPath>& paths)
{
    os << "[ ";
    for (size_t i = 0; i < paths.size(); ++i)
    {
        os << "[";
        for (size_t j = 0; j < paths[i].size(); ++j)
        {
            os << paths[i][j]->GetId();
            if (j < paths[i].size() - 1)
            {
                os << " -> ";
            }
        }
        os << "] ";
    }
    os << "]";
    return os;
}

std::pair<double, double>
IrsPropagationLossModel::CalcAngles(ns3::Vector a,
                                    ns3::Vector b,
                                    ns3::Vector irs,
                                    ns3::Vector irsNormal) const
{
    // Check if both a and b are on the correct side of the IRS (the side the normal vector points
    // to)
    double dotProductA =
        irsNormal.x * (a.x - irs.x) + irsNormal.y * (a.y - irs.y) + irsNormal.z * (a.z - irs.z);
    double dotProductB =
        irsNormal.x * (b.x - irs.x) + irsNormal.y * (b.y - irs.y) + irsNormal.z * (b.z - irs.z);

    // If either dot product is negative, the corresponding point is on the wrong side of the IRS
    if (dotProductA < std::numeric_limits<double>::epsilon() ||
        dotProductB < std::numeric_limits<double>::epsilon())
    {
        return std::make_pair(-1, -1);
    }

    // Vector from IRS to a and b
    ns3::Vector AI = irs - a;
    ns3::Vector IB = b - irs;

    double IAnorm = AI.GetLength();
    double IBnorm = IB.GetLength();

    // avoid division by zero
    if (IAnorm == 0 || IBnorm == 0)
    {
        return std::make_pair(-1, -1);
    }

    // Calculate the angle of incidence
    double cosThetaInc = (AI * irsNormal) / AI.GetLength();
    double thetaInc = std::acos(std::clamp(cosThetaInc, -1.0, 1.0));
    double thetaIncDeg = thetaInc * (180.0 / M_PI);

    // Calculate the angle of reflection
    double cosThetaRef = (IB * irsNormal) / IB.GetLength();
    double thetaRef = std::acos(std::clamp(cosThetaRef, -1.0, 1.0));
    double thetaRefDeg = thetaRef * (180.0 / M_PI);

    // Return both angles as a pair
    return std::make_pair(thetaIncDeg, thetaRefDeg);
}

void
IrsPropagationLossModel::CalcIrsPaths()
{
    m_irsPaths.clear();

    // Helper function to check if two IRS are facing each other
    auto facingEachOther = [](Ptr<Node> irs1, Ptr<Node> irs2) {
        auto mobility1 = irs1->GetObject<MobilityModel>();
        auto mobility2 = irs2->GetObject<MobilityModel>();
        auto irsObj1 = irs1->GetObject<IrsModel>();
        auto irsObj2 = irs2->GetObject<IrsModel>();

        NS_ASSERT_MSG(mobility1 && mobility2, "Mobility model not set for IRS node");
        NS_ASSERT_MSG(irsObj1 && irsObj2, "IRS object not set for node");

        Vector pos1 = mobility1->GetPosition();
        Vector pos2 = mobility2->GetPosition();

        Vector vec12 = pos2 - pos1;
        Vector vec21 = pos1 - pos2;

        // Check if the dot products are positive (vectors are pointing in the same general
        // direction)
        return (irsObj1->GetDirection() * vec12 > std::numeric_limits<double>::epsilon()) &&
               (irsObj2->GetDirection() * vec21 > std::numeric_limits<double>::epsilon());
    };

    // Generate all possible paths considering order
    for (uint32_t pathLength = 1; pathLength <= m_irsNodes->GetN(); ++pathLength)
    {
        std::vector<bool> v(m_irsNodes->GetN());
        std::fill(v.end() - pathLength, v.end(), true);

        do
        {
            std::vector<Ptr<Node>> currentPath;
            for (uint32_t i = 0; i < m_irsNodes->GetN(); ++i)
            {
                if (v[i])
                {
                    currentPath.push_back(m_irsNodes->Get(i));
                }
            }

            // Generate all permutations of the current path
            do
            {
                bool validPath = true;

                // Check if IRSs are facing each other for paths longer than 1
                if (currentPath.size() > 1)
                {
                    for (size_t i = 0; i < currentPath.size() - 1; ++i)
                    {
                        if (!facingEachOther(currentPath[i], currentPath[i + 1]))
                        {
                            validPath = false;
                            break;
                        }
                    }
                }

                if (validPath)
                {
                    m_irsPaths.push_back(currentPath);
                }
            } while (std::next_permutation(currentPath.begin(), currentPath.end()));

        } while (std::next_permutation(v.begin(), v.end()));
    }

    NS_LOG_DEBUG("Generated " << m_irsPaths.size() << " possible IRS path(s): " << m_irsPaths);
}

std::complex<double>
IrsPropagationLossModel::CalcPath(const IrsPath& path,
                                  double txPowerDbm,
                                  Ptr<MobilityModel> source,
                                  Ptr<MobilityModel> destination) const
{
    double pathLoss = txPowerDbm;
    double totalDistance = 0.0;
    double totalPhaseShift = 0.0;

    NS_LOG_DEBUG("-- new path --");

    for (auto curr = path.begin(); curr != path.end(); ++curr)
    {
        Ptr<MobilityModel> prev =
            (curr != path.begin()) ? (*(curr - 1))->GetObject<MobilityModel>() : source;
        Ptr<MobilityModel> next =
            (curr + 1 != path.end()) ? (*(curr + 1))->GetObject<MobilityModel>() : destination;
        Ptr<Node> irs = *curr;

        // Calculate angles
        std::pair<double, double> angles =
            CalcAngles(prev->GetPosition(),
                       next->GetPosition(),
                       irs->GetObject<MobilityModel>()->GetPosition(),
                       irs->GetObject<IrsModel>()->GetDirection());

        // Skip if IRS is not in line of sight
        if (angles.first < 0 || angles.second < 0)
        {
            return std::complex<double>(0.0, 0.0);
        }

        // Get IRS impact from lookuptable
        IrsEntry modifier = irs->GetObject<IrsModel>()->GetIrsEntry(std::round(angles.first),
                                                                    std::round(angles.second));
        NS_LOG_INFO("IRS Gain (dBm): " << modifier.gain
                                       << " | IRS phase shift (radians): " << modifier.phase_shift);
        // add path lenght and phase shift
        totalDistance += prev->GetDistanceFrom(irs->GetObject<MobilityModel>());
        totalPhaseShift += modifier.phase_shift;
        // calulate pathloss
        pathLoss = m_irsLossModel->CalcRxPower(pathLoss, prev, irs->GetObject<MobilityModel>());
        pathLoss += modifier.gain + m_rng->GetValue();
    }
    totalDistance += path.back()->GetObject<MobilityModel>()->GetDistanceFrom(destination);
    pathLoss =
        m_irsLossModel->CalcRxPower(pathLoss, path.back()->GetObject<MobilityModel>(), destination);
    // Calculate phase for the entire path
    double theta = WrapToPi(((2 * M_PI * totalDistance) / m_lambda) + totalPhaseShift);
    std::complex<double> phase_path(0.0, theta);

    NS_LOG_DEBUG("IRS - Node(s): " << path.size() << ", Distance: " << totalDistance << "m"
                                   << ", Path Loss: " << pathLoss << "dBm" << ", Phase: " << theta);

    return std::sqrt(DbmToW(pathLoss)) * std::exp(phase_path);
}

double
IrsPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    NS_ASSERT_MSG(m_irsLossModel, "IRS path loss model is not set");

    NS_LOG_DEBUG("--------- IRS Propagation Loss Model Debug Info ---------");
    NS_LOG_DEBUG("m_frequency (Hz): " << m_frequency);
    NS_LOG_DEBUG("TX Power (dBm): " << txPowerDbm);
    NS_LOG_DEBUG("TX Position: " << a->GetPosition());
    NS_LOG_DEBUG("RX Position: " << b->GetPosition());

    std::complex<double> totalSignal(0.0, 0.0);

    // Calculate contribution from each precomputed path
    for (const auto& path : m_irsPaths)
    {
        totalSignal += CalcPath(path, txPowerDbm, a, b);
    }

    // Add LOS/NLOS path contribution
    if (m_losLossModel)
    {
        double pl_direct = m_losLossModel->CalcRxPower(txPowerDbm, a, b);
        double distance = a->GetDistanceFrom(b);
        double theta = WrapToPi((2 * M_PI * distance) / m_lambda);
        std::complex<double> phase_direct(0.0, theta);
        std::complex<double> los_contribution =
            std::sqrt(DbmToW(pl_direct)) * std::exp(phase_direct);
        totalSignal += los_contribution;

        NS_LOG_DEBUG("LOS Path - Distance: " << distance << "m, Path Loss: " << pl_direct
                                             << "dBm, Phase: " << theta
                                             << ", Contribution: " << std::abs(los_contribution));
    }
    else
    {
        NS_LOG_DEBUG("No N/LOS propagation model specified. Calculating only IRS path.");
    }
    // Calculate final received power
    double rxPower = DbmFromW(std::pow(std::abs(totalSignal), 2));
    NS_LOG_DEBUG("Resulting RX Power (dBm): " << rxPower);

    return rxPower;
}

int64_t
IrsPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
