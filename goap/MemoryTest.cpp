#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/memory/ShortTermMemory.h"
#include "goap/sensory/IStimulus.h"
#include <cstdlib>

using namespace NAI::Goap;
using namespace testing;


static int ID_COUNTER = 0;

class StimulusMock : public IStimulus
{	
public:
	StimulusMock(int value) : mValue{value}
	{
		mId = ++ID_COUNTER;
		
		ON_CALL(*this, GetClassName).WillByDefault(
			[this]()
			{
				return typeid(*this).name();
			});
	}

	virtual ~StimulusMock() = default;

	unsigned int GetId() const override { return mId; }
	int GetValue() const { return mValue;}
	void SetValue(int value) { mValue = value;}
	
	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
	MOCK_CONST_METHOD0(GetDurationInMemory, float());

private:
	
	int mId;
	int mValue;
};

TEST(NAI_Memory, When_Created_Then_MemoryIsEmpty) 
{
	const ShortTermMemory<StimulusMock> memory;

	ASSERT_TRUE(memory.IsEmpty());
}

TEST(NAI_Memory, When_AddingElement_Then_MemoryIsNotEmpty)
{
	ShortTermMemory<StimulusMock> memory;

	const auto stimulus = std::make_shared<StimulusMock>(32);
	
	memory.Add(stimulus, 0.0f);
	
	ASSERT_FALSE(memory.IsEmpty());
}

TEST(NAI_Memory, When_AddingElement_For_A_Long_Then_TheMemoryWillBeEmptyAfterThatLong)
{
	ShortTermMemory<StimulusMock> memory;

	const auto stimulus = std::make_shared<StimulusMock>(43);

	const auto seconds = 3.0f;
	memory.Add(stimulus, seconds);

	const auto time = static_cast<int>(seconds/0.16f);
	
	for(auto i = 0; i < time; ++i)
	{
		memory.Update(0.16f);
	}
	ASSERT_FALSE(memory.IsEmpty());
	
	memory.Update(0.16f);
	ASSERT_TRUE(memory.IsEmpty());
}


TEST(NAI_Memory, When_AddingExistingElement_Then_ElementIsReplaced)
{
	ShortTermMemory<StimulusMock> memory;

	const auto stimulus = std::make_shared<StimulusMock>(36);
	
	memory.Add(stimulus, 0.0f);

	ASSERT_FALSE(memory.IsEmpty());
	
	stimulus->SetValue(423);

	memory.AddOrReplace(stimulus, 0.0f);

	unsigned int numElements = 0;
	memory.PerformActionForEach(
        [&numElements](std::shared_ptr<StimulusMock> stimulus) -> bool 
        {
        	numElements++;
            EXPECT_TRUE(stimulus->GetValue() == 423);
            return true;
        });

	ASSERT_TRUE(numElements == 1);
}

TEST(NAI_Memory, When_PerformActionOnEach_Then_ActionIsBeingExecutedForEachMemoryElement)
{
	ShortTermMemory<StimulusMock> memory;
	std::vector<int> ids;
	
	for(auto i = 0; i < 5; ++i)
	{
		const auto stimulus = std::make_shared<StimulusMock>(432);
		ids.push_back(stimulus->GetId());
		memory.Add(stimulus, 0.0f);
	}

	for (auto i = 0; i < 3; ++i)
	{
		const auto stimulus = std::make_shared<StimulusMock>(321);
		const auto time = static_cast<float>(std::rand() % 10);
		ids.push_back(stimulus->GetId());
		memory.Add(stimulus, time);
	}

	auto i = 0;
	memory.PerformActionForEach(
		[ids, &i](std::shared_ptr<StimulusMock> stimulus) -> bool 
		{
			//Expect_true instead of Assert_True because the lambda need to return a boolean and
			//assert_True returns a void causing an error.
			EXPECT_TRUE(stimulus->GetId() == ids[i]);
			++i;
			return true;
		});	
}