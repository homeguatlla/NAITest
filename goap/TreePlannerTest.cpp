#include "gtest/gtest.h"
#include "source/goap/planners/TreeGoapPlanner.h"
#include "source/goap/IGoal.h"
#include "source/goap/BaseGoal.h"
#include "source/goap/IAction.h"
#include "source/goap/BaseAction.h"
#include "source/goap/IPredicate.h"
#include "source/goap/BasePredicate.h"

using namespace NAI::Goap;

std::shared_ptr<IAction> CreateActionWith(std::vector<std::shared_ptr<IPredicate>> preconditions, std::vector<std::shared_ptr<IPredicate>> postconditions, unsigned int cost)
{
	return std::make_shared<BaseAction>(preconditions, postconditions, cost);
}

TEST(NAI_TreeGoalPlanner, When_NoPredicates_Then_NoPlan) 
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto planner = std::make_shared<TreeGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_TreeGoalPlanner, When_PredicatesButNoGoals_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	predicates.push_back(std::make_shared<BasePredicate>("A"));

	auto planner = std::make_shared<TreeGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_TreeGoalPlanner, When_PredicatesAnGoalsButNoSatisfied_Then_NoPlan)
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

	auto planner = std::make_shared<TreeGoapPlanner>();
	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_TreeGoalPlanner, When_OnePredicateMatchesPreconditionOfAnActionOfOneActionGoal_Then_Plan)
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

	auto planner = std::make_shared<TreeGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
}

TEST(NAI_TreeGoalPlanner, When_OnePredicateChainsOneActionAndThatActionAnotherOfTheSameGoal_Then_TwoActionsGoalPlan)
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

	auto planner = std::make_shared<TreeGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() != nullptr);
	ASSERT_TRUE(plan->GetNextAction() == nullptr);
}

TEST(NAI_TreeGoalPlanner, When_TwoGoalsAreSatisfied_Then_LessCostGoalPlan)
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

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
	ASSERT_EQ(plan->GetCost(), 1);
}

TEST(NAI_TreeGoalPlanner, When_MoreThanOnePredicateSatisfyTheSameGoal_LessCostGoalPlan)
{

}


TEST(NAI_TreeGoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_GoalPlan)
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

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	std::vector<std::shared_ptr<IPredicate>> desiredPredicates;
	desiredPredicates.push_back(predicateC);

	auto plan = planner->GetPlanToReach(goals, predicates, desiredPredicates);

	ASSERT_TRUE(!plan.empty());
	ASSERT_EQ(plan[0]->GetCost(), 1);
}

TEST(NAI_TreeGoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_LessCostGoalPlan)
{
	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");
	auto predicateD = std::make_shared<BasePredicate>("D");
	auto predicateE = std::make_shared<BasePredicate>("E");
	auto predicateF = std::make_shared<BasePredicate>("F");
	auto predicateG = std::make_shared<BasePredicate>("G");

	auto action1 = CreateActionWith(
		{predicateA},
		{predicateC},
		1
	);
	auto action2 = CreateActionWith(
		{ predicateC },
		{ predicateE },
		1
	);
	auto action3 = CreateActionWith(
		{ predicateA, predicateE },
		{ predicateF },
		1
	);
	auto action4 = CreateActionWith(
		{ predicateB },
		{ predicateD },
		1
	); 
	auto action5 = CreateActionWith(
		{ predicateC, predicateD },
		{ predicateG },
		1
	);
	auto action6 = CreateActionWith(
		{ predicateA },
		{ predicateF },
		4
	);
	auto action7 = CreateActionWith(
		{ predicateB, predicateF },
		{ predicateG },
		4
	);

	std::vector<std::shared_ptr<IAction>> actions = { action1, action2, action3, action4, action5 };
	auto goal1 = std::make_shared<BaseGoal>(
		actions
	);

	actions = { action6, action7 };
	auto goal2 = std::make_shared<BaseGoal>(
		actions
	);

	std::vector<std::shared_ptr<IGoal>> goals;
	goals.push_back(goal1);
	goals.push_back(goal2);

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	std::vector<std::shared_ptr<IPredicate>> desiredPredicates;
	desiredPredicates.push_back(predicateF);
	desiredPredicates.push_back(predicateG);

	auto plan = planner->GetPlanToReach(goals, predicates, desiredPredicates);

	ASSERT_TRUE(!plan.empty());
	ASSERT_EQ(plan[0]->GetCost(), 5);
}

TEST(NAI_TreeGoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicateWithMoreThanOneGoal_Then_LessCostGoalPlan)
{
	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");
	auto predicateD = std::make_shared<BasePredicate>("D");
	auto predicateE = std::make_shared<BasePredicate>("E");
	auto predicateF = std::make_shared<BasePredicate>("F");
	auto predicateG = std::make_shared<BasePredicate>("G");

	auto action1 = CreateActionWith(
		{ predicateA },
		{ predicateC },
		1
	);
	auto action2 = CreateActionWith(
		{ predicateC },
		{ predicateE },
		1
	);
	auto action3 = CreateActionWith(
		{ predicateA, predicateE },
		{ predicateF },
		1
	);
	auto action4 = CreateActionWith(
		{ predicateB },
		{ predicateD },
		1
	);
	auto action5 = CreateActionWith(
		{ predicateC, predicateD },
		{ predicateG },
		1
	);
	auto action6 = CreateActionWith(
		{ predicateA },
		{ predicateF },
		4
	);
	auto action7 = CreateActionWith(
		{ predicateB, predicateF },
		{ predicateG },
		4
	);

	std::vector<std::shared_ptr<IAction>> actions = { action1, action2, action3 };
	auto goal1 = std::make_shared<BaseGoal>(
		actions
		);
	actions = { action4, action5 };
	auto goal2 = std::make_shared<BaseGoal>(
		actions
		);
	actions = { action6, action7 };
	auto goal3 = std::make_shared<BaseGoal>(
		actions
		);

	std::vector<std::shared_ptr<IGoal>> goals;
	goals.push_back(goal1);
	goals.push_back(goal2);
	goals.push_back(goal3);

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	std::vector<std::shared_ptr<IPredicate>> desiredPredicates;
	desiredPredicates.push_back(predicateF);
	desiredPredicates.push_back(predicateG);

	auto plan = planner->GetPlanToReach(goals, predicates, desiredPredicates);

	ASSERT_TRUE(!plan.empty());
	ASSERT_EQ(plan[0]->GetCost(), 3);
	ASSERT_EQ(plan[1]->GetCost(), 2);
}