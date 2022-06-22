FROM alpine:latest as builder
LABEL description="Build QMsg"
RUN apk add --no-cache cmake alpine-sdk openssl-dev
RUN apk add --no-cache tcsh bash
#CMD /bin/tcsh

ARG TARGETOS
ARG TARGETARCH
RUN echo "Building for $TARGETOS/$TARGETARCH"

RUN apk add --no-cache \
        ca-certificates \
        clang lld curl

RUN curl https://sh.rustup.rs -sSf | bash -s -- -y


RUN mkdir -p /src/qmsg
WORKDIR /src/qmsg
COPY . .
RUN rm -rf /src/qmsg/build
#RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
WORKDIR /src/qmsg/build
RUN make -j 4
RUN cp  src/uiProc/uiProc  /usr/local/bin/.
RUN cp  src/secProc/secProc  /usr/local/bin/.
RUN cp  src/netProc/netProc  /usr/local/bin/.
RUN ls -lh /usr/local/bin


FROM alpine:latest
RUN apk add --no-cache libstdc++
RUN apk add --no-cache bash tcsh
#CMD /bin/tcsh

COPY --from=builder /usr/local/bin/uiProc /usr/local/bin/.
COPY --from=builder /usr/local/bin/secProc /usr/local/bin/.
COPY --from=builder /usr/local/bin/netProc /usr/local/bin/.

RUN addgroup -S qmsg
RUN adduser -D -S -S -G qmsg qmsg
USER qmsg
WORKDIR /home/qmsg

RUN  mkfifo /tmp/pipe-u2s
RUN  mkfifo /tmp/pipe-n2s
RUN  mkfifo /tmp/pipe-s2u
RUN  mkfifo /tmp/pipe-s2n

#EXPOSE 5004/udp
#CMD /usr/local/bin/qmsgRelay

