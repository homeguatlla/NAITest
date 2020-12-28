#include <typeindex>
#include <gmock/gmock-spec-builders.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "goap/sensory/ISensor.h"
#include "goap/sensory/IStimulus.h"
#include "goap/sensory/IThreshold.h"
#include "goap/sensory/SensorySystem.h"

using namespace NAI::Goap;
using namespace testing;


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

class HearingSensorMock : public ISensor
{
public:
	HearingSensorMock()
	{
		ON_CALL(*this, Subscribe).WillByDefault(
			[this](const std::shared_ptr<ISensorSubscriber> subscriber)
			{
				mSubscriber = subscriber;
			});

		ON_CALL(*this, Update).WillByDefault(
			[this](float elapsedTime)
			{
				if(mSubscriber)
				{
					mSubscriber->OnSensorNotification(std::make_shared<NiceMock<HearingStimulusMock>>());
				}
			});
	}

	virtual ~HearingSensorMock() = default;

	MOCK_METHOD1(Subscribe, void(const std::shared_ptr<ISensorSubscriber>));
	MOCK_METHOD1(Update, void (float));

private:
	std::shared_ptr<ISensorSubscriber> mSubscriber;
};

class VisualSensorMock : public ISensor
{
public:
	VisualSensorMock()
	{
		ON_CALL(*this, Subscribe).WillByDefault(
			[this](const std::shared_ptr<ISensorSubscriber> subscriber)
			{
				mSubscriber = subscriber;
			});

		ON_CALL(*this, Update).WillByDefault(
			[this](float elapsedTime)
			{
				if (mSubscriber)
				{
					mSubscriber->OnSensorNotification(std::make_shared<NiceMock<VisualStimulusMock>>());
				}
			});
	}

	virtual ~VisualSensorMock() = default;

	MOCK_METHOD1(Subscribe, void(const std::shared_ptr<ISensorSubscriber>));
	MOCK_METHOD1(Update, void(float));

private:
	std::shared_ptr<ISensorSubscriber> mSubscriber;
};

class HearingThresholdMock : public IThreshold
{
public:
	HearingThresholdMock(bool isPerceived) : mIsPerceived{isPerceived}
	{
		ON_CALL(*this, IsStimulusPerceived).WillByDefault(
			[this](const std::shared_ptr<IStimulus>)
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
			[this](const std::shared_ptr<IStimulus>)
			{
				return mIsPerceived;
			});
	}

	virtual ~VisualThresholdMock() = default;

	MOCK_CONST_METHOD1(IsStimulusPerceived, bool(const std::shared_ptr<IStimulus>));
private:
	bool mIsPerceived;
};

TEST(NAI_SensorySystem, When_Created_Then_TheListOfStimulusIsEmpty) 
{
	const SensorySystem sensorySystem;

	ASSERT_TRUE(sensorySystem.IsMemoryEmpty());
}

TEST(NAI_SensorySystem, When_SuscribedToASensor_And_NewStimulus_Then_TheStimulusIsReceived)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem>();

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);
	
	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto sensoryElements = sensorySystem->GetReceivedStimulus();
	ASSERT_FALSE(sensoryElements.empty());
}

TEST(NAI_SensorySystem, When_Update_And_StimulusIntoTheThreshold_Then_TheListOfStimulusIsNOTEmpty)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem>();

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);
	
	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto threshold = std::make_shared<NiceMock<HearingThresholdMock>>(true);
	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()] = threshold;
	sensorySystem->Update(0.16f, sensorThresholdsMap);
	
	ASSERT_FALSE(sensorySystem->IsMemoryEmpty());
}

TEST(NAI_SensorySystem, When_Update_And_StimulusOutsideTheThreshold_Then_TheListOfStimulusIsEmpty)
{
	HearingSensorMock hearingSensorMock;
	const auto sensorySystem = std::make_shared<SensorySystem>();

	EXPECT_CALL(hearingSensorMock, Subscribe).Times(1);
	EXPECT_CALL(hearingSensorMock, Update).Times(1);

	hearingSensorMock.Subscribe(sensorySystem);
	hearingSensorMock.Update(0.16f);

	const auto threshold = std::make_shared<NiceMock<HearingThresholdMock>>(false);
	std::map<std::string, std::shared_ptr<IThreshold>> sensorThresholdsMap;
	sensorThresholdsMap[typeid(NiceMock<HearingStimulusMock>).name()] = threshold;
	sensorySystem->Update(0.16f, sensorThresholdsMap);

	ASSERT_TRUE(sensorySystem->IsMemoryEmpty());
}
TEST(NAI_SensorySystem, When_Update_And_StimulusOfSensorA_And_StimulusOfSensorB_And_OnlyAInsideTheThreshold_Then_TheListOfStimulusIsTheA)
{
	HearingSensorMock hearingSensorMock;
	VisualSensorMock visualSensorMock;
	
	const auto sensorySystem = std::make_shared<SensorySystem>();

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
	
	sensorySystem->Update(0.16f, sensorThresholdsMap);

	ASSERT_FALSE(sensorySystem->IsMemoryEmpty());
}
