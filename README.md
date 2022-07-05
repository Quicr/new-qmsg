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

--- 
## Build Slower Relay and Publish to ECR

### Build Slower Image - amd64

> **Note**
> : Apple M1 folks will need to use [UTM](https://mac.getutm.app/) or an x86 build server to build AMD64

```
docker buildx build --progress=plain \
        --output type=docker --platform linux/amd64 \
        -f slow-relay.Dockerfile -t quicr/slow-relay:$(git rev-list --count HEAD)-amd64 .   
```

### Tag the image
```
docker tag quicr/slow-relay:$(git rev-list --count HEAD)-amd64 \
    017125485914.dkr.ecr.us-west-1.amazonaws.com/quicr/slow-relay:$(git rev-list --count HEAD)-amd64
```

### Login to ECR
Only need to do this once.

> **NOTE**: you can use [duo-sso cli](https://wiki.cisco.com/display/THEEND/AWS+SAML+Setup) to get
> an API KEY/SECRET using your CEC login to AWS.  Length of duration needs to be 3600 seconds or less.

```
export AWS_ACCESS_KEY_ID=<your key id>
export AWS_SECRET_ACCESS_KEY=<your secret key>

docker run --rm \
    -e AWS_ACCESS_KEY_ID=$AWS_ACCESS_KEY_ID -e AWS_SECRET_ACCESS_KEY=$AWS_SECRET_ACCESS_KEY \
    amazon/aws-cli \
    ecr get-login-password --region us-west-1 \
	|  docker login --username AWS --password-stdin 017125485914.dkr.ecr.us-west-1.amazonaws.com
```

### Push Slower Image

```
docker push 017125485914.dkr.ecr.us-west-1.amazonaws.com/quicr/slow-relay:$(git rev-list --count HEAD)-amd64
```
