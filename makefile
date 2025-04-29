CC = clang
SRC = main.c ./src/arena_alloc.c
INCLUDES = -I./include

# Toggle to 1 for debug builds
DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CFLAGS = -Wall -Werror -g -fsanitize=address,undefined -DDEBUG
else
	CFLAGS = -Wall -Werror -O2
endif

TARGET = main

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

