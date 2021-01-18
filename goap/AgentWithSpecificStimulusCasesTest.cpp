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


class PermanentPredicate : public BasePredicate
{
public:
	PermanentPredicate() : BasePredicate("PermanentPredicate") {}
	virtual ~PermanentPredicate() = default;
};

class DynamicPredicate : public BasePredicate
{
public:
	DynamicPredicate(int v) : BasePredicate("DynamicPredicate"), mValue{v} {}
	virtual ~DynamicPredicate() = default;

	int GetValue() const { return mValue; }
	void SetValue(int v) { mValue = v; }

private:
	int mValue;	
};

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

class PermanentStimulus : public IStimulus
{
public:
	PermanentStimulus() = default;
	virtual ~PermanentStimulus() = default;

	std::string GetClassName() const override { return typeid(PermanentStimulus).name(); }
	glm::vec3 GetPosition() const override { return glm::vec3(1.0f); }
	float GetDurationInMemory() const override { return 60.0f; }
	unsigned GetId() const override { return 1; }
};

class DynamicStimulus : public IStimulus
{
public:
	DynamicStimulus(int v) : mValue{v} {}
	virtual ~DynamicStimulus() = default;

	std::string GetClassName() const override { return typeid(DynamicStimulus).name(); }
	glm::vec3 GetPosition() const override { return glm::vec3(1.0f); }
	float GetDurationInMemory() const override { return 60.0f; }
	unsigned GetId() const override { return 1; }
	int GetValue() const { return mValue; }

private:
	int mValue;
};

class InstantStimulusSensor : public BaseSensor
{
public:
	InstantStimulusSensor() = default;
	virtual ~InstantStimulusSensor() = default;	
};

class PermanentStimulusSensor : public BaseSensor
{
public:
	PermanentStimulusSensor() = default;
	virtual ~PermanentStimulusSensor() = default;	
};

class DynamicStimulusSensor : public BaseSensor
{
public:
	DynamicStimulusSensor() = default;
	virtual ~DynamicStimulusSensor() = default;	
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
		assert(memory.GetSize() <= 1);
		
		std::shared_ptr<IPredicate> predicate = nullptr;
		
		memory.PerformActionForEach(
			[&predicate](std::shared_ptr<IStimulus> stimulus) -> bool
			{
				predicate =  std::make_shared<BasePredicate>("InstantPredicate");
				return true;
			});

		return predicate;
	}
};

class PermanentStimulusGoal : public BaseGoal
{
public:
	PermanentStimulusGoal() = default;
	virtual ~PermanentStimulusGoal() = default;

protected:
	std::shared_ptr<IPredicate>
    DoTransformStimulusIntoPredicates(const ShortTermMemory<IStimulus>& memory) const override
	{
		assert(memory.GetSize() <= 1);
		std::shared_ptr<IPredicate> predicate = nullptr;
		
		memory.PerformActionForEach(
            [&predicate](std::shared_ptr<IStimulus> stimulus) -> bool
            {
                predicate = std::make_shared<PermanentPredicate>();
                return true;
            });

		return predicate;
	}
};

class DynamicStimulusGoal : public BaseGoal
{
public:
	DynamicStimulusGoal() = default;
	virtual ~DynamicStimulusGoal() = default;

protected:
	std::shared_ptr<IPredicate>
    DoTransformStimulusIntoPredicates(const ShortTermMemory<IStimulus>& memory) const override
	{
		assert(memory.GetSize() <= 1);
		std::shared_ptr<IPredicate> predicate = nullptr;
		
		memory.PerformActionForEach(
            [&predicate](std::shared_ptr<IStimulus> stimulus) -> bool
            {
            	const auto dynamicStimulus = std::static_pointer_cast<DynamicStimulus>(stimulus);
                predicate = std::make_shared<DynamicPredicate>(dynamicStimulus->GetValue());
            	
                return true;
            });

		return predicate;
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
		ASSERT_TRUE(agent->GetPredicates().size() == 1);
	}
}

//Stimulus of something that happens and will never change, it remains the same forever, like a Building.
//This stimulus will be in memory for a while, if its seen again then will be updated in memory.
//The predicate created if the case will be always the same.

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfStaticPermanentThingWhichAlreadyExists_Then_PredicateIsAlwaysTheSame) 
{
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	PermanentStimulusSensor permanentStimulusSensor;
	
	permanentStimulusSensor.Subscribe(sensorySystem);

	const auto perceptionSystem = std::make_shared<NiceMock<MyPerceptionSystemMock>>(sensorySystem);

	std::vector<std::shared_ptr<IGoal>> goals = {std::make_shared<PermanentStimulusGoal>()};
	std::vector<std::shared_ptr<IPredicate>> predicates;
	auto agent = std::make_shared<MyAgent>(
        std::make_shared<TreeGoapPlanner>(),
        goals,
        predicates,
        perceptionSystem);

	agent->AddSensoryThreshold(typeid(PermanentStimulus).name(), std::make_shared<AcceptAllThreshold>());

	permanentStimulusSensor.NotifyAll(std::make_shared<PermanentStimulus>());
	agent->StartUp();
	agent->Update(0.016f);
	for(auto i = 0; i < 10; ++i)
	{
		agent->Update(0.016f);
		permanentStimulusSensor.NotifyAll(std::make_shared<PermanentStimulus>());
		ASSERT_TRUE(sensorySystem->GetReceivedStimulus().size() == 1);
		ASSERT_TRUE(perceptionSystem->GetMemory().GetSize() == 1);
		ASSERT_TRUE(agent->GetPredicates().size() == 1);
	}
}

//Stimulus of something that can be changing during time. For instance, another agent.
//We want to remember once it disappears as stimulus for a while.
//The predicate created if the case will be updated everytime, will be always the same

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfDinamicThingWhichAlreadyExists_Then_PredicateIsUpdated) 
{
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	DynamicStimulusSensor dynamicStimulusSensor;
	
	dynamicStimulusSensor.Subscribe(sensorySystem);

	const auto perceptionSystem = std::make_shared<NiceMock<MyPerceptionSystemMock>>(sensorySystem);

	std::vector<std::shared_ptr<IGoal>> goals = {std::make_shared<DynamicStimulusGoal>()};
	std::vector<std::shared_ptr<IPredicate>> predicates;
	auto agent = std::make_shared<MyAgent>(
        std::make_shared<TreeGoapPlanner>(),
        goals,
        predicates,
        perceptionSystem);

	agent->AddSensoryThreshold(typeid(DynamicStimulus).name(), std::make_shared<AcceptAllThreshold>());

	dynamicStimulusSensor.NotifyAll(std::make_shared<DynamicStimulus>(0));
	agent->StartUp();
	agent->Update(0.016f);
	for(auto i = 0; i < 10; ++i)
	{
		agent->Update(0.016f);
		dynamicStimulusSensor.NotifyAll(std::make_shared<DynamicStimulus>(i+1));
		ASSERT_TRUE(sensorySystem->GetReceivedStimulus().size() == 1);
		ASSERT_TRUE(perceptionSystem->GetMemory().GetSize() == 1);
		perceptionSystem->GetMemory().PerformActionForEach(
			[i](std::shared_ptr<IStimulus> stimulus)-> bool
			{
				auto dynamicStimulus = std::static_pointer_cast<DynamicStimulus>(stimulus);
				EXPECT_TRUE(dynamicStimulus->GetValue() == i);
				return true;
			});
		ASSERT_TRUE(agent->GetPredicates().size() == 1);
		auto dynamicPredicate = std::static_pointer_cast<DynamicPredicate>(agent->GetPredicates()[0]);
		ASSERT_TRUE(dynamicPredicate->GetValue() == i);
	}
}

TEST(NAI_AgentWithSpecificStimulusCases, When_StimulusOfDinamicThingDisappearsAfterAWhile_Then_PredicateRemainsForEverWithTheLastValueUpdated) 
{
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	DynamicStimulusSensor dynamicStimulusSensor;
	
	dynamicStimulusSensor.Subscribe(sensorySystem);

	const auto perceptionSystem = std::make_shared<NiceMock<MyPerceptionSystemMock>>(sensorySystem);

	std::vector<std::shared_ptr<IGoal>> goals = {std::make_shared<DynamicStimulusGoal>()};
	std::vector<std::shared_ptr<IPredicate>> predicates;
	auto agent = std::make_shared<MyAgent>(
        std::make_shared<TreeGoapPlanner>(),
        goals,
        predicates,
        perceptionSystem);

	agent->AddSensoryThreshold(typeid(DynamicStimulus).name(), std::make_shared<AcceptAllThreshold>());

	dynamicStimulusSensor.NotifyAll(std::make_shared<DynamicStimulus>(0));
	agent->StartUp();
	agent->Update(1.0f);
	for(auto i = 0; i < 10; ++i)
	{
		agent->Update(1.0f);
		dynamicStimulusSensor.NotifyAll(std::make_shared<DynamicStimulus>(i+1));
	}
	
	perceptionSystem->GetMemory().PerformActionForEach(
            [](std::shared_ptr<IStimulus> stimulus)-> bool
            {
                auto dynamicStimulus = std::static_pointer_cast<DynamicStimulus>(stimulus);
                EXPECT_TRUE(dynamicStimulus->GetValue() == 9);
                return true;
            });
	auto dynamicPredicate = std::static_pointer_cast<DynamicPredicate>(agent->GetPredicates()[0]);
	ASSERT_TRUE(dynamicPredicate->GetValue() == 9);

	//Now, the dynamic stimulus disappears, and remains in the memory for a while with the same value.
	for(auto i = 0; i < 59; ++i)
	{
		//the first frame it has the last new stimulus with 10.
		agent->Update(1.0f);
		ASSERT_TRUE(sensorySystem->GetReceivedStimulus().size() == 0);
		ASSERT_TRUE(perceptionSystem->GetMemory().GetSize() == 1);
		perceptionSystem->GetMemory().PerformActionForEach(
            [i](std::shared_ptr<IStimulus> stimulus)-> bool
            {
                auto dynamicStimulus = std::static_pointer_cast<DynamicStimulus>(stimulus);
                EXPECT_TRUE(dynamicStimulus->GetValue() == 10);
                return true;
            });
		ASSERT_TRUE(agent->GetPredicates().size() == 1);
		auto dynamicPredicate = std::static_pointer_cast<DynamicPredicate>(agent->GetPredicates()[0]);
		ASSERT_TRUE(dynamicPredicate->GetValue() == 10);
	}

	agent->Update(1.0f);
	ASSERT_TRUE(perceptionSystem->GetMemory().GetSize() == 0);
	ASSERT_TRUE(agent->GetPredicates().size() == 1);
}