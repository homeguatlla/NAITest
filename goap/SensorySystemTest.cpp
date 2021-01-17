#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/sensory/ISensor.h"
#include "goap/sensory/IStimulus.h"
#include "goap/sensory/IThreshold.h"
#include "goap/sensory/SensorySystem.h"
#include "goap/sensory/BaseSensor.h"

using namespace NAI::Goap;
using namespace testing;
using namespace core::utils::subscriber;


class HearingStimulusMock : public IStimulus
{
public:
	HearingStimulusMock()
	{
		ON_CALL(*this, GetClassName).WillByDefault(
			[this]()
			{
				return typeid(*this).name();
			});
	}
	
	virtual ~HearingStimulusMock() = default;

	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
};

class VisualStimulusMock : public IStimulus
{
public:
	VisualStimulusMock()
	{
		ON_CALL(*this, GetClassName).WillByDefault(
			[this]()
			{
				return typeid(*this).name();
			});
	}
	
	virtual ~VisualStimulusMock() = default;

	MOCK_CONST_METHOD0(GetClassName, std::string());
	MOCK_CONST_METHOD0(GetPosition, glm::vec3());
};

class HearingSensorMock
{
public:
	HearingSensorMock()
	{
		ON_CALL(*this, Subscribe).WillByDefault(
			[this](std::shared_ptr<ISubscriber<IStimulus>> subscriber)
			{
				mSubscriber = subscriber;
				return true;
			});

		ON_CALL(*this, Update).WillByDefault(
			[this](float elapsedTime)
			{
				if(mSubscriber)
				{
					mSubscriber->OnNotification(std::make_shared<NiceMock<HearingStimulusMock>>());
				}
			});
	}

	virtual ~HearingSensorMock() = default;
	MOCK_METHOD1(Subscribe, bool(std::shared_ptr<ISubscriber<IStimulus>>));
	MOCK_METHOD1(Update, void (float));

private:
	std::shared_ptr<ISubscriber<IStimulus>> mSubscriber;
};

class VisualSensorMock
{
public:
	VisualSensorMock()
	{
		ON_CALL(*this, Subscribe).WillByDefault(
			[this](std::shared_ptr<ISubscriber<IStimulus>> subscriber)
			{
				mSubscriber = subscriber;
				return true;
			});

		ON_CALL(*this, Update).WillByDefault(
			[this](float elapsedTime)
			{
				if (mSubscriber)
				{
					mSubscriber->OnNotification(std::make_shared<NiceMock<VisualStimulusMock>>());
				}
			});
	}

	virtual ~VisualSensorMock() = default;

	MOCK_METHOD1(Subscribe, bool(std::shared_ptr<ISubscriber<IStimulus>>));
	MOCK_METHOD1(Update, void(float));

private:
	std::shared_ptr<ISubscriber<IStimulus>> mSubscriber;
};

class HearingThresholdMock : public IThreshold
{
public:
	HearingThresholdMock(bool isPerceived) : mIsPerceived{isPerceived}
	{
		ON_CALL(*this, IsStimulusPerceived).WillByDefault(
			[this](const std::shared_ptr<IStimulus> stimulus)
			{
				return mIsPerceived;
			});
	}
	
	virtual ~HearingThresholdMock() = default;

	MOCK_CONST_METHOD1(IsStimulusPerceived, bool(const std::shared_ptr<IStimulus>));
private:
	bool mIsPerceived;
};

class VisualThresholdMock : public IThreshold
{
public:
	VisualThresholdMock(bool isPerceived) : mIsPerceived{ isPerceived }
	{
		ON_CALL(*this, IsStimulusPerceived).WillByDefault(
			[this](const std::shared_ptr<IStimulus> stimulus)
			{
				return mIsPerceived;
			});
	}

	virtual ~VisualThresholdMock() = default;

	MOCK_CONST_METHOD1(IsStimulusPerceived, bool(const std::shared_ptr<IStimulus>));
private:
	bool mIsPerceived;
};

TEST(NAI_SensorySystem, When_SuscribedToASensor_And_NewStimulus_Then_TheStimulusIsReceived)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);
	
	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto sensoryElements = sensorySystem->GetReceivedStimulus();
	ASSERT_FALSE(sensoryElements.empty());
}

TEST(NAI_SensorySystem, When_Update_And_StimulusIntoTheThreshold_Then_TheMemoryIsNOTEmpty)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	Memory<IStimulus> memory;
	
	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);
	
	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto threshold = std::make_shared<NiceMock<HearingThresholdMock>>(true);
	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()] = threshold;
	sensorySystem->Update(0.16f, memory, sensorThresholdsMap);
	
	ASSERT_FALSE(memory.IsEmpty());
}

TEST(NAI_SensorySystem, When_Update_And_StimulusIntoTheThreshold_Then_TheListOfStimulusIsEmpty)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	Memory<IStimulus> memory;
	
	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);
	
	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto threshold = std::make_shared<NiceMock<HearingThresholdMock>>(true);
	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()] = threshold;
	sensorySystem->Update(0.16f, memory, sensorThresholdsMap);
	
	ASSERT_TRUE(sensorySystem->GetReceivedStimulus().empty());
}

TEST(NAI_SensorySystem, When_Update_And_StimulusOutsideTheThreshold_Then_TheMemoryIsEmpty)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();
	Memory<IStimulus> memory;

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);

	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto threshold = std::make_shared<NiceMock<HearingThresholdMock>>(false);
	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()] = threshold;
	sensorySystem->Update(0.16f, memory, sensorThresholdsMap);

	ASSERT_TRUE(memory.IsEmpty());
}
TEST(NAI_SensorySystem, When_Update_And_StimulusOfSensorA_And_StimulusOfSensorB_And_OnlyAInsideTheThreshold_Then_TheMemoryConteinsA)
{
	HearingSensorMock hearingSensorMock;
	VisualSensorMock visualSensorMock;
	Memory<IStimulus> memory;
	
	const auto sensorySystem = std::make_shared<SensorySystem<IStimulus>>();

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);

	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);


	EXPECT_CALL(visualSensorMock, Subscribe).Times(1);
	EXPECT_CALL(visualSensorMock, Update).Times(1);

	visualSensorMock.Subscribe(sensorySystem);
	visualSensorMock.Update(0.16f);

	const auto hearingThreshold = std::make_shared<NiceMock<HearingThresholdMock>>(true);
	const auto visualThreshold = std::make_shared<NiceMock<VisualThresholdMock>>(false);

	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()]= hearingThreshold;
	sensorThresholdsMap[typeid(NiceMock<VisualStimulusMock>).name()] = visualThreshold;
	
	sensorySystem->Update(0.16f, memory, sensorThresholdsMap);

	ASSERT_FALSE(memory.IsEmpty());
	memory.PerformActionForEach([](std::shared_ptr<IStimulus> stimulus) -> bool
	{
		EXPECT_TRUE(stimulus->GetClassName() == typeid(NiceMock<HearingStimulusMock>).name());
		return true;
	});
}
