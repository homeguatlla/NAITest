#include "gtest/gtest.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/IGoal.h"
#include "goap/BaseGoal.h"
#include "goap/IAction.h"
#include "goap/BaseAction.h"
#include "goap/IPredicate.h"
#include "goap/BasePredicate.h"

using namespace NAI::Goap;

std::shared_ptr<IAction> CreateActionWith(std::vector<std::string> preconditions, std::vector<std::shared_ptr<IPredicate>> postconditions, unsigned int cost)
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

	predicates.push_back(std::make_shared<BasePredicate>(1, "A"));

	auto planner = std::make_shared<TreeGoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_TreeGoalPlanner, When_PredicatesAnGoalsButNoSatisfied_Then_NoPlan)
{
	std::vector<std::shared_ptr<IGoal>> goals;
	std::vector<std::shared_ptr<IPredicate>> predicates;

	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");
	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateB->GetText() };
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

	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateA->GetText() };
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

	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");

	predicates.push_back(predicateA);

	std::vector<std::string> preconditions = { predicateB->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));
	
	preconditions = { predicateA->GetText() };
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

	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");

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

TEST(NAI_TreeGoalPlanner, When_OneGoalMultipleActions_And_OneActionCanNotBeSatisfied_Then_NoPlan)
{
	const auto predicateA = std::make_shared<BasePredicate>(1, "A");
	const auto predicateB = std::make_shared<BasePredicate>(2, "B");
	const auto predicateC = std::make_shared<BasePredicate>(3, "C");
	const auto predicateD = std::make_shared<BasePredicate>(4, "D");
	const auto predicateE = std::make_shared<BasePredicate>(5, "E");

	std::vector<std::shared_ptr<IGoal>> goals;

	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::string> preconditions = { predicateD->GetText(), predicateA->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateE };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, 1));

	preconditions = { predicateB->GetText() };
	postconditions = { predicateD };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, 1));
	
	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateB);

	const auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(!plan);
}

TEST(NAI_TreeGoalPlanner, When_OneGoalMultipleActions_And_ActionsCanBeSatisfied_Then_Plan_And_ThePredicatesAcceptanceGoFirst)
{
	const auto predicateA = std::make_shared<BasePredicate>(1, "A");
	const auto predicateB = std::make_shared<BasePredicate>(2, "B");
	const auto predicateC = std::make_shared<BasePredicate>(3, "C");
	const auto predicateD = std::make_shared<BasePredicate>(4, "D");
	const auto predicateE = std::make_shared<BasePredicate>(5, "E");
	const auto predicateF = std::make_shared<BasePredicate>(6, "F");

	std::vector<std::shared_ptr<IGoal>> goals;

	std::vector<std::shared_ptr<IAction>> actions;
	std::vector<std::string> preconditions = { predicateD->GetText(), predicateA->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateE };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, 1));

	preconditions = { predicateB->GetText() };
	postconditions = { predicateD };
	actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions, 1));
	
	auto goal = std::make_shared<BaseGoal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<TreeGoapPlanner>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(predicateF);
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan);
	ASSERT_TRUE(predicates[0] == predicateB);
	ASSERT_TRUE(predicates[1] == predicateF);
}

TEST(NAI_TreeGoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_GoalPlan)
{
	int costActionGoal1 = 3;
	int costActionGoal2 = 1;

	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");

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
	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");
	auto predicateD = std::make_shared<BasePredicate>(4, "D");
	auto predicateE = std::make_shared<BasePredicate>(5, "E");
	auto predicateF = std::make_shared<BasePredicate>(6, "F");
	auto predicateG = std::make_shared<BasePredicate>(7, "G");

	auto action1 = CreateActionWith(
		{predicateA->GetText()},
		{predicateC},
		1
	);
	auto action2 = CreateActionWith(
		{ predicateC->GetText() },
		{ predicateE },
		1
	);
	auto action3 = CreateActionWith(
		{ predicateA->GetText(), predicateE->GetText() },
		{ predicateF },
		1
	);
	auto action4 = CreateActionWith(
		{ predicateB->GetText() },
		{ predicateD },
		1
	); 
	auto action5 = CreateActionWith(
		{ predicateC->GetText(), predicateD->GetText() },
		{ predicateG },
		1
	);
	auto action6 = CreateActionWith(
		{ predicateA->GetText() },
		{ predicateF },
		4
	);
	auto action7 = CreateActionWith(
		{ predicateB->GetText(), predicateF->GetText() },
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
	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");
	auto predicateD = std::make_shared<BasePredicate>(4, "D");
	auto predicateE = std::make_shared<BasePredicate>(5, "E");
	auto predicateF = std::make_shared<BasePredicate>(6, "F");
	auto predicateG = std::make_shared<BasePredicate>(7, "G");

	auto action1 = CreateActionWith(
		{ predicateA->GetText() },
		{ predicateC },
		1
	);
	auto action2 = CreateActionWith(
		{ predicateC->GetText() },
		{ predicateE },
		1
	);
	auto action3 = CreateActionWith(
		{ predicateA->GetText(), predicateE->GetText() },
		{ predicateF },
		1
	);
	auto action4 = CreateActionWith(
		{ predicateB->GetText() },
		{ predicateD },
		1
	);
	auto action5 = CreateActionWith(
		{ predicateC->GetText(), predicateD->GetText() },
		{ predicateG },
		1
	);
	auto action6 = CreateActionWith(
		{ predicateA->GetText() },
		{ predicateF },
		4
	);
	auto action7 = CreateActionWith(
		{ predicateB->GetText(), predicateF->GetText() },
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