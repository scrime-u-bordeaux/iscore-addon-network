
#include <algorithm>

#include <Network/Group/Group.hpp>
#include <Network/Group/GroupManager.hpp>
#include "RemoveClientFromGroup.hpp"
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/path/ObjectPath.hpp>

namespace Network
{
namespace Command
{
RemoveClientFromGroup::RemoveClientFromGroup(ObjectPath&& groupMgrPath, Id<Client> client, Id<Group> group):
    m_path{std::move(groupMgrPath)},
    m_client{client},
    m_group{group}
{
}

void RemoveClientFromGroup::undo(const score::DocumentContext& ctx) const
{
    m_path.find<GroupManager>(ctx).group(m_group)->addClient(m_client);
}

void RemoveClientFromGroup::redo(const score::DocumentContext& ctx) const
{
    m_path.find<GroupManager>(ctx).group(m_group)->removeClient(m_client);
}

void RemoveClientFromGroup::serializeImpl(DataStreamInput& s) const
{
    s << m_path << m_client << m_group;
}

void RemoveClientFromGroup::deserializeImpl(DataStreamOutput& s)
{
    s >> m_path >> m_client >> m_group;
}
}
}
