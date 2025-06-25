# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -g -pthread

# Source files
SRCS = main.cpp \
	common/defs/defs.cpp \
	common/token/defs/token_defs.cpp \
	common/token/token.cpp \
	common/abstract-syntax-tree/defs/ASTree_defs.cpp \
	common/abstract-syntax-tree/ASTree.cpp \
	common/symbol/symbol.cpp \
	common/intermediate-representation-tree/defs/IRTree_defs.cpp \
	common/intermediate-representation-tree/IRTree.cpp \
	lexer/defs/lexer_defs.cpp \
	lexer/lexer.cpp \
	parser/parser.cpp \
	symbol-handling/symbol-table/symbol_table.cpp \
	symbol-handling/scope-manager/scope_manager.cpp \
	thread-pool/thread_pool.cpp \
	analyzer/analyzer.cpp \
	intermediate-representation/intermediate_representation.cpp \
	code-generator/defs/code_generator_defs.cpp \
	code-generator/asm-generator/asm_generator.cpp \
	code-generator/code-generator/code_generator.cpp

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
	./$(EXEC) < testfile.txt

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(EXEC)

# Additional rule to remove dependencies (optional)
distclean: clean
	rm -f *.d

# Include dependencies (optional, but useful for large projects)
-include $(OBJS:.o=.d)

