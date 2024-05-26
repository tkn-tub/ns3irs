#ifndef IRS_LOOKUP_HELPER_H
#define IRS_LOOKUP_HELPER_H

#include "ns3/object.h"
#include "ns3/ptr.h"

struct IrsEntry
{
    double gain;
    double phase_shift;
};

struct hash_tuple
{
    std::size_t operator()(const std::pair<uint16_t, uint16_t>& p) const
    {
        // Combine the two 16-bit integers into a 32-bit integer
        uint32_t combined = (((uint32_t) p.first) << 16) | ((uint32_t) p.second);

        // Use a simple hash function, e.g., MurmurHash3 finalizer
        return std::hash<uint32_t>{}(combined);
    }
};

namespace ns3
{
class IrsLookupTable : public Object
{
  public:
    static TypeId GetTypeId();

    IrsLookupTable();

    void Insert(uint16_t in_angle, uint16_t out_angle, double gain, double phase_shift);

    IrsEntry GetIrsEntry(uint16_t in_angle, uint16_t out_angle) const;

  private:
    std::unordered_map<std::pair<uint16_t, uint16_t>, IrsEntry, hash_tuple> m_irsLookupTable;
};
} // namespace ns3
#endif // IRS_LOOKUP_HELPER_H
