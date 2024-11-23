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

#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/vector.h"

#include <complex>

// friend classes to test private fuctions
class IrsPropagationLossModelTestCase;
class IrsPropagationLossModelHelperFunctionsTestCase;

namespace ns3
{
typedef std::vector<Ptr<Node>> IrsPath;

class IrsPropagationLossModel : public PropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    IrsPropagationLossModel();
    ~IrsPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    IrsPropagationLossModel(const IrsPropagationLossModel&) = delete;
    IrsPropagationLossModel& operator=(const IrsPropagationLossModel&) = delete;

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

    /**
     * \param IRS nodes in the channel
     *
     * Set the IRS nodes in the channel. Nodes should contain irs model
     */
    void SetIrsNodes(Ptr<NodeContainer> nodes);

    /**
     * \return the irs nodes
     */
    Ptr<NodeContainer> GetIrsNodes() const;

    /**
     * Set the PropagationLossModel for the paths Tx -> IRS and IRS -> Rx
     *
     * \param model PropagationLossModel
     */
    void SetIrsPropagationModel(Ptr<PropagationLossModel> model);

    /**
     * \return the propagation model
     */
    Ptr<PropagationLossModel> GetIrsPropagatioModel() const;

    /**
     * Set the PropagationLossModel for the line-of-sight
     *
     * \param model PropagationLossModel
     */
    void SetLosPropagationModel(Ptr<PropagationLossModel> model);

    /**
     * \return the propagation model for the line-of-sight
     */
    Ptr<PropagationLossModel> GetLosPropagatioModel() const;

    /**
     * Output streamer.
     * IRS paths are written as "[1->2]".
     *
     * \param [in,out] os The stream.
     * \param [in] vector The vector to stream
     * \return The stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const std::vector<IrsPath>& paths);

    void SetErrorModel(std::tuple<double, double> values);
    std::tuple<double, double> GetErrorModel() const;

  private:
    /**
     * Transforms a Dbm value to Watt
     * \param w the Dbm value
     * \return the Watt
     */
    double DbmToW(double dbm) const;

    /**
     * Transforms a Watt value to Dbm
     * \param w the Watt value
     * \return the Dbm
     */
    double DbmFromW(double w) const;

    /**
     * Computes the angle of incidence and the angle of reflection for the vectors
     * from IRS to A and B, with respect to the direction of the IRS. If points A and B
     * are located on opposite sides of the IRS, the function returns (-1, -1) to indicate this
     * situation.
     *
     * @param a position of A
     * @param b Position of B
     * @param irs Position of IRS
     * @param irsNormal Normal vector of the IRS
     *
     * @return A std::pair containing:
     *         - The angle of incidence in degrees (0 to 180 degrees).
     *         - The angle of reflection in degrees (0 to 180 degrees).
     *         If points A and B are on opposite sides of the IRS, both angles will be set to -1.
     */
    std::pair<double, double> CalcAngles(ns3::Vector a,
                                         ns3::Vector b,
                                         ns3::Vector irs,
                                         ns3::Vector irsNormal) const;

    void CalcIrsPaths();

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
