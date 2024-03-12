# Compiler settings
CC = gcc
CFLAGS = -g -ansi -Wall -pedantic 
# Source files
SRCS =  directives.c labels.c  main.c instructions.c parser.c preprocessor.c writeFiles.c
OBJS = $(SRCS:.c=.o)
DEPS = instructions.h labels.h  directives.h parser.h utils.h preprocessor.h writeFiles.h

# Executable
TARGET = myprogram

# Rule to compile .c files into .o files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Default rule
all: $(TARGET)

# Rule to build the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
