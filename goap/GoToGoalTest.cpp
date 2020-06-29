#include "gtest/gtest.h"
#include "source/goap/IAction.h"
#include "source/goap/goals/GoToGoal.h"
#include "source/goap/BasePredicate.h"
#include "source/goap/GoapPlanner.h"
#include "source/goap/agent/BaseAgent.h"
#include "source/goap/predicates/GoapPredicates.h"

#include <memory>

using namespace NAI::Goap;

TEST(NAI_GoToGoalTest, When_AgentHasToGo_Then_Arrives)
{
	auto goapPlanner = std::make_shared<GoapPlanner>();
	
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<BasePredicate>("GoToSaloon"));

	std::vector<std::shared_ptr<IGoal>> goals;
	goals.push_back(std::make_shared<GoToGoal>());

	auto agent = std::make_shared<BaseAgent>(goapPlanner, goals, predicates);

	agent->Update(0.0f); //validate goal can be executed
	agent->Update(0.0f); //Starts executing first action

	ASSERT_TRUE(agent->HasPredicate(PREDICATE_AT_PLACE->GetID()));
}

TEST(NAI_GoToGoalTest, When_AgentHasTwoPlacesToGo_Then_ArrivesAtPlaceWithLessCost)
{
}

