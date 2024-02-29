#!/bin/bash

set -e
apt update

echo "${0}: install telnet."
apt install -y telnet
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to install telnet: $status"
  exit $status
fi

echo "${0}: install siege."
apt install -y siege
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to install siege: $status"
  exit $status
fi

sleep infinity