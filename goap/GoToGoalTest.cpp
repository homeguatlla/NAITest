#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/IAction.h"
#include "goap/goals/GoToGoal.h"
#include "goap/BasePredicate.h"
#include "goap/planners/DirectGoapPlanner.h"
#include "goap/planners/TreeGoapPlanner.h"
#include "goap/agent/BaseAgent.h"
#include "goap/predicates/GoapPredicates.h"
#include "navigation/INavigationPlanner.h"
#include "navigation/INavigationPath.h"

#include <memory>

using namespace NAI::Goap;
using namespace NAI::Navigation;
using ::testing::NiceMock;

static const float MOVEMENT_PRECISION = 0.01f;

class AgentWalkerMock : public BaseAgent
{
public:
	AgentWalkerMock(const glm::vec3& position, float speed, std::shared_ptr<IGoapPlanner> DirectGoapPlanner,
		std::vector<std::shared_ptr<IGoal>>& goals,
		std::vector<std::shared_ptr<IPredicate>>& predicates) : BaseAgent(DirectGoapPlanner, goals, predicates),
		mPosition { position },
		mSpeed { speed }
	{
	}

	virtual ~AgentWalkerMock() = default;

	glm::vec3 GetPosition() const override { return mPosition; }
	void MoveTo(float elapsedTime, const glm::vec3& point) override 
	{
		const auto direction = glm::normalize(point - mPosition);

		mPosition = mPosition + direction * mSpeed * elapsedTime;

		if(glm::dot(direction, point - mPosition) < 0)
		{
			//we gone farther, then we are at destination
			mPosition = point;
		}
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
	NavigationPlannerMock(const std::vector<glm::vec3>& path) : mPath {path}
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
					return glm::vec3(7, 1, 10);
				}
			});

		ON_CALL(*this, GetPathFromTo).WillByDefault(
			[this](const glm::vec3& origin, const glm::vec3& destination, std::function<void(std::shared_ptr<INavigationPath>)> callback)
			{
				std::vector<glm::vec3> path;
				path.push_back(origin);
				path.insert(path.end(), mPath.begin(), mPath.end());
				path.push_back(destination);

				callback(std::make_shared<NiceMock<NavigationPathMock>>(path));
			});
			
		ON_CALL(*this, GetAproxCost).WillByDefault(
			[this](const glm::vec3& origin, const glm::vec3& destination)
			{
				if (destination == glm::vec3(7, 1, 10))
				{
					return 3;
				}
				else
				{
					return 5;
				}
			});
	}
	virtual ~NavigationPlannerMock() = default;

	MOCK_CONST_METHOD1(GetLocationGivenAName, glm::vec3(const std::string&));
	MOCK_METHOD3(GetPathFromTo, void(const glm::vec3&, const glm::vec3&, PathFromToCallback));
	MOCK_CONST_METHOD2(GetAproxCost, unsigned int(const glm::vec3& origin, const glm::vec3& destination));

private:
	std::vector<glm::vec3> mPath;
};

class GoToGoalMock : public GoToGoal
{
public:
	GoToGoalMock(std::shared_ptr<INavigationPlanner> planner) : GoToGoal(planner)
	{
	}
	MOCK_METHOD0(DoCancel, void());
};


TEST(NAI_GoToGoalTest, When_AgentHasToGo_Then_Arrives)
{
	std::vector<glm::vec3> path;
	path.emplace_back(glm::vec3(0, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 10));

	auto goapPlanner = std::make_shared<NiceMock<DirectGoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>(path);

	std::string destinationPlaceName("Saloon");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceName));
	
	std::vector<std::shared_ptr<IGoal>> goals;
	const auto goal = std::make_shared<NiceMock<GoToGoalMock>>(navigationPlanner);
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	auto speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	agent->StartUp();
	
	for(auto i = 0; i < 936; ++i)
	{
		agent->Update(0.016f);
	}

	ASSERT_TRUE(agent->WhereIam() == "Saloon");
	ASSERT_TRUE(glm::distance(agent->GetPosition(), navigationPlanner->GetLocationGivenAName(destinationPlaceName)) < MOVEMENT_PRECISION);
}

TEST(NAI_GoToGoalTest, When_AgentHasToGoAndNewPredicate_Then_Abort)
{
	std::vector<glm::vec3> path;
	path.emplace_back(glm::vec3(0, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 10));
	
	auto goapPlanner = std::make_shared<NiceMock<DirectGoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>(path);

	std::string destinationPlaceName("Saloon");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceName));

	std::vector<std::shared_ptr<IGoal>> goals;
	const auto goal = std::make_shared<NiceMock<GoToGoalMock>>(navigationPlanner);	
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	auto speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	agent->StartUp();

	for (auto i = 0; i < 73; ++i)
	{
		agent->Update(0.016f);	
	}
	ASSERT_TRUE(agent->GetPosition() == glm::vec3(5.0f, 0.0f, 5.63999939f));
	EXPECT_CALL(*goal, DoCancel).Times(1);

	agent->OnNewPredicate(std::make_shared<BasePredicate>("NewPredicate"));
	agent->Update(0.016f);

	ASSERT_TRUE(agent->GetPosition() == glm::vec3(5.0f, 0.0f, 5.63999939f));
	ASSERT_FALSE(agent->WhereIam() == "Saloon");
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_GoToGoalTest, When_AgentHasToGoAndNewPredicate_Then_AbortAndRestartsTheSameGoal)
{
	std::vector<glm::vec3> path;
	path.emplace_back(glm::vec3(0, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 5));
	path.emplace_back(glm::vec3(5, 0, 10));
	
	auto goapPlanner = std::make_shared<NiceMock<DirectGoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>(path);

	std::string destinationPlaceName("Saloon");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceName));

	std::vector<std::shared_ptr<IGoal>> goals;
	const auto goal = std::make_shared<NiceMock<GoToGoalMock>>(navigationPlanner);
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	auto speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	agent->StartUp();

	for (auto i = 0; i < 73; ++i)
	{
		agent->Update(0.016f);
	}
	ASSERT_TRUE(agent->GetPosition() == glm::vec3(5.0f, 0.0f, 5.63999939f));
	EXPECT_CALL(*goal, DoCancel).Times(1);

	agent->OnNewPredicate(std::make_shared<BasePredicate>("NewPredicate"));
	
	for (auto i = 0; i < 80; ++i)
	{
		agent->Update(0.016f);
	}

	ASSERT_TRUE(glm::distance(agent->GetPosition(), navigationPlanner->GetLocationGivenAName(destinationPlaceName)) < MOVEMENT_PRECISION);
	ASSERT_TRUE(agent->WhereIam() == "Saloon");
	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_GoToGoalTest, When_AgentHasTwoPlacesToGo_Then_ArrivesAtPlaceWithLessCost)
{
	std::vector<glm::vec3> path;
	path.emplace_back(glm::vec3(0, 0, 5));
	
	auto goapPlanner = std::make_shared<NiceMock<TreeGoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>(path);

	std::string destinationPlaceSaloonName("Saloon");
	std::string destinationPlaceGeneralStoreName("GeneralStore");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceSaloonName));
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceGeneralStoreName));

	std::vector<std::shared_ptr<IGoal>> goals;
	const auto goal = std::make_shared<NiceMock<GoToGoalMock>>(navigationPlanner);
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	auto speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	agent->StartUp();

	//from 0,0,0 to 0, 0, 5 = dist  = 5 /0.016 = 312 speed = 10 --> 35 frames (no cuadra +3)
	//from 0, 0, 5 to 7, 1, 10 = dist = 8.66 / 0.016 = 541.2 speed 10 --> 55 frames
	//total 35 + 55 = 87 frames

	//we put the number of frames to make agent stops at General Store, more frame it will continue walking.
	for (auto i = 0; i < 93; ++i)
	{
		agent->Update(0.016f);
	}

	const std::string destinationPlaceName("GeneralStore");
	ASSERT_TRUE(
		glm::distance(
			agent->GetPosition(), 
			navigationPlanner->GetLocationGivenAName(destinationPlaceName)) < MOVEMENT_PRECISION);
	ASSERT_TRUE(agent->WhereIam() == "GeneralStore");
}

TEST(NAI_GoToGoalTest, When_AgentArrivedAtPlace_GoToPlanningAndNoPlan)
{
	std::vector<glm::vec3> path;
	path.emplace_back(glm::vec3(0, 0, 5));
	
	auto goapPlanner = std::make_shared<NiceMock<TreeGoapPlanner>>();
	auto navigationPlanner = std::make_shared<NiceMock<NavigationPlannerMock>>(path);

	std::string destinationPlaceGeneralStoreName("GeneralStore");
	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoTo", destinationPlaceGeneralStoreName));

	std::vector<std::shared_ptr<IGoal>> goals;
	const auto goal = std::make_shared<NiceMock<GoToGoalMock>>(navigationPlanner);
	goals.push_back(goal);

	glm::vec3 originPoint(0.0f);
	auto speed = 10.f;

	auto agent = std::make_shared<NiceMock<AgentWalkerMock>>(originPoint, speed, goapPlanner, goals, predicates);

	agent->StartUp();

	for (auto i = 0; i < 765; ++i)
	{
		agent->Update(0.016f);
	}

	const std::string destinationPlaceName("GeneralStore");
	ASSERT_TRUE(glm::distance(agent->GetPosition(), navigationPlanner->GetLocationGivenAName(destinationPlaceName)) < MOVEMENT_PRECISION);
	ASSERT_TRUE(agent->WhereIam() == "GeneralStore");

	agent->Update(0.016f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}