#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "source/goap/agent/IAgent.h"
#include "source/goap/agent/BaseAgent.h"
#include "source/goap/BaseGoal.h"
#include "source/goap/BasePredicate.h"
#include "source/goap/BaseAction.h"
#include "source/goap/IGoapPlanner.h"


using namespace NAI::Goap;
using ::testing::NiceMock;

class OneActionGoapPlannerMock : public IGoapPlanner
{
public:
	OneActionGoapPlannerMock()
	{
		ON_CALL(*this, GetPlan).WillByDefault(
		[this](	std::vector<std::shared_ptr<IGoal>>& goals,	std::vector<std::shared_ptr<IPredicate>>& predicates)
		{
			auto predicateA = std::make_shared<BasePredicate>("A");
			auto predicateB = std::make_shared<BasePredicate>("B");

			std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateA };
			std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateB };
			std::vector<std::shared_ptr<IAction>> actions;
			actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

			return std::make_shared<BaseGoal>(actions);
		});
	}
	virtual ~OneActionGoapPlannerMock() = default;

	MOCK_CONST_METHOD2(GetPlan, 
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&, 
			std::vector<std::shared_ptr<IPredicate>>&));
};

class TwoActionGoapPlannerMock : public IGoapPlanner
{
public:
	TwoActionGoapPlannerMock()
	{
		ON_CALL(*this, GetPlan).WillByDefault(
			[this](std::vector<std::shared_ptr<IGoal>>& goals, std::vector<std::shared_ptr<IPredicate>>& predicates)
			{
				auto predicateA = std::make_shared<BasePredicate>("A");
				auto predicateB = std::make_shared<BasePredicate>("B");
				auto predicateC = std::make_shared<BasePredicate>("C");

				std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateB };
				std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateC };
				std::vector<std::shared_ptr<IAction>> actions;
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				preconditions = { predicateA };
				postconditions = { predicateB };
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				return std::make_shared<BaseGoal>(actions);
			});
	}
	virtual ~TwoActionGoapPlannerMock() = default;
	MOCK_CONST_METHOD2(GetPlan,
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
};

class EmptyGoapPlannerMock : public IGoapPlanner
{
public:
	EmptyGoapPlannerMock()
	{
		ON_CALL(*this, GetPlan).WillByDefault(
			[this](std::vector<std::shared_ptr<IGoal>>& goals, std::vector<std::shared_ptr<IPredicate>>& predicates)
			{
				return nullptr;
			});
	}
	virtual ~EmptyGoapPlannerMock() = default;

	MOCK_CONST_METHOD2(GetPlan,
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
};

TEST(NAI_Agent, When_Start_Then_AgentIsPlanning) 
{
	auto goapPlannerMock = std::make_shared<NiceMock<EmptyGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>(goapPlannerMock, predicates);

	agent->Update(0.0f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_Planning_Then_GetPlan)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>(goapPlannerMock, predicates);

	EXPECT_CALL(*goapPlannerMock, GetPlan).Times(1);

	agent->Update(0.0f);
}

TEST(NAI_Agent, When_Plan_Then_Process)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>(goapPlannerMock, predicates);

	agent->Update(0.0f); //-->get a plan changing state processing
	
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PROCESSING);
}

TEST(NAI_Agent, When_ProcessingAndPlanFinished_Then_Planning)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { std::make_shared<BasePredicate>("A") };
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>(goapPlannerMock, predicates);

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing first action
	agent->Update(0.0f); //now it's processing second action and finishes plan

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_ProcessingAndPlanAborted_Then_Planning)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { std::make_shared<BasePredicate>("D") };
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>(goapPlannerMock, predicates);

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing first action
	agent->Update(0.0f); //now it's processing second action and finishes plan

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

//comprobar que sucede si se aborta una accion porque hay otra más importante porque ha llegado un evento, replanificar.
//comprobar que se resuelven todas las acciones del plan y los predicados crecen
//comprobar que si no se puede realizar una acción hay que abortar y replanificar.