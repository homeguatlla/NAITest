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

class NavigationPathMock : public INavigationPath
{
public:
	NavigationPathMock()
	{
		ON_CALL(*this, Empty).WillByDefault(
		[this]()
		{
			return false;
		});
	}

	virtual ~NavigationPathMock() = default;

	MOCK_CONST_METHOD0(Empty, bool());
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
				callback(std::make_shared<NavigationPathMock>());
			});
	}
	virtual ~NavigationPlannerMock() = default;

	MOCK_CONST_METHOD1(GetLocationGivenAName, glm::vec3(const std::string&));
	MOCK_CONST_METHOD3(GetPathFromTo, void(const glm::vec3&, const glm::vec3&, std::function<void(std::shared_ptr<INavigationPath>)>));
};


TEST(NAI_GoToGoalTest, When_AgentHasToGo_Then_Arrives)
{
	auto goapPlanner = std::make_shared<GoapPlanner>();
	auto navigationPlanner = std::make_shared<NavigationPlannerMock>();

	std::vector<std::shared_ptr<IPredicate>> predicates;
	predicates.push_back(std::make_shared<GoToPredicate>("GoToSaloon", "Saloon"));

	std::vector<std::shared_ptr<IGoal>> goals;
	std::shared_ptr<GoToGoal> goal = std::make_shared<GoToGoal>(navigationPlanner);
	goals.push_back(goal);

	auto agent = std::make_shared<BaseAgent>(goapPlanner, goals, predicates);

	//TODO this create can be of BaseGoal and implemented into the GoToGoal as DoCreate
	//then, we need a mechanism to call the DoCreate from BaseGoal and cannot be on a constructor
	//because of shared of this. Perhaps the agent can call create all goals.
	goal->Create(agent);	

	agent->Update(0.0f); //validate goal can be executed
	agent->Update(0.0f); //Starts executing first action

	ASSERT_TRUE(agent->HasPredicate(PREDICATE_AT_PLACE->GetID()));
}

TEST(NAI_GoToGoalTest, When_AgentHasTwoPlacesToGo_Then_ArrivesAtPlaceWithLessCost)
{
}

