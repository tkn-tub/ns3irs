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

#include "irs-lookup-helper.h"

#include "ns3/abort.h"
#include "ns3/irs-lookup-table.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/object.h"
#include "ns3/type-id.h"

#include <fstream>
#include <sstream>
#include <stdint.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IrsLookupHelper");

IrsLookupHelper::IrsLookupHelper()
{
    m_irs.SetTypeId("ns3::IrsLookupModel");
    m_direction = Vector(1, 0, 0);
}

IrsLookupHelper::~IrsLookupHelper()
{
}

void
IrsLookupHelper::Install(Ptr<Node> node) const
{
    Ptr<Object> object = node;
    Ptr<IrsLookupModel> irs = object->GetObject<IrsLookupModel>();
    if (!irs)
    {
        irs = m_irs.Create()->GetObject<IrsLookupModel>();
        NS_ABORT_MSG_IF(!irs,
                        "The requested irs model is not a irs model: \""
                            << m_irs.GetTypeId().GetName() << "\"");
        NS_LOG_DEBUG("node=" << object << ", irs=" << irs);
        object->AggregateObject(irs);
    }

    NS_ABORT_MSG_IF(
        !m_irsLookupTable,
        "No Lookup Table for IRS set. Please set a Lookup Table before installing the IRS.");

    irs->SetLookupTable(m_irsLookupTable);
    irs->SetDirection(m_direction);
}

void
IrsLookupHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    Install(node);
}

void
IrsLookupHelper::Install(NodeContainer container) const
{
    for (auto i = container.Begin(); i != container.End(); ++i)
    {
        Install(*i);
    }
}

void
IrsLookupHelper::InstallAll() const
{
    Install(NodeContainer::GetGlobal());
}

void
IrsLookupHelper::SetLookupTable(std::string filename)
{
    // Load Lookup Table from csv file
    std::ifstream file(filename);
    NS_ABORT_MSG_IF(!file.is_open(), "IRS Lookup Table file not found.");

    // Create the lookup table
    m_irsLookupTable = CreateObject<IrsLookupTable>();

    std::string line;
    // Skip the header
    std::getline(file, line);

    // Read the data
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string item;
        uint8_t in_angle, out_angle;
        double gain, phase_shift;

        // Read each value separated by comma
        std::getline(ss, item, ',');
        in_angle = std::stoi(item);
        std::getline(ss, item, ',');
        out_angle = std::stoi(item);
        std::getline(ss, item, ',');
        gain = std::stod(item);
        std::getline(ss, item, ',');
        phase_shift = std::stod(item);

        // Insert into the map
        m_irsLookupTable->Insert(in_angle, out_angle, gain, phase_shift);
    }

    file.close();
}

void
IrsLookupHelper::SetLookupTable(Ptr<IrsLookupTable> table)
{
    m_irsLookupTable = table;
}

void
IrsLookupHelper::SetDirection(Vector direction)
{
    m_direction = direction;
}
} // namespace ns3
