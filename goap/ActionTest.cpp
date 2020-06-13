#include "pch.h"
#include "../../NAI/source/goap/IAction.h"
#include "../../NAI/source/goap/BaseAction.h"
#include "../../NAI/source/goap/IPredicate.h"

#include <memory>

using namespace NAI::Goap;

TEST(NAI_Action, When_NoPredicates_Then_CostIsZero) 
{
	std::vector<std::shared_ptr<IPredicate>> preconditions;
	std::vector<std::shared_ptr<IPredicate>> postconditions;

	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	ASSERT_EQ(action->GetCost(), 0);
}
