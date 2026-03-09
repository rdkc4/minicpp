#ifndef INDENT_GUARD_HPP
#define INDENT_GUARD_HPP

struct IndentGuard {
public:
    IndentGuard(int& indent) : indent{ indent } {
        ++indent;
    }

    ~IndentGuard(){
        --indent;
    }

private:
    int& indent;

};

#endif