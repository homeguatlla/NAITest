#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/sensory/PerceptionSystem.h"

using namespace NAI::Goap;
using namespace testing;

TEST(NAI_PerceptionSystem, When_Created_Then_TheListOfPredicatesIsEmpty) 
{
	const PerceptionSystem perceptionSystem;

	ASSERT_TRUE(false);
}

TEST(NAI_PerceptionSystem, When_Update_Then_NewPredicatesAreNotifiedToAgent)
{
	const PerceptionSystem perceptionSystem;
	
	ASSERT_TRUE(false);
}
