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
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/vector.h"

class IrsPropagationLossModelTestCase;
class IrsPropagationLossModelHelperFunctionsTestCase;

namespace ns3
{

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

    double CalcRxPower(double txPowerDbm,
                       Ptr<MobilityModel> a,
                       Ptr<MobilityModel> b) const override;
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
    void SetPropagationModel(Ptr<PropagationLossModel> model);

    /**
     * \return the propagation model
     */
    Ptr<PropagationLossModel> GetPropagatioModel() const;

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
     * Calculates the angle in degrees between the vector from point B to point A
     * (`AB`) and the IRS normal vector (`N`). Also determines whether points A and B are on
     * opposite sides of the IRS, given the position of the IRS.
     *
     * \param A Position of A.
     * \param B Position of B.
     * \param N Normal vector of the IRS.
     * \param IrsPos Position of the IRS.
     *
     * \return The angles in degrees between the vector AB and the IRS normal vector.
     * If A and B are on opposite sides of the IRS it returns {-1, -1}.
     */

    /**
     * Computes the angle of incidence and the angle of reflection for the vectors
     * from IRS to A and B, with respect to the direction of the IRS. If points A and B
     * are located on opposite sides of the IRS, the function returns (-1, -1) to indicate this
     * situation.
     *
     * @param A position of A
     * @param B Position of B
     * @param I Position of IRS
     * @param N Normal vector of the IRS
     *
     * @return A std::pair containing:
     *         - The angle of incidence in degrees (0 to 180 degrees).
     *         - The angle of reflection in degrees (0 to 180 degrees).
     *         If points A and B are on opposite sides of the IRS, both angles will be set to -1.
     */
    std::pair<double, double> CalcAngles(ns3::Vector A,
                                         ns3::Vector B,
                                         ns3::Vector I,
                                         ns3::Vector N) const;

    /**
     * Wraps angle to the range -pi to pi
     * \param angle angle in radians
     * \return angle in radians (range -pi to pi)
     */
    double WrapToPi(double angle) const;

    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> a,
                         Ptr<MobilityModel> b) const override;

    int64_t DoAssignStreams(int64_t stream) override;

    Ptr<NodeContainer> m_irsNodes;
    Ptr<PropagationLossModel> m_lossModel;
    double m_frequency;
    double m_lambda;
    Ptr<NormalRandomVariable> m_rng;

    friend class ::IrsPropagationLossModelTestCase;
    friend class ::IrsPropagationLossModelHelperFunctionsTestCase;
};

} // namespace ns3

#endif // IRS_PROPAGATION_LOSS_MODEL_H
