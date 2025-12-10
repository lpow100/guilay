TARGET = opengl_test
SRC = opengl_test.c ../src/shader.c ../src/glad.c
INCLUDE_DIR = ../include
LIB_DIR = ../lib

ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLAGS = -I$(INCLUDE_DIR)
    LDFLAGS = -L$(LIB_DIR) -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32
    EXE = $(TARGET).exe
else
    CC = gcc
    CFLAGS = -I$(INCLUDE_DIR)
    LDFLAGS = -lglfw -lGL -lm -ldl -lpthread -lrt
    EXE = $(TARGET)
endif

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(EXE)
