import tkinter as tk
from pairing import PairingClient


class InputField:
    def __init__(self, tk_parent_element, name, default):
        self.name = name

        row = tk.Frame(tk_parent_element)
        label = tk.Label(row, width=15, text=name, anchor='w')
        row.pack(side=tk.TOP, fill=tk.X, padx=5, pady=1)
        label.pack(side=tk.LEFT)
        self.entry = tk.Entry(row)
        self.entry.insert(tk.END, default)
        self.entry.pack(side=tk.RIGHT, expand=tk.YES, fill=tk.X)

    def value(self):
        return self.entry.get()


class OutputField:
    def __init__(self, tk_parent_element, name, default):
        self.name = name

        row = tk.Frame(tk_parent_element)
        label = tk.Label(row, width=15, text=name + ":", anchor='w')
        row.pack(side=tk.TOP, fill=tk.X, padx=5, pady=1)
        label.pack(side=tk.LEFT)
        self.text = tk.Label(row, text=default, anchor='w')
        self.text.pack(side=tk.RIGHT, expand=tk.YES, fill=tk.X)

    def set(self, value):
        return self.text.config(text=value)


class PairingApplication:
    ConnectionStatus = {False: "Disconnected", True: "Connected"}

    Inputs = [{"name": "Endpoint IP", "default": "192.168.0.1"},
              {"name": "Endpoint Port", "default": "6001"},
              {"name": "SSID", "default": "PAZ-NETWORK"},
              {"name": "Password", "default": ""},
              {"name": "Host URL", "default": ""}]

    Actions = [{"name": "Connect",
                "command": (lambda self: lambda s=self: s.connect())},
               {"name": "Send",
                "command": (lambda self: lambda s=self: s.send())},
               {"name": "Disconnect",
                "command": (lambda self: lambda s=self: s.disconnect())},
               {"name": "Exit",
                "command": (lambda self: lambda s=self: s.exit())}]

    Outputs = [{"name": "Connection", "default": ConnectionStatus[False]},
               {"name": "Output", "default": "Ready"}]

    def __init__(self):
        self.client = PairingClient()

        self.top = tk.Tk()
        self.top.title("IoT Pairing")
        self.top.resizable(True, False)

        self.inputs = {}
        for field in self.Inputs:
            self.inputs[field["name"]] = InputField(self.top, field["name"], field["default"])

        self.outputs = {}
        for field in self.Outputs:
            self.outputs[field["name"]] = OutputField(self.top, field["name"], field["default"])

        for field in self.Actions:
            button = tk.Button(self.top, text=field["name"], command=field["command"](self))
            button.pack(side=tk.LEFT, padx=5, pady=5, expand=True)

        self.top.mainloop()

    def connect(self):
        ip = self.inputs["Endpoint IP"]
        port = self.inputs["Endpoint Port"]
        self.client.connect()
        self.set_status()

    def disconnect(self):
        self.client.disconnect()
        self.set_status()

    def send(self):
        result = self.client.send_pair_message(self.inputs["SSID"].value(),
                                               self.inputs["Password"].value())

        self.outputs["Output"].set(result)

    def set_status(self):
        connected = self.client.is_connected()
        self.outputs["Connection"].set(self.ConnectionStatus[connected])

    def exit(self):
        self.top.quit()


if __name__ == '__main__':
    PairingApplication()
