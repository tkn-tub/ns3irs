#ifndef IRS_LOOKUP_HELPER_H
#define IRS_LOOKUP_HELPER_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include <cstdint>

struct IrsEntry
{
    double gain;
    double phase_shift;
};

struct hash_tuple
{
    std::size_t operator()(const std::pair<uint8_t, uint8_t>& p) const
    {
        // Combine the two 16-bit integers into a 32-bit integer
        uint16_t combined = (((uint16_t) p.first) << 8) | ((uint16_t) p.second);

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

    void Insert(uint8_t in_angle, uint8_t out_angle, double gain, double phase_shift);

    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const;

  private:
    std::unordered_map<std::pair<uint8_t, uint8_t>, IrsEntry, hash_tuple> m_irsLookupTable;
};
} // namespace ns3
#endif // IRS_LOOKUP_HELPER_H
