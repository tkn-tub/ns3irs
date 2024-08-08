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

    void SetFrequency(double frequency);
    double GetFrequency() const;

    void SetIrsNodes(Ptr<NodeContainer> nodes);
    Ptr<NodeContainer> GetIrsNodes() const;

    void SetPropagationModel(Ptr<PropagationLossModel> model);
    Ptr<PropagationLossModel> GetPropagatioModel() const;

  private:
    double DbmToW(double dbm) const;
    double DbmFromW(double w) const;

    double CalcAngle(ns3::Vector a, ns3::Vector b, ns3::Vector n) const;
    double WrapToPi(double angle) const;

    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> a,
                         Ptr<MobilityModel> b) const override;

    int64_t DoAssignStreams(int64_t stream) override;

    Ptr<NodeContainer> m_irsNodes;
    Ptr<PropagationLossModel> m_lossModel;
    double m_frequency;
    double m_lambda;
};

} // namespace ns3

#endif // IRS_PROPAGATION_LOSS_MODEL_H
