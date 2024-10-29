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

#include "../helper/irs-lookup-helper.h"

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

class IrsSpectrumModel : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    IrsSpectrumModel();

    IrsEntry GetIrsEntry(Angles in, Angles out, double freq) const;

    void CalcRCoeffs(double dApSta, double dApIrsSta, double delta);

    void SetDirection(const Vector& direction);
    Vector GetDirection() const;

    void SetIngoingAngles(const Angles& angles);
    Angles GetIngoingAngles() const;

    void SetOutgoingAngles(const Angles& angles);
    Angles GetOutgoingAngles() const;

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

    Vector m_direction;

    double m_frequency;
    double m_lambda;

    uint16_t m_Nr;
    uint16_t m_Nc;

    double m_dr;
    double m_dc;

    uint16_t m_samples;

    Angles m_ingoing = Angles(0, 0);
    Angles m_outgoing = Angles(0, 0);

    Eigen::VectorXcd m_rcoeffs;
};

} // namespace ns3

#endif /* IRS_SPECTRUM_MODEL_H */
