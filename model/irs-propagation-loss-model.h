#ifndef IRS_PROPAGATION_LOSS_MODEL_H
#define IRS_PROPAGATION_LOSS_MODEL_H

#include "ns3/node-container.h"
#include "ns3/node.h"
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

    double CalcRxPower(double txPowerDbm, Ptr<MobilityModel> a, Ptr<MobilityModel> b) override;

  private:
    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> a,
                         Ptr<MobilityModel> b) const override;

    int64_t DoAssignStreams(int64_t stream) override;

    Ptr<NodeContainer> m_irsNodes;
};

} // namespace ns3

#endif // NS3_IRS_PROPAGATION_LOSS_MODEL_H
