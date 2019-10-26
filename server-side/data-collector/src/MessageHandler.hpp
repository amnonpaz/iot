#ifndef DATA_HANDLER_
#define DATA_HANDLER_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace comm {

    typedef std::vector<char> Payload;
    typedef std::tuple<std::string, Payload> Message;

    class MessageHandler {
        public:
            MessageHandler();
            ~MessageHandler();

            void enqueue(std::string topic, Payload message);

            bool start();
            void stop();

        protected:
            virtual void handle(std::string &&topic, Payload &&payload) = 0;

        private:
            void work();
            Message pop();
            bool wait();
            bool isRunning();

        private:
            std::thread m_thread;
            std::queue<Message> m_queue;
            std::condition_variable m_signal;
            std::mutex m_lock;
            bool m_running;

            static const uint32_t s_timeout_ms = 300;
    };

} // namespace comm

#endif // DATA_HANDLER_
