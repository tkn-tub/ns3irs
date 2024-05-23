#ifndef IRS_HELPER_H
#define IRS_HELPER_H

#include "ns3/irs.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "lookup-structs.h"

#include <cstdint>
#include <unordered_map>

namespace ns3
{

class IrsHelper
{
  public:
    IrsHelper();
    ~IrsHelper();

    void Install(Ptr<Node> node) const;
    void Install(std::string nodeName) const;
    void Install(NodeContainer container) const;
    void InstallAll() const;

    void SetLookupTable(std::string filename);
    void SetLookupTable(Ptr<std::unordered_map<std::pair<uint8_t, uint8_t>, IrsEntry, hash_tuple>> table);
  private:
    std::string m_filename;
    ObjectFactory m_irsFactory;
    std::unordered_map<std::pair<uint8_t, uint8_t>, IrsEntry, hash_tuple> m_irsLookupTable;
};
}

#endif /* IRS_HELPER_H */
