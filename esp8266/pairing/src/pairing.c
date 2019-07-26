#include <espressif/esp_common.h>

#include <string.h>
#include <dhcpserver.h>
#include <lwip/api.h>

#include "pairing.h"
#include "led.h"

#define AP_SSID "IoT-Endpoint"
#define AP_PSK "1234567890"

#define SERVER_PORT 6001

void pairingApSetup(void)
{
    sdk_wifi_set_opmode(SOFTAP_MODE);
    struct ip_info ap_ip;
    // TODO: Create macro for generalizing IP address
    IP4_ADDR(&ap_ip.ip, 192, 168, 0, 1);
    IP4_ADDR(&ap_ip.gw, 0, 0, 0, 0);
    IP4_ADDR(&ap_ip.netmask, 255, 255, 255, 0);
    sdk_wifi_set_ip_info(1, &ap_ip);

    struct sdk_softap_config ap_config = {
        .ssid = AP_SSID,
        .ssid_hidden = 0,
        .channel = 3,
        .ssid_len = strlen(AP_SSID),
        .authmode = AUTH_WPA_WPA2_PSK,
        .password = AP_PSK,
        .max_connection = 3,
        .beacon_interval = 100,
    };

    sdk_wifi_softap_set_config(&ap_config);
}

static struct netconn *pairingServerStart()
{
    struct netconn *nc = netconn_new(NETCONN_TCP);
    if (!nc)
    {
        printf("Status monitor: Failed to allocate socket.\r\n");
        return NULL;
    }
    netconn_bind(nc, IP_ANY_TYPE, SERVER_PORT);
    netconn_listen(nc);

    return nc;
}

static void pairingServerStop(struct netconn *server)
{
    netconn_delete(server);
}

int handlePairingMessage(struct pairingMessagePair *msg,
                         u16_t messageLen,
                         struct pairingContext *ctx)
{
    if (messageLen < sizeof(*msg))
        return -1;

    memcpy(ctx->ssid, msg->payload.ssid, sizeof(ctx->ssid));
    memcpy(ctx->passwd, msg->payload.passwd, sizeof(ctx->passwd));

    return 0;
}

void sendbackPairingAck(struct netconn *client)
{
    struct pairingMessageAck ack;
    ack.header.messageType = PAIRING_MESSAGE_TYPE_ACK;
    netconn_write(client, &ack, sizeof(ack), NETCONN_COPY);
}

void sendbackPairingError(struct netconn *client)
{
    struct pairingMessageError err;
    err.header.messageType = PAIRING_MESSAGE_TYPE_ERROR;
    netconn_write(client, &err, sizeof(err), NETCONN_COPY);
}

void pairingTask(void *pvParameters)
{
    struct pairingContext *ctx = (struct pairingContext *)pvParameters;
    int paired = 0;

    ip_addr_t first_client_ip;
    IP4_ADDR(&first_client_ip, 192, 168, 0, 2);
    dhcpserver_start(&first_client_ip, 4);

    struct netconn *server = pairingServerStart();
    // TODO: Check if server is null
    led_set(LED_RED, LED_STATE_ON, LED_BLINK_ON);

    while (!paired)
    {
        struct netconn *client = NULL;
        err_t err = netconn_accept(server, &client);
        if (err != ERR_OK)
        {
            if (client) {
                netconn_delete(client);
                client = NULL;
            }
            continue;
        }

        printf("New connection established\n");

        struct netbuf *buf;
        err = netconn_recv(client, &buf);
        if (err != ERR_OK)
        {
            printf("Error receiving data\n");
            netconn_delete(client);
            continue;
        }

        char *dataptr;
        u16_t len;
        err = netbuf_data(buf, (void **)&dataptr, &len);
        if (err != ERR_OK)
        {
            printf("Data error\n");
            netbuf_free(buf);
            netconn_delete(client);
            continue;
        }

        printf("Received: %.*s\n", len, dataptr);

        struct pairingMessageHeader *head = (struct pairingMessageHeader *)dataptr;
        switch (head->messageType)
        {
        case PAIRING_MESSAGE_TYPE_PAIR:
            if (!handlePairingMessage((struct pairingMessagePair *)head, len, ctx)) {
                paired = 1;
            }
            break;
        default:
            break;
        }

        netbuf_free(buf);

        if (paired) {
            printf("SSID: %s\n",ctx->ssid);
            printf("passwd: %s\n",ctx->passwd);
            sendbackPairingAck(client);
        } else {
            printf("Unkown message type\n");
            sendbackPairingError(client);
        }
        netconn_delete(client);
    }

    led_set(LED_RED, LED_STATE_OFF, LED_BLINK_OFF);
    led_set(LED_GREEN, LED_STATE_ON, LED_BLINK_ON);

    pairingServerStop(server);
    vTaskDelete(NULL);
}
