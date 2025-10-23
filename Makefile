CC := gcc
OBJ := src/snake.o libs/tigr.o libs/miniaudio.o
BIN := snake

WIN_LIBS := -lopengl32 -lgdi32
LINUX_LIBS := -lGL -lX11 -lm -lpthread

UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    LIBS := $(WIN_LIBS)
else ifeq ($(UNAME_S),Linux)
    LIBS := $(LINUX_LIBS)
else
	$(error Unsupported OS detected: $(UNAME_S). \
		    Only Linux and Windows are supported. \
			For MacOS, compile it yourself with -framework OpenGL and -framework Cocoa linkers)
endif

%.o: %.c
	$(CC) -c -o $@ $<

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

clean:
	rm *.exe
	rm src/*.o
	rm libs/*.o