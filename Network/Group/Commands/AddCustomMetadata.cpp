#include <Network/Group/Commands/AddCustomMetadata.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <score/document/DocumentContext.hpp>
#include <score/selection/SelectionStack.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
namespace Network
{
namespace Command
{
AddCustomMetadata::AddCustomMetadata(
    const QList<const Scenario::IntervalModel*>& c,
    const QList<const Scenario::EventModel*>& e,
    const QList<const Scenario::TimeSyncModel*>& n,
    const std::vector<std::pair<QString, QString> >& meta)
{
  m_intervals.reserve(c.size());
  for(auto elt : c)
  {
    MetadataUndoRedo<Scenario::IntervalModel> m;
    m.path = score::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_intervals.push_back(std::move(m));
  }

  m_events.reserve(e.size());
  for(auto elt : e)
  {
    MetadataUndoRedo<Scenario::EventModel> m;
    m.path = score::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_events.push_back(std::move(m));
  }

  m_nodes.reserve(n.size());
  for(auto elt : n)
  {
    MetadataUndoRedo<Scenario::TimeSyncModel> m;
    m.path = score::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_nodes.push_back(std::move(m));
  }

}

void AddCustomMetadata::undo(const score::DocumentContext& ctx) const
{
  for(auto& elt : m_intervals)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.before);
  }
  for(auto& elt : m_events)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.before);
  }
  for(auto& elt : m_nodes)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.before);
  }
}

void AddCustomMetadata::redo(const score::DocumentContext& ctx) const
{
  for(auto& elt : m_intervals)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.after);
  }
  for(auto& elt : m_events)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.after);
  }
  for(auto& elt : m_nodes)
  {
    elt.path.find(ctx).metadata().setExtendedMetadata(elt.after);
  }

}

void AddCustomMetadata::serializeImpl(DataStreamInput& s) const
{
  s << m_intervals << m_events << m_nodes;

}

void AddCustomMetadata::deserializeImpl(DataStreamOutput& s)
{
  s >> m_intervals >> m_events >> m_nodes;
}
}

void SetCustomMetadata(const score::DocumentContext& ctx,
                       std::vector<std::pair<QString, QString> > md)
{
  auto sel = ctx.selectionStack.currentSelection();

  QList<const Scenario::TimeSyncModel*> l;
  l += filterSelectionByType<Scenario::TimeSyncModel>(sel);

  auto states = filterSelectionByType<Scenario::StateModel>(sel);
  if(!states.empty())
  {
      auto& s = Scenario::parentScenario(*states.first());
      for(auto e : filterSelectionByType<Scenario::StateModel>(sel))
          l.append(&Scenario::parentTimeSync(*e, s));
  }
  l = l.toSet().toList();

  auto cmd = new Command::AddCustomMetadata{
             filterSelectionByType<Scenario::IntervalModel>(sel)
             , filterSelectionByType<Scenario::EventModel>(sel)
             , std::move(l)
             , md};

  CommandDispatcher<>{ctx.commandStack}.submitCommand(cmd);
}

}




template <typename T>
struct TSerializer<DataStream, Network::Command::MetadataUndoRedo<T>>
{
  static void readFrom(
      DataStream::Serializer& s,
      const Network::Command::MetadataUndoRedo<T>& obj)
  {
    s.stream() << obj.path << obj.before << obj.after;
  }

  static void writeTo(
      DataStream::Deserializer& s, Network::Command::MetadataUndoRedo<T>& obj)
  {
    s.stream() >> obj.path >> obj.before >> obj.after;
  }
};

