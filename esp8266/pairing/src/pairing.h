#ifndef __PAIRING_H__
#define __PAIRING_H__

/* APIs */
#define SSID_LEN 32
#define PASSWD_LEN 64

struct pairingContext {
    char ssid[SSID_LEN];
    char passwd[PASSWD_LEN];
};

void pairingApSetup(void);
void pairingTask(void *pvParameters);

/* Messages definitions */
#define PAIRING_MESSAGE_MAX_LEN 128

#define PAIRING_MESSAGE_TYPE_NULL   0
#define PAIRING_MESSAGE_TYPE_ERROR  1
#define PAIRING_MESSAGE_TYPE_PAIR   2
#define PAIRING_MESSAGE_TYPE_ACK    3

/* Pairing message header */
struct pairingMessageHeader {
    u32_t messageType;
};

/* Actual pairing message */
struct pairingMessagePair {
    struct pairingMessageHeader header;
    struct {
        char ssid[SSID_LEN];
        char passwd[PASSWD_LEN];
    } payload;
};

/* Pairing acknowlagement */
struct pairingMessageAck {
    struct pairingMessageHeader header;
};

/* Pairing error message */
struct pairingMessageError {
    struct pairingMessageHeader header;
};

#endif /* _PAIRING_H__ */