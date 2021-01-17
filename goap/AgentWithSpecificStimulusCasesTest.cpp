#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/agent/IAgent.h"
#include "goap/agent/BaseAgent.h"
#include "goap/agent/AgentBuilder.h"

#include "goap/BaseGoal.h"
#include "goap/BasePredicate.h"
#include "goap/BaseAction.h"
#include "goap/IGoapPlanner.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/sensory/BaseSensor.h"
#include "goap/sensory/ISensor.h"

#include "goap/sensory/IStimulus.h"

using namespace NAI::Goap;
using ::testing::NiceMock;

class AcceptAllThreshold : public IThreshold
{
public:
	AcceptAllThreshold() = default;
	virtual ~AcceptAllThreshold() = default;

	bool IsStimulusPerceived(std::shared_ptr<IStimulus> stimulus) const override
	{
		return true;
	}
};

class InstantStimulus : public IStimulus
{
public:
	InstantStimulus() = default;
	virtual ~InstantStimulus() = default;


	std::string GetClassName() const override { return typeid(InstantStimulus).name(); }
	glm::vec3 GetPosition() const override { return glm::vec3(0.0f); }
	float GetDurationInMemory() const override { return 60.0f; }
	unsigned GetId() const override { return 1; }
};

class InstantStimulusSensor : public BaseSensor
{
public:
	InstantStimulusSensor() = default;
	virtual ~InstantStimulusSensor() = default;
	
};

class InstantStimulusGoal : public BaseGoal
{
public:
	InstantStimulusGoal() = default;
	virtual ~InstantStimulusGoal() = default;


protected:
	std::shared_ptr<IPredicate>
	DoTransformStimulusIntoPredicates(const ShortTermMemory<IStimulus>& memory) const override
	{
		memory.PerformActionForEach(
			[](std::shared_ptr<IStimulus> stimulus) -> bool
			{
				return true;
			});

		if(!memory.IsEmpty())
		{
			return std::make_shared<BasePredicate>("InstantStimulusPredicate");
		}
		else
		{
			return nullptr;
		}
	}
};

class MyAgent : public BaseAgent
{
public:
	MyAgent(std::shared_ptr<IGoapPlanner> goapPlanner,
        const std::vector<std::shared_ptr<IGoal>>& goals, 
        const std::vector<std::shared_ptr<IPredicate>>& predicates) :
    BaseAgent(goapPlanner, goals, predicates) {}
	
	MyAgent(std::shared_ptr<IGoapPlanner> goapPlanner,
		const std::vector<std::shared_ptr<IGoal>>& goals, 
        const std::vector<std::shared_ptr<IPredicate>>& predicates,
        const std::shared_ptr<PerceptionSystem> perceptionSystem) :
	BaseAgent(goapPlanner, goals, predicates, perceptionSystem) {}

	virtual ~MyAgent() = default;

	glm::vec3 GetPosition() const override { return glm::vec3(0.0f); }
	void MoveTo(float elapsedTime, const glm::vec3& point) override { }
};

class MyPerceptionSystemMock : public PerceptionSystem
{
public:
	MyPerceptionSystemMock(std::shared_ptr<SensorySystem<IStimulus>> sensorySystem) : PerceptionSystem(sensorySystem) {}
	virtual ~MyPerceptionSystemMock() = default;
};

//Instant Stimulus is like a Shot, a Bell sound, or a one blink light
//The stimulus is instantaneous and we want to remember it for a while
//The stimulus will remain as a predicate and will not be updated.

TEST(NAI_AgentWithSpecificStimulusCases, When_InstantStimulus_Then_It_RemainsDuringATime)
{
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	InstantStimulusSensor instantStimulusSensor;
	
	instantStimulusSensor.Subscribe(sensorySystem);

	const auto perceptionSystem = std::make_shared<NiceMock<MyPerceptionSystemMock>>(sensorySystem);

	std::vector<std::shared_ptr<IGoal>> goals = {std::make_shared<InstantStimulusGoal>()};
	std::vector<std::shared_ptr<IPredicate>> predicates;
	auto agent = std::make_shared<MyAgent>(
        std::make_shared<TreeGoapPlanner>(),
        goals,
        predicates,
        perceptionSystem);

	agent->AddSensoryThreshold(typeid(InstantStimulus).name(), std::make_shared<AcceptAllThreshold>());

	instantStimulusSensor.NotifyAll(std::make_shared<InstantStimulus>());
	
	agent->StartUp();
	agent->Update(0.016f);
	for(auto i = 0; i < 10; ++i)
	{
		agent->Update(0.016f);
		ASSERT_TRUE(sensorySystem->GetReceivedStimulus().size() == 0);
		ASSERT_TRUE(perceptionSystem->GetMemory().GetSize() == 1);
	}
	
	ASSERT_TRUE(agent->GetPredicates().size() == 1);
}

TEST(NAI_AgentWithSpecificStimulusCases, When_InstantStimulus_Then_OnePredicateAdded) 
{
	
}

//Stimulus of something that happens and will never change, it remains the same forever, like a the agent
//see a Building.
//This stimulus will be in memory for a while, if its seen again then will be updated in memory.
//The predicate created if the case will be always the same.

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfStaticPermanentThing_Then_It_RemainsDuringATime) 
{
	
}

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfStaticPermanentThing_Then_OnePredicateAdded) 
{
	
}

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfStaticPermanentThingWhichAlreadyExists_Then_PredicateIsUpdated) 
{
	
}

//Stimulus of something that can be changing during time. For instance, another agent.
//We want to remember once it disappears as stimulus for a while.
//The predicate created if the case will be updated everytime, will be always the same

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfDinamicThing_Then_It_RemainsDuringATime) 
{
	
}

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfDinamicThing_Then_OnePredicateAdded) 
{
	
}

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfDinamicThingWhichAlreadyExists_Then_PredicateIsUpdated) 
{
	
}