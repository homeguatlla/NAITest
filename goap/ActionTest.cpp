//#include "pchTest.h"
#include "gtest/gtest.h"
#include "goap/BaseAction.h"
#include "goap/IPredicate.h"
#include "goap/BasePredicate.h"

#include <memory>
#include <vector>

using namespace NAI::Goap;

TEST(NAI_Action, When_NoPredicates_Then_CostIsZero) 
{
	std::vector<std::string> preconditions;
	std::vector<std::shared_ptr<IPredicate>> postconditions;

	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	ASSERT_EQ(action->GetCost(), 0);
}

TEST(NAI_Action, When_PredicatesDontSatisfyPreconditions_Then_EmptyList)
{
	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");

	std::vector<std::string> preconditions { predicateA->GetText(), predicateC->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions { predicateB };


	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	std::vector<std::shared_ptr<IPredicate>> inputPredicates { predicateC };

	auto predicatesSatisfy = action->GetPredicatesSatisfyPreconditions(inputPredicates);

	ASSERT_TRUE(predicatesSatisfy.size() == 0);
}

TEST(NAI_Action, When_PredicatesSatisfyPreconditions_Then_ListOfPredicates)
{
	auto predicateA = std::make_shared<BasePredicate>(1, "A");
	auto predicateB = std::make_shared<BasePredicate>(2, "B");
	auto predicateC = std::make_shared<BasePredicate>(3, "C");

	std::vector<std::string> preconditions{ predicateA->GetText(), predicateC->GetText() };
	std::vector<std::shared_ptr<IPredicate>> postconditions{ predicateB };


	auto action = std::make_shared<BaseAction>(preconditions, postconditions);

	std::vector<std::shared_ptr<IPredicate>> inputPredicates{ predicateC, predicateB, predicateA };

	auto predicatesSatisfy = action->GetPredicatesSatisfyPreconditions(inputPredicates);

	ASSERT_TRUE(predicatesSatisfy.size() == 2);
	ASSERT_TRUE(predicatesSatisfy[0] == predicateA);
	ASSERT_TRUE(predicatesSatisfy[1] == predicateC);
}