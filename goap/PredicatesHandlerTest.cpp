#include "gtest/gtest.h"
#include "goap/PredicatesHandler.h"
#include "goap/BasePredicate.h"

#include <memory>
#include <vector>


#include "../../NAI/source/goap/BasePredicate.h"
#include "../../NAI/source/goap/PredicatesHandler.h"

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

	const auto predicate = std::make_shared<BasePredicate>(1, std::string("I feel good"));
	predicatesHandler.AddOrReplace(predicate);
	
	const auto& predicatesList = predicatesHandler.GetPredicatesList();

	ASSERT_EQ(predicatesList.size(), 1);
	ASSERT_EQ(predicate->GetID(), predicatesList[0]->GetID());
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_Then_TheyAreSaved)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts { "I feel good", "I have an axe", "I am in my house"};
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;

	int i = 1;
	for(auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		originalPredicates.push_back(predicate);
		predicatesHandler.AddOrReplace(predicate);
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

	int i = 1;
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		predicatesHandler.AddOrReplace(predicate);
	}

	const auto predicateFound = predicatesHandler.FindByText("not inserted predicate");

	ASSERT_FALSE(predicateFound);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByText_Then_IsFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };

	int i = 1;
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		predicatesHandler.AddOrReplace(predicate);
	}

	const auto predicateFound = predicatesHandler.FindByText(predicatesTexts[1]);

	ASSERT_EQ(predicateFound->GetFullText(), predicatesTexts[1]);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByIDThatDoesntExist_Then_IsNotFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;

	int i = 1;
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++,text);
		originalPredicates.push_back(predicate);
		predicatesHandler.AddOrReplace(predicate);
	}

	const auto predicateFound = predicatesHandler.FindById(324);

	ASSERT_FALSE(predicateFound);
}

TEST(NAI_PredicatesHandler, When_AddingSomePredicates_And_FindAnyoneByID_Then_IsFound)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };
	std::vector<std::shared_ptr<IPredicate>> originalPredicates;

	int i = 1;
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		originalPredicates.push_back(predicate);
		predicatesHandler.AddOrReplace(predicate);
	}

	const auto predicateFound = predicatesHandler.FindById(originalPredicates[1]->GetID());

	ASSERT_EQ(predicateFound->GetID(), originalPredicates[1]->GetID());
}

TEST(NAI_PredicatesHandler, When_Reset_Then_NewResetDataIsSaved)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };

	int i = 1;
	for (auto&& text : predicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		predicatesHandler.AddOrReplace(predicate);
	}
	const auto predicatesList = predicatesHandler.GetPredicatesList();
	
	ASSERT_EQ(predicatesList.size(), predicatesTexts.size());
	
	std::vector<std::string> newPredicatesTexts{ "I have hungry", "My uncle is dead" };
	std::vector<std::shared_ptr<IPredicate>> newPredicates;

	for (auto&& text : newPredicatesTexts)
	{
		const auto predicate = std::make_shared<BasePredicate>(i++, text);
		newPredicates.emplace_back(predicate);
	}
	predicatesHandler.Reset(newPredicates);

	ASSERT_EQ(predicatesHandler.GetPredicatesList().size(), newPredicates.size());
}

TEST(NAI_PredicatesHandler, When_Remove_Then_PredicateIsRemoved)
{
	PredicatesHandler predicatesHandler;

	std::vector<std::string> predicatesTexts{ "I feel good", "I have an axe", "I am in my house" };

	for (auto i = 0; i < predicatesTexts.size(); ++i)
	{
		const auto predicate = std::make_shared<BasePredicate>(i, predicatesTexts[i]);
		predicatesHandler.AddOrReplace(predicate);
	}
	
	const auto sizeBeforeRemove = predicatesHandler.GetPredicatesList().size();
	
	ASSERT_EQ(sizeBeforeRemove, predicatesTexts.size());
	ASSERT_TRUE(predicatesHandler.FindByText(predicatesTexts[1]) != nullptr);
	
	predicatesHandler.Remove(1);

	const auto sizeAfterRemove = predicatesHandler.GetPredicatesList().size();
	
	ASSERT_EQ(sizeAfterRemove, predicatesTexts.size() - 1);
	ASSERT_FALSE(predicatesHandler.FindByText(predicatesTexts[1]) != nullptr);
}