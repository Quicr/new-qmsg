#!/usr/bin/env bash


[ ! -p /tmp/pipe-u2s ] && mkfifo /tmp/pipe-u2s
[ ! -p /tmp/pipe-n2s ] && mkfifo /tmp/pipe-n2s
[ ! -p /tmp/pipe-s2u ] && mkfifo /tmp/pipe-s2u
[ ! -p /tmp/pipe-s2n ] && mkfifo /tmp/pipe-s2n

trap_with_arg() { # from https://stackoverflow.com/a/2183063/804678
  local func="$1"; shift
  for sig in "$@"; do
    trap "$func $sig" "$sig"
  done
}

stop() {
  trap - SIGINT EXIT
  printf '\n%s\n' "received $1, killing child processes"
  kill -s SIGINT 0
}

trap_with_arg 'stop' EXIT SIGINT SIGTERM SIGHUP

build/src/slowNet/slowNet &

build/src/slowSec/slowSec &

build/src/slowUI/slowUI 

