#ifndef IRS_HELPER_H
#define IRS_HELPER_H

#include "irs-lookup-helper.h"

#include "ns3/irs.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

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
    void SetLookupTable(Ptr<IrsLookupTable> table);

    void SetDirection(Vector direction);
  private:
    ObjectFactory m_irs;
    Ptr<IrsLookupTable> m_irsLookupTable;
    Vector m_direction;
};
}

#endif /* IRS_HELPER_H */
