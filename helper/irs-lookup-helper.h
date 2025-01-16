/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jakob Rühlow <ruehlow@tu-berlin.de>
 *
 */

#ifndef IRS_LOOKUP_HELPER_H
#define IRS_LOOKUP_HELPER_H

#include "irs-lookup-table.h"

#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/object-factory.h"
#include "ns3/ptr.h"
#include "ns3/vector.h"

#include <string>

namespace ns3
{

/**
 * @class IrsLookupHelper
 * @brief Helper class to manage IRS lookup table installation and configuration.
 *
 * This class simplifies the process of installing IRS lookup tables on nodes
 * and setting related parameters such as direction and table source.
 */
class IrsLookupHelper
{
  public:
    IrsLookupHelper();
    ~IrsLookupHelper();

    /**
     * @brief Installs an IRS model with the configured lookup table on a specific node.
     * @param node A pointer to the node where the IRS model will be installed
     */
    void Install(Ptr<Node> node) const;

    /**
     * @brief Installs an IRS model with the configured lookup table on a node by name.
     * @param nodeName The name of the node where the IRS model will be installed
     */
    void Install(std::string nodeName) const;

    /**
     * @brief Installs an IRS model with the configured lookup table on a group of nodes.
     * @param container A container of nodes where the IRS model will be installed
     */
    void Install(NodeContainer container) const;

    /**
     * @brief Installs an IRS model with the configured lookup table on all nodes.
     */
    void InstallAll() const;

    /**
     * @brief Sets the IRS lookup table from a given csv file.
     * @param filename The file path containing the lookup table data
     */
    void SetLookupTable(std::string filename);

    /**
     * @brief Sets the IRS lookup table from a preloaded object.
     * @param table A pointer to an IRS lookup table object
     */
    void SetLookupTable(Ptr<IrsLookupTable> table);

    /**
     * @brief Sets the direction vector for IRS configuration.
     * @param direction A vector indicating the direction
     */
    void SetDirection(Vector direction);

  private:
    ObjectFactory m_irs;
    Ptr<IrsLookupTable> m_irsLookupTable;
    Vector m_direction;
};

} // namespace ns3

#endif /* IRS_LOOKUP_HELPER_H */
