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

#ifndef IRS_SPECTRUM_MODEL_H
#define IRS_SPECTRUM_MODEL_H

#include "irs-model.h"

#include "ns3/angles.h"
#include "ns3/object.h"
#include "ns3/vector.h"

#include <Eigen/Dense>
#include <cstdint>
#include <sys/types.h>

/**
 * \defgroup irs Description of the irs
 */
namespace ns3
{

class IrsSpectrumModel : public IrsModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    IrsSpectrumModel();

    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const override;
    IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const override;

    void CalcRCoeffs(double dApSta,
                     double dApIrsSta,
                     Angles inAngle,
                     Angles outAngle,
                     double delta);

    void CalcRCoeffs(Angles inAngle, Angles outAngle);

    void SetN(std::tuple<uint16_t, uint16_t> N);
    std::tuple<uint16_t, uint16_t> GetN() const;

    void SetSpacing(std::tuple<double, double> d);
    std::tuple<double, double> GetSpacing() const;

    void SetSamples(const uint16_t samples);
    uint16_t GetSamples() const;

    Eigen::VectorXcd GetRcoeffs() const;

    /**
     * \param frequency (Hz)
     *
     * Set the carrier frequency used in the irs model
     * calculation.
     */
    void SetFrequency(double frequency);

    /**
     * \return the current frequency (Hz)
     */
    double GetFrequency() const;

  private:
    Eigen::Vector3d CalcWaveVector(Angles angle, double lambda) const;
    Eigen::MatrixX3d CalcElementPositions() const;
    Eigen::VectorXcd CalcSteeringvector(Angles angle, double lambda) const;
    double CalcPhaseShift(double dApSta, double dApIrsSta, double delta) const;

    uint16_t m_Nr;
    uint16_t m_Nc;
    double m_dr;
    double m_dc;
    uint16_t m_samples;
    double m_frequency;
    double m_lambda;
    Eigen::VectorXcd m_rcoeffs;
    Eigen::MatrixX3d m_elementPos;

    struct CacheKey
    {
        Angles in;
        Angles out;
        double lambda;

        bool operator==(const CacheKey& other) const
        {
            constexpr double EPSILON = 1e-10;
            return std::abs(in.GetAzimuth() - other.in.GetAzimuth()) < EPSILON &&
                   std::abs(in.GetInclination() - other.in.GetInclination()) < EPSILON &&
                   std::abs(out.GetAzimuth() - other.out.GetAzimuth()) < EPSILON &&
                   std::abs(out.GetInclination() - other.out.GetInclination()) < EPSILON &&
                   std::abs(lambda - other.lambda) < EPSILON;
        }
    };

    struct CacheKeyHash
    {
        size_t operator()(const CacheKey& key) const
        {
            size_t h1 = std::hash<double>()(key.in.GetAzimuth());
            size_t h2 = std::hash<double>()(key.in.GetInclination());
            size_t h3 = std::hash<double>()(key.out.GetAzimuth());
            size_t h4 = std::hash<double>()(key.out.GetInclination());
            size_t h5 = std::hash<double>()(key.lambda);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
        }
    };

    // Cache storage
    mutable std::unordered_map<CacheKey, IrsEntry, CacheKeyHash> m_cache;
};

} // namespace ns3

#endif /* IRS_SPECTRUM_MODEL_H */
