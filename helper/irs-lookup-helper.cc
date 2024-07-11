#include "irs-lookup-helper.h"
#include <cstdint>

namespace ns3
{

TypeId
IrsLookupTable::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsLookupTable").SetParent<Object>().AddConstructor<IrsLookupTable>();
    return tid;
}

IrsLookupTable::IrsLookupTable()
{
}

void
IrsLookupTable::Insert(uint8_t in_angle, uint8_t out_angle, double gain, double phase_shift)
{
    m_irsLookupTable[{in_angle, out_angle}] = {gain, phase_shift};
}

IrsEntry
IrsLookupTable::GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const
{
    auto key = std::make_pair(in_angle, out_angle);
    auto it = m_irsLookupTable.find(key);

    if (it != m_irsLookupTable.end())
    {
        return it->second;
    }
    else
    {
        NS_FATAL_ERROR("Entry in IrsLookupTable with in_angle: " << in_angle << " and out_angle: "
                                                                 << out_angle << " not Found.");
    }
}
} // namespace ns3
