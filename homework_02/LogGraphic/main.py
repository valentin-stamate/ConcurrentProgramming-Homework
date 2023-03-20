import random

import matplotlib.pyplot as plt
import numpy as np

INPUT = "input.txt"


def get_data():
    data = []
    f = open(INPUT, 'r')
    lines = f.readlines()
    for line in lines:
        if "get_data:" in line:
            time = line.split("get_data:")[1].replace('ms', '').replace('\n', '').strip(' ')
            data.append(float(time))
    return data


def create_plot(data):
    points = np.array(data)

    plt.plot(points)
    plt.title('Response time for calls to the database from Lambda function')
    plt.xlabel('Request')
    plt.ylabel('Time(ms)')
    plt.savefig('chart.png')


def main():
    data = get_data()
    create_plot(data)


if __name__ == '__main__':
    main()
