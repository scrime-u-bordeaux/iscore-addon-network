#pragma once
#include <Network/Client/Client.hpp>
#include <Network/Document/Execution/SyncMode.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>

#include <iscore/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/actions/Action.hpp>
#include <core/document/Document.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <Engine/Executor/TimeNodeComponent.hpp>

#include <QObject>
#include <vector>
#include <functional>

class DataStream;
class JSONObject;
class QWidget;
struct VisitorVariant;
namespace iscore
{
class Document;
}
namespace Network
{
class NetworkDocumentPlugin;
}
UUID_METADATA(
    ,
    iscore::DocumentPluginFactory,
    Network::NetworkDocumentPlugin,
    "58c9e19a-fde3-47d0-a121-35853fec667d")

ISCORE_DECLARE_ACTION(NetworkPlay, "&Play (Network)", Network, QKeySequence::UnknownKey)
namespace Network
{

struct NetworkExpressionData
{
  Engine::Execution::TimeNodeComponent& component;

  iscore::hash_map<Id<Client>, optional<bool>> values;
  ExpressionPolicy pol;
  SyncMode sync;
  // Trigger : they all have to be set, and true
  // Event : when they are all set, the truth value of each is taken.

  // Expression observation has to be done on the network.
  // Saved in the network components ? For now in the document plugin?

};

class Session;
class GroupManager;
class GroupMetadata;
class EditionPolicy : public QObject
{
public:
  using QObject::QObject;
  virtual Session* session() const = 0;
  virtual void play() = 0;
};

class ExecutionPolicy : public QObject
{
public:
  using QObject::QObject;
};

class NetworkDocumentPlugin final :
    public iscore::SerializableDocumentPlugin
{
  Q_OBJECT

  ISCORE_SERIALIZE_FRIENDS
  SERIALIZABLE_MODEL_METADATA_IMPL(NetworkDocumentPlugin)
  public:
    NetworkDocumentPlugin(
      const iscore::DocumentContext& ctx,
      EditionPolicy* policy,
      Id<iscore::DocumentPlugin> id,
      QObject* parent);

  // Loading has to be in two steps since the plugin policy is different from the client
  // and server.
  template<typename Impl>
  NetworkDocumentPlugin(
      const iscore::DocumentContext& ctx,
      Impl& vis,
      QObject* parent):
    iscore::SerializableDocumentPlugin{ctx, vis, parent}
  {
    vis.writeTo(*this);
  }

  void setPolicy(EditionPolicy*);
  void setExecPolicy(ExecutionPolicy* e) { m_exec = e; e->setParent(this); }

  GroupManager& groupManager() const
  { return *m_groups; }

  EditionPolicy &policy() const
  { return *m_policy; }

  iscore::hash_map<Path<Scenario::TimeNodeModel>, std::function<void(Id<Client>)>> trigger_evaluation_entered;
  iscore::hash_map<Path<Scenario::TimeNodeModel>, std::function<void(Id<Client>, bool)>> trigger_evaluation_finished;
  iscore::hash_map<Path<Scenario::TimeNodeModel>, std::function<void(Id<Client>)>> trigger_triggered;
  iscore::hash_map<Path<Scenario::TimeNodeModel>, NetworkExpressionData> trigger_expression_true;

signals:
  void sessionChanged();

private:
  EditionPolicy* m_policy{};
  ExecutionPolicy* m_exec{};
  GroupManager* m_groups{};

};

class DocumentPluginFactory :
    public iscore::DocumentPluginFactory
{
  ISCORE_CONCRETE("58c9e19a-fde3-47d0-a121-35853fec667d")

  public:
    iscore::DocumentPlugin* load(
      const VisitorVariant& var,
      iscore::DocumentContext& doc,
      QObject* parent) override;
};
}

