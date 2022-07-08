#---------------------------------------------------------------------
# Slow-Relay Docker image
#
#  docker buildx build --progress=plain \
#         --output type=docker --platform linux/amd64 \
#         -f slow-relay.Dockerfile -t quicr/slow-relay:$(git rev-list --count HEAD)-amd64 .
#
#
#---------------------------------------------------------------------
FROM alpine:latest as builder

RUN apk add --no-cache cmake alpine-sdk openssl-dev
RUN apk add --no-cache tcsh bash
RUN apk add --no-cache \
        ca-certificates \
        clang lld curl

WORKDIR /ws

COPY ./src ./src
COPY ./lib ./lib
COPY ./contrib ./contrib
COPY ./CMakeLists.txt .

#RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=slow-relay -Dqmsg_ENABLE_QUICR=OFF \
    && cmake --build build

RUN cp  build/src/slowRelay/slowRelay  /usr/local/bin/. \
    && cp  build/src/slowTest/slowTest  /usr/local/bin/. \
    && cp  build/src/slowUI/slowUI /usr/local/bin/. \
    && cp  build/src/slowSec/slowSec  /usr/local/bin/. \
    && cp  build/src/slowNet/slowNet  /usr/local/bin/.


FROM alpine:latest
RUN apk add --no-cache libstdc++
RUN apk add --no-cache bash tcsh

COPY --from=builder /usr/local/bin/slowRelay /usr/local/bin/.
COPY --from=builder /usr/local/bin/slowTest /usr/local/bin/.

COPY --from=builder /usr/local/bin/slowUI /usr/local/bin/.
COPY --from=builder /usr/local/bin/slowSec /usr/local/bin/.
COPY --from=builder /usr/local/bin/slowNet /usr/local/bin/.

RUN addgroup -S qmsg
RUN adduser -D -S -S -G qmsg qmsg
USER qmsg
WORKDIR /home/qmsg

RUN  mkfifo /tmp/pipe-u2s
RUN  mkfifo /tmp/pipe-n2s
RUN  mkfifo /tmp/pipe-s2u
RUN  mkfifo /tmp/pipe-s2n

EXPOSE 5004/udp
CMD /usr/local/bin/slowRelay

