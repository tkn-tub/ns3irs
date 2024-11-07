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

#ifndef IRS_MODEL_H
#define IRS_MODEL_H

#include "ns3/angles.h"
#include "ns3/irs-lookup-table.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include "ns3/vector.h"

#include <stdint.h>

/**
 * \defgroup irs Description of the irs
 */
namespace ns3
{

class IrsModel : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    IrsModel();
    ~IrsModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    IrsModel(const IrsModel&) = delete;
    IrsModel& operator=(const IrsModel&) = delete;

    virtual IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const = 0;
    virtual IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const = 0;

    void SetDirection(const Vector& direction);
    Vector GetDirection() const;

  private:
    Vector m_direction;
};

} // namespace ns3

#endif /* IRS_MODEL_H */
