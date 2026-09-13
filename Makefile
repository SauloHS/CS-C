CC = zig cc
CFLAGS = -Iinclude -Wall -DCGLM_ALL_UNALIGNED
LDFLAGS = lib-mingw-w64/libglfw3.a -lopengl32 -lgdi32 -luser32 -lshell32
SRC = $(wildcard src/*.c)
OUT = bin/cs2.exe

.PHONY: all run

all:
	if not exist bin mkdir bin
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

run: all
	$(subst /,\,$(OUT))
