/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#include "irs-model.h"

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/object-base.h"

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

IrsModel::~IrsModel()
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
