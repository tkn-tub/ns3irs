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
