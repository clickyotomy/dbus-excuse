SHELL       = /bin/bash
PROG_NAME   = dbus-excuse
SRC_DIR     = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
APT         = $(shell which apt)
CC          = $(shell which clang)
PKG_CONF    = $(shell which pkg-config)
DBUS_RUN    = $(shell which dbus-run-session)
DBUS_CLEAN  = $(shell which dbus-cleanup-sockets)
GREP        = $(shell which grep)
PGREP       = $(shell which pgrep)
DBUS_VER    = dbus-1
CFLAGS      = -m64 -Wall -Werror -Wextra -pedantic -ggdb -O3 -std=c11
INCFLAGS    = $(shell $(PKG_CONF) --cflags $(DBUS_VER))
LDFLAGS     = $(shell $(PKG_CONF) --cflags --libs $(DBUS_VER))
OBJS        = excuse.o server.o
FMT         = $(shell which clang-format) -style='{IndentWidth: 4, TabWidth: 4}' -i

$(PROG_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCFLAGS) $< -c -o $@

default: $(PROG_NAME)

dep:
	DEBIAN_FRONTEND='noninteractive' $(APT) update
	DEBIAN_FRONTEND='noninteractive' $(APT) install -y build-essential \
		clang clang-format pkg-config dbus libdbus-1-dev

format:
	$(FMT) -- *.c *.h

start: default
	$(DBUS_RUN) -- ./$(PROG_NAME) &

test: default
	$(SRC_DIR)/scripts/test.sh

clean:
	$(DBUS_CLEAN)
	/bin/rm -rf *~ *.o $(PROG_NAME)


.PHONY: default format clean start test
