#pragma once
#include <Network/Group/Commands/DistributedScenarioCommandFactory.hpp>
#include <score/tools/std/Optional.hpp>
#include <score/command/Command.hpp>
#include <score/model/path/ObjectPath.hpp>

#include <score/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Network
{
class Client;
class Group;
namespace Command
{
class AddClientToGroup : public score::Command
{
        SCORE_COMMAND_DECL(DistributedScenarioCommandFactoryName(), AddClientToGroup, "AddClientToGroup")
    public:
        AddClientToGroup(ObjectPath&& groupMgrPath,
                         Id<Client> client,
                         Id<Group> group);

        void undo(const score::DocumentContext& ctx) const override;
        void redo(const score::DocumentContext& ctx) const override;

        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        ObjectPath m_path;
        Id<Client> m_client;
        Id<Group> m_group;
};
}
}
