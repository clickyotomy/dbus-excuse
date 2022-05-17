#! /bin/bash

# dep.sh: Install program dependencies.

APT=$(which apt)

DEBIAN_FRONTEND='noninteractive' ${APT} update
DEBIAN_FRONTEND='noninteractive' ${APT} install -y build-essential \
	clang clang-format pkg-config dbus libdbus-1-dev
