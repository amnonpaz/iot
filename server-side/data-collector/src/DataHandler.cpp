#include "DataHandler.hpp"

#include <chrono>

namespace data {

    Handler::Handler() :
        m_running{false}
    {
    }

    Handler::~Handler()
    {
        stop();
    }

    bool Handler::start()
    {
        std::unique_lock lk{m_lock};

        if (m_running)
            return true;

        m_running = true;

        try {
            m_thread = std::thread{&Handler::work, this};
        } catch(...) {
            m_running = false;
        }

        return m_running;
    }

    void Handler::stop()
    {
        std::lock_guard lk{m_lock};

        if (!m_running)
            return;

        m_running = false;
        m_signal.notify_all();

        m_thread.join();
    }

    void Handler::work()
    {
        while(wait()) {
            Message msg(pop());

            handle(std::move(std::get<0>(msg)), std::move(std::get<1>(msg)));
        }
    }

    Message Handler::pop()
    {
        std::unique_lock lk{m_lock};

        Message msg{m_queue.front()};
        m_queue.pop();

        return std::move(msg);
    }

    bool Handler::wait()
    {
        std::unique_lock lk{m_lock};

        m_signal.wait_for(lk, std::chrono::milliseconds(s_timeout_ms),
                [this](){ return !m_running || !m_queue.empty(); });

        return m_running;
    }

    void Handler::enqueue(std::string topic, Payload payload)
    {
        std::unique_lock lk{m_lock};

        m_queue.push(Message{std::move(topic), std::move(payload)});
    }

} // namespace data
