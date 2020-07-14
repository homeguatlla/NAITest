//#include "pchTest.h"
#include "gtest/gtest.h"
#include "source/goap/IAction.h"
#include "source/goap/BaseAction.h"
#include "source/goap/IPredicate.h"
#include "source/goap/BasePredicate.h"

#include <memory>
#include <vector>

using namespace NAI::Goap;

TEST(NAI_Action, When_NoPredicates_Then_CostIsZero) 
{
	std::vector<std::shared_ptr<IPredicate>> preconditions;
	std::vector<std::shared_ptr<IPredicate>> postconditions;

	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	ASSERT_EQ(action->GetCost(), 0);
}

TEST(NAI_Action, When_PredicatesDontSatisfyPreconditions_Then_EmptyList)
{
	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	std::vector<std::shared_ptr<IPredicate>> preconditions { predicateA, predicateC };
	std::vector<std::shared_ptr<IPredicate>> postconditions { predicateB };


	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	std::vector<std::shared_ptr<IPredicate>> inputPredicates { predicateC };

	auto predicatesSatisfy = action->GetPredicatesSatisfyPreconditions(inputPredicates);

	ASSERT_TRUE(predicatesSatisfy.size() == 0);
}

TEST(NAI_Action, When_PredicatesSatisfyPreconditions_Then_ListOfPredicates)
{
	auto predicateA = std::make_shared<BasePredicate>("A");
	auto predicateB = std::make_shared<BasePredicate>("B");
	auto predicateC = std::make_shared<BasePredicate>("C");

	std::vector<std::shared_ptr<IPredicate>> preconditions{ predicateA, predicateC };
	std::vector<std::shared_ptr<IPredicate>> postconditions{ predicateB };


	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	std::vector<std::shared_ptr<IPredicate>> inputPredicates{ predicateC, predicateB, predicateA };

	auto predicatesSatisfy = action->GetPredicatesSatisfyPreconditions(inputPredicates);

	ASSERT_TRUE(predicatesSatisfy.size() == 2);
	ASSERT_TRUE(predicatesSatisfy[0] == predicateA);
	ASSERT_TRUE(predicatesSatisfy[1] == predicateC);
}