import serial
from time import sleep

if __name__ == "__main__":
    arduinoComPort = "/dev/ttyACM0"
    baudRate = 9600
    serialPort = serial.Serial(arduinoComPort, baudRate, timeout=1)

    while True:
        input("Press enter")
        serialPort.write(bytes('a', 'utf-8'))
        print("Waiting for measure...")
        measure = serialPort.readline().decode()
        sleep(1)
        print(f"Measure: '{measure.strip()}'")
        if not measure:
            print("Something went wrong, try again")
            continue
        print("---" * 10)
        print()
