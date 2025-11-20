# Minicpp
Mini C compiler written in C++23, targeting x86_64 architecture.

### Getting Started

#### Prerequisites
- **Clang** (any version compatible with C++23)
- `make`
- **Pthread Support** (the build uses the -pthread flag)

#### Optional (for running unit tests)
- **Google Test (gtest)** for running unit tests.

#### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/rdkc4/minicpp.git
    cd minicpp
    ```
2. Build the project:
    ```bash
    make
    ```
    or (recommended, to use all available cores for a faster build):
    ```bash
    make -j$(nproc)
    ```

#### Usage
To compile a source file, run:
```bash
./minicpp <source-file> # Default output file: output.s
```
Alternatively, you can specify a custom output file:
```bash
./minicpp <source-file> <output-file>
```
Where:
- <source-file> - file you want to compile (e.g. testfile.txt)
- <output-file> - optional output file name (defaults to output.s if not provided)

Creating an executable:
```bash
# assemble to object file
as output.s -o output.o
# link to create executable
ld output.o -o executable
```

Running the executable:
```bash
# run the executable
./executable
# print return value
echo $?
```

#### Unit Tests (optional)
Running the tests:
```bash
make test
```