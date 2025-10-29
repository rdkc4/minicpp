# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -g -pthread

# Source files
SRCS = main.cpp \
	common/defs/defs.cpp \
	common/token/defs/token_defs.cpp \
	common/token/token.cpp \
	common/abstract-syntax-tree/defs/ASTree_defs.cpp \
	common/abstract-syntax-tree/source/ASTNode.cpp \
	common/abstract-syntax-tree/source/ASTExpression.cpp \
	common/abstract-syntax-tree/source/ASTBinaryExpression.cpp \
	common/abstract-syntax-tree/source/ASTFunctionCall.cpp \
	common/abstract-syntax-tree/source/ASTLiteral.cpp \
	common/abstract-syntax-tree/source/ASTId.cpp \
	common/abstract-syntax-tree/source/ASTParameter.cpp \
	common/abstract-syntax-tree/source/ASTStatement.cpp \
	common/abstract-syntax-tree/source/ASTSwitchBlock.cpp \
	common/abstract-syntax-tree/source/ASTPrintfSt.cpp \
	common/abstract-syntax-tree/source/ASTCompoundSt.cpp \
	common/abstract-syntax-tree/source/ASTAssignSt.cpp \
	common/abstract-syntax-tree/source/ASTReturnSt.cpp \
	common/abstract-syntax-tree/source/ASTIfSt.cpp \
	common/abstract-syntax-tree/source/ASTWhileSt.cpp \
	common/abstract-syntax-tree/source/ASTDoWhileSt.cpp \
	common/abstract-syntax-tree/source/ASTForSt.cpp \
	common/abstract-syntax-tree/source/ASTCaseSt.cpp \
	common/abstract-syntax-tree/source/ASTDefaultSt.cpp \
	common/abstract-syntax-tree/source/ASTSwitchSt.cpp \
	common/abstract-syntax-tree/source/ASTVariable.cpp \
	common/abstract-syntax-tree/source/ASTFunction.cpp \
	common/abstract-syntax-tree/source/ASTProgram.cpp \
	common/symbol/symbol.cpp \
	common/intermediate-representation-tree/defs/IRTree_defs.cpp \
	common/intermediate-representation-tree/source/IRNode.cpp \
	common/intermediate-representation-tree/source/IRExpression.cpp \
	common/intermediate-representation-tree/source/IRBinaryExpression.cpp \
	common/intermediate-representation-tree/source/IRTemporary.cpp \
	common/intermediate-representation-tree/source/IRFunctionCall.cpp \
	common/intermediate-representation-tree/source/IRLiteral.cpp \
	common/intermediate-representation-tree/source/IRId.cpp \
	common/intermediate-representation-tree/source/IRStatement.cpp \
	common/intermediate-representation-tree/source/IRVariable.cpp \
	common/intermediate-representation-tree/source/IRAssignSt.cpp \
	common/intermediate-representation-tree/source/IRPrintfSt.cpp \
	common/intermediate-representation-tree/source/IRIfSt.cpp \
	common/intermediate-representation-tree/source/IRCompoundSt.cpp \
	common/intermediate-representation-tree/source/IRReturnSt.cpp \
	common/intermediate-representation-tree/source/IRWhileSt.cpp \
	common/intermediate-representation-tree/source/IRDoWhileSt.cpp \
	common/intermediate-representation-tree/source/IRForSt.cpp \
	common/intermediate-representation-tree/source/IRSwitchBlock.cpp \
	common/intermediate-representation-tree/source/IRCaseSt.cpp \
	common/intermediate-representation-tree/source/IRDefaultSt.cpp \
	common/intermediate-representation-tree/source/IRSwitchSt.cpp \
	common/intermediate-representation-tree/source/IRParameter.cpp \
	common/intermediate-representation-tree/source/IRFunction.cpp \
	common/intermediate-representation-tree/source/IRProgram.cpp \
	lexer/defs/lexer_defs.cpp \
	lexer/lexer.cpp \
	parser/source/token_consumer.cpp \
	parser/source/expression_parser.cpp \
	parser/source/statement_parser.cpp \
	parser/source/function_parser.cpp \
	parser/source/parser.cpp \
	symbol-handling/symbol-table/symbol_table.cpp \
	symbol-handling/scope-manager/scope_manager.cpp \
	thread-pool/thread_pool.cpp \
	analyzer/source/expression_analyzer.cpp \
	analyzer/source/statement_analyzer.cpp \
	analyzer/source/function_analyzer.cpp \
	analyzer/source/analyzer.cpp \
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
