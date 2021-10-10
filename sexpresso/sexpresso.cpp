#include "sexpresso.hpp"
#include <vector>
#include <string>
#include <cctype>
#include <stack>
#include <algorithm>
#include <sstream>
#include <array>
#include <iostream>
#include <cassert>

#define shouldNeverReachHere() fprintf(stderr, "Error: Should never reach here " __FILE__ ":%d\n", __LINE__)

//todo todo
static const std::array<char, 11> escape_chars = {'\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};
static const std::array<char, 11> escape_vals = {'\'', '"', '\?', '\\', '\a', '\b', '\f', '\n', '\r', '\t', '\v'};

static std::vector<std::string> splitPathString(const std::string &path) {
    std::vector<std::string> paths{};
    if (path.empty()) {
        return paths;
    }
    auto start = path.begin();
    for (auto i = path.begin() + 1; i != path.end(); i++) {
        if (*i == '/') {
            paths.emplace_back(start, i);
            start = i + 1;
        }
    }
    paths.emplace_back(start, path.end());
    return paths;
}

static bool isEscapeValue(char c) {
    return std::find(escape_vals.begin(), escape_vals.end(), c) != escape_vals.end();
}

static std::size_t countEscapeValues(const std::string &str) {
    return std::count_if(str.begin(), str.end(), isEscapeValue);
}

static std::string escape(const std::string &str) {
    auto escape_count = countEscapeValues(str);
    if (escape_count == 0) {
        return str;
    }
    std::string result_str{};
    result_str.reserve(str.size() + escape_count);
    for (auto c: str) {
        auto loc = std::find(escape_vals.begin(), escape_vals.end(), c);
        if (loc == escape_vals.end()) {
            result_str.push_back(c);
        } else {
            result_str.push_back('\\');
            result_str.push_back(escape_chars[loc - escape_vals.begin()]);
        }
    }
    return result_str;
}

std::size_t Sexpression::childCount() const {
    switch (m_kind) {
        case SexpValueKind::SEXP:
            return m_sexp.size();
        case SexpValueKind::STRING:
            return 1;
    }
    shouldNeverReachHere();
    return 0;
}

Sexpression *Sexpression::findChild(const std::string &name) {
    auto findPred = [&name](Sexpression &s) {
        return s.m_str == name;
    };


    auto loc = std::find_if(m_sexp.begin(), m_sexp.end(), findPred);
    if (loc == m_sexp.end()) {
        return nullptr;
    } else {
        return loc.base();
    }
}

Sexpression *Sexpression::getChild(const std::string &path) {
    if (m_kind == SexpValueKind::STRING) {
        return nullptr;
    }

    auto *curr = this;
    for (auto &p: splitPathString(path)) {
        auto *ptr = curr->findChild(p);
        if (ptr != nullptr) {
            curr = ptr;
        } else {
            return nullptr;
        }
    }
    return curr;
}

Sexpression &Sexpression::createPath(const std::vector<std::string> &path) {
    auto el = this;
    auto pc = path.begin();
    for (; pc != path.end(); ++pc) {
        auto nxt = el->findChild(*pc);
        if (nxt == nullptr) {
            break;
        } else {
            el = nxt;
        }
    }
    for (; pc != path.end(); ++pc) {
        el->addChild(Sexpression(SexpValueKind::SEXP, *pc));
        el = &(el->getChild(el->childCount() - 1));
    }
    return *el;
}

Sexpression &Sexpression::createPath(const std::string &path) {
    return createPath(splitPathString(path));
}

static std::string stringValToString(const std::string &s) {
    if (s.empty()) {
        return "\"\"";
    }
    if ((std::find(s.begin(), s.end(), ' ') == s.end()) && countEscapeValues(s) == 0) {
        return s;
    }
    return std::string("\"") + escape(s) + "\"";
}

std::string Sexpression::toString() const {
    std::ostringstream ostream{};
    switch (m_kind) {
        case SexpValueKind::STRING:
            return stringValToString(m_str);
        case SexpValueKind::SEXP: {
            ostream << '(' << m_str;
            if (!m_sexp.empty()) {
                ostream << ' ';
            }
            for (auto i = m_sexp.begin(); i != m_sexp.end(); i++) {
                ostream << i->toString();
                if (i != m_sexp.end() - 1) {
                    ostream << ' ';
                }
            }
            ostream << ')';
        }
    }
    return ostream.str();
}

bool Sexpression::operator==(const Sexpression &other) const {
    if (m_kind != other.m_kind) {
        return false;
    }
    switch (m_kind) {
        case SexpValueKind::SEXP: {
            if (m_str != other.m_str || m_sexp.size() != other.m_sexp.size()) {
                return false;
            }
            for (std::size_t i = 0; i < m_sexp.size(); ++i) {
                if (m_sexp[i] != other.m_sexp[i]) {
                    return false;
                }
            }
            return true;
        }

        case SexpValueKind::STRING:
            return m_str == other.m_str;
    }
    shouldNeverReachHere();
    return false;
}

Sexpression Sexpression::makeFromStr(const std::string &string) {
    return {SexpValueKind::STRING, escape(string)};
}

//Sexpression Sexpression::parse(const std::string &str) {
//    std::stack<Sexpression> sexprstack{};
//    Lexer lex(str);
//    if (lex.peek() != '(') {
//        throw std::runtime_error(format("in %d line expect '('", lex.lines()));
//    }
//    while (!lex.eof()) {
//        lex.skipSpaces();
//        switch (lex.peek()) {
//            case '(': {
//                lex.get();
//                Sexpression el = makeFromStr(lex.getString());
//                el.m_kind = SexpValueKind::SEXP;
//                sexprstack.push(std::move(el));
//                break;
//            }
//            case ')': {
//                lex.get();
//                auto topsexp = std::move(sexprstack.top());
//                sexprstack.pop();
//                if (sexprstack.empty()) {
//                    return topsexp;
//                }
//                auto &top = sexprstack.top();
//                top.addChild(std::move(topsexp));
//                break;
//            }
//            case '\"': {
//                sexprstack.top().addChild(lex.getStringLiteral());
//                break;
//            }
//            case ';': {
//                lex.get();
//                lex.skipComment();
//                break;
//            }
//            default: {
//                auto &top = sexprstack.top();
//                top.addChild(makeFromStr(escape(lex.getString())));
//            }
//        }
//    }
//    if (sexprstack.size() != 1) {
//        throw std::runtime_error("not enough s-expressions were closed by the end of parsing");
//    } else {
//        return std::move(sexprstack.top());
//    }
//}
