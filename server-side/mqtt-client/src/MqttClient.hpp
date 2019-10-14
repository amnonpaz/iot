#ifndef _MQTT_CLIENT_HPP
#define _MQTT_CLIENT_HPP

#include <mosquittopp.h>

#include <string>
#include <mutex>
#include <tuple>

class MqttClient : public mosqpp::mosquittopp {
    private:
        class SharedResources {
            public:
                SharedResources() noexcept : m_count(0) {};
                void up();
                void down();

            private:
                std::mutex m_lock;
                uint32_t m_count;
        };

    public:
        class Exception : public std::exception {
            public:
                explicit Exception(std::string msg) :
                    m_msg("MqttClient: " + std::move(msg)) {}
                char const* what() const noexcept override {
                    return m_msg.c_str();
                }

            private:
                std::string m_msg;
        };

    public:
        MqttClient(std::string brokerUrl,
                   uint16_t brokerPort);
        ~MqttClient();

    private:
        const std::tuple<std::string, uint16_t> m_brokerAddress;

        static SharedResources s_sharedResources; 
};

#endif // _MQTT_CLIENT_HPP
