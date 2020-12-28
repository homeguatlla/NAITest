#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/memory/Memory.h"
#include "goap/sensory/IStimulus.h"
#include <cstdlib>

using namespace NAI::Goap;
using namespace testing;


static int ID_COUNTER = 0;

class StimulusMock : public IStimulus
{	
public:
	StimulusMock()
	{
		mId = ++ID_COUNTER;
		
		ON_CALL(*this, GetClassName).WillByDefault(
			[this]()
			{
				return typeid(*this).name();
			});
	}

	virtual ~StimulusMock() = default;

	int GetId() const { return mId; }
	
	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());

private:
	
	int mId;
};

TEST(NAI_Memory, When_Created_Then_MemoryIsEmpty) 
{
	const Memory<StimulusMock> memory;

	ASSERT_TRUE(memory.IsEmpty());
}

TEST(NAI_Memory, When_AddingElement_Then_MemoryIsNotEmpty)
{
	Memory<StimulusMock> memory;

	const auto stimulus = std::make_shared<StimulusMock>();
	
	memory.Add(stimulus);
	
	ASSERT_FALSE(memory.IsEmpty());
}

TEST(NAI_Memory, When_AddingElement_For_A_Long_Then_TheMemoryWillBeEmptyAfterThatLong)
{
	Memory<StimulusMock> memory;

	const auto stimulus = std::make_shared<StimulusMock>();

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

TEST(NAI_Memory, When_PerformActionOnEach_Then_ActionIsBeingExecutedForEachMemoryElement)
{
	Memory<StimulusMock> memory;
	std::vector<int> ids;
	
	for(auto i = 0; i < 5; ++i)
	{
		const auto stimulus = std::make_shared<StimulusMock>();
		ids.push_back(stimulus->GetId());
		memory.Add(stimulus);
	}

	for (auto i = 0; i < 3; ++i)
	{
		const auto stimulus = std::make_shared<StimulusMock>();
		const auto time = static_cast<float>(std::rand() % 10);
		ids.push_back(stimulus->GetId());
		memory.Add(stimulus, time);
	}

	auto i = 0;
	memory.PerformActionForEach(
		[ids, &i](std::shared_ptr<StimulusMock> stimulus)
		{
			ASSERT_TRUE(stimulus->GetId() == ids[i]);
			++i;
		});	
}