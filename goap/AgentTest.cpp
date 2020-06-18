#include "pchTest.h"
#include "source/goap/agent/IAgent.h"
#include "source/goap/agent/BaseAgent.h"
#include "source/goap/GoapPlanner.h"


using namespace NAI::Goap;


class GoapPlannerMock : public GoapPlanner
{
public:
	GoapPlannerMock()
	{
		//ON_CALL(*this, GetID()).WillByDefault(Return(id));
	}
	virtual ~GoapPlannerMock() = default;

	//MOCK_CONST_METHOD0(GetTest, bool());
	/*MOCK_CONST_METHOD2(GetPlan, 
		std::shared_ptr<IGoal>(
			std::vector<std::shared_ptr<IGoal>>&, 
			std::vector<std::shared_ptr<IPredicate>>&));*/
};

TEST(NAI_Agent, When_Start_Then_AgentIsPlanning) 
{
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>();

	agent->Update(0.0f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PLANNING);
}

TEST(NAI_Agent, When_Planning_Then_GetPlan)
{
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>();

	agent->Update(0.0f);

	//call getPlan of goal planner
	//ASSERT_TRUE();
}

TEST(NAI_Agent, When_Plan_Then_Process)
{
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>();

	agent->Update(0.0f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_PROCESSING);
}