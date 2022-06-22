# qmsg

Messaging example on top of QuicR 


# Build

```
./build.sh
```

# Run programs

```
./run.sh
```

This creates named pipes for communications and starts the three
process. Each process can be individual run it's own terminal.

# Build with Docker

```
docker build --platform linux/amd64 --tag qmsg-amd64:latest .
docker build --platform linux/arm64/v8 --tag qmsg-arm64:latest . 
```

# Run with Docker

On Intel 
```
docker run --rm -it qmsg-amd64:latest /bin/bash 
```
On Mac M1
```
docker run --rm -it qmsg-arm64:latest /bin/bash 
```

The do
```
( netProc & ) ; ( secProc & ) ; uiProc 
```

