FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cp .env.example /src/build/.env && \
    cd build && \
    make && \
    openssl dhparam -out dh-params.pem 4096 && \
    openssl req -newkey rsa:4096 -nodes -keyout private.pem -x509 -days 365 -out public.pem -subj "/C=CL/ST=Santiago/L=Santiago/O=Ian Torres/OU=Project Authority/CN=project.local"

WORKDIR /src/build
RUN ./test

ENTRYPOINT ["./app"]