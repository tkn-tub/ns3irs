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
