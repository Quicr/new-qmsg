# QMsg

Messaging example on top of QuicR 


## Build

```
./build.sh
```

## Run programs

```
./run.sh
```

This creates named pipes for communications and starts the three
process. Each process can be individual run it's own terminal.

## Build with Docker

```
docker build --no-cache --platform linux/amd64 --tag qmsg-amd64:latest .
docker build --no-cache --platform linux/arm64/v8 --tag qmsg-arm64:latest . 
```

## Run with Docker

On Intel:
```
docker run --rm -it qmsg-amd64:latest /bin/bash 
```

On Mac M1:
```
docker run --rm -it qmsg-arm64:latest /bin/bash 
```

Then to play with messaging app, in the shell run:
```
( netProc & ) ; ( secProc & ) ; uiProc 
```

## Run relay with Docker

Build docker images then (replace amd with arm for M1)

```
docker run --rm -p '2022:2022/udp` -it qmsg-amd64:latest

```

