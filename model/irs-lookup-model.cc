/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
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
    NS_ABORT_MSG_UNLESS(table, "Lookup table can not be null.");
    m_irsLookupTable = table;
}

Ptr<IrsLookupTable>
IrsLookupModel::GetLookupTable() const
{
    return m_irsLookupTable;
}

} // namespace ns3
