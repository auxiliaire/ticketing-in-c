FROM ubuntu:22.04
RUN apt update
RUN apt upgrade -y
RUN apt install make
RUN apt install libsqlite3-0
RUN apt install libglib2.0-0 -y
RUN apt install libjson-c5
RUN apt install libjansson4
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
