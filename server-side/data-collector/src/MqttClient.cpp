#include "MqttClient.hpp"

#include <iostream>

#include "Utils.hpp"

namespace mqtt {

    void Client::SharedResources::up()
    {
        std::unique_lock lk(m_lock);

        if (m_count == 0)
            mosqpp::lib_init();

        m_count++;
    }

    void Client::SharedResources::down()
    {
        std::unique_lock lk(m_lock);

        m_count--;

        if (m_count == 0)
            mosqpp::lib_cleanup();

    }

    Client::SharedResources Client::s_sharedResources;

    Client::Client(std::string brokerUrl,
                           uint16_t brokerPort,
                           std::string clientId,
                           comm::MessageHandler &messageHandler) :
        mosquittopp{clientId.c_str()},
        m_brokerAddress({std::move(brokerUrl), brokerPort}),
        m_messageHandler(messageHandler),
        m_state{StateInit} {
        s_sharedResources.up();
    }

    Client::~Client() {
        s_sharedResources.down();
    }

    void Client::connect() {
        int rc;

        if (m_state == StateDisconnected) {
            rc = mosquittopp::reconnect();
        } else {
            rc = mosquittopp::connect_async(std::get<0>(m_brokerAddress).c_str(),
                                            std::get<1>(m_brokerAddress),
                                            s_keepAliveS);
        }

        if (rc) {
            printErrorMessage(rc);
        }
    }

    void Client::reconnect() {
        std::cerr << "Trying to reconnect in "
                  << s_reconnectTimeoutS << " seconds\n";
        utils::TimedAsyncExecution(s_reconnectTimeoutS*1000,
                                   [this](){ connect(); });
    }

    void Client::subscribe(std::string topic, int qos) {
        if (m_state == StateConnected) {
            mosquittopp::subscribe(nullptr, topic.c_str(), qos);
        }
        m_subscriptions.push_back(Subscription{std::move(topic), qos});
    }

    void Client::on_connect(int rc) {
        if (!rc) {
            std::cout << "Connected...\n";
            m_state = StateConnected;
            for_each(m_subscriptions.begin(), m_subscriptions.end(),
                     [this](Subscription sub) {
                        mosquittopp::subscribe(nullptr, std::get<0>(sub).c_str(), std::get<1>(sub));
                     });
        } else {
            printErrorMessage(rc);
            m_state = StateDisconnected;
        }
    }

    void Client::on_disconnect(int rc) {
        printErrorMessage(rc);
        std::cout << "Disconected...\n";
        m_state = StateDisconnected;
    }

    void Client::on_message(const struct mosquitto_message *msg) {
        char *data = reinterpret_cast<char *>(msg->payload);
        comm::Payload payload{data, data + msg->payloadlen};

        m_messageHandler.enqueue(std::string{msg->topic}, std::move(payload));
    }

    void Client::printErrorMessage(int rc) {
        std::cerr << "Error: " << std::string(mosqpp::strerror(rc)) << '\n';
    }

} // namespace mqtt
