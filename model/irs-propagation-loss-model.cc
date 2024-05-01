#include "irs-propagation-loss-model.h"

#include "ns3/pointer.h"
#include "ns3/mobility-model.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IrsPropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED(IrsPropagationLossModel);

TypeId
IrsPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::IrsPropagationLossModel")
                            .SetParent<PropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<IrsPropagationLossModel>()
                            .AddAttribute("IrsNodes",
                                          "The IRS nodes in the network.",
                                          PointerValue(),
                                          MakePointerAccessor(&IrsPropagationLossModel::m_irsNodes),
                                          MakePointerChecker<NodeContainer>());
    return tid;
}

IrsPropagationLossModel::IrsPropagationLossModel()
    : PropagationLossModel()
{
}

IrsPropagationLossModel::~IrsPropagationLossModel()
{
}

double
IrsPropagationLossModel::CalcRxPower(double txPowerDbm,
                                     Ptr<ns3::MobilityModel> a,
                                     Ptr<ns3::MobilityModel> b)
{
    double pl_other = txPowerDbm;
    Ptr<PropagationLossModel> next = GetNext();

    // we need at least one more propagation loss model
    NS_ASSERT_MSG(next != nullptr, "No propagation loss model found");

    if (next)
    {
        pl_other = next->CalcRxPower(pl_other, a, b);
    }

    double pl_irs = DoCalcRxPower(txPowerDbm, a, b);

    return pl_irs + pl_other;
}

double
IrsPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    for(auto irsNode = m_irsNodes->Begin(); irsNode != m_irsNodes->End(); irsNode++)
    {
        Ptr<Node> irs = *irsNode;
        std::cout << "IRS Positions (" << irs->GetId() << ") : " << irs->GetObject<MobilityModel>()->GetPosition() << std::endl;
    }
    return 0.0;
}

int64_t
IrsPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
