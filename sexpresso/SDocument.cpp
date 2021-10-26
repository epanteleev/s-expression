#include <stack>
#include <stdexcept>
#include <cassert>
#include <array>
#include <sstream>
#include "SDocument.h"
#include "Lexer.h"


SDocument SDocument::parse(const std::string &str) {
    std::stack<Sexpression> sexprstack{};
    std::vector<Sexpression> roots;

    Lexer lex(str);
    if (!lex.is<Tok::OPEN_PAREN>()) {
        throw std::runtime_error(format("in %d line expect '('", lex.message().c_str()));
    }
    while (!lex.eof()) {
        lex.skipSpaces();
        if (lex.is<Tok::OPEN_PAREN>()) {
            lex.get();
            lex.skipSpaces();
            if (!lex.is<Tok::STRING>()) {
                throw std::runtime_error(format("in %s expect s-expression name", lex.message().c_str()));
            }
            sexprstack.push(Sexpression::make(lex.peek<Tok::STRING>()));

        } else if (lex.is<Tok::CLOSE_PAREN>()) {
            lex.get();
            if (sexprstack.empty()) {
                throw std::runtime_error(format("in %d a lot ')' detected", lex.message().c_str()));
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
            lex.get();
            lex.skipComment();

        } else if (lex.is<Tok::STRING>()) {
            auto &top = sexprstack.top();
            top.addChild(Sexpression::makeFromStr(lex.peek<Tok::STRING>()));

        } else {
            throw std::runtime_error(format("in %s parse error", lex.message().c_str()));
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

Sexpression *SDocument::operator[](std::string_view name) noexcept {
    for (auto& i: m_sexp) {
        if (i.getString() == name) {
            return &i;
        }
    }
    return nullptr;
}
