#pragma once

#include <string>

namespace detail::escape {
    // Wrap param s as "s".
    // If s don't contains escape sequence, function will return s.
    std::string escapeWrap(std::string_view s);

    std::string escapeUnwrap(std::string_view s);
}

