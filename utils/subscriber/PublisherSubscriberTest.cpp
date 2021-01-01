#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils/subscriber/BasePublisher.h"

#include <memory>
#include <gmock/gmock-spec-builders.h>

using namespace core::utils::subscriber;

template<class T>
class SubscriberMock : public ISubscriber<T>
{
public:
	SubscriberMock()
	{
		ON_CALL(*this, OnNotification).WillByDefault(
			[this](std::shared_ptr<T> event)
			{
				notification = *event;
			});
	}

	virtual ~SubscriberMock() = default;

	int GetNotification() const { return notification; }
	MOCK_METHOD1_T(OnNotification, void(std::shared_ptr<T>));

private:
	int notification;
};

TEST(CORE_UTILS_PublisherSubscriber, When_Created_Then_SubscribersIsEmpty) 
{
	const BasePublisher<int> publisher;
	
	ASSERT_FALSE(publisher.HasSubscribers());
}

TEST(CORE_UTILS_PublisherSubscriber, When_SomeoneSubscribe_Then_ThereIsOneSubscriber)
{
	const auto subscriberMock = std::make_shared<SubscriberMock<int>>();
	BasePublisher<int> publisher;

	publisher.Subscribe(subscriberMock);
	
	ASSERT_TRUE(publisher.HasSubscribers());
}

TEST(CORE_UTILS_PublisherSubscriber, When_SubscribingAnAlreadySubscribed_Then_ThereIsOneSubscriber)
{
	const auto subscriberMock = std::make_shared<SubscriberMock<int>>();
	BasePublisher<int> publisher;

	ASSERT_FALSE(publisher.HasSubscribers());
	
	publisher.Subscribe(subscriberMock);
	ASSERT_TRUE(publisher.HasSubscribers());
	
	auto subscribed = publisher.Subscribe(subscriberMock);

	ASSERT_FALSE(subscribed);
}

TEST(CORE_UTILS_PublisherSubscriber, When_Subscribers_Then_TheyAreNotified)
{
	const auto subscriberMockPtr = std::make_shared<SubscriberMock<int>>();
	BasePublisher<int> publisher;

	publisher.Subscribe(subscriberMockPtr);

	EXPECT_CALL(*subscriberMockPtr, OnNotification).Times(1);
	
	publisher.NotifyAll(std::make_shared<int>(7));

	ASSERT_TRUE(subscriberMockPtr->GetNotification() == 7);
}

TEST(CORE_UTILS_PublisherSubscriber, When_SubscribersAndUnSubscribe_Then_UnSubcribed)
{
	const auto subscriberMockPtr = std::make_shared<SubscriberMock<int>>();
	BasePublisher<int> publisher;

	publisher.Subscribe(subscriberMockPtr);
	
	ASSERT_TRUE(publisher.HasSubscribers());

	publisher.UnSubscribe(subscriberMockPtr);

	ASSERT_FALSE(publisher.HasSubscribers());
}