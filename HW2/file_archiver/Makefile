# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -g

# Source files
SRCS := main.c

# Object files
OBJS := $(SRCS:.c=.o)

# Output executable
TARGET := arcx

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
