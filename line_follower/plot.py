import joblib
import matplotlib.pyplot as plt

def load_file(fname):
    f = joblib.load(fname)

def parse_data(raw_data):
    # raw_data = [(a, b, c, d, e, f) ...]
    motorR, motorL, sensorIR, sensorIL, sensorOR, sensorOL, time = list(zip(*raw_data))
    
    return motorL, motorR, sensorIR, sensorIL, sensorOR, sensorOL, time

def plot_motor_speeds(time, motorR, motorL):
    plot1 = plt.figure(1)
    plt.plot(time, motorR, c = 'r') 
    plt.plot(time, motorL, c = 'b') 
    

def plot_sensor(sensorIR, sensorOR, sensorIL, sensorOL):
    pplot2 = plt.figure(2)
    plt.plot(time, sensorIR, c = 'r') 
    plt.plot(time, sensorOR, c = 'b') 
    plt.plot(time, sensorIL, c = 'g') 
    plt.plot(time, sensorOL, c = 'p') 
    

if __name__ == '__main__':
    data_list = load_file('filename.txt')
    motorL, motorR, sensorIR, sensorIL, sensorOR, sensorOL, time = parse_data(data_list)

    time_norm = time - time[0]

    plot_motor_speeds(time_norm, motorR, motorL)
    plot_sensor(time_norm, sensorIR, sensorOR, sensorIL, sensorOL)

    plt.show()
    