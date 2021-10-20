from tkinter import *
from serial_cmd import Serial_cmd

serial_port = Serial_cmd() # attempt to open serial port and run scan
MOTOR_SPEED = 0
ERROR_COEF = 0
SENSOR_THRESH = 0



def handleStart():
    writeSerial('0', '1')

def handleStop():
    writeSerial('0', '0')

def handleSubmit():
    if eSensorThresh.get().isnumeric():
        print(type(eSensorThresh.get()))
        writeSerial('3', eSensorThresh.get())
    if eMotorSpeed.get().isnumeric():
        writeSerial('1', eMotorSpeed.get())
    if eErrorCoef.get().isnumeric():
        writeSerial('2', eErrorCoef.get())


def updateConsts(old_constants):
    print(old_constants)

def writeSerial(index, newval):
    if serial_port.connected:
        serial_port.write("<" + index + "," + newval + ">")

def readSerial():
    if serial_port.connected:

        currentconsts = serial_port.read()

        print(currentconsts)
        if '<>' in currentconsts:
            updateConsts(currentconsts)



root = Tk()
root.title("Line follower bot!")
root.geometry('240x320+0+0')
root.configure(bg='white')

eSensorThresh = Entry(root, width = 50)
eMotorSpeed = Entry(root, width = 50)
eErrorCoef = Entry(root, width = 50)
eSensorThresh.pack()
eMotorSpeed.pack()
eErrorCoef.pack()

eSensorThresh.insert(0, "Enter a new sensor threshold")
eMotorSpeed.insert(0, "Enter a new motor speed")
eErrorCoef.insert(0, "Enter a new error coef")

submitButton = Button(root, text="Submit new constants", highlightbackground='black', command = handleSubmit)
startButton = Button(root, text="Start", highlightbackground='blue', command = handleStart)

stopButton = Button(root, text="Stop", highlightbackground = 'red', command = handleStop)

submitButton.pack()
startButton.pack()
stopButton.pack()


root.mainloop()

#readSerial()
