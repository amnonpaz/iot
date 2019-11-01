#ifndef _MQTT_CLIENT
#define _MQTT_CLIENT

#include <mosquittopp.h>

#include <string>
#include <mutex>
#include <tuple>
#include <list>

namespace mqtt {

    class Client : public mosqpp::mosquittopp {
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

            enum State {
                StateInit,
                StateConnected,
                StateDisconnected
            };

            typedef std::tuple<std::string, int> Subscription;

        public:
            Client(std::string brokerUrl,
                       uint16_t brokerPort,
                       std::string clientId);
            ~Client();

            void connect();
            void subscribe(std::string topic, int qos = 0);

        protected:
            void on_connect(int rc) override;
            void on_disconnect(int rc) override;
            void on_message(const struct mosquitto_message *msg);

        private:
            void reconnect();
            static void printErrorMessage(int rc);

        private:
            const std::tuple<std::string, uint16_t> m_brokerAddress;
            std::list<Subscription> m_subscriptions;
            State m_state;

            static const uint32_t s_keepAliveS = 120;
            static const uint32_t s_reconnectTimeoutS = 30;
            static SharedResources s_sharedResources;
    };

} // namespace mqtt

#endif // _MQTT_CLIENT
