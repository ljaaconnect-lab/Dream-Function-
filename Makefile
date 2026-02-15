CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra
TARGET = dream_function
SRC = dream_function.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) 100 200 1000

.PHONY: all clean test
