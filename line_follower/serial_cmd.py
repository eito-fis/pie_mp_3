# Serial hardware interface module for communicating
# with arduino. Implementation courtesy of Prof. Brad Minch.
# See the link below for source:
# Source: https://www.youtube.com/watch?v=2HeIXhYVpBU

from serial import Serial
from serial.tools import list_ports

BAUD_RATE = 115200

class Serial_cmd:

    Arduino_IDs = ((0x2341, 0x0043), (0x2341, 0x0001),
                   (0x2A03, 0x0043), (0x2341, 0x0243),
                   (0x0403, 0x6001), (0x1A86, 0x7523))

    def __init__(self, port = ''):
        if port == '':
            self.dev = None
            self.connected = False
            devices = list_ports.comports()
            for device in devices:
                if (device.vid, device.pid) in Serial_cmd.Arduino_IDs:
                    try:
                        self.dev = Serial(device.device, BAUD_RATE)
                        self.connected = True
                        print(f'Connected to {device.device}')
                    except:
                        print('Could not connect to device!')
                if self.connected:
                    break
        else:
            try:
                self.dev = Serial(port, BAUD_RATE)
                self.connected = True
            except:
                self.dev = None
                self.connected = False

    def write(self, command):
        if self.connected:
            self.dev.write('{!s}\r'.format(command).encode())

    def read(self):
        if self.connected:
            return self.dev.readline().decode()
