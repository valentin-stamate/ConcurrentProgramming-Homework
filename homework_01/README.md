# Homework 1

## How to run?
Build all the sources using:```cmake . && make```

Now you can execute the client and the serve: ```./server```, ```./client```.

To find the server ip address: ```ifconfig | grep inet | grep 192 | xargs | cut -d' ' -f2```

## Docker
* ```docker build -t homework_01 .```
* ```docker run --name homework_01 -p 10000:10000 -d homework_01```

## Docker compose
* ```docker compose up --build```

## Run
* Server TCP: ```./server 1```
* Server UDP: ```./server 2```
* Client TCP: ```./client 1 127.0.0.1 10000 4096 2 1```
* Client UDP: ```./client 2 127.0.0.1 10001 4096 2 1```
* Client General: ```./client protocol ip port package_size dataset_type acknowledge``` `dataset_type` is 1 or 2 and  ```protocol``` is 1(TCP) or 2(UDP)

The TCP server is opened at port 10000 and UDP at 10001

## Resources
* https://www.geeksforgeeks.org/socket-programming-cc/
* https://www.geeksforgeeks.org/udp-server-client-implementation-c/
* https://stackoverflow.com/a/8438663/10805602
* https://www.kaggle.com/
* https://stackoverflow.com/a/49920624/10805602
* https://stackoverflow.com/a/48243640/10805602
* https://stackoverflow.com/a/27596415/10805602