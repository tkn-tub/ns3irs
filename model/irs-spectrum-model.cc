/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "irs-spectrum-model.h"

#include "Eigen/src/Core/Matrix.h"

#include "ns3/abort.h"
#include "ns3/double.h"
#include "ns3/object-base.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"
#include "ns3/vector.h"

#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(IrsSpectrumModel);

TypeId
IrsSpectrumModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsSpectrumModel")
            .SetParent<IrsModel>()
            .SetGroupName("IrsModel")
            .AddConstructor<IrsSpectrumModel>()
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
            .AddAttribute("Frequency",
                          "Frequency in Hz",
                          DoubleValue(5.21e9),
                          MakeDoubleAccessor(&IrsSpectrumModel::SetFrequency,
                                             &IrsSpectrumModel::GetFrequency),
                          MakeDoubleChecker<double>());
    return tid;
}

IrsSpectrumModel::IrsSpectrumModel()
{
}

void
IrsSpectrumModel::CalcRCoeffs(double dApSta,
                              double dApIrsSta,
                              Angles inAngle,
                              Angles outAngle,
                              double delta)
{
    m_elementPos = CalcElementPositions();
    m_cache.clear();

    Eigen::VectorXcd stv_in = CalcSteeringvector(inAngle, m_lambda, m_elementPos).array().arg();
    Eigen::VectorXcd stv_out = CalcSteeringvector(outAngle, m_lambda, m_elementPos).array().arg();
    double shift = CalcPhaseShift(dApSta, dApIrsSta, delta);

    m_rcoeffs = (std::complex<double>(0, 1) *
                 (Eigen::VectorXd::Constant(m_Nr * m_Nc, shift) - stv_in - stv_out))
                    .array()
                    .exp();
}

void
IrsSpectrumModel::CalcRCoeffs(Angles inAngle, Angles outAngle)
{
    m_elementPos = CalcElementPositions();
    m_cache.clear();

    Eigen::VectorXcd stv_in = CalcSteeringvector(inAngle, m_lambda, m_elementPos).array().arg();
    Eigen::VectorXcd stv_out = CalcSteeringvector(outAngle, m_lambda, m_elementPos).array().arg();

    m_rcoeffs = (std::complex<double>(0, 1) * (-stv_in - stv_out)).array().exp();
}

void
IrsSpectrumModel::SetRcoeffs(Eigen::VectorXcd rcoeffs)
{
    m_cache.clear();
    m_rcoeffs = rcoeffs;
}

void
IrsSpectrumModel::SetElementPos(Eigen::MatrixX3d positions)
{
    m_elementPos = positions;
}

Eigen::MatrixX3d
IrsSpectrumModel::GetElementPos() const
{
    return m_elementPos;
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
    Eigen::MatrixX3d element_positions = Eigen::Matrix<double, Eigen::Dynamic, 3>::NullaryExpr(
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
    return element_positions;
}

Eigen::VectorXcd
IrsSpectrumModel::CalcSteeringvector(Angles angle, double lambda, Eigen::MatrixX3d elementPos) const
{
    Eigen::Vector3d k = CalcWaveVector(angle, lambda);

    return (-std::complex<double>(0, 1) * (elementPos * k).array()).exp();
}

double
IrsSpectrumModel::CalcPhaseShift(double dApSta, double dApIrsSta, double delta) const
{
    return delta + ((2 * M_PI * dApIrsSta) / m_lambda) - ((2 * M_PI * dApSta) / m_lambda);
}

IrsEntry
IrsSpectrumModel::GetIrsEntry(Angles in, Angles out, double lambda) const
{
    // Create cache key
    CacheKey key{in, out, lambda};

    // Check if result is in cache
    auto it = m_cache.find(key);
    if (it != m_cache.end())
    {
        return it->second;
    }

    NS_ABORT_MSG_UNLESS(m_rcoeffs.size() > 0,
                        "Reflection coefficients must be calculated before use.");

    Eigen::VectorXcd stv_in = CalcSteeringvector(in, lambda, m_elementPos);
    Eigen::VectorXcd stv_out = CalcSteeringvector(out, lambda, m_elementPos);

    NS_ASSERT_MSG((stv_in.array() == stv_in.array()).all(), "stv_in contains NaN values!");
    NS_ASSERT_MSG((stv_out.array() == stv_out.array()).all(), "stv_out contains NaN values!");
    NS_ASSERT_MSG((m_rcoeffs.array() == m_rcoeffs.array()).all(), "m_rcoeffs contains NaN values!");

    Eigen::MatrixXcd signal_inc = stv_in.transpose();
    Eigen::MatrixXcd signal_weighted = signal_inc.array().rowwise() * m_rcoeffs.transpose().array();
    Eigen::VectorXcd signal_ref = signal_weighted * stv_out;

    double gain =
        10 * std::log10((signal_ref.conjugate().array() * signal_ref.array()).real().mean());
    double shift = -signal_ref.array().arg().mean();

    IrsEntry result(gain, shift);
    m_cache.emplace(key, result);
    return result;
}

IrsEntry
IrsSpectrumModel::GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const
{
    return GetIrsEntry(Angles(DegreesToRadians(in_angle), 0),
                       Angles(DegreesToRadians(out_angle), 0),
                       m_lambda);
}

void
IrsSpectrumModel::SetN(std::tuple<uint16_t, uint16_t> N)
{
    m_Nr = std::get<0>(N);
    m_Nc = std::get<1>(N);
    NS_ABORT_MSG_UNLESS(m_Nr > 0 && m_Nc > 0, "Amount of elements should be greater zero.");
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
    NS_ABORT_MSG_UNLESS(m_dr > 0 && m_dc > 0, "Element spacing should be greater zero.");
}

std::tuple<double, double>
IrsSpectrumModel::GetSpacing() const
{
    return {m_dr, m_dc};
}

void
IrsSpectrumModel::SetFrequency(double frequency)
{
    NS_ABORT_MSG_UNLESS(frequency > 0, "Frequency should be greater zero (in Hz).");
    m_frequency = frequency;
    static const double c = 299792458.0; // speed of light in vacuum
    m_lambda = c / frequency;
}

double
IrsSpectrumModel::GetFrequency() const
{
    return m_frequency;
}

Eigen::VectorXcd
IrsSpectrumModel::GetRcoeffs() const
{
    return m_rcoeffs;
}
} // namespace ns3
