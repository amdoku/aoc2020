CC = gcc
CXX = g++

CFLAGS = -Wall -Wextra -std=c11
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = solution

BINDIR = bin

OBJECTS= main.o value.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$@ $^

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE)

.POSIX:
.PHONY: all $(BIN)/* .cpp.o simple clean
