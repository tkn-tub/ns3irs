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

#ifndef IRS_H
#define IRS_H

#include "irs-model.h"
#include "ns3/irs-lookup-helper.h"
#include "ns3/object.h"
#include "ns3/vector.h"

/**
 * \defgroup irs Description of the irs
 */
namespace ns3
{

class IrsLookupModel : public IrsModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    IrsLookupModel();

    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const override;
    IrsEntry GetIrsEntry(Angles in, Angles out, double lambda) const override;

    void SetLookupTable(Ptr<IrsLookupTable> table);
    Ptr<IrsLookupTable> GetLookupTable() const;

  private:
    Ptr<IrsLookupTable> m_irsLookupTable;
};

} // namespace ns3

#endif /* IRS_H */
