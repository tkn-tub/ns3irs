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

#include "irs-model.h"

#include "ns3/pointer.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IrsModel");
NS_OBJECT_ENSURE_REGISTERED(IrsModel);

TypeId
IrsModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::IrsModel")
            .SetParent<Object>()
            .SetGroupName("IrsModel")
            .AddAttribute("Direction",
                          "The direction of the IRS.",
                          VectorValue(Vector(1.0, 0.0, 0.0)),
                          MakeVectorAccessor(&IrsModel::SetDirection, &IrsModel::GetDirection),
                          MakeVectorChecker());
    return tid;
}

IrsModel::IrsModel()
{
}

void
IrsModel::SetDirection(const Vector& direction)
{
    double m = direction.GetLength();
    NS_ABORT_MSG_IF(m == 0, "Direction vector cannot have zero length.");
    // Normalize the vector
    m_direction = Vector3D(direction.x / m, direction.y / m, direction.z / m);
}

Vector
IrsModel::GetDirection() const
{
    return m_direction;
}
} // namespace ns3
