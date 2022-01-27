#include <format>
#include <algorithm>
#include "Escape.h"

namespace detail::escape {

    static const std::array<char, 11> escape_chars = {
            '\'',
            '"',
            '?',
            '\\',
            'a',
            'b',
            'f',
            'n',
            'r',
            't',
            'v'
    };
    static const std::array<char, 11> escape_vals = {
            '\'',
            '"',
            '\?',
            '\\',
            '\a',
            '\b',
            '\f',
            '\n',
            '\r',
            '\t',
            '\v'
    };

    static bool isEscapeValue(char c) {
        return std::find(escape_vals.begin(), escape_vals.end(), c) != escape_vals.end();
    }

    static std::size_t countEscapeValues(std::string_view str) {
        return std::count_if(str.begin(), str.end(), isEscapeValue);
    }

    std::string escapeWrap(std::string_view s) {
        if (s.empty()) {
            return "\"\"";
        }
        //Todo testing it
        if ((std::find(s.begin(), s.end(), ' ') == s.end()) && countEscapeValues(s) == 0) {
            return std::string(s);
        }
        return std::format("\"{}\"", std::string(s));
    }

    char isValidEscape(char ch) {
        auto pos = std::find(escape_chars.begin(), escape_chars.end(), ch);
        if (pos == escape_chars.end()) {
            throw std::runtime_error(std::format("invalid escape char {}'\'", ch));
        }
        return escape_vals[pos - escape_chars.begin()];
    }

    std::string escapeUnwrap(std::string_view s) {
        std::string result{};
        for (auto pos = s.begin(); pos != s.end(); pos++) {
            if (*pos == '\\') {
                pos++;
                if (pos == s.end()) {
                    throw std::runtime_error("unfinished escape sequence at the end of the string");
                }
                result.push_back(isValidEscape(*pos));
            } else {
                result.push_back(*pos);
            }
        }
        return result;
    }
}