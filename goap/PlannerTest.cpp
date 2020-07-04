#include "gtest/gtest.h"
#include "source/goap/GoapPlanner.h"
#include "source/goap/IGoal.h"
#include "source/goap/BaseGoal.h"
#include "source/goap/IAction.h"
#include "source/goap/BaseAction.h"
#include "source/goap/IPredicate.h"
#include "source/goap/BasePredicate.h"

using namespace NAI::Goap;

TEST(NAI_GoalPlanner, When_NoPredicates_Then_NoPlan) 
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto planner = std::make_shared<GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_PredicatesButNoGoals_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	predicates.push_back(std::make_shared<BasePredicate>("A"));

	auto planner = std::make_shared<GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_PredicatesAnGoalsButNoSatisfied_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");
	predicates.push_back(predicateA);

	std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	std::vector<std::shared_ptr<IAction>> actions;
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<GoapPlanner>();
	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_OnePredicateMatchesPreconditionOfAnActionOfOneActionGoal_Then_Plan)
{
	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	predicates.push_back(predicateA);

	std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateA };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateB };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
}

TEST(NAI_GoalPlanner, When_OnePredicateChainsOneActionAndThatActionAnotherOfTheSameGoal_Then_TwoActionsGoalPlan)
{
	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	predicates.push_back(predicateA);

	std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));
	
	preconditions = { predicateA };
	postconditions = { predicateB };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() == nullptr);
}

TEST(NAI_GoalPlanner, When_TwoGoalsAreSatisfied_Then_LessCostGoalPlan)
{
	int costActionGoal1 = 3;
	int costActionGoal2 = 1;

	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	std::vector<std::shared_ptr<IGoal>> goals;

	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, costActionGoal1));

	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	preconditions = { predicateA };
	postconditions = { predicateC };
	actions = {};
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, costActionGoal2));

	goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<GoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan->GetCost(), 1);
}

TEST(NAI_GoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_GoalPlan)
{
	//TODO
}