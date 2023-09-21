FROM zentrackspa/container:latest

WORKDIR /src
COPY src .

RUN mkdir build && \
    cmake -B build/ -S . && \
    cd build && \
    make && \
    mv app /usr/bin/app && \
        rm /src -Rf

RUN app

ENTRYPOINT ["/usr/bin/app"]