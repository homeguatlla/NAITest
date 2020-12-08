#include "gtest/gtest.h"
#include "goap/PredicatesHandler.h"
#include "goap/BasePredicate.h"

#include <memory>
#include <vector>

using namespace NAI::Goap;

TEST(NAI_PredicatesHandler, When_NoPredicates_Then_Empty) 
{
	const PredicatesHandler predicatesHandler;

	const auto& predicatesList = predicatesHandler.GetPredicatesList();

	ASSERT_TRUE(predicatesList.empty());
}

TEST(NAI_PredicatesHandler, When_AddingAPredicate_Then_OneResultIsReturned)
{
	PredicatesHandler predicatesHandler;

	const auto predicate = std::make_shared<BasePredicate>(std::string("I feel good"));
	predicatesHandler.Add(predicate);
	
	const auto& predicatesList = predicatesHandler.GetPredicatesList();

	ASSERT_EQ(predicatesList.size(), 1);
	ASSERT_EQ(predicate->GetID(), predicatesList[0]->GetID());
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_Then_TheyAreSaved)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts { "I feel good", "I have an axe", "I am in my house"};
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;
	
	for(auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		originalPredicates.push_back(predicate);
		predicatesHandler.Add(predicate);
	}
	
	const auto& predicatesList = predicatesHandler.GetPredicatesList();

	ASSERT_EQ(predicatesList.size(), predicatesTexts.size());

	for (size_t i = 0; i < predicatesList.size(); ++i)
	{
		ASSERT_EQ(originalPredicates[i]->GetID(), predicatesList[i]->GetID());
	}
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByTextThatDoesntExist_Then_IsNotFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };

	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		predicatesHandler.Add(predicate);
	}

	const auto predicateFound = predicatesHandler.FindByText("not inserted predicate");

	ASSERT_FALSE(predicateFound);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByText_Then_IsFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };
	
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		predicatesHandler.Add(predicate);
	}

	const auto predicateFound = predicatesHandler.FindByText(predicatesTexts[1]);

	ASSERT_EQ(predicateFound->GetFullText(), predicatesTexts[1]);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByIDThatDoesntExist_Then_IsNotFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;

	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		originalPredicates.push_back(predicate);
		predicatesHandler.Add(predicate);
	}

	const auto predicateFound = predicatesHandler.FindById(324);

	ASSERT_FALSE(predicateFound);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByID_Then_IsFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;

	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		originalPredicates.push_back(predicate);
		predicatesHandler.Add(predicate);
	}

	const auto predicateFound = predicatesHandler.FindById(originalPredicates[1]->GetID());

	ASSERT_EQ(predicateFound->GetID(), originalPredicates[1]->GetID());
}

TEST(NAI_PredicatesHandler, When_Reset_Then_NewResetDataIsSaved)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };

	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		predicatesHandler.Add(predicate);
	}
	const auto predicatesList = predicatesHandler.GetPredicatesList();
	
	ASSERT_EQ(predicatesList.size(), predicatesTexts.size());
	
	std::vector<std::string> newPredicatesTexts{ "I have hungry", "My uncle is dead" };
	std::vector<std::shared_ptr<IPredicate>> newPredicates;
	
	for (auto&& text : newPredicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(text);
		newPredicates.emplace_back(predicate);
	}
	predicatesHandler.Reset(newPredicates);

	ASSERT_EQ(predicatesHandler.GetPredicatesList().size(), newPredicates.size());
}