#include "MqttClient.hpp"

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
                       uint16_t brokerPort) :
    m_brokerAddress({std::move(brokerUrl), brokerPort}) {
    s_sharedResources.up();
}

MqttClient::~MqttClient() {
    s_sharedResources.down();
}

} // namespace comm
