FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cd build && \
    make && \
    mv app /usr/bin/app && \
    mv test /usr/bin/app-test

RUN app-test

ENTRYPOINT ["/usr/bin/app"]