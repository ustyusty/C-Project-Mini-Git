CC = clang
CFLAGS = -Wall -Wextra -Iinclude -g

OBJ = src/main.o src/core.o src/storage.o

# Определение ОС
ifeq ($(OS),Windows_NT)
    RM_FILE = del /Q
    RM_DIR = rmdir /S /Q
    EXE = .exe
else
    RM_FILE = rm -f
    RM_DIR = rm -rf
    EXE =
endif

TARGET = minigit$(EXE)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	-$(RM_FILE) .minigit/history.dat
	-$(RM_DIR) .minigit/objects
	-$(RM_FILE) src\*.o $(TARGET)

reset: clean all