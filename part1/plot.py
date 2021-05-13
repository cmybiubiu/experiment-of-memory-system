import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import os.path
import time


def main():
    while not os.path.exists("data_a.txt"):
        time.sleep(10)
    # Open part a data file
    f_a = open("data_a.txt")
    
    read_a = f_a.read()
    split_a = read_a.split('\n')

    # Get data out of the data file
    size_a = []
    time_a = []

    for item in split_a:
        if item:
            size_time_rate = item.split(' ')
            size_a.append(int(size_time_rate[0]))
            time_a.append(float(size_time_rate[1]) * 1000)

    # Plot the graph
    plt.plot(time_a, size_a, 'ro')
    plt.xlabel('Time ms')
    plt.ylabel('Size MB')
    plt.savefig('part1_a.png')

    # Clean up
    plt.close()
    f_a.close()

    ############################################

    while not os.path.exists("data_b.txt"):
        time.sleep(10)
    # Open part b data file
    f_b = open("data_b.txt")
    read_b = f_b.read()
    split_b = read_b.split('\n')

    # Get data out of the data file
    size_b = []
    time_b = []

    for item in split_b:
        if item:
            size_time_rate = item.split(' ')
            size_b.append(int(size_time_rate[0]))
            time_b.append(float(size_time_rate[1]))

    # Plot the graph
    plt.scatter(size_b, time_b)
    plt.xlabel('Size KB')
    plt.ylabel('Time ns')
    plt.xscale('log')
    plt.savefig('part1_b.png')

    # Clean up
    plt.close()
    f_b.close()


if __name__ == '__main__':
    main()
