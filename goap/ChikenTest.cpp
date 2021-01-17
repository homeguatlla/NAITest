#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/BaseAction.h"
#include "goap/BaseGoal.h"
#include "goap/BasePredicate.h"
#include "goap/GoapUtils.h"

#include "goap/agent/BaseAgent.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/sensory/BaseSensor.h"

using ::testing::NiceMock;
using  namespace NAI::Goap;


class FoodPredicate : public BasePredicate
{
public:
    FoodPredicate() : BasePredicate("FOOD") {};

    FoodPredicate(const glm::vec3& position, unsigned int amount) : BasePredicate("FOOD"),
    mPosition{position},
    mAmount{amount} {}
    
    virtual ~FoodPredicate() = default;

    glm::vec3 GetPosition() const { return mPosition; }
    unsigned int GetAmount() const { return mAmount; }

private:
    glm::vec3 mPosition;
    unsigned int mAmount;
};

class VisionStimulus : public IStimulus
{
public:
    VisionStimulus() = default;
    virtual ~VisionStimulus() = default;
};

class FoodStimulus : public VisionStimulus
{
public:
    FoodStimulus(const glm::vec3& position, unsigned int amount): mPosition {position}, mAmount{amount} {}
    
    virtual ~FoodStimulus() = default;
    
    std::string GetClassName() const override { return typeid(FoodStimulus).name(); }
    glm::vec3 GetPosition() const override { return mPosition; }
    unsigned int GetAmount() const { return mAmount; }
    
public:
    glm::vec3 mPosition;
    unsigned int mAmount;
};

class ChickenStimulus : public VisionStimulus
{
public:
    ChickenStimulus(const glm::vec3& position): mPosition {position} {}
    
    virtual ~ChickenStimulus() = default;
    
    std::string GetClassName() const override { return typeid(ChickenStimulus).name(); }
    glm::vec3 GetPosition() const override { return mPosition; }
    
public:
    glm::vec3 mPosition;
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

    void NotifyFood(const glm::vec3& position, unsigned int amount)
    {
        NotifyAll(std::make_shared<FoodStimulus>(position, amount));
    }
    
    void NotifyChicken(const glm::vec3& position)
    {
        NotifyAll(std::make_shared<ChickenStimulus>(position));
    }
};

class Chicken : public BaseAgent
{
public:
    Chicken(const std::vector<std::shared_ptr<IGoal>>& goals,
            const std::shared_ptr<PerceptionSystem> perceptionSystem, unsigned int hungry) :
        BaseAgent(std::make_shared<NiceMock<TreeGoapPlanner>>(), goals, {}, perceptionSystem),
        mHungry(hungry)
    {
        
    }
    virtual ~Chicken() = default;


    glm::vec3 GetPosition() const override { return mPosition; }
    void MoveTo(float elapsedTime, const glm::vec3& point) override {}
    
    void Eat(unsigned int amount)
    {
        mHungry -= amount;
        mHungry = std::max(0, mHungry);
        std::cout << "Eating " << amount << " of food..." << std::endl;
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
                const auto predicateMatch = GetPredicateMatchedPreconditionWithIndex(0);
                const auto foodPredicate = std::static_pointer_cast<FoodPredicate>(predicateMatch);
                agent->Eat(foodPredicate->GetAmount());
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
    
    virtual ~EatGoal() = default;
    
    void DoCreate(const std::shared_ptr<IAgent>& agent) override
    {
        mChicken = std::static_pointer_cast<Chicken>(agent);
        Reset();
    }

    void DoReset() override
    {
        std::vector<std::shared_ptr<IPredicate>> preConditions = {std::make_shared<FoodPredicate>()};
        std::vector<std::shared_ptr<IPredicate>> postConditions;
        const auto eatAction = std::make_shared<EatAction>(preConditions, postConditions, mChicken);
        mActions.push_back(eatAction);
    }

    void DoAccomplished(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
        Utils::RemovePredicateWith(predicates, "FOOD");
    }

    std::shared_ptr<IPredicate> DoTransformStimulusIntoPredicates(const Memory<IStimulus>& memory) const override
    {
        std::vector<std::shared_ptr<FoodStimulus>> foodStimulusList;
        std::vector<std::shared_ptr<ChickenStimulus>> chickenStimulusList;
        
        memory.PerformActionForEach(
            [&foodStimulusList, &chickenStimulusList](std::shared_ptr<IStimulus> stimulus) -> bool
            {
                if(stimulus->GetClassName() == typeid(FoodStimulus).name())
                {
                    const auto foodStimulus = std::static_pointer_cast<FoodStimulus>(stimulus);
                    foodStimulusList.push_back(foodStimulus);
                    
                    return true;
                }
                if(stimulus->GetClassName() == typeid(ChickenStimulus).name())
                {
                    const auto chickenStimulus = std::static_pointer_cast<ChickenStimulus>(stimulus);
                    chickenStimulusList.push_back(chickenStimulus);
                    
                    return true;
                }
                return false;
            });

        if(foodStimulusList.empty())
        {
            return nullptr;
        }
        
        std::sort(foodStimulusList.begin(), foodStimulusList.end(),
            [this](const std::shared_ptr<FoodStimulus>& a, const std::shared_ptr<FoodStimulus>& b)->bool
            {
                return glm::distance(a->GetPosition(), mChicken->GetPosition()) < glm::distance(b->GetPosition(), mChicken->GetPosition());
            });
        
        float minDistance = std::numeric_limits<float>::max();
        std::shared_ptr<FoodStimulus> nearFood = nullptr;
            
        for(auto chicken : chickenStimulusList)
        {
            for(auto food : foodStimulusList)
            {
                auto distance = glm::distance(food->GetPosition(), chicken->GetPosition());
                if(distance < minDistance)
                {
                    nearFood = food;
                    minDistance = distance;
                }
            }
        }
        if(nearFood == nullptr && !foodStimulusList.empty())
        {
            nearFood = foodStimulusList[0];
        }
       
        return std::make_shared<FoodPredicate>(nearFood->GetPosition(), nearFood->GetAmount());        
    }

public:
    unsigned GetCost(std::vector<std::shared_ptr<IPredicate>>& inputPredicates) const override
    {
        //We know there is always only one predicate into the list.
        if(!mChicken->HasHungry())
        {
            return std::numeric_limits<unsigned>::max();
        }
        
        const auto foodPredicate = std::static_pointer_cast<FoodPredicate>(inputPredicates[0]);
        return static_cast<unsigned int>(glm::distance(mChicken->GetPosition(), foodPredicate->GetPosition()));
    }

private:
    std::shared_ptr<Chicken> mChicken;
};

std::shared_ptr<EatGoal> CreateEatGoal()
{
    auto eatGoal = std::make_shared<EatGoal>();
    return eatGoal;
}

TEST(Chiken, When_ChickenAlive_Then_NoPredicates) 
{
    const Chicken chicken({},nullptr, 1);

    ASSERT_TRUE(chicken.GetPredicates().empty());
}

TEST(Chiken, When_ChickenHasFoodNear_Then_Eat) 
{
    auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
    ChickenVisionSensor visionSensor;
    visionSensor.Subscribe(sensorySystem);

    const auto perceptionSystem = std::make_shared<PerceptionSystem>(sensorySystem);
   
    const auto eatGoal = CreateEatGoal();
    const std::vector<std::shared_ptr<IGoal>> goals = { eatGoal };

    auto chicken = std::make_shared<Chicken>(goals, perceptionSystem, 1);
    chicken->AddSensoryThreshold(typeid(FoodStimulus).name(), std::make_shared<VisionThreshold>());

    visionSensor.NotifyFood(glm::vec3(0.0f), 1);

    ASSERT_TRUE(chicken->HasHungry());
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    chicken->Update(0.16f);
    chicken->Update(0.16f);

    ASSERT_FALSE(chicken->HasHungry());
}

TEST(Chiken, When_ChickenHasMoreThanOneFoodNear_Then_EatTheNearerFood) 
{
    auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
    ChickenVisionSensor visionSensor;
    visionSensor.Subscribe(sensorySystem);

    const auto perceptionSystem = std::make_shared<PerceptionSystem>(sensorySystem);
    const auto eatGoal = CreateEatGoal();
    const std::vector<std::shared_ptr<IGoal>> goals = { eatGoal };

    auto chicken = std::make_shared<Chicken>(goals, perceptionSystem, 3);
    chicken->AddSensoryThreshold(typeid(FoodStimulus).name(), std::make_shared<VisionThreshold>());

    //Notify more than one food.
    visionSensor.NotifyFood(glm::vec3(1.0f, 0.0, 0.0f), 1);
    visionSensor.NotifyFood(glm::vec3(0.0f), 4);

    ASSERT_TRUE(chicken->HasHungry());
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    chicken->Update(0.16f);
    chicken->Update(0.16f);

    ASSERT_FALSE(chicken->HasHungry());
}

TEST(Chiken, When_ChickenHasMoreThanOneFoodNear_Then_EatFoodNearerOtherChicken) 
{
    auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
    ChickenVisionSensor visionSensor;
    visionSensor.Subscribe(sensorySystem);

    const auto perceptionSystem = std::make_shared<PerceptionSystem>(sensorySystem);
    
    const auto eatGoal = CreateEatGoal();
    const std::vector<std::shared_ptr<IGoal>> goals = { eatGoal };

    auto chicken = std::make_shared<Chicken>(goals, perceptionSystem, 3);
    chicken->AddSensoryThreshold(typeid(FoodStimulus).name(), std::make_shared<VisionThreshold>());
    chicken->AddSensoryThreshold(typeid(ChickenStimulus).name(), std::make_shared<VisionThreshold>());

    //Notify more than one food one nearer other chicken
    visionSensor.NotifyFood(glm::vec3(0.0f, 0.0, 0.0f), 5);
    visionSensor.NotifyFood(glm::vec3(1.0f, 0.0, 1.0f), 5);
    visionSensor.NotifyFood(glm::vec3(1.0f, 0.0f, -1.0f), 5);
    visionSensor.NotifyChicken(glm::vec3(1.0, 0.0, 1.5f));

    ASSERT_TRUE(chicken->HasHungry());
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    chicken->Update(0.16f);
    chicken->Update(0.16f);

    ASSERT_FALSE(chicken->HasHungry());
}

