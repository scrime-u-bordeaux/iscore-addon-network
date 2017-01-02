#include <Network/Communication/MessageMapper.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <QByteArray>
#include <QDataStream>
#include <algorithm>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Network/Group/Group.hpp>
#include <Network/Group/GroupMetadata.hpp>

#include <Engine/ApplicationPlugin.hpp>
#include "MasterPolicy.hpp"
#include <Network/Group/GroupManager.hpp>
#include <Network/Communication/NetworkMessage.hpp>
#include <Network/Document/Execution/BasicPruner.hpp>

#include <iscore/application/ApplicationContext.hpp>
#include <core/command/CommandStack.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/locking/ObjectLocker.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore/tools/Todo.hpp>
#include <iscore/actions/ActionManager.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>
#include <Network/Session/MasterSession.hpp>

namespace Network
{

class Client;

MasterTimekeep::MasterTimekeep(MasterSession& s):
  m_session{s}
{
  connect(&m_timer, &QTimer::timeout,
          this, &MasterTimekeep::ping_all);
  m_timer.setInterval(1000);
  m_timer.start();

  con(m_session, &MasterSession::clientAdded,
      this, [=] (auto client) {
    m_timestamps.insert({client->id(), ClientTimes{}});
  });
  // TODO clientRemoved
}

auto us(const std::chrono::nanoseconds u) { return std::chrono::duration_cast<std::chrono::microseconds>(u).count(); }

void MasterTimekeep::ping_all()
{
  auto t = clk::now().time_since_epoch();
  m_session.broadcastToAllClients(m_session.makeMessage("/ping"));

  auto b = m_timestamps.begin();
  auto e = m_timestamps.end();
  for(auto it = b; it != e; ++it)
  {
    it.value().last_sent = t;
  }
}


void MasterTimekeep::on_pong(NetworkMessage m)
{
  auto pong_date = clk::now().time_since_epoch();

  Id<Client> client_id{m.clientId};
  QDataStream reader(m.data);

  qint64 ns;
  reader >> ns;

  auto it = m_timestamps.find(client_id);
  if(it != m_timestamps.end())
  {
    ClientTimes& times = it.value();
    times.last_received = pong_date;
    times.roundtrip_latency = times.last_received - times.last_sent;

    // Note : here we just assume that the half-trip latency is half the round trip latency.
    times.clock_difference = std::chrono::nanoseconds(ns) - (times.last_sent + times.roundtrip_latency / 2);
  }
}

MasterNetworkPolicy::MasterNetworkPolicy(
    MasterSession* s,
    const iscore::DocumentContext& c):
  m_session{s},
  m_ctx{c},
  m_keep{*s}
{
  auto& stack = c.document.commandStack();

  /////////////////////////////////////////////////////////////////////////////
  /// From the master to the clients
  /////////////////////////////////////////////////////////////////////////////
  con(stack, &iscore::CommandStack::localCommand,
      this, [=] (iscore::Command* cmd)
  {
    m_session->broadcastToAllClients(
          m_session->makeMessage("/command/new",iscore::CommandData{*cmd}));
  });

  // Undo-redo
  con(stack, &iscore::CommandStack::localUndo,
      this, [&] ()
  { m_session->broadcastToAllClients(m_session->makeMessage("/command/undo")); });
  con(stack, &iscore::CommandStack::localRedo,
      this, [&] ()
  { m_session->broadcastToAllClients(m_session->makeMessage("/command/redo")); });
  con(stack, &iscore::CommandStack::localIndexChanged,
      this, [&] (int32_t idx)
  {
    m_session->broadcastToAllClients(m_session->makeMessage("/command/index", idx));
  });

  // Lock - unlock
  con(c.objectLocker, &iscore::ObjectLocker::lock,
      this, [&] (QByteArray arr)
  { m_session->broadcastToAllClients(m_session->makeMessage("/lock", arr)); });
  con(c.objectLocker, &iscore::ObjectLocker::unlock,
      this, [&] (QByteArray arr)
  { m_session->broadcastToAllClients(m_session->makeMessage("/unlock", arr)); });

  // Play
  auto& play_act = c.app.actions.action<Actions::NetworkPlay>();
  connect(play_act.action(), &QAction::triggered,
          this, [&] {
    m_session->broadcastToAllClients(m_session->makeMessage("/play"));
    play();
  });


  /////////////////////////////////////////////////////////////////////////////
  /// From a client to the master and the other clients
  /////////////////////////////////////////////////////////////////////////////
  s->mapper().addHandler("/command/new", [&] (NetworkMessage m)
  {
    iscore::CommandData cmd;
    DataStreamWriter writer{m.data};
    writer.writeTo(cmd);

    stack.redoAndPushQuiet(
          m_ctx.app.instantiateUndoCommand(cmd));


    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });

  // Undo-redo
  s->mapper().addHandler("/command/undo", [&] (NetworkMessage m)
  {
    stack.undoQuiet();
    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });
  s->mapper().addHandler("/command/redo", [&] (NetworkMessage m)
  {
    stack.redoQuiet();
    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });

  s->mapper().addHandler("/command/index", [&] (NetworkMessage m)
  {
    QDataStream stream{m.data};
    int32_t idx;
    stream >> idx;
    stack.setIndexQuiet(idx);
    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });


  // Lock-unlock
  s->mapper().addHandler("/lock", [&] (NetworkMessage m)
  {
    QDataStream stream{m.data};
    QByteArray data;
    stream >> data;
    m_ctx.objectLocker.on_lock(data);
    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });

  s->mapper().addHandler("/unlock", [&] (NetworkMessage m)
  {
    QDataStream stream{m.data};
    QByteArray data;
    stream >> data;
    m_ctx.objectLocker.on_unlock(data);
    m_session->broadcastToOthers(Id<Client>(m.clientId), m);
  });

  s->mapper().addHandler("/play", [&] (NetworkMessage m)
  {
    m_session->broadcastToAllClients(m_session->makeMessage("/play"));
    play();
  });

  s->mapper().addHandler("/pong", [&] (NetworkMessage m)
  {
    m_keep.on_pong(m);
  });
}


void MasterNetworkPolicy::play()
{
  auto sm = iscore::IDocument::try_get<Scenario::ScenarioDocumentModel>(m_ctx.document);
  if(sm)
  {
    auto& plug = m_ctx.app.applicationPlugin<Engine::ApplicationPlugin>();
    plug.on_play(
          sm->baseConstraint(),
          true,
          BasicPruner{m_ctx.plugin<NetworkDocumentPlugin>()},
          TimeValue{});
  }
}

}
