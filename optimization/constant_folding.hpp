#include "optimization.hpp"

#include <cassert>
#include <string>
#include <format>

#include "../common/defs/defs.hpp"

namespace Optimization {

    namespace ConstantFolding {

        template<typename T>
        struct MergeResult {
            T result;
            std::string error = "";
        };

        template<typename T>
        MergeResult<T> mergeValues(T l, T r, Operators op, size_t line, size_t column){
            MergeResult<T> constFold;
            switch(op){
                case Operators::ADD:
                    constFold.result = l + r;
                    return constFold;
                case Operators::SUB:
                    constFold.result = l - r;
                    return constFold;
                case Operators::MUL:
                    constFold.result = l * r;
                    return constFold;
                case Operators::DIV:
                    if(r == 0){
                        constFold.result = 0;
                        constFold.error = std::format("Line {}, Column {}: SEMANTIC ERROR -> division by ZERO", line, column);
                        return constFold;
                    }
                    constFold.result = l / r;
                    return constFold;
                case Operators::ANDB:
                    constFold.result = l & r;
                    return constFold;
                case Operators::ORB:
                    constFold.result = l | r;
                    return constFold;
                case Operators::XOR:
                    constFold.result = l ^ r;
                    return constFold;
                case Operators::LSHIFT:
                    constFold.result = l << r;
                    return constFold;
                case Operators::RSHIFT:
                    constFold.result = l >> r;
                    return constFold;
                case Operators::GREATER:
                    constFold.result = static_cast<T>(l > r);
                    return constFold;
                case Operators::LESS:
                    constFold.result = static_cast<T>(l < r);
                    return constFold;
                case Operators::GEQUAL:
                    constFold.result = static_cast<T>(l >= r);
                    return constFold;
                case Operators::LEQUAL:
                    constFold.result = static_cast<T>(l <= r);
                    return constFold;
                case Operators::EQUAL:
                    constFold.result = static_cast<T>(l == r);
                    return constFold;
                case Operators::NEQUAL:
                    constFold.result = static_cast<T>(l != r);
                    return constFold;
                default:
                    assert(false && "unreachable");
            }
        }
    }
};