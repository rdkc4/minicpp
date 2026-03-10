#ifndef INDENT_GUARD_HPP
#define INDENT_GUARD_HPP

/**
 * @struct IndentGuard
 * @brief scope-based indentation control
*/
struct IndentGuard {
public:
    /**
     * @brief creates the instance of the indent guard and increments the indentation
     * @param indent - reference to indent
    */
    IndentGuard(int& indent) : indent{ indent } {
        ++indent;
    }

    /**
     * @brief deletes the indent guard instance and decrements the indentation
    */
    ~IndentGuard(){
        --indent;
    }

private:
    /// reference to indent
    int& indent;

};

#endif