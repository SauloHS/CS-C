CC = zig cc
CFLAGS = -Iinclude -Wall
LDFLAGS = lib-mingw-w64/libglfw3.a -lopengl32 -lgdi32 -luser32 -lshell32
SRC = src/main.c src/glad.c src/util.c src/shader.c src/window.c src/mesh.c src/camera.c src/variables.c
OUT = bin/cs2.exe

.PHONY: all run

all:
	if not exist bin mkdir bin
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

run: all
	$(subst /,\,$(OUT))
