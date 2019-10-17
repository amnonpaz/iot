#include "DataHandler.hpp"

#include <iostream>

#include "gtest/gtest.h"

class TestDataHandler : public ::testing::Test, public data::Handler {
    protected:
        void SetUp() override {
            start();
        };

        void TearDown() override {
            stop();
        };

        bool waitAndCompare(const data::Message &expected);

    private:
        void handle(std::string &&topic, data::Payload &&payload) override;
        std::mutex m_latestLock;
        std::condition_variable m_latestSig;
        bool m_ready;
        data::Message m_latest;

        static const uint32_t s_testTimeout_ms = 500;
};

bool TestDataHandler::waitAndCompare(const data::Message &expected) {
    std::unique_lock<std::mutex> lk(m_latestLock);
    m_latestSig.wait_for(lk, std::chrono::milliseconds(s_testTimeout_ms));
    if (!m_ready) {
        return false;
    }
    m_ready = false;

    return expected == m_latest;
}

void TestDataHandler::handle(std::string &&topic,
                             data::Payload &&payload) {
    std::unique_lock<std::mutex> lk(m_latestLock);

    m_latest = data::Message(topic, payload);
    m_ready = true;
    m_latestSig.notify_all();
}

TEST_F(TestDataHandler, BasicTests) {
    std::string topic{"TestThis1"};
    data::Payload payload{ 0, 1, 2, 3, 4 };

    enqueue(topic, payload);
    EXPECT_TRUE(waitAndCompare(data::Message{topic, payload}));

    std::string topic2{"TestThis2"};
    enqueue(topic, payload);
    EXPECT_FALSE(waitAndCompare(data::Message{topic2, payload}));

    data::Payload payload2{ 0, 1, 2, 3 };
    enqueue(topic, payload);
    EXPECT_FALSE(waitAndCompare(data::Message{topic, payload2}));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
