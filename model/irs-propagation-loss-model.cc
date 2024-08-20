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

#include "irs.h"

#include "ns3/angles.h"
#include "ns3/channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/double.h"
#include "ns3/irs.h"
#include "ns3/mobility-model.h"
#include "ns3/object-factory.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>
#include <ostream>
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
                          MakePointerAccessor(&IrsPropagationLossModel::m_irsNodes),
                          MakePointerChecker<NodeContainer>())
            .AddAttribute("LossModel",
                          "The propagation loss model.",
                          PointerValue(),
                          MakePointerAccessor(&IrsPropagationLossModel::m_lossModel),
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
}

IrsPropagationLossModel::~IrsPropagationLossModel()
{
}

void
IrsPropagationLossModel::SetFrequency(double frequency)
{
    NS_ABORT_MSG_UNLESS(frequency > 0, "Frequency should be greater zero (in Hz).");
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
    NS_ABORT_MSG_UNLESS(nodes->GetN() > 0, "The IRS NodeContainer must contain at least one node.");
    m_irsNodes = nodes;
}

Ptr<NodeContainer>
IrsPropagationLossModel::GetIrsNodes() const
{
    return m_irsNodes;
}

void
IrsPropagationLossModel::SetPropagationModel(Ptr<PropagationLossModel> model)
{
    m_lossModel = model;
}

Ptr<PropagationLossModel>
IrsPropagationLossModel::GetPropagatioModel() const
{
    return m_lossModel;
}

// copied from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmToW(double dbm) const
{
    double mw = std::pow(10.0, dbm / 10.0);
    return mw / 1000.0;
}

// copied from FriisPropagationLossModel
double
IrsPropagationLossModel::DbmFromW(double w) const
{
    double dbm = std::log10(w * 1000.0) * 10.0;
    return dbm;
}

std::pair<double, double>
IrsPropagationLossModel::CalcAngles(ns3::Vector A,
                                    ns3::Vector B,
                                    ns3::Vector I,
                                    ns3::Vector N) const
{
    // check if A and B are on opposite sites
    double e1 = N.x * (A.x - I.x) + N.y * (A.y - I.y) + N.z * (A.z - I.z);
    double e2 = N.x * (B.x - I.x) + N.y * (B.y - I.y) + N.z * (B.z - I.z);
    if (e1 * e2 < std::numeric_limits<double>::epsilon())
    {
        return std::make_pair(-1, -1);
    }

    // Vector from IRS to A and B
    ns3::Vector AI = I - A;
    ns3::Vector IB = B - I;

    double IAnorm = AI.GetLength();
    double IBnorm = IB.GetLength();

    // avoid division by zero
    if (IAnorm == 0 || IBnorm == 0)
    {
        return std::make_pair(-1, -1);
    }

    // Calculate the angle of incidence
    double cosThetaInc = (AI * N) / AI.GetLength();
    double thetaInc = std::acos(std::clamp(cosThetaInc, -1.0, 1.0));
    double thetaIncDeg = thetaInc * (180.0 / M_PI);

    // Calculate the angle of reflection
    double cosThetaRef = (IB * N) / IB.GetLength();
    double thetaRef = std::acos(std::clamp(cosThetaRef, -1.0, 1.0));
    double thetaRefDeg = thetaRef * (180.0 / M_PI);

    // Return both angles as a pair
    return std::make_pair(thetaIncDeg, thetaRefDeg);
}

double
IrsPropagationLossModel::WrapToPi(double angle) const
{
    // Wrap the angle to the range -2*pi to 2*pi
    angle = fmod(angle, 2.0 * M_PI);

    // Wrap the angle to the range -pi to pi
    if (angle > M_PI)
    {
        angle -= 2.0 * M_PI;
    }
    else if (angle < -M_PI)
    {
        angle += 2.0 * M_PI;
    }

    return angle;
}

double
IrsPropagationLossModel::CalcRxPower(double txPowerDbm,
                                     Ptr<ns3::MobilityModel> a,
                                     Ptr<ns3::MobilityModel> b) const
{
    // complex envelope of recieved signal
    std::complex<double> r(0.0, 0.0);

#ifdef NS3_BUILD_PROFILE_DEBUG
    // get current frequency - only possible when WifiNetDevice is used
    Ptr<Node> nodeA = a->GetObject<Node>();
    if (nodeA)
    {
        Ptr<NetDevice> device = nodeA->GetDevice(0);
        Ptr<WifiNetDevice> wifiNetDevice = DynamicCast<WifiNetDevice>(device);
        if (wifiNetDevice)
        {
            Ptr<Channel> channel = wifiNetDevice->GetChannel();
            Ptr<WifiPhy> wifiPhy = wifiNetDevice->GetPhy();
            if (wifiPhy)
            {
                NS_LOG_DEBUG("Frequency (MHz): " << wifiPhy->GetFrequency());
            }
        }
    }
#endif

    NS_LOG_DEBUG("--------- IRS Propagation Loss Model Debug Info ---------\n"
                 << "m_frequency (Hz): " << m_frequency << "\n"
                 << "TX Power (dBm): " << txPowerDbm);

    for (auto irsNode = m_irsNodes->Begin(); irsNode != m_irsNodes->End(); irsNode++)
    {
        Ptr<Node> node = *irsNode;
        Ptr<Irs> irs = node->GetObject<Irs>();

        std::pair<double, double> angles =
            CalcAngles(a->GetPosition(),
                       b->GetPosition(),
                       irs->GetObject<MobilityModel>()->GetPosition(),
                       irs->GetObject<Irs>()->GetDirection());

        NS_LOG_DEBUG("" << "IRS Position (" << node->GetId()
                        << "): " << node->GetObject<MobilityModel>()->GetPosition() << "\n"
                        << "TX Position: " << a->GetPosition() << "\n"
                        << "RX Position: " << b->GetPosition() << "\n"
                        << "IRS Direction: " << node->GetObject<Irs>()->GetDirection() << "\n"
                        << "Ingoing Angle (degrees): " << angles.first << "\n"
                        << "Outgoing Angle (degrees): " << angles.second);

        // if the incoming angle is < 1 or > 179, then the IRS is not in the line of sight
        // if Nodes are on opposite sides angles are (-1, -1)
        if (angles.first < 0 || angles.second < 0)
        {
            NS_LOG_INFO("IRS (" << node->GetId() << ") with position: "
                                << node->GetObject<MobilityModel>()->GetPosition()
                                << " is not in LOS between " << a->GetPosition() << " and "
                                << b->GetPosition());
            continue;
        }
        IrsEntry modifier =
            irs->GetLookupTable()->GetIrsEntry(std::round(angles.first), std::round(angles.second));

        // distance of irs path
        double d = a->GetDistanceFrom(node->GetObject<MobilityModel>()) +
                   node->GetObject<MobilityModel>()->GetDistanceFrom(b);

        // pathloss tx to irs
        double pl_irs = m_lossModel->CalcRxPower(txPowerDbm, a, node->GetObject<MobilityModel>());
        NS_LOG_DEBUG("PL TX to IRS (dBm): " << pl_irs);

        // gain of irs
        pl_irs += modifier.gain;

        // add random variable to account for small-scale fading or measurement noise
        // Normal Distribution (0, 0.1)
        Ptr<NormalRandomVariable> rng = CreateObject<NormalRandomVariable>();
        rng->SetAttribute("Mean", DoubleValue(0.0));
        rng->SetAttribute("Variance", DoubleValue(0.1));
        pl_irs += rng->GetValue();

        NS_LOG_DEBUG("PL with IRS Gain (dBm): " << pl_irs);
        // pathloss irs to rx
        pl_irs = m_lossModel->CalcRxPower(pl_irs, node->GetObject<MobilityModel>(), b);

        // calculate phase of irs path
        double theta = (2 * M_PI * d) / m_lambda;
        // add phase shift of irs to theta
        theta += modifier.phase_shift;
        // wrap to pi
        theta = WrapToPi(theta);
        // theta to complex
        std::complex<double> phase_irs(0.0, theta);

        NS_LOG_INFO("IRS Gain (dBm): " << modifier.gain
                                       << " | IRS phase shift (radians): " << modifier.phase_shift);

        NS_LOG_DEBUG("" << "Distance over IRS (m): " << d << "\n"
                        << "Phase IRS (radians): " << phase_irs << "\n"
                        << "RX Power IRS (dBm): " << pl_irs);
        // Add IRS path contribution
        r += std::sqrt(DbmToW(pl_irs)) * std::exp(phase_irs);
    }

    Ptr<PropagationLossModel> next = GetNext();
    if (next)
    {
        // calculate amplitude
        double pl_other = next->CalcRxPower(txPowerDbm, a, b);

        // calculate phase
        double theta = (2 * M_PI * a->GetDistanceFrom(b)) / m_lambda;
        theta = WrapToPi(theta);
        std::complex<double> phase_other(0.0, theta);

        NS_LOG_DEBUG("" << "RX Power LOS (dBm): " << pl_other << "\n"
                        << "Phase LOS (radians): " << phase_other << "\n"
                        << "Distance LOS (m): " << a->GetDistanceFrom(b));

        r += std::sqrt(DbmToW(pl_other)) * std::exp(phase_other);
    }
    else
    {
        NS_LOG_DEBUG("No N/LOS propagation model specified. Calculating only IRS path.");
    }

    // Log resulting received power
    double rxPower = DbmFromW(std::pow(std::abs(r), 2));
    NS_LOG_DEBUG("Resulting RX Power (dBm): " << rxPower);

    return rxPower;
}

double
IrsPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    return 0.0;
}

int64_t
IrsPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
