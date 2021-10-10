#include "sexpresso.hpp"
#include <vector>
#include <string>
#include <cctype>
#include <stack>
#include <algorithm>
#include <sstream>
#include <array>
#include <iostream>
#include <fmt/core.h>

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

static void shouldNeverReachHere() {
    fmt::print(stderr, "Error: Should never reach here {}:{}\n", __FILE__, __LINE__);
}

void Sexpression::addExpression(const std::string &string) {
    auto exp = parse(string);
    for (auto &&c: exp.m_sexp) {
        addChild(std::move(c));
    }
}

std::size_t Sexpression::childCount() const {
    switch (this->m_kind) {
        case SexpValueKind::SEXP:
            return this->m_sexp.size();
        case SexpValueKind::STRING:
            return 1;
    }
    shouldNeverReachHere();
    return 0;
}

Sexpression* Sexpression::findChild(const std::string &name) {
    auto findPred = [&name](Sexpression &s) {
        switch (s.m_kind) {
            case SexpValueKind::SEXP: {
                if (s.childCount() == 0) {
                    return false;
                }
                auto &hd = s.getChild(0);
                switch (hd.m_kind) {
                    case SexpValueKind::SEXP:
                        return false;
                    case SexpValueKind::STRING:
                        return hd.getString() == name;
                }
                break;
            }
            case SexpValueKind::STRING:
                return s.getString() == name;
        }
        shouldNeverReachHere();
        return false;
    };
    auto loc = std::find_if(m_sexp.begin(), m_sexp.end(), findPred);
    if (loc == m_sexp.end()) {
        return nullptr;
    } else {
        return &(*loc);
    }
}

Sexpression *Sexpression::getChild(const std::string& path) {
    if (m_kind == SexpValueKind::STRING) {
        return nullptr;
    }

    auto *curr = this;
    for (auto &path: splitPathString(path)) {
        auto *ptr = curr->findChild(path);
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
        std::vector<Sexpression> v; //Todo
        v.emplace_back(Sexpression(*pc));
        el->addChild(std::move(v));
        el = &(el->getChild(el->childCount() - 1));
    }
    return *el;
}

Sexpression &Sexpression::createPath(const std::string &path) {
    const auto paths = splitPathString(path);
    return createPath(paths);
}

static std::string stringValToString(const std::string &s) {
    if (s.empty()) {
        return "\"\"";
    }
    if ((std::find(s.begin(), s.end(), ' ') == s.end()) && countEscapeValues(s) == 0) {
        return s;
    }
    return fmt::format("\"{}\"", escape(s));
}

static void toStringImpl(const Sexpression &sexp, std::ostringstream &ostream) {
    switch (sexp.m_kind) {
        case SexpValueKind::STRING:
            ostream << stringValToString(sexp.m_str);
            break;
        case SexpValueKind::SEXP:
            switch (sexp.m_sexp.size()) {
                case 0:
                    ostream << "()";
                    break;
                case 1:
                    ostream << '(';
                    toStringImpl(sexp.m_sexp[0], ostream);
                    ostream << ')';
                    break;
                default:
                    ostream << '(';
                    for (auto i = sexp.m_sexp.begin(); i != sexp.m_sexp.end(); ++i) {
                        toStringImpl(*i, ostream);
                        if (i != sexp.m_sexp.end() - 1){
                            ostream << ' ';
                        }
                    }
                    ostream << ')';
            }
    }
}

std::string Sexpression::toString() const {
    std::ostringstream ostream{};
    switch (m_kind) {
        case SexpValueKind::STRING:
            return stringValToString(m_str);
        case SexpValueKind::SEXP:
            for (auto i = m_sexp.begin(); i != m_sexp.end(); ++i) {
                toStringImpl(*i, ostream);
                if (i != m_sexp.end() - 1){
                    ostream << ' ';
                }
            }
    }
    return ostream.str();
}

static bool childrenEqual(const std::vector<Sexpression> &a, const std::vector<Sexpression> &b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool Sexpression::operator==(const Sexpression &other) const {
    if (m_kind != other.m_kind) {
        return false;
    }
    switch (m_kind) {
        case SexpValueKind::SEXP:
            return childrenEqual(m_sexp, other.m_sexp);
        case SexpValueKind::STRING:
            return m_str == other.m_str;
    }
    shouldNeverReachHere();
    return false;
}

Sexpression::Sexpression(const std::string &string) :
        m_kind(SexpValueKind::STRING),
        m_str(escape(string)) {}

class Lexer final {
public:
    explicit Lexer(const std::string &string) :
            m_pos(string.begin()),
            m_end(string.end()) {}

public:
    inline void skipSpaces() {
        while (std::isspace(*m_pos)) {
            if (*m_pos == '\n') {
                m_lineCount++;
            }
            m_pos++;
        }
    }

    void skipComment() {
        while (*m_pos != '\n') {
            m_pos++;
        }
        m_lineCount++;
        m_pos++;
    }

    inline char peek() noexcept {
        return *m_pos;
    }

    inline void get() noexcept {
        m_pos++;
    }

    std::string getString() {
        auto symend = std::find_if(m_pos, m_end,
                                   [](char const &c) {
                                       return std::isspace(c) || c == ')' || c == '(';
                                   }
        );
        auto result = std::string(m_pos, symend);
        m_pos = symend;
        return result;
    }

    [[nodiscard]]
    std::string::const_iterator getLiteralEnd() const {
        auto i = m_pos + 1;
        for (; i != m_end; ++i) {
            if (*i == '\\') {
                ++i;
                continue;
            }
            if (*i == '"') {
                return i;
            }
            if (*i == '\n') {
                throw std::runtime_error("Unexpected newline in string literal");
            }
        }
        return m_end;
    }

    static char isValidEscape(char ch) {
        auto pos = std::find(escape_chars.begin(), escape_chars.end(), ch);
        if (pos == escape_chars.end()) {
            throw std::runtime_error(std::string("invalid escape char '") + ch + '\''); //Todo std::format
        }
        return escape_vals[pos - escape_chars.begin()];
    }

    std::string getStringLiteral() {
        auto end = getLiteralEnd();
        std::string result{};
        for (m_pos = m_pos + 1; m_pos != end; m_pos++) {
            if (*m_pos == '\\') {
                m_pos++;
                if (m_pos == m_end) {
                    throw std::runtime_error("Unfinished escape sequence at the end of the string");
                }
                result.push_back(isValidEscape(*m_pos));
            } else {
                result.push_back(*m_pos);
            }
        }
        m_pos++;
        return result;
    }

    [[nodiscard]]
    inline bool eof() const noexcept {
        return m_pos == m_end;
    }

    [[nodiscard]]
    inline std::size_t lines() const noexcept {
        return m_lineCount;
    }

private:
    std::string::const_iterator m_pos;
    std::string::const_iterator m_end;
    std::size_t m_lineCount{};
};

Sexpression Sexpression::parse(const std::string &str) {
    std::stack<Sexpression> sexprstack{};
    sexprstack.push(Sexpression{}); // root
    Lexer lex(str);
    while (!lex.eof()) {
        lex.skipSpaces();
        switch (lex.peek()) {
            case '(': {
                lex.get();
                sexprstack.push(Sexpression{});
                break;
            }
            case ')': {
                lex.get();
                auto topsexp = std::move(sexprstack.top());
                sexprstack.pop();
                if (sexprstack.empty()) {
                    throw std::runtime_error(
                            fmt::format(
                                    "too many ')' characters detected in {}s, closing s-expression that don't exist.",
                                    lex.lines()));
                }
                auto &top = sexprstack.top();
                top.addChild(std::move(topsexp));
                break;
            }
            case '\"': {
                sexprstack.top().addChild(lex.getStringLiteral());
                break;
            }
            case ';': {
                lex.get();
                lex.skipComment();
                break;
            }
            default: {
                auto &top = sexprstack.top();
                top.addChild(Sexpression(lex.getString()));
            }
        }
    }
    if (sexprstack.size() != 1) {
        throw std::runtime_error("not enough s-expressions were closed by the end of parsing");
    } else {
        return std::move(sexprstack.top());
    }
}