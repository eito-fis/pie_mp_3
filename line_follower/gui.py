from tkinter import *
from serial_cmd import Serial_cmd
import joblib
from tqdm import tqdm

serial_port = Serial_cmd() # attempt to open serial port and run scan
MOTOR_SPEED = 0
ERROR_COEF = 0
SENSOR_THRESH = 0

COLLECTION_TIME = 10
SLEEP_INTERVAL = 0.1


def handleStartBot():
    writeSerial('0', '1')

def handleStopBot():
    writeSerial('0', '0')

def parse_message(message):
  '''Splits incoming message from Arduino into individual values,
     delimited by commas'''
  split_data = message.split(",")
  return [int(value) for value in split_data]

def handleStartData():
    writeSerial('4', '')
    data = []
    print("Collecting data...")
    for _ in tqdm(range(int(COLLECTION_TIME/SLEEP_INTERVAL))):
        t, motorL, motorR, *measures = readSerialForData();
        data.append((motorR, motorL, measures[2], measures[1], measures[3], measures[0], t))
    print(f"Data: {data}")
    joblib.dump(data, "data.jl")
    print("Saved to data.jl")
    writeSerial('5', '')


def handleStopData():
    writeSerial('5', '')


def handleSubmit():
    updates = 0
    if eSensorThresh.get().isnumeric():
        writeSerial('3', eSensorThresh.get())
        updates += 1
    if eMotorSpeed.get().isnumeric():
        writeSerial('1', eMotorSpeed.get())
        updates += 1
    if eErrorCoef.get().isnumeric():
        writeSerial('2', eErrorCoef.get())
        updates += 1
    readSerialForConsts(n=updates)


def parseData(raw_data):
    start = raw_data.find('{')
    end = raw_data.find('}')
    elements = raw_data[start + 1:end]
    elements_l = elements.split(',')
    return elements_l


def displayOldConstants(const_l):
    MSvar.set(const_l[0])
    ECvar.set(const_l[1])
    STvar.set(const_l[2])


def writeSerial(index, newval):
    if serial_port.connected:
        print(newval)
        serial_port.write("<" + index + "," + newval + ">")


def readSerialForData():
    if serial_port.connected:
        data = serial_port.read()
        split_data = data.split(",")
        while (
            ('{' not in data or '}' not in data) or
            (not len(split_data) == 7)
        ):
            data = serial_port.read()
            split_data = data.split(",")

        values = parseData(data)
        return [value for value in values]


def readSerialForConsts(n=1):
    if serial_port.connected:
        print("Retreiving constants...")
        for _ in range(n):
            # Each constant we update generates its own response, so we must read once
            # for every value that we updated. Defaults to 1.
            currentconsts = serial_port.read()
        currentconsts_l = currentconsts.split(',')
        print(f"Got: {currentconsts_l}")

        while (
            ('{' not in currentconsts or '}' not in currentconsts) or
            (not len(currentconsts_l) == 3)
        ):
            currentconsts = serial_port.read()
            currentconsts_l = currentconsts.split(',')

        constants = parseData(currentconsts)
        displayOldConstants(constants)


if __name__ == "__main__":
    root = Tk()
    root.title("Line follower bot!")
    root.geometry('240x520+0+0')
    root.configure(bg='white')

    MSvar = StringVar()
    MSlabel2 = Label( root, text="Motor speed variable: ", relief=RAISED )
    MSlabel = Label( root, textvariable=MSvar, relief=RAISED )


    ECvar = StringVar()
    EClabel2 = Label( root, text="Error coefficient variable: ", relief=RAISED )
    EClabel = Label( root,  textvariable= ECvar, relief=RAISED )


    STvar = StringVar()
    STlabel2 = Label( root, text="Sensor threshold variable: ", relief=RAISED )
    STlabel = Label( root, textvariable= STvar, relief=RAISED )


    eSensorThresh = Entry(root, width = 50)
    eMotorSpeed = Entry(root, width = 50)
    eErrorCoef = Entry(root, width = 50)


    eSensorThresh.insert(0, "Enter a new sensor threshold")
    eMotorSpeed.insert(0, "Enter a new motor speed")
    eErrorCoef.insert(0, "Enter a new error coef")

    submitButton = Button(
        root,
        text="Submit new constants",
        highlightbackground='black',
        command=handleSubmit
    )

    startBotButton = Button(
        root,
        text="Start Bot",
        highlightbackground='blue',
        command=handleStartBot
    )
    stopBotButton = Button(
        root,
        text="Stop Bot",
        highlightbackground='red',
        command=handleStopBot
    )

    startDataButton = Button(
        root,
        text="Start Data Collection",
        highlightbackground='blue',
        command = handleStartData
    )
    stopDataButton = Button(
        root,
        text="Stop Data Collection",
        highlightbackground='red',
        command=handleStopData
    )


    MSlabel2.pack()
    MSlabel.pack()
    EClabel2.pack()
    EClabel.pack()
    STlabel2.pack()
    STlabel.pack()

    eMotorSpeed.pack()
    eErrorCoef.pack()
    eSensorThresh.pack()

    submitButton.pack()

    startBotButton.pack()
    stopBotButton.pack()

    startDataButton.pack()
    stopDataButton.pack()

    root.after(2000, readSerialForConsts)
    root.mainloop()
