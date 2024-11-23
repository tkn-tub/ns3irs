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

#ifndef IRS_LOOKUP_MODEL_H
#define IRS_LOOKUP_MODEL_H

#include "irs-model.h"

#include "ns3/angles.h"
#include "ns3/irs-lookup-table.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"

#include <stdint.h>

/**
 * \defgroup irs Intelligent Reflecting Surface (IRS) Models
 * This module provides base classes and utilities for simulating Intelligent Reflecting Surfaces
 */

namespace ns3
{

/**
 * \class IrsLookupModel
 * \brief Implements an IRS model using a precomputed lookup table.
 *
 * The \c IrsLookupModel class is a concrete implementation of the \c IrsModel interface.
 * It uses a precomputed lookup table to provide reflection coefficients or other IRS parameters
 * for specific input and output angles.
 */
class IrsLookupModel : public IrsModel
{
  public:
    /**
     * \brief Get the TypeId of this class.
     * \return The object TypeId
     */
    static TypeId GetTypeId();
    IrsLookupModel();

    /**
     * \brief Get an IRS entry for the specified input and output angles.
     * \param in_angle Input angle in degrees.
     * \param out_angle Output angle in degrees.
     * \return The corresponding \c IrsEntry from the lookup table.
     *
     * This method retrieves a precomputed IRS entry for the given input and output angles.
     */
    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const override;

    /**
     * \brief Get an IRS entry for the specified input and output angles.
     *
     * Is not frequency dependent - just looks up \c IrsEntry using the azimuth angles
     */
    IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const override;

    /**
     * \brief Set the lookup table.
     * \param table A pointer to the \c IrsLookupTable.
     */
    void SetLookupTable(Ptr<IrsLookupTable> table);

    /**
     * \brief Get the lookup table associated with this model.
     * \return A pointer to the current \c IrsLookupTable.
     */
    Ptr<IrsLookupTable> GetLookupTable() const;

  private:
    Ptr<IrsLookupTable> m_irsLookupTable;
};
} // namespace ns3

#endif /* IRS_LOOKUP_MODEL_H */
