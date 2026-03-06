# Compiler and flags
CXX = clang++
AS = clang
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -g -pthread -O1

SANITIZER = -fsanitize=address,undefined

# Source files
SRCS = main.cpp \
	common/preprocessing/source/preprocessing_libraries.cpp \
	preprocessor/preprocessor.cpp \
	common/defs/defs.cpp \
	common/token/defs/token_defs.cpp \
	common/token/token.cpp \
	common/abstract-syntax-tree/defs/ast_defs.cpp \
	common/abstract-syntax-tree/source/ast_node.cpp \
	common/abstract-syntax-tree/source/ast_expr.cpp \
	common/abstract-syntax-tree/source/ast_binary_expr.cpp \
	common/abstract-syntax-tree/source/ast_function_call_expr.cpp \
	common/abstract-syntax-tree/source/ast_literal_expr.cpp \
	common/abstract-syntax-tree/source/ast_id_expr.cpp \
	common/abstract-syntax-tree/source/ast_parameter.cpp \
	common/abstract-syntax-tree/source/ast_stmt.cpp \
	common/abstract-syntax-tree/source/ast_switch_block_stmt.cpp \
	common/abstract-syntax-tree/source/ast_compound_stmt.cpp \
	common/abstract-syntax-tree/source/ast_assign_stmt.cpp \
	common/abstract-syntax-tree/source/ast_return_stmt.cpp \
	common/abstract-syntax-tree/source/ast_if_stmt.cpp \
	common/abstract-syntax-tree/source/ast_while_stmt.cpp \
	common/abstract-syntax-tree/source/ast_dowhile_stmt.cpp \
	common/abstract-syntax-tree/source/ast_for_stmt.cpp \
	common/abstract-syntax-tree/source/ast_case_stmt.cpp \
	common/abstract-syntax-tree/source/ast_default_stmt.cpp \
	common/abstract-syntax-tree/source/ast_switch_stmt.cpp \
	common/abstract-syntax-tree/source/ast_variable_decl_stmt.cpp \
	common/abstract-syntax-tree/source/ast_function_call_stmt.cpp \
	common/abstract-syntax-tree/source/ast_include_dir.cpp \
	common/abstract-syntax-tree/source/ast_dir.cpp \
	common/abstract-syntax-tree/source/ast_function.cpp \
	common/abstract-syntax-tree/source/ast_program.cpp \
	common/abstract-syntax-tree/source/ast_dumper.cpp \
	common/symbol/symbol.cpp \
	common/intermediate-representation-tree/defs/ir_defs.cpp \
	common/intermediate-representation-tree/source/ir_node.cpp \
	common/intermediate-representation-tree/source/ir_expr.cpp \
	common/intermediate-representation-tree/source/ir_binary_expr.cpp \
	common/intermediate-representation-tree/source/ir_temporary_expr.cpp \
	common/intermediate-representation-tree/source/ir_function_call_expr.cpp \
	common/intermediate-representation-tree/source/ir_literal_expr.cpp \
	common/intermediate-representation-tree/source/ir_id_expr.cpp \
	common/intermediate-representation-tree/source/ir_stmt.cpp \
	common/intermediate-representation-tree/source/ir_variable_decl_stmt.cpp \
	common/intermediate-representation-tree/source/ir_assign_stmt.cpp \
	common/intermediate-representation-tree/source/ir_if_stmt.cpp \
	common/intermediate-representation-tree/source/ir_compound_stmt.cpp \
	common/intermediate-representation-tree/source/ir_return_stmt.cpp \
	common/intermediate-representation-tree/source/ir_while_stmt.cpp \
	common/intermediate-representation-tree/source/ir_dowhile_stmt.cpp \
	common/intermediate-representation-tree/source/ir_for_stmt.cpp \
	common/intermediate-representation-tree/source/ir_function_call_stmt.cpp \
	common/intermediate-representation-tree/source/ir_switch_block_stmt.cpp \
	common/intermediate-representation-tree/source/ir_case_stmt.cpp \
	common/intermediate-representation-tree/source/ir_default_stmt.cpp \
	common/intermediate-representation-tree/source/ir_switch_stmt.cpp \
	common/intermediate-representation-tree/source/ir_parameter.cpp \
	common/intermediate-representation-tree/source/ir_function.cpp \
	common/intermediate-representation-tree/source/ir_program.cpp \
	lexer/defs/lexer_defs.cpp \
	lexer/lexer.cpp \
	parser/source/token_consumer.cpp \
	parser/source/expression_parser.cpp \
	parser/source/statement_parser.cpp \
	parser/source/function_parser.cpp \
	parser/source/directive_parser.cpp \
	parser/source/parser.cpp \
	symbol-handling/symbol-table/symbol_table.cpp \
	symbol-handling/scope-manager/scope_manager.cpp \
	thread-pool/thread_pool.cpp \
	analyzer/source/expression_analyzer.cpp \
	analyzer/source/statement_analyzer.cpp \
	analyzer/source/function_analyzer.cpp \
	analyzer/source/directive_analyzer.cpp \
	analyzer/source/analyzer.cpp \
	optimization/source/dead_code.cpp \
	optimization/source/stack_memory.cpp \
	intermediate-representation/source/expression_intermediate_representation.cpp \
	intermediate-representation/source/statement_intermediate_representation.cpp \
	intermediate-representation/source/function_intermediate_representation.cpp \
	intermediate-representation/source/directive_intermediate_representation.cpp \
	intermediate-representation/source/intermediate_representation.cpp \
	code-generator/defs/code_generator_defs.cpp \
	code-generator/asm-generator/source/asm_instruction_generator.cpp \
	code-generator/code-generator/source/expression_code_generator.cpp \
	code-generator/code-generator/source/statement_code_generator.cpp \
	code-generator/code-generator/source/function_code_generator.cpp \
	code-generator/code-generator/source/code_generator.cpp \
	compiler/compiler.cpp

# Assembly files
ASM_SRCS = libmcpp/libio/libio.s

# Object files (derived from the source files)
OBJS = $(SRCS:.cpp=.o)

# Assembly object files
ASM_OBJS = $(ASM_SRCS:.s=.o)

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
TEST_DEPS = $(filter-out main.o, $(OBJS) $(ASM_OBJS))

# Phony targets
.PHONY: all clean distclean test run

# Default target
all: $(EXEC)

# Rule to build main executable
$(EXEC): $(OBJS) $(ASM_OBJS)
	$(CXX) $(OBJS) $(ASM_OBJS) -o $@ $(SANITIZER)

# Generic compilation rule with dependency generation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZER) -MMD -MP -c $< -o $@

# Assembly compilation rule
%.o: %.s
	$(AS) -c $< -o $@

# Run the program with input
run: $(EXEC)
	./$(EXEC) testfile.mcpp

# Run all unit tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Build test executable by linking test + required source objects
$(TEST_EXEC): $(TEST_OBJS) $(TEST_DEPS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SANITIZER) $(GTEST_LIBS)

# Clean up all binaries and object files
clean:
	rm -f $(OBJS) $(ASM_OBJS) $(EXEC) $(TEST_OBJS) $(TEST_EXEC)

# Clean up dependencies
distclean: clean
	rm -f $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# Include dependency files
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
