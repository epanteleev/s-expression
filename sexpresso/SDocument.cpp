#include <stack>
#include <stdexcept>
#include <cassert>
#include <array>
#include <sstream>
#include "SDocument.h"

static const std::array<char, 11> escape_chars = {'\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};
static const std::array<char, 11> escape_vals = {'\'', '"', '\?', '\\', '\a', '\b', '\f', '\n', '\r', '\t', '\v'};

template<typename ... Args>
std::string format(const std::string &format, Args ... args) noexcept {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...);
    assert(size_s > 0);
    auto size = static_cast<std::size_t>(size_s) + 1;
    std::vector<char> buf(size);
    std::snprintf(buf.data(), size, format.c_str(), args ...);
    return std::string{buf.begin(), buf.end()};
}

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
        auto symend = m_pos;
        while (true) {
            if (std::isspace(*symend) || *symend == ')' || *symend == '(' || *symend == '\0') {
                break;
            }
            symend += 1;
        }
        if (symend == m_pos) {
            throw std::runtime_error(format("in line %d. Expect string", lines()));
        }
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
                throw std::runtime_error("unexpected newline in string literal");
            }
        }
        return m_end;
    }

    static char isValidEscape(char ch) {
        auto pos = std::find(escape_chars.begin(), escape_chars.end(), ch);
        if (pos == escape_chars.end()) {
            throw std::runtime_error(format("invalid escape char %c'\'", ch));
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
                    throw std::runtime_error("unfinished escape sequence at the end of the string");
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

SDocument SDocument::parse(const std::string &str) {
    std::stack<Sexpression> sexprstack{};
    std::vector<Sexpression> roots;

    Lexer lex(str);
    if (lex.peek() != '(') {
        throw std::runtime_error(format("in %d line expect '('", lex.lines()));
    }
    while (!lex.eof()) {
        lex.skipSpaces();
        switch (lex.peek()) {
            case '(': {
                lex.get();
                sexprstack.push(Sexpression::make(lex.getString()));
                break;
            }
            case ')': {
                lex.get();
                if (sexprstack.empty()) {
                    throw std::runtime_error(format("in line %d. A lot ')' detected", lex.lines()));
                }
                auto topsexp = std::move(sexprstack.top());
                sexprstack.pop();
                if (sexprstack.empty()) {
                    roots.push_back(std::move(topsexp));
                } else {
                    auto &top = sexprstack.top();
                    top.addChild(std::move(topsexp));
                }
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
                top.addChild(Sexpression::makeFromStr(lex.getString()));
            }
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
