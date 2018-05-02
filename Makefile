TARGET = ./bin/breakout
LIBS = -lSDL2 -lSDL2_image -lGL -lGLEW
CC = g++

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = src/main.cpp $(wildcard src/**/*.cpp)
HEADERS = $(wildcard src/**/*.hpp)

%.o: %.c $(HEADERS)
	$(CC)  $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
