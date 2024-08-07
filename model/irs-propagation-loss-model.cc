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
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"

#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>
#include <ostream>

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
                "The carrier frequency (in Hz) at which propagation occurs (default is 2.412 GHz).",
                DoubleValue(2.412e9),
                MakeDoubleAccessor(&IrsPropagationLossModel::SetFrequency,
                                   &IrsPropagationLossModel::GetFrequency),
                MakeDoubleChecker<double>());
    return tid;
}

IrsPropagationLossModel::IrsPropagationLossModel()
    : PropagationLossModel()
{
    // set default values for m_lossModel if not set
    if (!m_lossModel)
    {
        m_lossModel = ObjectFactory("ns3::LogDistancePropagationLossModel")
                          .Create<LogDistancePropagationLossModel>();
    }
}

IrsPropagationLossModel::~IrsPropagationLossModel()
{
}

void
IrsPropagationLossModel::SetFrequency(double frequency)
{
    m_frequency = frequency;
    static const double c = 299792458.0; // speed of light in vacuum
    m_lambda = c / frequency;
}

double
IrsPropagationLossModel::GetFrequency() const
{
    return m_frequency;
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

double
IrsPropagationLossModel::CalcAngle(ns3::Vector A, ns3::Vector B, ns3::Vector N) const
{
    Vector AB = A - B;
    double dotProduct = AB.x * N.x + AB.y * N.y + AB.z * N.z;
    double normAB = std::sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);

    // TODO: norm N at init so normN = 1
    double normN = std::sqrt(N.x * N.x + N.y * N.y + N.z * N.z);

    double cosTheta = dotProduct / (normAB * normN);
    double theta = std::acos(cosTheta);
    double thetaDeg = theta * (180.0 / M_PI);
    return thetaDeg;
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

    // --------------------
    // Ptr<Node> nodeA = a->GetObject<Node>();
    // if (nodeA)
    // {
    //     Ptr<NetDevice> device = nodeA->GetDevice(0);
    //     Ptr<WifiNetDevice> wifiNetDevice = DynamicCast<WifiNetDevice>(device);
    //     Ptr<Channel> channel = wifiNetDevice->GetChannel();
    //     Ptr<WifiPhy> wifiPhy = wifiNetDevice->GetPhy();
    //     NS_LOG_DEBUG("Frequency: " << wifiPhy->GetFrequency());
    //     NS_LOG_DEBUG("m_frequency: " << m_frequency);
    // }

    // -------------------------

    // std::cout << "txPower: " << txPowerDbm << std::endl;
    NS_LOG_INFO("--------------------------");
    NS_LOG_DEBUG("TX Power: " << txPowerDbm);
    for (auto irsNode = m_irsNodes->Begin(); irsNode != m_irsNodes->End(); irsNode++)
    {
        Ptr<Node> node = *irsNode;
        Ptr<Irs> irs = node->GetObject<Irs>();

        double inAngle = CalcAngle(a->GetPosition(),
                                   irs->GetObject<MobilityModel>()->GetPosition(),
                                   irs->GetObject<Irs>()->GetDirection());
        double outAngle = CalcAngle(b->GetPosition(),
                                    irs->GetObject<MobilityModel>()->GetPosition(),
                                    irs->GetObject<Irs>()->GetDirection());

        NS_LOG_DEBUG("IRS Position (" << node->GetId()
                                      << ") : " << node->GetObject<MobilityModel>()->GetPosition());
        NS_LOG_DEBUG("TX Position: " << a->GetPosition());
        NS_LOG_DEBUG("RX Position: " << b->GetPosition());
        NS_LOG_DEBUG("IRS Direction: " << node->GetObject<Irs>()->GetDirection());
        NS_LOG_DEBUG("Ingoing Angle: " << inAngle << " degrees");
        NS_LOG_DEBUG("Outgoing Angle: " << outAngle << " degrees");

        // if the incoming angle is < 1 or > 179, then the IRS is not in the line of sight
        if (inAngle < 1 || inAngle > 179 || outAngle < 1 || outAngle > 179)
        {
            NS_LOG_INFO("IRS (" << node->GetId() << ") with pos: "
                                 << node->GetObject<MobilityModel>()->GetPosition()
                                 << " not in LOS between " << a->GetPosition() << " and "
                                 << b->GetPosition() << "");
            continue;
        }
        IrsEntry modifier =
            irs->GetLookupTable()->GetIrsEntry(std::round(inAngle), std::round(outAngle));

        // distance of irs path
        double d = a->GetDistanceFrom(node->GetObject<MobilityModel>()) +
                   node->GetObject<MobilityModel>()->GetDistanceFrom(b);
        NS_LOG_DEBUG("Distance over IRS: " << d);

        // pathloss tx to irs
        double pl_irs = m_lossModel->CalcRxPower(txPowerDbm, a, node->GetObject<MobilityModel>());
        NS_LOG_DEBUG("PL TX to IRX: " << pl_irs);
        // gain of irs
        pl_irs += modifier.gain;
        NS_LOG_DEBUG("PL with IRS gain: " << pl_irs);
        // pathloss irs to rx
        pl_irs = m_lossModel->CalcRxPower(pl_irs, node->GetObject<MobilityModel>(), b);
        NS_LOG_DEBUG("PL IRS to RX: " << pl_irs);

        // convert dBm to watts
        pl_irs = DbmToW(pl_irs);
        // TODO: error model

        // calculate phase of irs path
        double theta = (2 * M_PI * d) / m_lambda; // in radians
        // add phase shift of irs to theta
        theta += modifier.phase_shift;

        // wrap to pi
        theta = WrapToPi(theta);
        // theta to complex
        std::complex<double> phase_irs(0.0, theta);
        NS_LOG_DEBUG("Phase irs: " << phase_irs);

        NS_LOG_INFO("IRS Gain: " << modifier.gain << " IRS Phase-Shift: " << modifier.phase_shift);

        // add irs path to
        r += std::sqrt(pl_irs) * std::exp(phase_irs);
        NS_LOG_DEBUG(std::sqrt(pl_irs) * std::exp(phase_irs));
    }
    NS_LOG_DEBUG("RX Power only IRS: " << DbmFromW(std::pow(std::abs(r), 2)));

    Ptr<PropagationLossModel> next = GetNext();
    if (next)
    {
        // calculate amplitude
        double pl_other = next->CalcRxPower(txPowerDbm, a, b);
        NS_LOG_DEBUG("PL LOS: " << pl_other);
        pl_other = DbmToW(pl_other);

        // calculate phase
        double theta = (2 * M_PI * a->GetDistanceFrom(b)) / m_lambda;
        theta = WrapToPi(theta);
        std::complex<double> phase_other(0.0, theta);

        NS_LOG_DEBUG("Phase LOS: " << phase_other);
        NS_LOG_DEBUG("Distance LOS: " << a->GetDistanceFrom(b));

        r += std::sqrt(pl_other) * std::exp(phase_other);
    }
    else
    {
        NS_LOG_DEBUG("No N/LOS propagation model specified. Calculating only IRS path.");
    }

    // recieved power
    NS_LOG_INFO("Resulting RX Power: " << DbmFromW(std::pow(std::abs(r), 2)));
    return DbmFromW(std::pow(std::abs(r), 2));
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
