#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/BaseGoal.h"
#include "goap/agent/AgentBuilder.h"
#include "goap/agent/BaseAgent.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/predicates/PlaceIamPredicate.h"
#include "goap/sensory/CognitiveSystem.h"
#include "goap/sensory/IStimulus.h"

using namespace NAI::Goap;
using namespace testing;

class AgentCognitiveMock : public BaseAgent
{
public:
	AgentCognitiveMock(
        std::shared_ptr<NAI::Goap::IGoapPlanner> planner,
        const std::vector<std::shared_ptr<IGoal>>& goals,
        const std::vector<std::shared_ptr<IPredicate>>& predicates) :
	AgentCognitiveMock(planner, goals, predicates, nullptr)
	{
		
	}
	AgentCognitiveMock(
		std::shared_ptr<NAI::Goap::IGoapPlanner> planner,
		const std::vector<std::shared_ptr<IGoal>>& goals,
		const std::vector<std::shared_ptr<IPredicate>>& predicates,
		const std::shared_ptr<PerceptionSystem> perceptionSystem) :
	BaseAgent(planner, goals, predicates, perceptionSystem),
	mHasReceivedNewPredicate { false }
	{
		ON_CALL(*this, TransformStimulusIntoPredicates).WillByDefault(
            [this](const Memory<IStimulus>& memory)
            {
            	std::vector<std::shared_ptr<IPredicate>> result;
            	result.push_back(std::make_shared<PlaceIamPredicate>("AtHome"));
            	
				return result;
            });
		ON_CALL(*this, OnNewPredicate).WillByDefault(
            [this](std::shared_ptr<IPredicate> predicate)
            {
            	mHasReceivedNewPredicate = true;
            });
	}
	
	virtual ~AgentCognitiveMock() = default;

	bool HasNewPredicate() const { return mHasReceivedNewPredicate; }
	glm::vec3 GetPosition() const { return {}; }
	void MoveTo(float elapsedTime, const glm::vec3& point) {}

	MOCK_METHOD1(OnNewPredicate, void(std::shared_ptr<IPredicate>));
	MOCK_CONST_METHOD1(TransformStimulusIntoPredicates, const std::vector<std::shared_ptr<IPredicate>>(const Memory<IStimulus>&));

private:
	bool mHasReceivedNewPredicate;
};

class MemoryMock : public Memory<IStimulus>
{
public:
	MemoryMock() = default;
	virtual ~MemoryMock() = default;	
};

class StimulusCognitiveMock : public IStimulus
{
public:
	StimulusCognitiveMock() = default;
	virtual ~StimulusCognitiveMock() = default;

	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
};

TEST(NAI_CognitiveSystem, When_Update_AndMemoryNotEmpty_AndGoalsAcceptingStimulus_Then_NewPredicatesAreNotifiedToAgent)
{
	CognitiveSystem cognitiveSystem;
	MemoryMock memory;
	memory.Add(std::make_shared<NiceMock<StimulusCognitiveMock>>());
	
	const auto goapPlanner = std::make_shared<TreeGoapPlanner>();
	
	AgentBuilder agentBuilder;
	const auto agent =	agentBuilder.WithGoapPlanner(goapPlanner)
														.Build<AgentCognitiveMock>();
	
	cognitiveSystem.Update(0.16f, memory, agent);

	auto agentCognitive = std::static_pointer_cast<AgentCognitiveMock>(agent);
	
	//EXPECT_CALL(*agent, OnNewPredicate).Times(1);
	ASSERT_TRUE(agentCognitive->HasNewPredicate());
}
