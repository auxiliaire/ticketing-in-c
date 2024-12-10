FROM ubuntu:22.04
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install apt-utils
RUN apt-get install make
RUN apt-get install pkg-config -y
RUN apt-get install libsqlite3-0
RUN apt-get install libglib2.0-0 -y
RUN apt-get install libjson-c5
RUN apt-get install libjansson4
WORKDIR /opt
COPY ["./build/mustach-latest/", "./mustach-latest/"]
WORKDIR /opt/mustach-latest
RUN make install
WORKDIR /app
COPY ["./build/lib*.so", "./build/"]
COPY ["./build/ticketing", "./build/"]
COPY ["./dev.sqlite", "./"]
COPY ["./view", "./view"]
COPY ["./public", "./public"]
EXPOSE 8000
ENTRYPOINT ["./build/ticketing"]
CMD ["-d", "/app/public", "-v", "2"]
