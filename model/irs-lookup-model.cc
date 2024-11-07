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

#include "irs-lookup-model.h"

#include "irs-model.h"

#include "ns3/object-base.h"
#include "ns3/pointer.h"

#include <cstdint>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(IrsLookupModel);

TypeId
IrsLookupModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::IrsLookupModel")
                            .SetParent<IrsModel>()
                            .SetGroupName("IrsModel")
                            .AddConstructor<IrsLookupModel>()
                            .AddAttribute("LookupTable",
                                          "The lookup table for the IRS.",
                                          TypeId::ATTR_SET | TypeId::ATTR_GET,
                                          PointerValue(),
                                          MakePointerAccessor(&IrsLookupModel::SetLookupTable,
                                                              &IrsLookupModel::GetLookupTable),
                                          MakePointerChecker<IrsLookupTable>());
    return tid;
}

IrsLookupModel::IrsLookupModel()
{
}

IrsEntry
IrsLookupModel::GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const
{
    return m_irsLookupTable->GetIrsEntry(in_angle, out_angle);
}

IrsEntry
IrsLookupModel::GetIrsEntry(Angles in, Angles out, double lambda) const
{
    // this function should generally not be called
    return GetIrsEntry(RadiansToDegrees(in.GetAzimuth()), RadiansToDegrees(out.GetAzimuth()));
}

void
IrsLookupModel::SetLookupTable(const Ptr<IrsLookupTable> table)
{
    m_irsLookupTable = table;
}

Ptr<IrsLookupTable>
IrsLookupModel::GetLookupTable() const
{
    return m_irsLookupTable;
}

} // namespace ns3
