SHELL := /bin/bash
CXXFLAGS = -Wall -Werror -std=c++11

# Files
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,obj/%.o,$(SRC))
INC = $(wildcard include/*.h*)
LDFLAGS= -lbcm2835 -lrt -lSSD1306_OLED_RPI 
BIN = bin/build

# Targets
all: $(BIN)

$(BIN): $(OBJ)
	g++ $(CXXFLAGS) $(OBJ) -o $(BIN) $(LDFLAGS)

obj/%.o: src/%.cpp
# Replace the letter for the example
	g++ $(CXXFLAGS) -c $< -o $@  -Iinclude
clean:
	rm -f $(OBJ) $(BIN)
