# Minicpp
Mini compiler written in C++23, targeting x86_64 architecture.

### Getting Started

#### Prerequisites
- **Clang** (any version compatible with C++23)
- `make`
- **Pthread Support** (the build uses the -pthread flag)

#### Unit Tests (optional)
- **Google Test (gtest)** for running unit tests.

#### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/rdkc4/minicpp.git
    cd minicpp
    ```
2. Build the project:
    ```bash
    make [-j$(nproc)]
    ```

#### Usage
To compile a source file, run:
```bash
./minicpp <source-file> [-o <output-file>] [--dump-ast --dump-ir -s]
```

Where:
- <source-file> - file you want to compile (e.g. testfile.mcpp)
- -o <output-file> - output file name, no extension (optional, defaults to "output" if not provided)
- --dump-ast - dumps the structure of the abstract syntax tree (optional)
- --dump-ir - dumps the structure of the intermediate representation (optional)
- -s - stop compilation after generating .s file

#### Unit Tests
Running the tests:
```bash
make test [-j$(nproc)]
```