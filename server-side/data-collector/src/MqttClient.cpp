#include "MqttClient.hpp"

#include <iostream>

#include "Utils.hpp"

namespace comm {

void MqttClient::SharedResources::up()
{
    std::unique_lock lk(m_lock);

    if (m_count == 0)
        mosqpp::lib_init();

    m_count++;
}

void MqttClient::SharedResources::down()
{
    std::unique_lock lk(m_lock);

    m_count--;

    if (m_count == 0)
        mosqpp::lib_cleanup();

}

MqttClient::SharedResources MqttClient::s_sharedResources; 

MqttClient::MqttClient(std::string brokerUrl,
                       uint16_t brokerPort,
                       std::string clientId) :
    mosquittopp{clientId.c_str()},
    m_brokerAddress({std::move(brokerUrl), brokerPort}),
    m_state{StateInit} {
    s_sharedResources.up();
}

MqttClient::~MqttClient() {
    s_sharedResources.down();
}

void MqttClient::connect() {
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
        reconnect();
    }
}

void MqttClient::reconnect() {
    std::cerr << "Trying to reconnect in "
              << s_reconnectTimeoutS << " seconds\n";
    utils::TimedAsyncExecution(s_reconnectTimeoutS*1000,
                               [this](){ connect(); });
}

void MqttClient::on_connect(int rc) {
    if (!rc) {
        std::cout << "Connected...\n";
        m_state = StateConnected;
    } else {
        printErrorMessage(rc);
        if (m_state != StateInit)
            m_state = StateDisconnected;

        reconnect();
    }
}

void MqttClient::printErrorMessage(int rc) {
    std::cerr << "Error: " << std::string(mosqpp::strerror(rc)) << '\n';
}

} // namespace comm
