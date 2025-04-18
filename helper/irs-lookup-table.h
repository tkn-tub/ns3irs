/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#ifndef IRS_LOOKUP_TABLE_H
#define IRS_LOOKUP_TABLE_H

#include "ns3/object.h"
#include "ns3/type-id.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>

struct IrsEntry
{
    double gain;
    double phase_shift;
};

/**
 * @struct hash_tuple
 * @brief A custom hash function for std::pair<uint8_t, uint8_t>.
 *
 * Combines two 8-bit integers into a 16-bit value and hashes it,
 * using a simple hash function for efficient lookups.
 */
struct hash_tuple
{
    std::size_t operator()(const std::pair<uint8_t, uint8_t>& p) const
    {
        // Combine the two 8-bit integers into a 16-bit integer
        uint16_t combined = (static_cast<uint16_t>(p.first) << 8) | static_cast<uint16_t>(p.second);

        // Use a standard hash function on the combined value
        return std::hash<uint16_t>{}(combined);
    }
};

namespace ns3
{

/**
 * @class IrsLookupTable
 * @brief Represents a lookup table for IRS entries.
 *
 * Stores precomputed gain and phase shift values for given input and output angles.
 */
class IrsLookupTable : public Object
{
  public:
    /**
     * @brief Get the TypeId of this class.
     * @return the TypeId
     */
    static TypeId GetTypeId();
    IrsLookupTable();
    ~IrsLookupTable() override;

    /**
     * @brief Inserts an IRS entry into the lookup table.
     * @param in_angle Input angle as an 8-bit value
     * @param out_angle Output angle as an 8-bit value
     * @param gain Gain value associated with the angles
     * @param phase_shift Phase shift value associated with the angles
     */
    void Insert(uint8_t in_angle, uint8_t out_angle, double gain, double phase_shift);

    /**
     * @brief Retrieves an IRS entry from the lookup table.
     * @param in_angle Input angle as an 8-bit value
     * @param out_angle Output angle as an 8-bit value
     * @return The IRS entry associated with the angles
     */
    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const;

  private:
    std::unordered_map<std::pair<uint8_t, uint8_t>, IrsEntry, hash_tuple> m_irsLookupTable;
};

} // namespace ns3

#endif // IRS_LOOKUP_TABLE_H
