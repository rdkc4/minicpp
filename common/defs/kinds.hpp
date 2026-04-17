#ifndef KINDS_HPP
#define KINDS_HPP

#include <cstddef>
#include <array>
#include <string_view>

namespace semantics {
    /** 
     * @enum Kind
     * @brief kinds of symbols
    */
    enum class Kind {
        NO_KIND = 0,   //< undefined kind, invalid state
        LIT,           //< literal constant
        FUN,           //< function
        VAR,           //< variable
        PAR            //< function parameter
    };

    /// number of kinds
    constexpr size_t KIND_COUNT{ static_cast<size_t>(Kind::PAR) + 1 };

    /// maps kinds to their string representations
    constexpr std::array<std::string_view, KIND_COUNT> kindStringRepresentations {
        [] {
            /**
             * @brief converts kind enum to array index
             * @param kind - element of the kind enum
             * @returns index in the array for provided kind
            */
            constexpr auto idx {
                [](Kind kind) noexcept -> size_t {
                    return static_cast<size_t>(kind);
                }
            };

            std::array<std::string_view, KIND_COUNT> kinds{};

            kinds[idx(Kind::NO_KIND)] = "NO_KIND";
            kinds[idx(Kind::LIT)]     = "LIT";
            kinds[idx(Kind::FUN)]     = "FUN";
            kinds[idx(Kind::VAR)]     = "VAR";
            kinds[idx(Kind::PAR)]     = "PAR";

            return kinds;
        }()
    };

    /**
     * @brief getter for the string representation of the kind
     * @param kind - kind of the token
     * @returns string representation of the kind
    */
    constexpr std::string_view kindToStr(Kind kind){
        return kindStringRepresentations[static_cast<size_t>(kind)];
    }

}

#endif