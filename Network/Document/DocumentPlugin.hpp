#pragma once
#include <Network/Client/Client.hpp>
#include <Network/Document/Execution/SyncMode.hpp>

#include <score/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/actions/Action.hpp>
#include <core/document/Document.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <Network/Group/Group.hpp>
#include <Network/Group/GroupManager.hpp>
#include <score_addon_network_export.h>
#include <hopscotch_set.h>
#include <QObject>
#include <vector>
#include <functional>

class DataStream;
class JSONObject;
class QWidget;
struct VisitorVariant;
namespace Scenario {
  class IntervalModel;
  class TimeSyncModel;
}
namespace Execution {
class EventComponent;
class IntervalComponent;
class TimeSyncComponent;
}
namespace Network
{
class Group;
class NetworkDocumentPlugin;
}
UUID_METADATA(
    ,
    score::DocumentPluginFactory,
    Network::NetworkDocumentPlugin,
    "58c9e19a-fde3-47d0-a121-35853fec667d")

namespace Network
{

struct NetworkExpressionData
{
  NetworkExpressionData(Execution::TimeSyncComponent& c): component{c} {}
  Execution::TimeSyncComponent& component;

  //! Will fill itself with received messages
  score::hash_map<Id<Client>, optional<bool>> values;
  tsl::hopscotch_set<Id<Client>> previousCompleted;

  Id<Group> thisGroup;
  std::vector<Id<Group>> prevGroups, nextGroups;
  ExpressionPolicy pol;
  SyncMode sync;
  // Trigger : they all have to be set, and true
  // Event : when they are all set, the truth value of each is taken.

  // Expression observation has to be done on the network.
  // Saved in the network components ? For now in the document plugin?

  bool ready(std::size_t count_ready, std::size_t num_clients)
  {
    switch(pol)
    {
      case ExpressionPolicy::OnFirst:
        return count_ready == 1;
      case ExpressionPolicy::OnAll:
        return count_ready >= num_clients; // todo what happens if someone disconnects before sending.
      case ExpressionPolicy::OnMajority:
        return count_ready > (num_clients / 2);
      default:
        return false;
    }
  }
};

class Session;
class GroupManager;
class GroupMetadata;
class SCORE_ADDON_NETWORK_EXPORT EditionPolicy : public QObject
{
public:
  using QObject::QObject;
  virtual ~EditionPolicy();
  virtual Session* session() const = 0;
  virtual void play() = 0;
  virtual void stop() = 0;
};

class SCORE_ADDON_NETWORK_EXPORT ExecutionPolicy : public QObject
{
public:
  using QObject::QObject;
  virtual ~ExecutionPolicy();
};

class SCORE_ADDON_NETWORK_EXPORT NetworkDocumentPlugin final :
    public score::SerializableDocumentPlugin
{
  W_OBJECT(NetworkDocumentPlugin)

  SCORE_SERIALIZE_FRIENDS
  MODEL_METADATA_IMPL(NetworkDocumentPlugin)
  public:
    NetworkDocumentPlugin(
      const score::DocumentContext& ctx,
      EditionPolicy* policy,
      Id<score::DocumentPlugin> id,
      QObject* parent);

  virtual ~NetworkDocumentPlugin();

  // Loading has to be in two steps since the plugin policy is different from the client
  // and server.
  template<typename Impl>
  NetworkDocumentPlugin(
      const score::DocumentContext& ctx,
      Impl& vis,
      QObject* parent):
    score::SerializableDocumentPlugin{ctx, vis, parent}
  {
    vis.writeTo(*this);
  }

  void setEditPolicy(EditionPolicy*);
  void setExecPolicy(ExecutionPolicy* e);

  GroupManager& groupManager() const
  { return *m_groups; }

  EditionPolicy &policy() const
  { return *m_policy; }

  struct NonCompensated {
  score::hash_map<Path<Scenario::TimeSyncModel>, std::function<void(Id<Client>)>> trigger_evaluation_entered;
  score::hash_map<Path<Scenario::TimeSyncModel>, std::function<void(Id<Client>, bool)>> trigger_evaluation_finished;
  score::hash_map<Path<Scenario::TimeSyncModel>, std::function<void(Id<Client>)>> trigger_triggered;
  score::hash_map<Path<Scenario::IntervalModel>, std::function<void(const Id<Client>&, double)>> interval_speed_changed;
  score::hash_map<Path<Scenario::TimeSyncModel>, NetworkExpressionData> network_expressions;
  } noncompensated;

  struct Compensated {
      score::hash_map<Path<Scenario::TimeSyncModel>, std::function<void(Id<Client>, qint64)>> trigger_triggered;
  } compensated;

  void on_stop();

  void sessionChanged() W_SIGNAL(sessionChanged);

private:
  EditionPolicy* m_policy{};
  ExecutionPolicy* m_exec{};
  GroupManager* m_groups{};

};

class DocumentPluginFactory :
    public score::DocumentPluginFactory
{
  SCORE_CONCRETE("58c9e19a-fde3-47d0-a121-35853fec667d")

  public:
    score::DocumentPlugin* load(
      const VisitorVariant& var,
      score::DocumentContext& doc,
      QObject* parent) override;
};
}

