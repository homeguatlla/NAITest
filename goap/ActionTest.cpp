#include "pch.h"
#include "../../NAI/source/goap/IAction.h"
#include "../../NAI/source/goap/BaseAction.h"
#include "../../NAI/source/goap/IPredicate.h"

#include <memory>

TEST(NAI_Action, When_NoPredicates_Then_CostIsZero) 
{
	std::vector<std::shared_ptr<NAI::IPredicate>> preconditions;
	std::vector<std::shared_ptr<NAI::IPredicate>> postconditions;

	auto action = std::make_shared<NAI::BaseAction>(preconditions, postconditions);

	ASSERT_EQ(action->GetCost(), 0);
}
