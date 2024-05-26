#include "irs-propagation-loss-model.h"

#include "ns3/irs.h"
#include "ns3/mobility-model.h"
#include "ns3/pointer.h"

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
                                     Ptr<ns3::MobilityModel> b) const
{
    double pl_other = txPowerDbm;
    Ptr<PropagationLossModel> next = GetNext();

    if (next)
    {
        pl_other = next->CalcRxPower(pl_other, a, b);
    }
    else{
        NS_FATAL_ERROR("No propagation loss model found");
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
        Ptr<Node> node = *irsNode;
        Ptr<Irs> irs = node->GetObject<Irs>();
        std::cout << "IRS Positions (" << node->GetId() << ") : " << node->GetObject<MobilityModel>()->GetPosition() << std::endl;
        uint16_t in_angle = 20;
        uint16_t out_angle = 20;
        IrsEntry entry = irs->GetIrsEntry(in_angle, out_angle);
        std::cout << "IRS Lookup test: in/out angle: " << in_angle << " " << out_angle << " gain: " << entry.gain << " phase_shift: " << entry.phase_shift << std::endl;
    }
    return 0.0;
}

int64_t
IrsPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
