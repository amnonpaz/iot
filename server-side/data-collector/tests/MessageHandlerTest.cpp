#include "MessageHandler.hpp"

#include <iostream>

#include "gtest/gtest.h"

class TestMessageHandler : public ::testing::Test, public comm::MessageHandler {
    protected:
        void SetUp() override {
            start();
        };

        void TearDown() override {
            stop();
        };

        bool waitAndCompare(const comm::Message &expected);

    private:
        void handle(std::string &&topic, comm::Payload &&payload) override;
        std::mutex m_latestLock;
        std::condition_variable m_latestSig;
        bool m_ready;
        comm::Message m_latest;

        static const uint32_t s_testTimeout_ms = 500;
};

bool TestMessageHandler::waitAndCompare(const comm::Message &expected) {
    std::unique_lock<std::mutex> lk(m_latestLock);
    m_latestSig.wait_for(lk, std::chrono::milliseconds(s_testTimeout_ms));
    if (!m_ready) {
        return false;
    }
    m_ready = false;

    return expected == m_latest;
}

void TestMessageHandler::handle(std::string &&topic,
                             comm::Payload &&payload) {
    std::unique_lock<std::mutex> lk(m_latestLock);

    m_latest = comm::Message(topic, payload);
    m_ready = true;
    m_latestSig.notify_all();
}

TEST_F(TestMessageHandler, BasicTests) {
    std::string topic{"TestThis1"};
    comm::Payload payload{ 0, 1, 2, 3, 4 };

    enqueue(topic, payload);
    EXPECT_TRUE(waitAndCompare(comm::Message{topic, payload}));

    std::string topic2{"TestThis2"};
    enqueue(topic, payload);
    EXPECT_FALSE(waitAndCompare(comm::Message{topic2, payload}));

    comm::Payload payload2{ 0, 1, 2, 3 };
    enqueue(topic, payload);
    EXPECT_FALSE(waitAndCompare(comm::Message{topic, payload2}));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
