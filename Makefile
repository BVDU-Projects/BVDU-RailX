# Compiler settings
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
TARGET  = railx

# Directories
SRC_DIR = src
OBJ_DIR = src

# Source files
SRCS    = $(SRC_DIR)/graph.c \
          $(SRC_DIR)/timetable.c \
          $(SRC_DIR)/booking.c \
          $(SRC_DIR)/main.c

# Object files
OBJS    = $(SRCS:.c=.o)

# Default target: Build executable
all: $(TARGET)

# Link object files into executable binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile individual source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build and execute immediately
run: $(TARGET)
	./$(TARGET)

# Remove build artifacts
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET) $(TARGET).exe

.PHONY: all run clean
