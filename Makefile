CC ?= cc
CFLAGS ?= -D_POSIX_C_SOURCE=200809L -std=c11 -Wall -Wextra -Wpedantic -O2
TARGET = tinyshell
SOURCES = tinyshell.c env.c history.c rawmode.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(TARGET)
