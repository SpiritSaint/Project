FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cp .env.example build/.env && \
    cd build && \
    make

RUN /src/build/test

ENTRYPOINT ["/src/build/app"]