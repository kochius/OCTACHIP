OBJS = main.cpp \
	src/emulator.cpp \
	src/frame.cpp \
	src/interpreter.cpp \
	src/opcode.cpp \
	src/random.cpp \
	src/io/input.cpp \
	src/io/renderer.cpp

CC = g++

COMPILER_FLAGS = -Wall -Wextra -Werror

LINKER_FLAGS = -lSDL2

OBJ_NAME = chip8

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)