#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "source/goap/IAction.h"
#include "source/goap/goals/GoToGoal.h"
#include "source/goap/BasePredicate.h"
#include "source/goap/GoapPlanner.h"
#include "source/goap/agent/BaseAgent.h"
#include "source/goap/predicates/GoapPredicates.h"
#include "source/navigation/INavigationPlanner.h"
#include "source/navigation/INavigationPath.h"

#include <memory>

using namespace NAI::Goap;
using namespace NAI::Navigation;
using ::testing::NiceMock;


class AgentWalkerMock : public BaseAgent
{
public:
	AgentWalkerMock(const glm::vec3& position, float speed, std::shared_ptr<IGoapPlanner> goapPlanner,
		std::vector<std::shared_ptr<IGoal>>& goals,
		std::vector<std::shared_ptr<IPredicate>>& predicates) : BaseAgent(goapPlanner, goals, predicates),
		mPosition { position },
		mSpeed { speed }
	{
	}

	virtual ~AgentWalkerMock() = default;

	glm::vec3 GetPosition() const override { return mPosition; }
	void MoveTo(float elapsedTime, const glm::vec3& point) override 
	{
		auto direction = glm::normalize(point - mPosition);

		mPosition = mPosition + direction * mSpeed * elapsedTime;
	}

private:
	glm::vec3 mPosition;
	float mSpeed;
};

class NavigationPathMock : public INavigationPath
{
public:
	NavigationPathMock(const std::vector<glm::vec3>& points) : mPoints { points }
	{
		ON_CALL(*this, Empty).WillByDefault(
		[this]()
		{
			return mPoints.empty();
		});
		ON_CALL(*this, HasReachedPoint).WillByDefault(
			[this](int index, const glm::vec3& position, float precision)
			{
				return glm::distance(mPoints[index], position) < precision;
			});
		ON_CALL(*this, GetPoint).WillByDefault(
			[this](int index)
			{
				return mPoints[index];
			});
		ON_CALL(*this, IsEndOfPath).WillByDefault(
			[this](int index)
			{
				return index == mPoints.size() - 1;
			});
	}

	virtual ~NavigationPathMock() = default;

	MOCK_CONST_METHOD0(Empty, bool());
	MOCK_CONST_METHOD3(HasReachedPoint, bool(int, const glm::vec3&, float));
	MOCK_CONST_METHOD1(GetPoint, glm::vec3(int));
	MOCK_CONST_METHOD1(IsEndOfPath, bool(int));

	private:
		std::vector<glm::vec3> mPoints;
};

class NavigationPlannerMock: public INavigationPlanner
{
public:
	NavigationPlannerMock()
	{
		ON_CALL(*this, GetLocationGivenAName).WillByDefault(
			[this](const std::string& locationName)
			{
				if (locationName == "Saloon")
				{
					return glm::vec3(10, 0, 10);
				}
				else
				{
					return glm::vec3(0.0f);
				}
			});

		ON_CALL(*this, GetPathFromTo).WillByDefault(
			[this](const glm::vec3& origin, const glm::vec3& destination, std::function<void(std::shared_ptr<INavigationPath>)> callback)
			{
				std::vector<glm::vec3> path;
				path.push_back(origin);
				path.push_back(glm::vec3(0, 0, 5));
				path.push_back(glm::vec3(5, 0, 5));
				path.push_back(glm::vec3(5, 0, 10));
				path.push_back(destination);

				callback(std::make_shared<NiceMock<NavigationPathMock>>(path));
			});
	}
	virtual ~NavigationPlannerMock() = default;

	MOCK_CONST_METHOD1(GetLocationGivenAName, glm::vec3(const std::string&));
	MOCK_CONST_METHOD3(GetPathFromTo, void(const glm::vec3&, const glm::vec3&, std::function<void(std::shared_ptr<INavigationPath>)>));
};


TEST(NAI_GoToGoalTest, When_AgentHasToGo_Then_Arrives)
{
	auto goapPlanner = std::make_shared<NiceMock<GoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>();

	std::string destinationPlaceName("Saloon");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoToSaloon", destinationPlaceName));
	
	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<GoToGoal> goal = std::make_shared<GoToGoal>(navigationPlanner);
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	float speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	//TODO this create can be of BaseGoal and implemented into the GoToGoal as DoCreate
	//then, we need a mechanism to call the DoCreate from BaseGoal and cannot be on a constructor
	//because of shared of this. Perhaps the agent can call create all goals.
	
	goal->Create(agent);	
	
	for(auto i = 0; i < 132; ++i)
	{
		agent->Update(0.016f);
	}

	ASSERT_TRUE(agent->HasPredicate(Predicates::PREDICATE_AT_PLACE->GetID()));
	ASSERT_TRUE(glm::distance(agent->GetPosition(), navigationPlanner->GetLocationGivenAName(destinationPlaceName)) < 0.1f);
}

TEST(NAI_GoToGoalTest, When_AgentHasToGoAndNewPredicate_Then_Abort)
{
}

TEST(NAI_GoToGoalTest, When_AgentHasTwoPlacesToGo_Then_ArrivesAtPlaceWithLessCost)
{
}

