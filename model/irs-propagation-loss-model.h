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

#ifndef IRS_PROPAGATION_LOSS_MODEL_H
#define IRS_PROPAGATION_LOSS_MODEL_H

#include "ns3/angles.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/vector.h"

#include <complex>
#include <optional>

// friend classes to test private fuctions
class IrsPropagationLossModelTestCase;
class IrsPropagationLossModelHelperFunctionsTestCase;

/**
 * \defgroup irs Intelligent Reflecting Surface (IRS) Models
 * This module provides base classes and utilities for simulating Intelligent Reflecting Surfaces
 */

namespace ns3
{
typedef std::vector<Ptr<Node>> IrsPath;

/**
 * \class IrsPropagationLossModel
 * \brief Models signal propagation with IRS nodes in the channel.
 *
 * The \c IrsPropagationLossModel class extends the \c PropagationLossModel to account for
 * Intelligent Reflecting Surfaces (IRS) in the channel. It computes signal losses for paths
 * involving IRS nodes and includes parameters such as carrier frequency, error model, and
 * propagation loss models for line-of-sight (LoS) and IRS-reflected paths.
 */
class IrsPropagationLossModel : public PropagationLossModel
{
  public:
    /**
     * \brief Get the TypeId of this class.
     * \return The object TypeId.
     */
    static TypeId GetTypeId();
    IrsPropagationLossModel();
    ~IrsPropagationLossModel() override;
    IrsPropagationLossModel(const IrsPropagationLossModel&) = delete;
    IrsPropagationLossModel& operator=(const IrsPropagationLossModel&) = delete;

    /**
     * \brief Set the carrier frequency used in the IRS model calculations.
     * \param frequency The carrier frequency in Hz.
     */
    void SetFrequency(double frequency);

    /**
     * \brief Get the current carrier frequency.
     * \return The carrier frequency in Hz.
     */
    double GetFrequency() const;

    /**
     * \brief Set the IRS nodes involved in the channel.
     * \param nodes A pointer to the \c NodeContainer containing IRS nodes.
     *
     * The specified nodes should include an \c IrsModel and an \c MobilityModel.
     */
    void SetIrsNodes(Ptr<NodeContainer> nodes);

    /**
     * \brief Get the current IRS nodes in the channel.
     * \return A pointer to the \c NodeContainer containing IRS nodes.
     */
    Ptr<NodeContainer> GetIrsNodes() const;

    /**
     * \brief Set the propagation loss model for paths involving IRS nodes.
     * \param model A pointer to the \c PropagationLossModel used for IRS paths.
     *
     * This model is applied to compute losses for signal paths that include IRS nodes.
     */
    void SetIrsPropagationModel(Ptr<PropagationLossModel> model);

    /**
     * \brief Get the propagation loss model used for IRS paths.
     * \return A pointer to the \c PropagationLossModel.
     */
    Ptr<PropagationLossModel> GetIrsPropagatioModel() const;

    /**
     * \brief Set the propagation loss model for line-of-sight (LoS) paths.
     * \param model A pointer to the \c PropagationLossModel used for LoS paths.
     */
    void SetLosPropagationModel(Ptr<PropagationLossModel> model);

    /**
     * \brief Get the propagation loss model used for LoS paths.
     * \return A pointer to the \c PropagationLossModel.
     */
    Ptr<PropagationLossModel> GetLosPropagatioModel() const;

    /**
     * \brief Set the error model for the IRS.
     * \param values A tuple with two elements:
     *               - \c mean: The mean of the error in dB.
     *               - \c variance: The variance of the error in dB.
     */
    void SetErrorModel(std::tuple<double, double> values);

    /**
     * \brief Get the current error model.
     * \return A tuple with the mean and variance of the error in dB.
     */
    std::tuple<double, double> GetErrorModel() const;

    /**
     * \brief Compute angles of incidence and reflection with respect to the IRS.
     * \param a Position of point A (source or receiver).
     * \param b Position of point B (source or receiver).
     * \param irs Position of the IRS.
     * \param irsNormal Normal vector of the IRS.
     * \return A pair of angles in degrees:
     *         - The angle of incidence.
     *         - The angle of reflection.
     *         Returns nullopt if A and B are on opposite sides of the IRS.
     */
    static std::optional<std::pair<double, double>> CalcAngles(ns3::Vector a,
                                                        ns3::Vector b,
                                                        ns3::Vector irs,
                                                        ns3::Vector irsNormal);

    /**
     * \brief Compute angles of incidence with respect to the IRS.
     * \param node Position of node (source or receiver).
     * \param irs Position of the IRS.
     * \param irsNormal Normal vector of the IRS.
     * \return azimuth and elevation Angles - nullopt when not on correct side
     */
    static std::optional<Angles> CalcAngles3D(ns3::Vector node,
                                       ns3::Vector irs,
                                       ns3::Vector irsNormal);

    /**
     * IRS paths are represented as strings of the form "[Node1->Node2]".
     */
    friend std::ostream& operator<<(std::ostream& os, const std::vector<IrsPath>& paths);

  private:
    /**
     * \brief Convert a dBm value to Watts.
     * \param dbm The power in dBm.
     * \return The power in Watts.
     */
    double DbmToW(double dbm) const;

    /**
     * \brief Convert a power value in Watts to dBm.
     * \param w The power in Watts.
     * \return The power in dBm.
     */
    double DbmFromW(double w) const;

    /**
     * \brief Compute all possible signal paths involving IRS nodes.
     */
    void CalcIrsPaths();

    /**
     * \brief Compute the signal contribution of a specific IRS path.
     * \param path The IRS path as a vector of \c Ptr<Node>.
     * \param txPowerDbm Transmitter power in dBm.
     * \param source Mobility model of the source node.
     * \param destination Mobility model of the destination node.
     * \return The path contribution as a complex number.
     */
    std::complex<double> CalcPath(const IrsPath& path,
                                  double txPowerDbm,
                                  Ptr<MobilityModel> source,
                                  Ptr<MobilityModel> destination) const;

    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> a,
                         Ptr<MobilityModel> b) const override;

    int64_t DoAssignStreams(int64_t stream) override;

    void DoInitialize() override;

    std::vector<IrsPath> m_irsPaths;
    Ptr<NodeContainer> m_irsNodes;
    Ptr<PropagationLossModel> m_irsLossModel;
    Ptr<PropagationLossModel> m_losLossModel;
    Ptr<NormalRandomVariable> m_rng;
    double m_frequency = 5.21e9;
    double m_lambda = 0.05754;
    bool m_initialized = false;

    // friend classes to test private functions
    friend class ::IrsPropagationLossModelTestCase;
    friend class ::IrsPropagationLossModelHelperFunctionsTestCase;
};

} // namespace ns3

#endif // IRS_PROPAGATION_LOSS_MODEL_H
