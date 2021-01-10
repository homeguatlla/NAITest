#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/BaseAction.h"
#include "goap/BaseGoal.h"
#include "goap/BasePredicate.h"
#include "goap/GoapUtils.h"

#include "goap/agent/BaseAgent.h"
#include "goap/sensory/BaseSensor.h"

using ::testing::NiceMock;
using  namespace NAI::Goap;

const auto foodPredicate = std::make_shared<BasePredicate>("FOOD");

class VisionStimulus : public IStimulus
{
public:
    VisionStimulus() = default;
    virtual ~VisionStimulus() = default;
};

class FoodStimulus : public VisionStimulus
{
public:
    FoodStimulus() = default;
    virtual ~FoodStimulus() = default;
    
    std::string GetClassName() const override { return typeid(FoodStimulus).name(); }
    glm::vec3 GetPosition() const override { return glm::vec3(0.0f); }
};

class VisionThreshold : public IThreshold
{
public:
    VisionThreshold() = default;
    virtual ~VisionThreshold() = default;

    bool IsStimulusPerceived(std::shared_ptr<IStimulus> stimulus) const override
    {
        return true;
    }
};

class ChickenVisionSensor : public BaseSensor
{
public:
    ChickenVisionSensor() = default;
    virtual ~ChickenVisionSensor() = default;

    void NotifyFood()
    {
        NotifyAll(std::make_shared<FoodStimulus>());
    }
};

class Chicken : public BaseAgent
{
public:
    Chicken(const std::vector<std::shared_ptr<IGoal>>& goals,
            const std::shared_ptr<PerceptionSystem> perceptionSystem) :
        BaseAgent(std::make_shared<NiceMock<DirectGoapPlanner>>(), goals, {}, perceptionSystem),
        mHungry(1)
    {
        
    }
    virtual ~Chicken() = default;


    glm::vec3 GetPosition() const override { return mPosition; }
    void MoveTo(float elapsedTime, const glm::vec3& point) override {}
    
    void Eat()
    {
        mHungry--;
        mHungry = std::max(0, mHungry);
        std::cout << "Eating..." << std::endl;
    }
    
    bool HasHungry() const { return mHungry > 0; }

private:
    glm::vec3 mPosition;
    int mHungry;
};

class EatAction : public BaseAction
{
public:
    EatAction(
        const std::vector<std::shared_ptr<IPredicate>>& preConditions,
        const std::vector<std::shared_ptr<IPredicate>>& postConditions,
        const std::weak_ptr<Chicken>& agent) :
        BaseAction(preConditions, postConditions),
        mAgent{agent}
    {
        mHasAccomplished = false;
    }
    
    virtual ~EatAction() = default;

    void Process(float elapsedTime) override
    {
        if (auto agent = mAgent.lock())
        {
            if(agent->HasHungry())
            {
                agent->Eat();
                mHasAccomplished = true;
            }
        }
    }
private:
    std::weak_ptr<Chicken> mAgent;
};

class EatGoal : public BaseGoal
{
public:
    EatGoal()
    {
        
    }
    
    void DoCreate(const std::shared_ptr<IAgent>& agent) override
    {
        mChicken = std::static_pointer_cast<Chicken>(agent);
        Reset();
    }

    void DoReset() override
    {
        std::vector<std::shared_ptr<IPredicate>> preConditions = {foodPredicate};
        std::vector<std::shared_ptr<IPredicate>> postConditions;
        const auto eatAction = std::make_shared<EatAction>(preConditions, postConditions, mChicken);
        mActions.push_back(eatAction);
    }

    void DoAccomplished(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
        Utils::RemovePredicateWith(predicates, "FOOD");
    }
    
    virtual ~EatGoal() = default;

private:
    std::shared_ptr<Chicken> mChicken;
};



TEST(Chiken, When_ChickenAlive_Then_NoPredicates) 
{
    const Chicken chicken({},nullptr);

    ASSERT_TRUE(chicken.GetPredicates().empty());
}

TEST(Chiken, When_ChickenHasFoodNear_Then_Eat) 
{
    auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
    ChickenVisionSensor visionSensor;
    visionSensor.Subscribe(sensorySystem);

    const auto perceptionSystem = std::make_shared<PerceptionSystem>(sensorySystem);
    auto eatGoal = std::make_shared<EatGoal>();
    
    eatGoal->AddStimulusAcceptance(
        typeid(FoodStimulus).name(),
        [](std::shared_ptr<IStimulus> stimulus)
        {
            return foodPredicate;
        });

    const std::vector<std::shared_ptr<IGoal>> goals = { eatGoal };

    auto chicken = std::make_shared<Chicken>(goals, perceptionSystem);

    const auto visionStimulus = std::make_shared<FoodStimulus>();
    const auto visionThreshold = std::make_shared<VisionThreshold>();
    
    chicken->AddSensoryThreshold(visionStimulus->GetClassName(), visionThreshold);

    visionSensor.NotifyFood();

    ASSERT_TRUE(chicken->HasHungry());
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    chicken->Update(0.16f);
    chicken->Update(0.16f);

    ASSERT_FALSE(chicken->HasHungry());
}