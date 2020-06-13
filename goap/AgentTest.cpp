#include "pch.h"
#include "source/goap/agent/IAgent.h"
#include "source/goap/agent/BaseAgent.h"

#include <memory>

using namespace NAI::Goap;

TEST(NAI_Agent, When_Start_Then_AgentIsIdle) 
{
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>();

	agent->Update(0.0f);

	ASSERT_TRUE(agent->GetCurrentState() == AgentState::STATE_IDLE);
}

TEST(NAI_Agent, When_Idle_Then_GetPlan)
{
	std::shared_ptr<IAgent> agent = std::make_shared<BaseAgent>();

	agent->Update(0.0f);

	//call getPlan of goal planner
	//ASSERT_TRUE();
}
