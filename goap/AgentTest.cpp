#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/agent/IAgent.h"
#include "goap/agent/BaseAgent.h"
#include "goap/BaseGoal.h"
#include "goap/BasePredicate.h"
#include "goap/BaseAction.h"
#include "goap/IGoapPlanner.h"
#include <goap/sensory/IStimulus.h>

using namespace NAI::Goap;
using ::testing::NiceMock;

class AgentMock : public BaseAgent
{
public:
	AgentMock(std::shared_ptr<IGoapPlanner> goapPlanner,
		std::vector<std::shared_ptr<IGoal>>& goals,
		std::vector<std::shared_ptr<IPredicate>>& predicates) : BaseAgent(goapPlanner, goals, predicates)
	{
	}

	virtual ~AgentMock() = default;

	glm::vec3 GetPosition() const override { return position; }
	void MoveTo(float elapsedTime, const glm::vec3& point) override {}

private:
	glm::vec3 position;
};

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

		ON_CALL(*this, GetPlanToReach).WillByDefault(
			[this](
			std::vector<std::shared_ptr<IGoal>>& goals, 
			std::vector<std::shared_ptr<IPredicate>>& predicates,
			std::vector<std::shared_ptr<IPredicate>>& desiredPredicates)
			{
				auto predicateA = std::make_shared<BasePredicate>("A");

				std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateA };
				std::vector<std::shared_ptr<IPredicate>> postconditions = desiredPredicates;
				std::vector<std::shared_ptr<IAction>> actions;
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				std::vector<std::shared_ptr<IGoal>> result;
				result.push_back(std::make_shared<BaseGoal>(actions));

				return result;
			});
	}
	virtual ~OneActionGoapPlannerMock() = default;

	MOCK_CONST_METHOD2(GetPlan, 
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&, 
			std::vector<std::shared_ptr<IPredicate>>&));
	MOCK_CONST_METHOD3(GetPlanToReach,
		std::vector<std::shared_ptr<IGoal>>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
};

class TwoActionGoapPlannerMock : public IGoapPlanner
{
public:
	TwoActionGoapPlannerMock()
	{
		predicateA = std::make_shared<BasePredicate>("A");
		predicateB = std::make_shared<BasePredicate>("B");
		predicateC = std::make_shared<BasePredicate>("C");

		ON_CALL(*this, GetPlan).WillByDefault(
			[this](std::vector<std::shared_ptr<IGoal>>& goals, std::vector<std::shared_ptr<IPredicate>>& predicates)
			{
				std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateA };
				std::vector<std::shared_ptr<IPredicate>> postconditions = { predicateB };
				std::vector<std::shared_ptr<IAction>> actions;
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				preconditions = { predicateB };
				postconditions = { predicateC };
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				return std::make_shared<BaseGoal>(actions);
			});

		ON_CALL(*this, GetPlanToReach).WillByDefault(
			[this](
				std::vector<std::shared_ptr<IGoal>>& goals,
				std::vector<std::shared_ptr<IPredicate>>& predicates,
				std::vector<std::shared_ptr<IPredicate>>& desiredPredicates)
			{
				auto predicateA = std::make_shared<BasePredicate>("A");

				std::vector<std::shared_ptr<IPredicate>> preconditions = { predicateA };
				std::vector<std::shared_ptr<IPredicate>> postconditions = desiredPredicates;
				std::vector<std::shared_ptr<IAction>> actions;
				actions.push_back(std::make_shared<BaseAction>(preconditions, postconditions));

				std::vector<std::shared_ptr<IGoal>> result;
				result.push_back(std::make_shared<BaseGoal>(actions));

				return result;
			});
	}
	virtual ~TwoActionGoapPlannerMock() = default;
	MOCK_CONST_METHOD2(GetPlan,
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
	MOCK_CONST_METHOD3(GetPlanToReach,
		std::vector<std::shared_ptr<IGoal>>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&,
			std::vector<std::shared_ptr<IPredicate>>&));

	std::shared_ptr<IPredicate> predicateA, predicateB, predicateC;
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

		ON_CALL(*this, GetPlanToReach).WillByDefault(
			[this](
				std::vector<std::shared_ptr<IGoal>>& goals,
				std::vector<std::shared_ptr<IPredicate>>& predicates,
				std::vector<std::shared_ptr<IPredicate>>& desiredPredicates)
			{
				return std::vector<std::shared_ptr<IGoal>>();
			});
	}
	virtual ~EmptyGoapPlannerMock() = default;

	MOCK_CONST_METHOD2(GetPlan,
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
	MOCK_CONST_METHOD3(GetPlanToReach,
		std::vector<std::shared_ptr<IGoal>>(
			std::vector<std::shared_ptr<IGoal>>&,
			std::vector<std::shared_ptr<IPredicate>>&,
			std::vector<std::shared_ptr<IPredicate>>&));
};

class GoalAcceptanceHearingStimulusMock : public BaseGoal
{
public:
	GoalAcceptanceHearingStimulusMock()
	{
		ON_CALL(*this, IsStimulusAccepted).WillByDefault(
            [this](std::shared_ptr<IStimulus> stimulus)
            {
                return true;
            });
		ON_CALL(*this, TransformStimulusIntoPredicates).WillByDefault(
            [this](std::shared_ptr<IStimulus> stimulus)
            {
                return std::make_shared<BasePredicate>("PredicateA");
            });
	}
	virtual ~GoalAcceptanceHearingStimulusMock() = default;

	MOCK_CONST_METHOD1(IsStimulusAccepted, bool(std::shared_ptr<IStimulus>));
	MOCK_CONST_METHOD1(TransformStimulusIntoPredicates, std::shared_ptr<IPredicate>(std::shared_ptr<IStimulus>));
};

class HearingStimulusMock : public IStimulus
{
public:
	HearingStimulusMock()
	{
		ON_CALL(*this, GetClassName).WillByDefault(
			[this]()
			{
				return typeid(*this).name();
			});
	}

	virtual ~HearingStimulusMock() = default;

	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
};

TEST(NAI_Agent, When_Start_Then_AgentIsPlanning) 
{
	auto goapPlannerMock = std::make_shared<NiceMock<EmptyGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	agent->Update(0.0f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_Planning_Then_GetPlan)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	EXPECT_CALL(*goapPlannerMock, GetPlan).Times(1);

	agent->Update(0.0f);
}

TEST(NAI_Agent, When_Plan_Then_Process)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	agent->Update(0.0f); //-->get a plan changing state processing
	
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PROCESSING);
}

TEST(NAI_Agent, When_ProcessingAndPlanFinished_Then_Planning)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { goapPlannerMock->predicateA };
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing first action
	agent->Update(0.0f); //now it's processing second action and finishes plan

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_ProcessingAndPlanFinished_Then_PostConditionAddedToPredicatesList)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { goapPlannerMock->predicateA };
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	ASSERT_FALSE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing first action
	agent->Update(0.0f); //now it's processing second action and finishes plan

	ASSERT_TRUE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));
}

TEST(NAI_Agent, When_ProcessingAndPlanAborted_Then_Planning)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { std::make_shared<BasePredicate>("D") };
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	ASSERT_FALSE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing but no actions to process then abort plan

	ASSERT_FALSE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_ProcessingAPlanAndNewPredicatesAddedIntoThePredicates_Then_Planning)
{
	auto goapPlannerMock = std::make_shared<NiceMock<TwoActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates = { goapPlannerMock->predicateA };
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);


	EXPECT_CALL(*goapPlannerMock, GetPlan).Times(1);
	ASSERT_FALSE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));

	agent->Update(0.0f); //-->get a plan changing state processing
	agent->Update(0.0f); //now it's processing the first action
	
	agent->OnNewPredicate(goapPlannerMock->predicateC);
	
	agent->Update(0.0f); //as new predicate arrived, abort and planning

	ASSERT_TRUE(agent->HasPredicate(goapPlannerMock->predicateC->GetID()));
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_TransformAnStimulusToPredicatesRelatedToAGoal_Then_NewPredicate)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;

	auto goal = std::make_shared<NiceMock<GoalAcceptanceHearingStimulusMock>>();
	goals.push_back(goal);
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	auto stimulus = std::make_shared<HearingStimulusMock>();
	
	auto newPredicatesList = agent->TransformStimulusIntoPredicates(stimulus);
	
	ASSERT_FALSE(newPredicatesList.empty());
}

TEST(NAI_Agent, When_TransformAnStimulusToPredicatesUnRelatedToAGoal_Then_Null)
{
	auto goapPlannerMock = std::make_shared<NiceMock<OneActionGoapPlannerMock>>();
	std::vector<std::shared_ptr<IPredicate>> predicates;
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<IAgent> agent = std::make_shared<NiceMock<AgentMock>>(goapPlannerMock, goals, predicates);

	const auto stimulus = std::make_shared<HearingStimulusMock>();
	const auto isAccepted = agent->IsStimulusAccepted(stimulus);
	
	ASSERT_FALSE(isAccepted);
}