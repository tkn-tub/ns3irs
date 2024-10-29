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

#include "irs-spectrum-model.h"

#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"

#include <cstdint>
#include <iostream>
#include <ostream>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(IrsSpectrumModel);

TypeId
IrsSpectrumModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsSpectrumModel")
            .SetParent<Object>()
            .SetGroupName("IrsSpectrumModel")
            .AddConstructor<IrsSpectrumModel>()
            .AddAttribute("InAngle",
                          "Ingoing angle Irs is optimized for.",
                          AnglesValue(Angles(0.0, 0.0)),
                          MakeAnglesAccessor(&IrsSpectrumModel::SetIngoingAngles,
                                             &IrsSpectrumModel::GetIngoingAngles),
                          MakeAnglesChecker())
            .AddAttribute("OutAngle",
                          "Outgoing angle Irs is optimized for.",
                          AnglesValue(Angles(0.0, 0.0)),
                          MakeAnglesAccessor(&IrsSpectrumModel::SetOutgoingAngles,
                                             &IrsSpectrumModel::GetOutgoingAngles),
                          MakeAnglesChecker())
            .AddAttribute(
                "N",
                "Number of elements. First row then column",
                TupleValue<UintegerValue, UintegerValue>({1, 1}),
                MakeTupleAccessor<UintegerValue, UintegerValue>(&IrsSpectrumModel::SetN,
                                                                &IrsSpectrumModel::GetN),
                MakeTupleChecker<UintegerValue, UintegerValue>(MakeUintegerChecker<uint16_t>(),
                                                               MakeUintegerChecker<uint16_t>()))
            .AddAttribute(
                "Spacing",
                "Spacing of Elements. First row then column.",
                TupleValue<DoubleValue, DoubleValue>({0.05, 0.05}),
                MakeTupleAccessor<DoubleValue, DoubleValue>(&IrsSpectrumModel::SetSpacing,
                                                            &IrsSpectrumModel::GetSpacing),
                MakeTupleChecker<DoubleValue, DoubleValue>(MakeDoubleChecker<double>(),
                                                           MakeDoubleChecker<double>()))
            .AddAttribute(
                "Frequency",
                "The carrier frequency (in Hz) at which propagation occurs (default is 5.21 GHz).",
                DoubleValue(5.21e9),
                MakeDoubleAccessor(&IrsSpectrumModel::SetFrequency,
                                   &IrsSpectrumModel::GetFrequency),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "Samples",
                "Amount of samples for wave.",
                UintegerValue(1000),
                MakeDoubleAccessor(&IrsSpectrumModel::SetSamples, &IrsSpectrumModel::GetSamples),
                MakeUintegerChecker<uint16_t>())
            .AddAttribute("Direction",
                          "The direction of the IRS.",
                          VectorValue(Vector(1.0, 0.0, 0.0)),
                          MakeVectorAccessor(&IrsSpectrumModel::SetDirection,
                                             &IrsSpectrumModel::GetDirection),
                          MakeVectorChecker());
    return tid;
}

IrsSpectrumModel::IrsSpectrumModel()
{
}

void
IrsSpectrumModel::CalcRCoeffs(double dApSta, double dApIrsSta, double delta)
{
    // Calculate steering vectors and their arguments
    Eigen::VectorXd stv_in = CalcSteeringvector(m_ingoing, m_lambda).array().arg();
    Eigen::VectorXd stv_out = CalcSteeringvector(m_outgoing, m_lambda).array().arg();
    // Calculate phase shift
    double shift = CalcPhaseShift(dApSta, dApIrsSta, delta);

    m_rcoeffs = (std::complex<double>(0, 1) *
                 (Eigen::VectorXd::Constant(m_Nr * m_Nc, shift) - stv_in - stv_out))
                    .array()
                    .exp();
}

Eigen::Vector3d
IrsSpectrumModel::CalcWaveVector(Angles angle, double lambda) const
{
    double factor = 2 * M_PI / lambda;
    double cos_incl = std::cos(angle.GetInclination());

    return Eigen::Vector3d(factor * cos_incl * std::cos(angle.GetAzimuth()),
                           factor * cos_incl * std::sin(angle.GetAzimuth()),
                           factor * std::sin(angle.GetInclination()));
}

Eigen::MatrixX3d
IrsSpectrumModel::CalcElementPositions() const
{
    // FIXME: must be dependent on m_direction and actual position - for now lies in
    // yz-plane at 0,0,0

    return Eigen::Matrix<double, Eigen::Dynamic, 3>::NullaryExpr(
        m_Nr * m_Nc,
        3,
        [this](Eigen::Index row, Eigen::Index col) {
            int i = row / m_Nc;
            int j = row % m_Nc;
            switch (col)
            {
            case 0:
                return 0.0; // x
            case 1:
                return (i - (m_Nr - 1) / 2.0) * m_dr; // y
            case 2:
                return ((m_Nc - 1) / 2.0 - j) * m_dc; // z
            default:
                return 0.0;
            }
        });
}

Eigen::VectorXcd
IrsSpectrumModel::CalcSteeringvector(Angles angle, double lambda) const
{
    Eigen::Vector3d k = CalcWaveVector(angle, lambda);
    Eigen::MatrixX3d r = CalcElementPositions();

    return (-std::complex<double>(0, 1) * (r * k).array()).exp();
}

double
IrsSpectrumModel::CalcPhaseShift(double dApSta, double dApIrsSta, double delta) const
{
    return delta + ((2 * M_PI * dApIrsSta) / m_lambda) - ((2 * M_PI * dApSta) / m_lambda);
}

IrsEntry
IrsSpectrumModel::GetIrsEntry(Angles in, Angles out, double freq) const
{
    // TODO: use freq
    static const double c = 299792458.0; // speed of light in vacuum
    double lambda = c / freq;

    Eigen::VectorXcd stv_in = CalcSteeringvector(in, lambda);
    Eigen::VectorXcd stv_out = CalcSteeringvector(out, lambda);

    Eigen::VectorXd signal_in = Eigen::VectorXd::Ones(m_samples);
    Eigen::MatrixXcd signal_inc = signal_in * stv_in.transpose();
    Eigen::MatrixXcd signal_weighted = signal_inc.array().rowwise() * m_rcoeffs.transpose().array();
    Eigen::VectorXcd signal_ref = signal_weighted * stv_out;

    double gain =
        10 * std::log10((signal_ref.conjugate().array() * signal_ref.array()).real().mean()) -
        10 * std::log10((signal_in.conjugate().array() * signal_in.array()).real().mean());

    // TODO: wrap shift to pi
    double shift = (signal_in.array().arg() - signal_ref.array().arg()).mean();

    return IrsEntry(gain, shift);
}

void
IrsSpectrumModel::SetDirection(const Vector& direction)
{
    // Normalize the vector
    double m = direction.GetLength();
    if (m != 0)
    {
        m_direction = Vector3D(direction.x / m, direction.y / m, direction.z / m);
    }
    else
    {
        m_direction = direction;
    }
}

Vector
IrsSpectrumModel::GetDirection() const
{
    return m_direction;
}

void
IrsSpectrumModel::SetIngoingAngles(const Angles& angles)
{
    // TODO: Check bounds
    m_ingoing = angles;
}

Angles
IrsSpectrumModel::GetIngoingAngles() const
{
    return m_ingoing;
}

void
IrsSpectrumModel::SetOutgoingAngles(const Angles& angles)
{
    // TODO: Check bounds
    m_outgoing = angles;
}

Angles
IrsSpectrumModel::GetOutgoingAngles() const
{
    return m_outgoing;
}

void
IrsSpectrumModel::SetN(std::tuple<uint16_t, uint16_t> N)
{
    m_Nr = std::get<0>(N);
    m_Nc = std::get<1>(N);
}

std::tuple<uint16_t, uint16_t>
IrsSpectrumModel::GetN() const
{
    return {m_Nr, m_Nc};
}

void
IrsSpectrumModel::SetSpacing(std::tuple<double, double> d)
{
    m_dr = std::get<0>(d);
    m_dc = std::get<1>(d);
}

std::tuple<double, double>
IrsSpectrumModel::GetSpacing() const
{
    return {m_dr, m_dc};
}

void
IrsSpectrumModel::SetFrequency(double frequency)
{
    NS_ASSERT_MSG(frequency > 0, "Frequency should be greater zero (in Hz)");
    m_frequency = frequency;
    static const double c = 299792458.0; // speed of light in vacuum
    m_lambda = c / frequency;
}

double
IrsSpectrumModel::GetFrequency() const
{
    return m_frequency;
}

void
IrsSpectrumModel::SetSamples(uint16_t samples)
{
    m_samples = samples;
}

uint16_t
IrsSpectrumModel::GetSamples() const
{
    return m_samples;
}

Eigen::VectorXcd
IrsSpectrumModel::GetRcoeffs() const
{
    return m_rcoeffs;
}

} // namespace ns3
