CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = physics-app
SRC = main.cpp
OBJ = main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

$(OBJ): $(SRC)
	$(CXX) -c $(SRC) $(CXXFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
