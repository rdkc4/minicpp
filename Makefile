# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -g

# Source files
SRCS = main.cpp src/lexer.cpp src/parser.cpp src/symbol.cpp src/symbol_table.cpp src/scope_manager.cpp src/ASTree.cpp src/analyzer.cpp src/IRTree.cpp src/intermediate_representation.cpp src/code_generator.cpp

# Object files (derived from the source files)
OBJS = $(SRCS:.cpp=.o)

# Output executable
EXEC = minicpp

# Default target
all: $(EXEC)

# Rule to link object files into the executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC)

# Rule to compile each source file into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(EXEC)

# Additional rule to remove dependencies (optional)
distclean: clean
	rm -f *.d

# Include dependencies (optional, but useful for large projects)
-include $(OBJS:.o=.d)

