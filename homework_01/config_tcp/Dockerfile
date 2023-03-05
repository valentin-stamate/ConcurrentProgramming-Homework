FROM ubuntu:latest

WORKDIR /home/app

RUN apt-get update \
    && apt-get -y install build-essential \
    && apt-get install -y wget \
    && apt install -y g++ \
    && apt install -y cmake

COPY ../.. .

RUN cmake . && make

EXPOSE 10000

CMD ["./server", "1"]