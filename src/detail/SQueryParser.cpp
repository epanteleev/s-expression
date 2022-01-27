#include <stdexcept>
#include "SQueryParser.h"

namespace detail::parser {

    std::vector<std::string> SQueryParser::parseRelativePath() {
        using namespace lexer;
        std::vector<std::string> paths;

        while (!lex.eof()) {
            if (lex.is<Tok::STRING>()) {
                paths.emplace_back(lex.peek<Tok::STRING>());
            } else {
                throw std::runtime_error(std::format("in {} expect string", lex.message()));
            }
            if (!lex.is<Tok::SLASH>()) {
                break;
            }
        }

        return paths;
    }

    std::tuple<std::size_t, std::size_t> SQueryParser::parseRange() {
        using namespace lexer;
        std::size_t begin{};
        std::size_t end = std::numeric_limits<std::size_t>::max();

        if (!lex.is<Tok::OPEN_BRACE>()) {
            throw std::runtime_error(std::format("in {} expect '{'", lex.message()));
        }
        lex.skipSpaces();
        if (lex.is<Tok::UINTEGER>()) {
            begin = lex.peek<Tok::UINTEGER>();
        } else {
            throw std::runtime_error(std::format("in {} expect unsigned integer", lex.message()));
        }
        lex.skipSpaces();

        if (lex.is<Tok::COMMA>()) {
            lex.skipSpaces();
            if (lex.is<Tok::UINTEGER>()) {
                end = lex.peek<Tok::UINTEGER>();
            } else {
                throw std::runtime_error(std::format("in {} expect unsigned integer", lex.message()));
            }
            lex.skipSpaces();
        }

        if (!lex.is<Tok::CLOSE_BRACE>()) {
            throw std::runtime_error(std::format("in {} expect '}'", lex.message()));
        }
        return std::make_tuple(begin, end);
    }

    command::SQuery::Pointer SQueryParser::parseFilter() {
        using namespace lexer;
        using namespace command;
        lex.skipSpaces();
        if (!lex.is<Tok::DOLLAR>()) {
            throw std::runtime_error(std::format("in {} expect '$'", lex.message()));
        }

        if (lex.is<Tok::RANGE>() && lex.skipSpaces() && lex.is<Tok::EQUALITY>()) { // $range={<num> ?(, <num>)}
            lex.skipSpaces();
            auto range = parseRange();
            return std::make_unique<SQueryFilter>(SQueryFilter::FilterType::RANGE, std::move(range));

        } else if (lex.is<Tok::INDEX>() && lex.skipSpaces() && lex.is<Tok::EQUALITY>()) { // $idx=<index>
            lex.skipSpaces();
            if (lex.is<Tok::INTEGER>()) {
                return std::make_unique<SQueryFilter>(SQueryFilter::FilterType::INDEX, lex.peek<Tok::INTEGER>());
            } else {
                throw std::runtime_error(std::format("in {} expect integer", lex.message()));
            }

        } else {
            throw std::runtime_error(std::format("in {} undefined command", lex.message()));
        }
    }

    command::SQuery::Pointer SQueryParser::apply() {
        using namespace lexer;
        using namespace command;

        auto commandList = std::make_unique<SQueryCombination>();
        while(!lex.eof()) {
            if (lex.is<Tok::SLASH>()) {
                auto paths = parseRelativePath();
                commandList->append(std::make_unique<SQueryExpressionByAbsolutePath>(std::move(paths)));

            } else if (lex.is<Tok::STRING>()) {
                auto paths = parseRelativePath();
                commandList->append(std::make_unique<SQueryExpressionByRelativePath>(std::move(paths)));

            } else if (lex.is<Tok::OPEN_SQUARE_BRACKET>()) {
                auto command = parseFilter();
                lex.skipSpaces();
                if (!lex.is<Tok::CLOSE_SQUARE_BRACKET>()) {
                    throw std::runtime_error(std::format("in {} expect ']'", lex.message()));
                }
                commandList->append(std::move(command));
            } else {
                throw std::runtime_error(std::format("in {} incorrect query", lex.message()));
            }
            lex.skipSpaces();
        }
        return commandList;
    }
}
