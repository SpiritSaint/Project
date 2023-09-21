FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cp .env.example /src/build/.env && \
    cd build && \
    make

WORKDIR /src/build
RUN ./test

ENTRYPOINT ["./app"]