#include "pch.h"
#include "../NAI/source/goap/GoapPlanner.h"
#include "../NAI/source/goap/Goal.h"
#include "../NAI/source/goap/Action.h"
#include "../NAI/source/goap/Predicate.h"

TEST(NAI_GoalPlanner, When_NoPredicates_Then_NoPlan) 
{
	std::vector<std::shared_ptr<NAI::Goal>> goals;
	std::vector<std::shared_ptr<NAI::Predicate>> predicates;

	auto planner = std::make_shared<NAI::GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_PredicatesButNoGoals_Then_NoPlan)
{
	std::vector<std::shared_ptr<NAI::Goal>> goals;
	std::vector<std::shared_ptr<NAI::Predicate>> predicates;

	predicates.push_back(std::make_shared<NAI::Predicate>("A"));

	auto planner = std::make_shared<NAI::GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_PredicatesAnGoalsButNoSatisfied_Then_NoPlan)
{
	std::vector<std::shared_ptr<NAI::Goal>> goals;
	std::vector<std::shared_ptr<NAI::Predicate>> predicates;

	auto predicateA = std::make_shared<NAI::Predicate>("A");
	auto predicateB = std::make_shared<NAI::Predicate>("B");
	auto predicateC = std::make_shared<NAI::Predicate>("C");
	predicates.push_back(predicateA);

	std::vector<std::shared_ptr<NAI::Predicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<NAI::Predicate>> postconditions = { predicateC };
	std::vector<std::shared_ptr<NAI::Action>> actions;
	actions.push_back(std::make_shared<NAI::Action>(preconditions, postconditions));

	auto goal = std::make_shared<NAI::Goal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<NAI::GoapPlanner>();
	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan, nullptr);
}

TEST(NAI_GoalPlanner, When_OnePredicateMatchesPreconditionOfAnActionOfOneActionGoal_Then_Plan)
{
	std::vector<std::shared_ptr<NAI::Action>> actions;
	std::vector<std::shared_ptr<NAI::Predicate>> predicates;
	std::vector<std::shared_ptr<NAI::Goal>> goals;

	auto predicate = std::make_shared<NAI::Predicate>("A");
	predicates.push_back(predicate);

	actions.push_back(std::make_shared<NAI::Action>(predicates, predicates));

	auto goal = std::make_shared<NAI::Goal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<NAI::GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan != nullptr);
}

TEST(NAI_GoalPlanner, When_OnePredicateChainsOneActionAndThatActionAnotherOfTheSameGoal_Then_TwoActionsGoalPlan)
{
	std::vector<std::shared_ptr<NAI::Action>> actions;
	std::vector<std::shared_ptr<NAI::Predicate>> predicates;
	std::vector<std::shared_ptr<NAI::Goal>> goals;

	auto predicateA = std::make_shared<NAI::Predicate>("A");
	auto predicateB = std::make_shared<NAI::Predicate>("B");
	auto predicateC = std::make_shared<NAI::Predicate>("C");

	predicates.push_back(predicateA);

	std::vector<std::shared_ptr<NAI::Predicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<NAI::Predicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<NAI::Action>(preconditions, postconditions));
	
	preconditions = { predicateA };
	postconditions = { predicateB };
	actions.push_back(std::make_shared<NAI::Action>(preconditions, postconditions));

	auto goal = std::make_shared<NAI::Goal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<NAI::GoapPlanner>();

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_TRUE(plan->GetActions().size() == 2);
}

TEST(NAI_GoalPlanner, When_TwoGoalsAreSatisfied_Then_LessCostGoalPlan)
{
	int costActionGoal1 = 3;
	int costActionGoal2 = 1;

	auto predicateA = std::make_shared<NAI::Predicate>("A");
	auto predicateB = std::make_shared<NAI::Predicate>("B");
	auto predicateC = std::make_shared<NAI::Predicate>("C");

	std::vector<std::shared_ptr<NAI::Goal>> goals;

	std::vector<std::shared_ptr<NAI::Action>> actions;
	std::vector<std::shared_ptr<NAI::Predicate>> preconditions = { predicateB };
	std::vector<std::shared_ptr<NAI::Predicate>> postconditions = { predicateC };
	actions.push_back(std::make_shared<NAI::Action>(preconditions, postconditions, costActionGoal1));

	auto goal = std::make_shared<NAI::Goal>(actions);
	goals.push_back(goal);

	preconditions = { predicateA };
	postconditions = { predicateC };
	actions = {};
	actions.push_back(std::make_shared<NAI::Action>(preconditions, postconditions, costActionGoal2));

	goal = std::make_shared<NAI::Goal>(actions);
	goals.push_back(goal);

	auto planner = std::make_shared<NAI::GoapPlanner>();

	std::vector<std::shared_ptr<NAI::Predicate>> predicates;
	predicates.push_back(predicateA);
	predicates.push_back(predicateB);

	auto plan = planner->GetPlan(goals, predicates);

	ASSERT_EQ(plan->GetCost(), 1);
}

TEST(NAI_GoalPlanner, When_WeWantAPlanThatSatisfiesAGivenPredicate_Then_GoalPlan)
{
	//TODO
}