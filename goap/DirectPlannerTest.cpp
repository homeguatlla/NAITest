#include "gtest/gtest.h"
#include "goap/planners/DirectGoapPlanner.h"
#include "goap/IGoal.h"
#include "goap/BaseGoal.h"
#include "goap/IAction.h"
#include "goap/BaseAction.h"
#include "goap/IPredicate.h"
#include "goap/BasePredicate.h"

using namespace NAI::Goap;

TEST(NAI_DirectGoalPlanner, When_NoPredicates_Then_NoPlan) 
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto planner = std::make_shared<DirectGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_DirectGoalPlanner, When_PredicatesButNoGoals_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	predicates.push_back(std::make_shared<BasePredicate>("A"));

	auto planner = std::make_shared<DirectGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_DirectGoalPlanner, When_PredicatesAnGoalsButNoSatisfied_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");
	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateB->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	std::vector<std::shared_ptr<IAction>> actions;
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<DirectGoapPlanner>();
	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_DirectGoalPlanner, When_OnePredicateMatchesPreconditionOfAnActionOfOneActionGoal_Then_Plan)
{
	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateA->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateB };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<DirectGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
}

TEST(NAI_DirectGoalPlanner, When_OnePredicateChainsOneActionAndThatActionAnotherOfTheSameGoal_Then_TwoActionsGoalPlan)
{
	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateB->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));
	
	preconditions = { predicateA->GetText() };
	postconditions = { predicateB };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<DirectGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() == nullptr);
}

TEST(NAI_DirectGoalPlanner, When_TwoGoalsAreSatisfied_Then_LessCostGoalPlan)
{
	int costActionGoal1 = 3;
	int costActionGoal2 = 1;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	std::vector<std::shared_ptr<IGoal>> goals;

	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::string> preconditions = { predicateB->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, costActionGoal1));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	preconditions = { predicateA->GetText() };
	postconditions = { predicateC };
	actions = {};
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, costActionGoal2));

	goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<DirectGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan->GetCost(), 1);
}

TEST(NAI_DirectGoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_GoalPlan)
{
	//TODO
}