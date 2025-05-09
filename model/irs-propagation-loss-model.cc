/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "irs-propagation-loss-model.h"

#include "irs-lookup-model.h"
#include "irs-model.h"
#include "irs-spectrum-model.h"

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
#include "ns3/tuple.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
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
                "ErrorModel",
                "Gaussian-distributed error applied to the IRS gain in dB. The first value "
                "specifies the mean, and the second specifies the variance.",
                TupleValue<DoubleValue, DoubleValue>({0, 0}),
                MakeTupleAccessor<DoubleValue, DoubleValue>(
                    &IrsPropagationLossModel::SetErrorModel,
                    &IrsPropagationLossModel::GetErrorModel),
                MakeTupleChecker<DoubleValue, DoubleValue>(MakeDoubleChecker<double>(),
                                                           MakeDoubleChecker<double>()))
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
}

IrsPropagationLossModel::~IrsPropagationLossModel()
{
}

void
IrsPropagationLossModel::DoInitialize()
{
    NS_ASSERT_MSG(!m_initialized, "DoInitialize should be only called once.");
    NS_ABORT_MSG_UNLESS(m_irsNodes, "IRS nodes not set.");
    NS_ABORT_MSG_UNLESS(m_irsLossModel, "IRS loss model not set.");
    CalcIrsPaths();
    m_initialized = true;
    PropagationLossModel::DoInitialize();
}

// from FriisPropagationLossModel
void
IrsPropagationLossModel::SetFrequency(double frequency)
{
    NS_ABORT_MSG_UNLESS(frequency > 0, "Frequency should be greater zero (in Hz)");
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
    NS_ABORT_MSG_UNLESS(nodes && nodes->GetN() > 0, "IRS container is null or container is empty.");
    for (uint32_t i = 0; i < nodes->GetN(); ++i)
    {
        NS_ABORT_MSG_UNLESS(nodes->Get(i)->GetObject<MobilityModel>(),
                            "Mobility model not set for IRS node.");
        NS_ABORT_MSG_UNLESS(nodes->Get(i)->GetObject<IrsModel>(),
                            "IRS object not set for IRS node.");
    }
    m_irsNodes = nodes;
    if (!m_initialized && m_irsLossModel)
    {
        DoInitialize();
    }
}

Ptr<NodeContainer>
IrsPropagationLossModel::GetIrsNodes() const
{
    return m_irsNodes;
}

void
IrsPropagationLossModel::SetIrsPropagationModel(Ptr<PropagationLossModel> model)
{
    NS_ABORT_MSG_UNLESS(model, "Provided IRS propagation model is null.");
    m_irsLossModel = model;
    if (!m_initialized && m_irsNodes)
    {
        DoInitialize();
    }
}

Ptr<PropagationLossModel>
IrsPropagationLossModel::GetIrsPropagatioModel() const
{
    return m_irsLossModel;
}

void
IrsPropagationLossModel::SetLosPropagationModel(Ptr<PropagationLossModel> model)
{
    NS_ABORT_MSG_UNLESS(model, "Provided LOS propagation model is null.");
    m_losLossModel = model;
}

Ptr<PropagationLossModel>
IrsPropagationLossModel::GetLosPropagatioModel() const
{
    return m_losLossModel;
}

void
IrsPropagationLossModel::SetErrorModel(std::tuple<double, double> values)
{
    m_rng = CreateObject<NormalRandomVariable>();
    m_rng->SetAttribute("Mean", DoubleValue(std::get<0>(values)));
    m_rng->SetAttribute("Variance", DoubleValue(std::get<0>(values)));
}

std::tuple<double, double>
IrsPropagationLossModel::GetErrorModel() const
{
    return {m_rng->GetMean(), m_rng->GetVariance()};
}

// from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmToW(double dbm)
{
    double mw = std::pow(10.0, dbm / 10.0);
    return mw / 1000.0;
}

// from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmFromW(double w)
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

std::optional<std::pair<double, double>>
IrsPropagationLossModel::CalcAngles(ns3::Vector a,
                                    ns3::Vector b,
                                    ns3::Vector irs,
                                    ns3::Vector irsNormal)
{
    // Check if both a and b are on the correct side of the IRS (the side the irsNormal vector
    // points to)
    double dotProductA =
        irsNormal.x * (a.x - irs.x) + irsNormal.y * (a.y - irs.y) + irsNormal.z * (a.z - irs.z);
    double dotProductB =
        irsNormal.x * (b.x - irs.x) + irsNormal.y * (b.y - irs.y) + irsNormal.z * (b.z - irs.z);

    // If either dot product is negative, the corresponding point is on the wrong side of the IRS
    if (dotProductA < std::numeric_limits<double>::epsilon() ||
        dotProductB < std::numeric_limits<double>::epsilon())
    {
        return std::nullopt;
    }

    // Vector from IRS to a and b
    ns3::Vector AI = irs - a;
    ns3::Vector IB = b - irs;

    double IAnorm = AI.GetLength();
    double IBnorm = IB.GetLength();

    // avoid division by zero
    if (IAnorm == 0 || IBnorm == 0)
    {
        return std::nullopt;
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

std::optional<Angles>
IrsPropagationLossModel::CalcAngles3D(ns3::Vector node, ns3::Vector irs, ns3::Vector irsNormal)
{
    NS_ASSERT_MSG(node != irs, "Node and IRS are on same position");

    auto cross = [](const ns3::Vector& a, const ns3::Vector& b) -> ns3::Vector {
        return ns3::Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    };

    auto normalize = [](const ns3::Vector& vec) -> ns3::Vector {
        double length = vec.GetLength();
        return ns3::Vector(vec.x / length, vec.y / length, vec.z / length);
    };

    // Calculate incident vector (from node to IRS) and normalize
    ns3::Vector incident = normalize(node - irs);

    // Check if the node is on the correct side
    double dotProduct = incident * irsNormal;
    if (dotProduct < std::numeric_limits<double>::epsilon())
    {
        // Node is on the wrong side of the IRS
        return std::nullopt;
    }

    // Create a local coordinate system for the IRS
    ns3::Vector z_axis = irsNormal;

    ns3::Vector reference =
        (std::abs(irsNormal.z) > 0.9) ? ns3::Vector(1, 0, 0) : ns3::Vector(0, 0, 1);
    ns3::Vector x_axis = normalize(cross(reference, irsNormal));
    ns3::Vector y_axis = cross(z_axis, x_axis);

    // Project incident vector onto local coordinate system
    double x = incident * x_axis;
    double y = incident * y_axis;
    double z = incident * z_axis;

    // Calculate angles
    double azimuth = std::atan2(y, x);
    double inclination = std::acos(z); // Inclination: [0, π]
    return Angles(azimuth, inclination);
}

void
IrsPropagationLossModel::CalcIrsPaths()
{
    m_irsPaths.clear();
    auto facingEachOther = [](Ptr<MobilityModel> mobility1,
                              Ptr<MobilityModel> mobility2,
                              Ptr<IrsModel> irsObj1,
                              Ptr<IrsModel> irsObj2) {
        NS_ASSERT_MSG(mobility1 && mobility2, "Mobility Model can't be null.");
        NS_ASSERT_MSG(irsObj1 && irsObj2, "IRS Model can't be null.");
        Vector pos1 = mobility1->GetPosition();
        Vector pos2 = mobility2->GetPosition();
        return (irsObj1->GetDirection() * (pos2 - pos1) > std::numeric_limits<double>::epsilon()) &&
               (irsObj2->GetDirection() * (pos1 - pos2) > std::numeric_limits<double>::epsilon());
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
            do
            {
                bool validPath = true;
                double loss = 0.0;
                if (currentPath.size() > 1)
                {
                    for (size_t i = 0; i < currentPath.size() - 1; ++i)
                    {
                        auto mobility1 = currentPath[i]->GetObject<MobilityModel>();
                        auto mobility2 = currentPath[i + 1]->GetObject<MobilityModel>();
                        auto irsObj1 = currentPath[i]->GetObject<IrsModel>();
                        auto irsObj2 = currentPath[i + 1]->GetObject<IrsModel>();
                        // prune path if IRS aren't facing each other
                        if (!facingEachOther(mobility1, mobility2, irsObj1, irsObj2))
                        {
                            validPath = false;
                            break;
                        }
                        // calc pathloss over IRS
                        double segmentLoss = m_irsLossModel->CalcRxPower(0, mobility1, mobility2);
                        loss += segmentLoss;
                        if (loss < -100)
                        {
                            validPath = false;
                            break;
                        }
                        // For paths 3+ nodes, add IRS gain at intermediate nodes
                        if (currentPath.size() > 2 && i < currentPath.size() - 2)
                        {
                            auto angles = CalcAngles(
                                mobility1->GetPosition(),
                                currentPath[i + 2]->GetObject<MobilityModel>()->GetPosition(),
                                mobility2->GetPosition(),
                                irsObj2->GetDirection());

                            if (!angles)
                            {
                                validPath = false;
                                break;
                            }
                            loss += irsObj2
                                        ->GetIrsEntry(std::round(angles->first),
                                                      std::round(angles->second))
                                        .gain;
                        }
                    }
                }

                // prune paths, which lie under the noise floor
                if (validPath && loss > -100)
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

        Ptr<IrsModel> irsModel = irs->GetObject<IrsModel>();
        if (dynamic_cast<IrsLookupModel*>(PeekPointer(irsModel)))
        {
            // Calculate angles
            auto angles = CalcAngles(prev->GetPosition(),
                                     next->GetPosition(),
                                     irs->GetObject<MobilityModel>()->GetPosition(),
                                     irs->GetObject<IrsModel>()->GetDirection());
            if (!angles)
            {
                return std::complex<double>(0.0, 0.0);
            }
            IrsEntry modifier =
                irsModel->GetIrsEntry(std::round(angles->first), std::round(angles->second));
            NS_LOG_INFO("IRS Gain (dBm): " << modifier.gain << " | IRS phase shift (radians): "
                                           << modifier.phase_shift);
            // add path lenght and phase shift
            totalDistance += prev->GetDistanceFrom(irs->GetObject<MobilityModel>());
            totalPhaseShift += modifier.phase_shift;
            // calulate pathloss
            pathLoss = m_irsLossModel->CalcRxPower(pathLoss, prev, irs->GetObject<MobilityModel>());
            pathLoss += modifier.gain + m_rng->GetValue();
        }
        else if (dynamic_cast<IrsSpectrumModel*>(PeekPointer(irsModel)))
        {
            // Calculate angles
            auto anglesIn = CalcAngles3D(prev->GetPosition(),
                                         irs->GetObject<MobilityModel>()->GetPosition(),
                                         irs->GetObject<IrsModel>()->GetDirection());
            auto anglesOut = CalcAngles3D(next->GetPosition(),
                                          irs->GetObject<MobilityModel>()->GetPosition(),
                                          irs->GetObject<IrsModel>()->GetDirection());
            if (!anglesIn || !anglesOut)
            {
                return std::complex<double>(0.0, 0.0);
            }
            IrsEntry modifier =
                irsModel->GetIrsEntry(anglesIn.value(), anglesOut.value(), m_lambda);
            NS_LOG_INFO("IRS Gain (dBm): " << modifier.gain << " | IRS phase shift (radians): "
                                           << modifier.phase_shift);
            // add path lenght and phase shift
            totalDistance += prev->GetDistanceFrom(irs->GetObject<MobilityModel>());
            totalPhaseShift += modifier.phase_shift;
            // calulate pathloss
            pathLoss = m_irsLossModel->CalcRxPower(pathLoss, prev, irs->GetObject<MobilityModel>());
            pathLoss += modifier.gain + m_rng->GetValue();
        }
    }
    totalDistance += path.back()->GetObject<MobilityModel>()->GetDistanceFrom(destination);
    pathLoss =
        m_irsLossModel->CalcRxPower(pathLoss, path.back()->GetObject<MobilityModel>(), destination);
    // Calculate phase for the entire path
    double theta = WrapToPi(((2 * M_PI * totalDistance) / m_lambda) + totalPhaseShift);
    std::complex<double> phase_path(0.0, theta);

    NS_LOG_DEBUG("IRS - Node(s): " << path.size() << ", Distance: " << totalDistance << "m"
                                   << ", Path Loss: " << pathLoss << "dBm" << ", Phase: " << theta);

    NS_ASSERT_MSG(!std::isnan(pathLoss), "Path loss is NaN");
    return std::sqrt(DbmToW(pathLoss)) * std::exp(phase_path);
}

double
IrsPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    NS_ASSERT_MSG(a && b, "Mobility models a and be can not be null.");
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
