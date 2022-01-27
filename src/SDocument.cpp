#include <stack>
#include <stdexcept>

#include "SDocument.h"
#include <format>
#include <fstream>

SDocument SDocument::parse(std::string_view str) {
    using namespace detail::lexer;
    using namespace detail;

    std::stack<Sexpression> sexprstack{};
    std::vector<Sexpression> roots;

    Lexer lex(str);
    while (!lex.eof()) {
        lex.skipSpaces();
        if (lex.eof()) {
            break;
        }
        if (lex.is<Tok::OPEN_PAREN>()) {
            lex.skipSpaces();
            if (!lex.is<Tok::STRING>()) {
                throw std::runtime_error(std::format("\"in {} expect s-expression name\"", lex.message()));
            }
            sexprstack.push(Sexpression::make(lex.peek<Tok::STRING>()));

        } else if (lex.is<Tok::CLOSE_PAREN>()) {
            if (sexprstack.empty()) {
                throw std::runtime_error(std::format("in {} a lot ')' detected", lex.message()));
            }
            auto topsexp = std::move(sexprstack.top());
            sexprstack.pop();
            if (sexprstack.empty()) {
                roots.push_back(std::move(topsexp));
            } else {
                auto &top = sexprstack.top();
                top.addChild(std::move(topsexp));
            }

        } else if (lex.is<Tok::STRING_LITERAL>()) {
            sexprstack.top().addChild(lex.peek<Tok::STRING_LITERAL>());

        } else if (lex.is<Tok::SEMICOLON>()) {
            lex.skipComment();

        } else if (lex.is<Tok::STRING>()) {
            if (sexprstack.empty()) {
                throw std::runtime_error(std::format("in {} line expect '('", lex.message()));
            }
            auto &top = sexprstack.top();
            top.addChild(Sexpression::makeFromStr(lex.peek<Tok::STRING>()));

        } else {
            throw std::runtime_error(std::format("in {} parse error", lex.message()));
        }
    }
    return SDocument(std::move(roots));
}

std::string SDocument::toString()  {
    std::ostringstream stream{};
    for (auto i = m_sexp.begin(); i != m_sexp.end(); i++) {
        stream << i->toString();
        if (i != m_sexp.end() - 1) {
            stream << ' ';
        }
    }
    return stream.str();
}

Sexpression &SDocument::operator[](std::string_view basename) noexcept {
    const auto ch = findChild(basename);
    if (ch == end()) {
        return addChild(Sexpression::make(std::string(basename)));
    } else {
        return *ch;
    }
}

SDocument SDocument::load(std::filesystem::path& string) {
    std::ifstream istream;
    istream.open(string);
    if (!istream.is_open()) {
        throw std::runtime_error(std::format("File {} wasn't opened", string.string()));
    }
    std::ostringstream sstr;
    sstr << istream.rdbuf();
    return parse(sstr.view());
}

std::string SDocument::dump() {
    std::ostringstream stream{};
    for (auto & i : m_sexp) {
        stream << i.dump() << std::endl;
    }
    return stream.str();
}
