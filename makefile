CC = clang
SRC = main.c ./src/arena_alloc.c
INCLUDES = -I./include

CFLAGS = -Wall -Werror -O2

TARGET = main

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

