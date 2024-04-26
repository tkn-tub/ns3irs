#include "irs-propagation-loss-model.h"

#include "ns3/pointer.h"
#include "ns3/string.h"

namespace ns3
{
NS_OBJECT_ENSURE_REGISTERED(IrsPropagationLossModel);

TypeId
IrsPropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsPropagationLossModel")
            .SetParent<PropagationLossModel>()
            .SetGroupName("Propagation")
            .AddConstructor<IrsPropagationLossModel>()
            .AddAttribute(
                "Environment",
                "The environment scenario",
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
IrsPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    return 0.0;
}

int64_t
IrsPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
