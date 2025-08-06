# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17 

# Find all .cpp files in current folder
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

# Output binary
TARGET = main
# Default rule
all: $(TARGET)

runtofile : all
	./$(TARGET) < input.txt > output.txt

run : all
	./$(TARGET) < input.txt


# Linking step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)