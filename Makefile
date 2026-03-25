CC = clang
CFLAGS = -Wall -Wextra -Iinclude -g

OBJ = src/main.o src/core.o src/storage.o 

all: minigit

minigit: $(OBJ)
	$(CC) $(CFLAGS) -o minigit $(OBJ)

clean:
	rm -f src/*.o minigit