#include "irs-helper.h"

#include "ns3/names.h"

#include <cstdint>
#include <fstream>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IrsHelper");

IrsHelper::IrsHelper()
{
    m_irs.SetTypeId("ns3::Irs");
    m_direction = Vector(0, 0, 0);
}

IrsHelper::~IrsHelper()
{
}

void
IrsHelper::Install(Ptr<Node> node) const
{
    Ptr<Object> object = node;
    Ptr<Irs> irs = object->GetObject<Irs>();
    if (!irs)
    {
        irs = m_irs.Create()->GetObject<Irs>();
        if (!irs)
        {
            NS_FATAL_ERROR("The requested irs model is not a irs model: \""
                           << m_irs.GetTypeId().GetName() << "\"");
        }
        NS_LOG_DEBUG("node=" << object << ", irs=" << irs);
        object->AggregateObject(irs);
    }
    if (!m_irsLookupTable)
    {
        NS_FATAL_ERROR(
            "No Lookup Table for IRS set. Please set a Lookup Table before installing the IRS.");
    }
    irs->SetLookupTable(m_irsLookupTable);
    irs->SetDirection(m_direction);
}

void
IrsHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    Install(node);
}

void
IrsHelper::Install(NodeContainer container) const
{
    for (auto i = container.Begin(); i != container.End(); ++i)
    {
        Install(*i);
    }
}

void
IrsHelper::InstallAll() const
{
    Install(NodeContainer::GetGlobal());
}

void
IrsHelper::SetLookupTable(std::string filename)
{
    // Load Lookup Table from csv file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        NS_FATAL_ERROR("IRS Lookup Table file not found.");
    }

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
IrsHelper::SetLookupTable(Ptr<IrsLookupTable> table)
{
    m_irsLookupTable = table;
}

void
IrsHelper::SetDirection(Vector direction)
{
    m_direction = direction;
}
} // namespace ns3
