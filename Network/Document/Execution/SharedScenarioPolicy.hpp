#pragma once
#include <Network/Document/Execution/Context.hpp>

namespace Network
{

struct SharedScenarioPolicy
{
  NetworkPrunerContext& ctx;

  void operator()(
      Engine::Execution::ProcessComponent& comp,
      Scenario::ScenarioInterface& ip,
      const Group& cur);

  void operator()(Engine::Execution::ConstraintComponent& cst, const Group& cur);

  //! Todo isn't this the code for the mixed mode actually ?
  //! In the "shared" mode we could assume that evaluation entering / leaving is the same
  //! for everyone...
  void operator()(Engine::Execution::TimeNodeComponent& comp, const Group& parent_group);

  void setupMaster(
      Engine::Execution::TimeNodeComponent& comp,
      Path<Scenario::TimeNodeModel> p,
      const Group& parent_group,
      SyncMode sync);
};

}