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
        AgentPerceptionMock(planner, goals, predicates, nullptr)
	{
	}
	
	AgentPerceptionMock(
		std::shared_ptr<NAI::Goap::IGoapPlanner> planner,
		const std::vector<std::shared_ptr<IGoal>>& goals,
		const std::vector<std::shared_ptr<IPredicate>>& predicates,
		const std::shared_ptr<PerceptionSystem> perceptionSystem) :
		BaseAgent(planner, goals, predicates, perceptionSystem)
	{
		ON_CALL(*this, TransformStimulusIntoPredicates).WillByDefault(
            [this](const Memory<IStimulus>& memory)
            {
            	std::vector<std::shared_ptr<IPredicate>> result;
            	result.push_back(std::make_shared<PlaceIamPredicate>("AtHome"));
            	
				return result;
            });
	}
	
	virtual ~AgentPerceptionMock() = default;

	glm::vec3 GetPosition() const { return {}; }
	void MoveTo(float elapsedTime, const glm::vec3& point) {}

	MOCK_CONST_METHOD1(TransformStimulusIntoPredicates, const std::vector<std::shared_ptr<IPredicate>>(const Memory<IStimulus>&));
};

class SensoryMock : public SensorySystem<IStimulus>
{
public:
	SensoryMock() = default;
	virtual ~SensoryMock() = default;
	
};

class PerceptionStimulusMock : public IStimulus
{
public:
	PerceptionStimulusMock() = default;
	virtual ~PerceptionStimulusMock() = default;

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

TEST(NAI_PerceptionSystem, When_Created_Then_MemoryIsEmpty)
{
	const auto sensoryMock = std::make_shared<NiceMock<SensoryMock>>();
	PerceptionSystem perceptionSystem(sensoryMock);

	auto& memory = perceptionSystem.GetMemory();

	ASSERT_TRUE(memory.IsEmpty());
}

TEST(NAI_PerceptionSystem, When_Update_Then_NewPredicatesAreAddedToTheAgent)
{
	const auto stimulus = std::make_shared<NiceMock<PerceptionStimulusMock>>();
	const auto threshold = std::make_shared<NiceMock<ThresholdMock>>(true);
	const auto sensoryMock = std::make_shared<NiceMock<SensoryMock>>();
	
	AgentBuilder agentBuilder;
	auto agent =	agentBuilder.WithGoapPlanner(std::make_shared<DirectGoapPlanner>())
								.WithSensoryThreshold(stimulus->GetClassName(), threshold)
								.WithPerceptionSystem(sensoryMock)
								.Build<AgentPerceptionMock>();

	ASSERT_TRUE(agent->GetPredicates().empty());

	sensoryMock->OnNotification(stimulus);
	agent->Update(0.16f); // to make the current state be set.
	agent->Update(0.16f);
	
	ASSERT_FALSE(agent->GetPredicates().empty());
}

//TODO tests pendientes, comprobar que se vacía la memoria, si hay sonido se queda temporal si es visual se borra?

