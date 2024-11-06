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

#ifndef IRS_LOOKUP_HELPER_H
#define IRS_LOOKUP_HELPER_H

#include "ns3/irs-model.h"
#include "ns3/object.h"
#include "ns3/ptr.h"

#include <cstdint>

struct hash_tuple
{
    std::size_t operator()(const std::pair<uint8_t, uint8_t>& p) const
    {
        // Combine the two 16-bit integers into a 32-bit integer
        uint16_t combined = (((uint16_t)p.first) << 8) | ((uint16_t)p.second);

        // Use a simple hash function, e.g., MurmurHash3 finalizer
        return std::hash<uint16_t>{}(combined);
    }
};

namespace ns3
{
class IrsLookupTable : public Object
{
  public:
    static TypeId GetTypeId();

    IrsLookupTable();
    ~IrsLookupTable() override;

    void Insert(uint8_t in_angle, uint8_t out_angle, double gain, double phase_shift);

    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const;

  private:
    std::unordered_map<std::pair<uint8_t, uint8_t>, IrsEntry, hash_tuple> m_irsLookupTable;
};
} // namespace ns3
#endif // IRS_LOOKUP_HELPER_H
