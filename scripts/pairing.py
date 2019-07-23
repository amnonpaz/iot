import socket
import struct


class PairingClient:
    socketTimeout = 5.0
    readBuffSize = 16

    ssidLen = 32
    passwdLen = 64

    messageTypeNull = 0
    messageTypeError = 1
    messageTypePair = 2
    messageTypeAck = 3

    Replies = {messageTypeNull:  "NULL",
               messageTypeError: "Error",
               messageTypePair:  "Paired",
               messageTypeAck:   "Ack"}

    def __init__(self, pairing_server_ip = '192.168.0.1', pairing_port = 6001):
        self.pairingServerIp = pairing_server_ip
        self.pairingPort = pairing_port
        self.connected = False
        self.sock = None

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(self.socketTimeout)
        try:
            self.sock.connect((self.pairingServerIp, self.pairingPort))
            self.connected = True
        except Exception as e:
            print("Error connecting to pairing server: ", e)

        return self.connected

    def disconnect(self):
        self.sock.close()
        self.connected = False

    def send_pair_message(self, ssid, passwd):
        if not self.connected:
            print("Cannot send message without server connection")
            return False

        try:
            self.sock.send(self.prepare_pairing_message(ssid, passwd))
        except Exception as e:
            print("Error sending pairing message: ", e)
            return False

        return True

    def prepare_pairing_message(self, ssid, passwd):
        return struct.pack("L" + str(self.ssidLen) + "s" + str(self.passwdLen) + "s",
                           self.messageTypePair, ssid.encode(), passwd.encode())

    def read_reply(self):
        reply = None
        try:
            reply = self.sock.recv(self.readBuffSize)
        except Exception as e:
            print("Error receiving reply: ", e)

        message = "No reply from endpoint"
        if reply is not None:
            reply_data = struct.unpack("L", reply)[0]
            if reply_data < len(self.Replies):
                message = self.Replies[reply_data]
            else:
                message = "Unknown reply"

        return message

    def is_connected(self):
        return self.connected
