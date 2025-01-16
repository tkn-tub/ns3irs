/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "irs-lookup-table.h"

#include "ns3/fatal-error.h"

#include <cstdint>
#include <ostream>

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

IrsLookupTable::~IrsLookupTable()
{
    m_irsLookupTable.clear();
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
