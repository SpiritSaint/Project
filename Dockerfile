FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cp .env.example /src/build/.env && \
    cd build && \
    make && \
    openssl genrsa -out private.pem 4096 && \
    openssl rsa -in private.pem -pubout -out public.pem

WORKDIR /src/build
RUN ./test

ENTRYPOINT ["./app"]