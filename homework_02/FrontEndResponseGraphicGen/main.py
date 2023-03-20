
def main():
    import requests
    import time
    import matplotlib.pyplot as plt

    response_times = []
    for i in range(100):
        print(i)
        start_time = time.time()
        _ = requests.get('http://18.184.112.27:8080/')
        end_time = time.time()
        response_times.append(end_time - start_time)

    plt.plot(response_times)
    plt.title("Response time for frontend component")
    plt.xlabel('Request number')
    plt.ylabel('Response time (s)')
    plt.savefig('backend-time.png')


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()


