CC = gcc
CFLAGS = -Wall -Wextra -pthread -Wno-unused-parameter
LDFLAGS =

# Folders
SRCDIRS = . auth commands utils connection
INCLUDES = $(addprefix -I, $(SRCDIRS))

SRCS = $(wildcard main.c auth/*.c commands/*.c utils/*.c connection/*.c)

OBJS = $(SRCS:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
