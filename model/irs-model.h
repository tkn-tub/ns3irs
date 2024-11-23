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
 * \defgroup irs Intelligent Reflecting Surface (IRS) Models
 * This module provides base classes and utilities for simulating Intelligent Reflecting Surfaces
 * (IRS).
 */

namespace ns3
{

/**
 * \class IrsModel
 * \brief Abstract base class for IRS models.
 *
 * The \c IrsModel class defines the interface for all IRS models.
 * It provides methods for retrieving IRS entries based on input/output angles and wavelength,
 * as well as for setting and getting the direction of the IRS.
 */
class IrsModel : public Object
{
  public:
    /**
     * \brief Get the TypeId of this class.
     * \return The object TypeId.
     */
    static TypeId GetTypeId();

    IrsModel();
    ~IrsModel() override;
    IrsModel(const IrsModel&) = delete;
    IrsModel& operator=(const IrsModel&) = delete;

    /**
     * \brief Retrieve an IRS entry based on input and output angles.
     * \param in_angle Input angle (azimuth) in degrees.
     * \param out_angle Output angle (azimuth) in degrees.
     * \return The corresponding \c IrsEntry object.
     */
    virtual IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const = 0;

    /**
     * \brief Retrieve an IRS entry based on angles and wavelength.
     * \param in Input angles (azimuth and elevation in radians) as an \c Angles object.
     * \param out Output angles (azimuth and elevation in radians) as an \c Angles object.
     * \param lambda Wavelength of the signal in meters.
     * \return The corresponding \c IrsEntry object.
     */
    virtual IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const = 0;

    /**
     * \brief Set the direction of the IRS.
     * \param direction A \c Vector specifying the direction of the IRS in 3D space.
     *
     * The direction vector defines the orientation of the IRS.
     */
    void SetDirection(const Vector& direction);

    /**
     * \brief Get the direction of the IRS.
     * \return A \c Vector representing the current direction of the IRS in 3D space.
     *
     * This method retrieves the direction vector previously set using \c SetDirection.
     */
    Vector GetDirection() const;

  private:
    Vector m_direction;
};

} // namespace ns3

#endif /* IRS_MODEL_H */
