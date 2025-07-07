# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Source files
SRC = webdriver.cpp main.cpp
TEST_SRC = webdriver.cpp test.cpp

# Object files
OBJ = $(SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Executable names
TARGET = main
TEST_TARGET = test

# Default rule
all: $(TARGET)

# Build main program
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lcurl

# Build test executable
$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lcurl
	chmod +x $@

# Run test binary (renamed target to avoid conflict)
run-tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean build files
clean:
	rm -f *.o $(TARGET) $(TEST_TARGET)

# Mark phony targets
.PHONY: all run-tests clean
