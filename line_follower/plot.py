import joblib
import matplotlib.pyplot as plt

def load_file(fname):
    '''Loads in txt file for motor and sensor data'''
    f = joblib.load(fname)
    return f

def parse_data(raw_data):
    '''parses line of data into individual motor and sensor values'''
    # raw_data = [(a, b, c, d, e, f) ...]
    values = list(zip(*raw_data))
    values = [[int(i) for i in l] for l in values]

    # Order: motorR, motorL, sensorIR, sensorIL, sensorOR, sensorOL, time
    return (v for v in values)


def plot_both(time, motorR, motorL, sensorIR, sensorOR, sensorIL, sensorOL):
    '''plot both motor and sensor data as a function of time'''
    _, axs = plt.subplots(nrows=2, ncols=1, sharex=True)

    c_left = plt.cm.viridis(0)
    c_right = plt.cm.viridis(0.6)

    sensor_data = [("Inner", sensorIR, sensorIL), ("Outer", sensorOR, sensorOL)]
    for i, (label, sr, sl) in enumerate(sensor_data):
        host = axs[i]
        par1 = host.twinx()
        host.set_xlim(0, 8000)
        host.set_ylim(0, 250)
        par1.set_ylim(0, 1000)

        host.set_xlabel("Time (Milliseconds)")
        host.set_ylabel("Motor Speed")
        par1.set_ylabel("Sensor Reading")

        pmr, = host.plot(time, motorR, c=c_right, label="Motor Right", linewidth=1.5, zorder=1)
        pml, = host.plot(time, motorL, c=c_left, label="Motor Left", linewidth=1.5, zorder=1)
        psr, = par1.plot(time, sr, c=c_right, label=label + " Right Sensor", linestyle="--",
                         alpha=1, zorder=2, linewidth=1)
        psl, = par1.plot(time, sl, c=c_left, label=label + " Left Sensor", linestyle="--",
                         alpha=1, zorder=2, linewidth=1)
        host.legend(handles=[pmr, pml, psr, psl], loc='best')
        host.set_title(label + " Sensors vs Motor Speed")


if __name__ == '__main__':
    data_list = load_file('data.jl')
    motorR, motorL, sensorIR, sensorIL, sensorOR, sensorOL, time = parse_data(data_list)

    time_norm = [int(t) - int(time[0]) for t in time] #normalize time by subtracting the start time from every time value

    plot_both(time_norm, motorR, motorL, sensorIR, sensorOR, sensorIL, sensorOL)

    plt.show()
