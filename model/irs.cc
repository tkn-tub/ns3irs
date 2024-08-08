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

#include "irs.h"

#include "ns3/pointer.h"
#include <cstdint>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Irs);

TypeId
Irs::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::Irs")
            .SetParent<Object>()
            .SetGroupName("Irs")
            .AddConstructor<Irs>()
            .AddAttribute("LookupTable",
                          "The lookup table for the IRS.",
                          TypeId::ATTR_SET | TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&Irs::SetLookupTable, &Irs::GetLookupTable),
                          MakePointerChecker<IrsLookupTable>())
            .AddAttribute("Direction",
                          "The direction of the IRS.",
                          VectorValue(Vector(1.0, 0.0, 0.0)),
                          MakeVectorAccessor(&Irs::SetDirection, &Irs::GetDirection),
                          MakeVectorChecker());
    return tid;
}

Irs::Irs()
{
}

Irs::~Irs()
{
}

IrsEntry
Irs::GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const
{
    return m_irsLookupTable->GetIrsEntry(in_angle, out_angle);
}

void
Irs::SetLookupTable(const Ptr<IrsLookupTable> table)
{
    m_irsLookupTable = table;
}

Ptr<IrsLookupTable>
Irs::GetLookupTable() const
{
    return m_irsLookupTable;
}

void
Irs::SetDirection(const Vector& direction)
{
    // Normalize the vector
    double m = direction.GetLength();
    if (m != 0) {
        m_direction = Vector3D(direction.x / m, direction.y / m, direction.z / m);
    } else {
        m_direction = direction;
    }
}

Vector
Irs::GetDirection() const
{
    return m_direction;
}
} // namespace ns3
