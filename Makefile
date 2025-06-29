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
	code-generator/code-generator/code_generator.cpp \
	compiler/compiler.cpp

# Object files (derived from the source files)
OBJS = $(SRCS:.cpp=.o)

# Output executable
EXEC = minicpp

# === Unit Testing ===

# Automatically find all test sources
TEST_SRCS = $(shell find unit-tests -name '*.cpp')
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_EXEC = run_tests

# GTest libraries (adjust if gtest is installed in a custom path)
GTEST_LIBS = -lgtest -lgtest_main -pthread

# Common dependencies shared with tests
# Test dependencies: all OBJS except main.o
TEST_DEPS = $(filter-out main.o, $(OBJS))

# Phony targets
.PHONY: all clean distclean test run

# Default target
all: $(EXEC)

# Rule to build main executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Generic compilation rule with dependency generation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Run the program with input
run: $(EXEC)
	./$(EXEC) < testfile.txt

# Run all unit tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Build test executable by linking test + required source objects
$(TEST_EXEC): $(TEST_OBJS) $(TEST_DEPS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LIBS)

# Clean up all binaries and object files
clean:
	rm -f $(OBJS) $(EXEC) $(TEST_OBJS) $(TEST_EXEC)

# Clean up dependencies
distclean: clean
	rm -f $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# Include dependency files
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
