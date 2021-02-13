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
    FoodPredicate() : BasePredicate(1, "FOOD"), mPosition{0.0f}, mAmount{0} {};

    FoodPredicate(int id, const glm::vec3& position, unsigned int amount) : BasePredicate(id, "FOOD"),
    mPosition{position},
    mAmount{amount} {}
    
    virtual ~FoodPredicate() = default;

    glm::vec3 GetPosition() const { return mPosition; }
    void SetPosition(const glm::vec3& position) { mPosition = position; }
    unsigned int GetAmount() const { return mAmount; }
    void SetAmount(unsigned int amount) { mAmount = amount; }

private:
    glm::vec3 mPosition;
    unsigned int mAmount;
};

class EscapePredicate : public BasePredicate
{
public:
    EscapePredicate() : BasePredicate(2, "ESCAPE"), mEscapePosition(0.0f) {}
    EscapePredicate(int id, const glm::vec3& escapePosition) : BasePredicate(id, "ESCAPE"), mEscapePosition(escapePosition) {}
    virtual ~EscapePredicate() = default;
    
    glm::vec3 GetEscapePoint() const { return mEscapePosition; }
    void SetEscapePoint(const glm::vec3& escapePoint) { mEscapePosition = escapePoint; }

private:
    glm::vec3 mEscapePosition;
};

class VisionStimulus : public IStimulus
{
public:
    VisionStimulus() = default;
    virtual ~VisionStimulus() = default;
};

class SoundStimulus : public IStimulus
{
public:
    SoundStimulus() = default;
    virtual ~SoundStimulus() = default;
};

class FoodStimulus : public VisionStimulus
{
public:
    FoodStimulus(unsigned int id, const glm::vec3& position, unsigned int amount):
        mPosition {position}, mAmount{amount}, mId { id} {}
    
    virtual ~FoodStimulus() = default;
    
    std::string GetClassName() const override { return typeid(FoodStimulus).name(); }
    glm::vec3 GetPosition() const override { return mPosition; }
    unsigned int GetAmount() const { return mAmount; }
    float GetDurationInMemory() const override { return 3.0f; }
    unsigned int GetId() const override { return mId; }
public:
    glm::vec3 mPosition;
    unsigned int mAmount;
    unsigned int mId;
};

class ChickenStimulus : public VisionStimulus
{
public:
    ChickenStimulus(unsigned int id, const glm::vec3& position): mPosition {position}, mId{id} {}
    
    virtual ~ChickenStimulus() = default;
    
    std::string GetClassName() const override { return typeid(ChickenStimulus).name(); }
    glm::vec3 GetPosition() const override { return mPosition; }
    float GetDurationInMemory() const override { return 3.0f; }
    unsigned int GetId() const override { return mId; }

public:
    glm::vec3 mPosition;
    unsigned int mId;
};

class DangerNoiseStimulus : public SoundStimulus
{
public:
    DangerNoiseStimulus(unsigned int id, const glm::vec3& position): mPosition {position}, mId{id} {}
    
    virtual ~DangerNoiseStimulus() = default;
    
    std::string GetClassName() const override { return typeid(DangerNoiseStimulus).name(); }
    glm::vec3 GetPosition() const override { return mPosition; }
    float GetDurationInMemory() const override { return 3.0f; }
    unsigned int GetId() const override { return mId; }

public:
    glm::vec3 mPosition;
    unsigned int mId;
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

class SoundThreshold : public IThreshold
{
public:
    SoundThreshold() = default;
    virtual ~SoundThreshold() = default;

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

    void NotifyFood(unsigned int id, const glm::vec3& position, unsigned int amount)
    {
        NotifyAll(std::make_shared<FoodStimulus>(id, position, amount));
    }
    
    void NotifyChicken(unsigned int id, const glm::vec3& position)
    {
        NotifyAll(std::make_shared<ChickenStimulus>(id, position));
    }
};

class ChickenListeningSensor : public BaseSensor
{
public:
    ChickenListeningSensor() = default;
    virtual ~ChickenListeningSensor() = default;

    void NotifyDanger(unsigned int id, const glm::vec3& position)
    {
        NotifyAll(std::make_shared<DangerNoiseStimulus>(id, position));
    }
};

class Chicken : public BaseAgent
{
public:
    Chicken(const std::vector<std::shared_ptr<IGoal>>& goals,
            const std::shared_ptr<PerceptionSystem> perceptionSystem, unsigned int hungry) :
        BaseAgent(std::make_shared<NiceMock<TreeGoapPlanner>>(), goals, {}, perceptionSystem),
        mHungry(hungry), mIsSave(true)
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

    void EscapeFrom(const glm::vec3& position)
    {
        std::cout << "Escaping from (" << position.x << ", " << position.y << ", " << position.z << ") " << std::endl;
        mIsSave = true;
    }
    
    bool HasHungry() const { return mHungry > 0; }
    bool IsSave() const { return mIsSave; }
    void SetInDanger() { mIsSave = false; }

private:
    glm::vec3 mPosition;
    int mHungry;
    bool mIsSave;
};

class EatAction : public BaseAction
{
public:
    EatAction(
        const std::vector<std::string>& preConditions,
        const std::vector<std::shared_ptr<IPredicate>>& postConditions,
        const std::weak_ptr<Chicken>& agent) :
        BaseAction(preConditions, postConditions),
        mAgent{agent}
    {
        mHasAccomplished = false;
    }
    
    virtual ~EatAction() = default;

    void DoProcess(float elapsedTime) override
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

    void DoReset(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
        Reset();
    }

    void Reset()
    {
        std::vector<std::string> preConditions = {"FOOD"};
        std::vector<std::shared_ptr<IPredicate>> postConditions;
        const auto eatAction = std::make_shared<EatAction>(preConditions, postConditions, mChicken);
        mActions.push_back(eatAction);
    }

    void DoAccomplished(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
        Utils::RemovePredicateWith(predicates, "FOOD");
    }

    std::shared_ptr<IPredicate> DoTransformStimulusIntoPredicates(const ShortTermMemory<IStimulus>& memory) const override
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

        return std::make_shared<FoodPredicate>(1, nearFood->GetPosition(), nearFood->GetAmount());
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

class EscapeAction : public BaseAction
{
public:
    EscapeAction(
        const std::vector<std::string>& preConditions,
        const std::vector<std::shared_ptr<IPredicate>>& postConditions,
        const std::weak_ptr<Chicken>& agent) :
        BaseAction(preConditions, postConditions),
        mAgent{agent}
    {
        mHasAccomplished = false;
    }
    
    virtual ~EscapeAction() = default;

    void DoProcess(float elapsedTime) override
    {
        if (auto agent = mAgent.lock())
        {
            if(!agent->IsSave())
            {
                const auto predicateMatch = GetPredicateMatchedPreconditionWithIndex(0);
                const auto escapePredicate = std::static_pointer_cast<EscapePredicate>(predicateMatch);
                agent->EscapeFrom(escapePredicate->GetEscapePoint());
                mHasAccomplished = true;
            }
        }
    }
private:
    std::weak_ptr<Chicken> mAgent;
};

class EscapeGoal : public BaseGoal
{
public:
    EscapeGoal()
    {
        
    }
    
    virtual ~EscapeGoal() = default;
    
    void DoCreate(const std::shared_ptr<IAgent>& agent) override
    {
        mChicken = std::static_pointer_cast<Chicken>(agent);
        Reset();
    }

    void DoReset(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
       Reset();
    }

    void Reset()
    {
        std::vector<std::string> preConditions = {"ESCAPE"};
        std::vector<std::shared_ptr<IPredicate>> postConditions;
        const auto dangerAction = std::make_shared<EscapeAction>(preConditions, postConditions, mChicken);
        mActions.push_back(dangerAction);
    }

    void DoAccomplished(std::vector<std::shared_ptr<IPredicate>>& predicates) override
    {
        Utils::RemovePredicateWith(predicates, "ESCAPE");
    }

    std::shared_ptr<IPredicate> DoTransformStimulusIntoPredicates(const ShortTermMemory<IStimulus>& memory) const override
    {
        std::vector<std::shared_ptr<DangerNoiseStimulus>> dangerStimulusList;
        
        memory.PerformActionForEach(
            [&dangerStimulusList](std::shared_ptr<IStimulus> stimulus) -> bool
            {
                if(stimulus->GetClassName() == typeid(DangerNoiseStimulus).name())
                {
                    const auto dangerStimulus = std::static_pointer_cast<DangerNoiseStimulus>(stimulus);
                    dangerStimulusList.push_back(dangerStimulus);
                    
                    return true;
                }
                return false;
            });

        if(dangerStimulusList.empty())
        {
            return nullptr;
        }
        
        glm::vec3 directionToEscape(0.0f);
        
        for(auto danger : dangerStimulusList)
        {
           directionToEscape += danger->GetPosition();
        }
        mChicken->SetInDanger();
        //This predicate is unique
        return std::make_shared<EscapePredicate>(2, directionToEscape);
    }

public:
    unsigned GetCost(std::vector<std::shared_ptr<IPredicate>>& inputPredicates) const override
    {
        //We know there is always only one predicate into the list.
        if(mChicken->IsSave())
        {
            return std::numeric_limits<unsigned>::max();
        }
        
        const auto escapePredicate = std::static_pointer_cast<EscapePredicate>(inputPredicates[0]);
        return 1;
    }

private:
    std::shared_ptr<Chicken> mChicken;
};

std::shared_ptr<EatGoal> CreateEatGoal()
{
    return std::make_shared<EatGoal>();
}

std::shared_ptr<EscapeGoal> CreateEscapeGoal()
{
    return std::make_shared<EscapeGoal>();
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

    visionSensor.NotifyFood(0, glm::vec3(0.0f), 1);

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
    visionSensor.NotifyFood(0, glm::vec3(1.0f, 0.0, 0.0f), 1);
    visionSensor.NotifyFood(1, glm::vec3(0.0f), 4);

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
    visionSensor.NotifyFood(0, glm::vec3(0.0f, 0.0, 0.0f), 5);
    visionSensor.NotifyFood(1, glm::vec3(1.0f, 0.0, 1.0f), 5);
    visionSensor.NotifyFood(2, glm::vec3(1.0f, 0.0f, -1.0f), 5);
    visionSensor.NotifyChicken(3, glm::vec3(1.0, 0.0, 1.5f));

    ASSERT_TRUE(chicken->HasHungry());
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    chicken->Update(0.16f);
    chicken->Update(0.16f);

    ASSERT_FALSE(chicken->HasHungry());
}

TEST(Chiken, When_ChickenHearsSomeNoise_Then_Escapes) 
{
    const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
    ChickenListeningSensor listeningSensor;
    listeningSensor.Subscribe(sensorySystem);

    const auto perceptionSystem = std::make_shared<PerceptionSystem>(sensorySystem);
    
    const auto escapeGoal = CreateEscapeGoal();
    const std::vector<std::shared_ptr<IGoal>> goals = { escapeGoal };

    auto chicken = std::make_shared<Chicken>(goals, perceptionSystem, 3);
    chicken->AddSensoryThreshold(typeid(DangerNoiseStimulus).name(), std::make_shared<SoundThreshold>());

    listeningSensor.NotifyDanger(1, glm::vec3(1.0f, 0.0f, 0.0f));
    
    chicken->StartUp();
    chicken->Update(0.16f); //setting current state
    
    chicken->Update(0.16f);
    ASSERT_FALSE(chicken->IsSave());

    chicken->Update(0.16f);
    ASSERT_TRUE(chicken->IsSave());
}

TEST(Chiken, When_ChickenHearsMoreThanOneNoise_Then_EscapesFromAllNoises) 
{
}