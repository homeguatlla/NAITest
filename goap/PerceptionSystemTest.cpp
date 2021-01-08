#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/BaseGoal.h"
#include "goap/agent/AgentBuilder.h"
#include "goap/agent/BaseAgent.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/predicates/PlaceIamPredicate.h"
#include "goap/sensory/PerceptionSystem.h"
#include "goap/sensory/IStimulus.h"
#include "goap/sensory/SensorySystem.h"

using namespace NAI::Goap;
using namespace testing;

class AgentPerceptionMock : public BaseAgent
{
public:
	AgentPerceptionMock(
		std::shared_ptr<NAI::Goap::IGoapPlanner> planner,
		const std::vector<std::shared_ptr<IGoal>>& goals,
		const std::vector<std::shared_ptr<IPredicate>>& predicates) :
	BaseAgent(planner, goals, predicates)
	{
		ON_CALL(*this, IsStimulusAccepted).WillByDefault(
            [this](std::shared_ptr<IStimulus> stimulus)
            {
                return true;
            });

		ON_CALL(*this, TransformStimulusIntoPredicates).WillByDefault(
            [this](std::shared_ptr<IStimulus> stimulus)
            {
            	std::vector<std::shared_ptr<IPredicate>> result;
            	result.push_back(std::make_shared<PlaceIamPredicate>("AtHome"));
            	
				return result;
            });
	}
	
	virtual ~AgentPerceptionMock() = default;

	glm::vec3 GetPosition() const { return {}; }
	void MoveTo(float elapsedTime, const glm::vec3& point) {}

	MOCK_CONST_METHOD1(IsStimulusAccepted, bool(std::shared_ptr<IStimulus>));
	MOCK_CONST_METHOD1(TransformStimulusIntoPredicates, const std::vector<std::shared_ptr<IPredicate>>(std::shared_ptr<IStimulus> stimulus));
};

class MemoryMock : public Memory<IStimulus>
{
public:
	MemoryMock() = default;
	virtual ~MemoryMock() = default;	
};

class SensoryMock : public SensorySystem<IStimulus>
{
public:
	SensoryMock() = default;
	virtual ~SensoryMock() = default;
	
};

class StimulusMock : public IStimulus
{
public:
	StimulusMock() = default;
	virtual ~StimulusMock() = default;

	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
};

class ThresholdMock : public IThreshold
{
public:
	ThresholdMock(bool isPerceived) : mIsPerceived{isPerceived}
	{
		ON_CALL(*this, IsStimulusPerceived).WillByDefault(
            [this](const std::shared_ptr<IStimulus> stimulus)
            {
                return mIsPerceived;
            });
	}
	
	virtual ~ThresholdMock() = default;

	MOCK_CONST_METHOD1(IsStimulusPerceived, bool(const std::shared_ptr<IStimulus>));
private:
	bool mIsPerceived;
};

class GoalMock : public BaseGoal
{
public:
	GoalMock()
	{
		ON_CALL(*this, IsStimulusAccepted).WillByDefault(
            [this](const std::shared_ptr<IStimulus> stimulus)
            {
                return true;
            });
		ON_CALL(*this, TransformStimulusIntoPredicates).WillByDefault(
            [this](const std::shared_ptr<IStimulus> stimulus)
            {
                return std::make_shared<BasePredicate>("PredicateReactionToStimulus");
            });
	}

	~GoalMock() = default;
	MOCK_CONST_METHOD1(IsStimulusAccepted, bool(std::shared_ptr<IStimulus>));
	MOCK_CONST_METHOD1(TransformStimulusIntoPredicates, std::shared_ptr<IPredicate>(std::shared_ptr<IStimulus> stimulus));
};

TEST(NAI_PerceptionSystem, When_Created_Then_MemoryIsEmpty)
{
	const auto sensoryMock = std::make_shared<NiceMock<SensoryMock>>();
	PerceptionSystem perceptionSystem(sensoryMock);

	auto& memory = perceptionSystem.GetMemory();

	ASSERT_TRUE(memory.IsEmpty());
}

TEST(NAI_PerceptionSystem, When_Update_Then_NewPredicatesAreAddedToTheAgent)
{
	const auto stimulus = std::make_shared<NiceMock<StimulusMock>>();
	const auto threshold = std::make_shared<NiceMock<ThresholdMock>>(true);
	const auto sensoryMock = std::make_shared<NiceMock<SensoryMock>>();
	
	AgentBuilder agentBuilder;
	auto agent =	agentBuilder.WithGoapPlanner(std::make_shared<NiceMock<DirectGoapPlanner>>())
											.WithSensoryThreshold(stimulus->GetClassName(), threshold)
											.Build<AgentPerceptionMock>();

	PerceptionSystem perceptionSystem(sensoryMock);
	
	ASSERT_TRUE(agent->GetPredicates().empty());

	sensoryMock->OnNotification(stimulus);
	agent->Update(0.16f); // to make the current state be set.
	perceptionSystem.Update(0.16f, agent);
	
	ASSERT_FALSE(agent->GetPredicates().empty());
}

