from serial_cmd import Serial_cmd 

serial_port = Serial_cmd() # attempt to open serial port and run scan

if serial_port.connected:
    while True: 
        variable_int = input("Which variable do you want to change?")
        new_val = input("What should the variable be set to?")
        serial_port.write("<" + variable_int + "," + new_val + ">")
