TARGET = main
SRC = src/main.c src/guilay.c src/common/glad.c src/common/shader.c src/common/elements.c src/common/quadtree.c
INCLUDE_DIR = include
LIB_DIR = lib

ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLAGS = -I$(INCLUDE_DIR)
    LDFLAGS = -L$(LIB_DIR) -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lfreetype
    EXE = $(TARGET).exe
else
    CC = gcc
    CFLAGS = -I$(INCLUDE_DIR)
    LDFLAGS = -lglfw -lGL -lm -ldl -lpthread -lrt -lfreetype
    EXE = $(TARGET)
endif

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(EXE)
