#! /bin/bash

# test.sh: Test the server with a single message on the bus.

PROG="dbus-excuse"
DBUS="$(which dbus-run-session)"
GREP="$(which grep)"
PGRP="$(which pgrep)"
STRN="$(which strings)"
DSND="$(which dbus-send)"

${DBUS} -- ./${PROG} &
sleep 1

DPID=$(${PGRP} ${PROG})
ADDR="$(${STRN} "/proc/${DPID}/environ" | grep 'DBUS_SESSION_BUS_ADDRESS' | cut -d '=' -f2-)"
TEST="${DSND} --bus=\"${ADDR}\" --print-reply=literal --dest='org.bofh' '/' 'org.bofh.loop.test' | ${GREP} -F 'Clock speed.' >/dev/null"

if eval "${TEST}"; then
	echo "OK"
else
	echo "FAIL"
fi
