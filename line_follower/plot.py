import joblib
import matplotlib.pyplot as plt

def load_file(fname):
    f = joblib.load(fname)
    return f

def parse_data(raw_data):
    # raw_data = [(a, b, c, d, e, f) ...]
    values = list(zip(*raw_data))
    values = [[int(i) for i in l] for l in values]
    motorR, motorL, sensorIR, sensorIL, sensorOR, sensorOL, time = values

    return motorL, motorR, sensorIR, sensorIL, sensorOR, sensorOL, time

def plot_motor_speeds(time, motorR, motorL):
    plot1 = plt.figure(1)
    plt.plot(time, motorR, c = 'r')
    plt.plot(time, motorL, c = 'b')


def plot_sensor(time, sensorIR, sensorOR, sensorIL, sensorOL):
    pplot2 = plt.figure(2)
    plt.plot(time, sensorIR, c = 'r')
    plt.plot(time, sensorOR, c = 'b')
    plt.plot(time, sensorIL, c = 'g')
    plt.plot(time, sensorOL, c = 'purple')


if __name__ == '__main__':
    data_list = load_file('data.jl')
    motorL, motorR, sensorIR, sensorIL, sensorOR, sensorOL, time = parse_data(data_list)

    time_norm = [int(t) - int(time[0]) for t in time]

    plot_motor_speeds(time_norm, motorR, motorL)
    plot_sensor(time_norm, sensorIR, sensorOR, sensorIL, sensorOL)

    plt.show()
