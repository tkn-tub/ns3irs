/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
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
 * @defgroup irs Intelligent Reflecting Surface (IRS) Models
 * This module provides base classes and utilities for simulating Intelligent Reflecting Surfaces
 */
namespace ns3
{

/**
 * @class IrsSpectrumModel
 * @brief Implements an IRS model compatible with spectrum-based propagation models.
 *
 * This class calculates IRS reflection coefficients, steering vectors, and
 * element positions based on the spectrum propagation model.
 */
class IrsSpectrumModel : public IrsModel
{
  public:
    /**
     * @brief Get the type ID for runtime type identification.
     * @return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * @brief Default constructor for the IRS spectrum model.
     */
    IrsSpectrumModel();

    /**
     * @brief Retrieve an IRS entry based on input and output angles.
     * @param in_angle Input angle (azimuth) in degrees.
     * @param out_angle Output angle (azimuth) in degrees.
     * @return The corresponding \c IrsEntry object.
     */
    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const override;

    /**
     * @brief Retrieve an IRS entry based on angles and wavelength.
     * @param in Input angles (azimuth and elevation in radians) as an \c Angles object.
     * @param out Output angles (azimuth and elevation in radians) as an \c Angles object.
     * @param lambda Wavelength of the signal in meters.
     * @return The corresponding \c IrsEntry object.
     */
    IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const override;

    /**
     * @brief Calculate reflection coefficients based on path distances, angles, and phase offset.
     * @param dApSta Distance between the access point and the station
     * @param dApIrsSta Distance from the access point to the IRS and then to the station
     * @param inAngle Incident angles
     * @param outAngle Reflection angles
     * @param delta Phase offset
     */
    void CalcRCoeffs(double dApSta,
                     double dApIrsSta,
                     Angles inAngle,
                     Angles outAngle,
                     double delta = 0);

    /**
     * @brief Calculate reflection coefficients for the given angles.
     * @param inAngle Incident angles
     * @param outAngle Reflection angles
     */
    void CalcRCoeffs(Angles inAngle, Angles outAngle);

    /**
     * @brief Set reflection coefficients.
     * @param A vector of complex reflection coefficients
     */
    void SetRcoeffs(Eigen::VectorXcd rcoeffs);

    /**
     * @brief Retrieve the calculated reflection coefficients.
     * @return A vector of complex reflection coefficients
     */
    Eigen::VectorXcd GetRcoeffs() const;

    /**
     * @brief Set element positions.
     * @param A matrix of element positions
     */
    void SetElementPos(Eigen::MatrixX3d positions);

    /**
     * @brief Retrieve the calculated reflection coefficients.
     * @return A vector of complex reflection coefficients
     */
    Eigen::MatrixX3d GetElementPos() const;

    /**
     * @brief Set the number of IRS elements along each dimension.
     * @param N Tuple containing the number of rows (Nr) and columns (Nc)
     */
    void SetN(std::tuple<uint16_t, uint16_t> N);

    /**
     * @brief Retrieve the number of IRS elements.
     * @return Tuple with the number of rows (Nr) and columns (Nc)
     */
    std::tuple<uint16_t, uint16_t> GetN() const;

    /**
     * @brief Set the element spacing of the IRS.
     * @param d Tuple containing the row spacing (dr) and column spacing (dc) in meters
     */
    void SetSpacing(std::tuple<double, double> d);

    /**
     * @brief Retrieve the IRS element spacing.
     * @return Tuple with the row spacing (dr) and column spacing (dc) in meters
     */
    std::tuple<double, double> GetSpacing() const;

    /**
     * @brief Set the carrier frequency for IRS calculations.
     * @param frequency Carrier frequency in Hz
     */
    void SetFrequency(double frequency);

    /**
     * @brief Retrieve the carrier frequency.
     * @return Carrier frequency in Hz
     */
    double GetFrequency() const;

    /**
     * @brief Calculate the positions of IRS elements in 3D space.
     * @return A matrix where each row corresponds to an element's 3D position
     */
    Eigen::MatrixX3d CalcElementPositions() const;

    /**
     * @brief Calculate the steering vector for the given angles and wavelength.
     * @param angle Incident or reflection angles
     * @param lambda Wavelength in meters
     * @return A vector representing the steering vector
     */
    Eigen::VectorXcd CalcSteeringvector(Angles angle,
                                        double lambda,
                                        Eigen::MatrixX3d elementPos) const;

    /**
     * @brief Calculate the phase shift for a given configuration.
     * @param dApSta Distance between the access point and the station
     * @param dApIrsSta Distance from the access point to the IRS and then to the station
     * @param delta Phase offset
     * @return Phase shift in radians
     */
    double CalcPhaseShift(double dApSta, double dApIrsSta, double delta) const;

  private:
    /**
     * @brief Calculate the wave vector based on angles and wavelength.
     * @param angle Incident or reflection angles
     * @param lambda Wavelength in meters
     * @return A wave vector
     */
    Eigen::Vector3d CalcWaveVector(Angles angle, double lambda) const;

    uint16_t m_Nr;
    uint16_t m_Nc;
    double m_dr;
    double m_dc;
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
            const double RAD_TO_DEG = 180.0 / M_PI;
            return static_cast<int>(in.GetAzimuth() * RAD_TO_DEG) ==
                       static_cast<int>(other.in.GetAzimuth() * RAD_TO_DEG) &&
                   static_cast<int>(in.GetInclination() * RAD_TO_DEG) ==
                       static_cast<int>(other.in.GetInclination() * RAD_TO_DEG) &&
                   static_cast<int>(out.GetAzimuth() * RAD_TO_DEG) ==
                       static_cast<int>(other.out.GetAzimuth() * RAD_TO_DEG) &&
                   static_cast<int>(out.GetInclination() * RAD_TO_DEG) ==
                       static_cast<int>(other.out.GetInclination() * RAD_TO_DEG) &&
                   std::abs(lambda - other.lambda) < EPSILON;
        }
    };

    struct CacheKeyHash
    {
        size_t operator()(const CacheKey& key) const
        {
            const double RAD_TO_DEG = 180.0 / M_PI;
            // Convert angles to integers in degrees
            auto inAzimuthDeg = static_cast<uint16_t>(key.in.GetAzimuth() * RAD_TO_DEG);
            auto inInclinationDeg = static_cast<uint16_t>(key.in.GetInclination() * RAD_TO_DEG);
            auto outAzimuthDeg = static_cast<uint16_t>(key.out.GetAzimuth() * RAD_TO_DEG);
            auto outInclinationDeg = static_cast<uint16_t>(key.out.GetInclination() * RAD_TO_DEG);

            // Hash individual components
            size_t h1 = std::hash<uint16_t>()(inAzimuthDeg);
            size_t h2 = std::hash<uint16_t>()(inInclinationDeg);
            size_t h3 = std::hash<uint16_t>()(outAzimuthDeg);
            size_t h4 = std::hash<uint16_t>()(outInclinationDeg);
            size_t h5 = std::hash<double>()(key.lambda);

            // Combine hashes
            size_t seed = h1;
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };

    // Cache storage
    mutable std::unordered_map<CacheKey, IrsEntry, CacheKeyHash> m_cache;
};

} // namespace ns3

#endif /* IRS_SPECTRUM_MODEL_H */
