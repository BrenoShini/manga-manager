CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = manga_manager
SOURCE = manga_manager.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) *.dat

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
