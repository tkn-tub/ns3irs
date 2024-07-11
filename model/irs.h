#ifndef IRS_H
#define IRS_H

#include "../helper/irs-lookup-helper.h"

#include "ns3/object.h"
#include "ns3/vector.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup irs Description of the irs
 */
namespace ns3
{

class Irs : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    Irs();
    ~Irs() override;

    IrsEntry GetIrsEntry(uint8_t in_angle, uint8_t out_angle) const;

    void SetLookupTable(Ptr<IrsLookupTable> table);
    Ptr<IrsLookupTable> GetLookupTable() const;

    void SetDirection(const Vector& direction);
    Vector GetDirection() const;

  private:
    Ptr<IrsLookupTable> m_irsLookupTable;
    Vector m_direction;
};

} // namespace ns3

#endif /* IRS_H */
