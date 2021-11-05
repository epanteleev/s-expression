#pragma once
#include "detail/Lexer.h"
#include "Sexpression.h"
#include "detail/SQuery.h"

namespace detail::parser {

    class SQueryParser final {
    public:
        SQueryParser(std::string_view query) : lex(query) {}

    public:
        command::SQuery::Pointer apply();

    private:
        std::vector<std::string> parseRelativePath();
        std::tuple<std::size_t, std::size_t> parseRange();

    private:
        lexer::Lexer lex;

        command::SQuery::Pointer parseFilter();
    };

}