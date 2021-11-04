#include <stdexcept>
#include "Lexer.h"

std::string Lexer::getString(Lexer::const_iterator& it) const {
    const auto pos = findStringEnd();
    if (pos == m_pos) {
        throw std::runtime_error(format("in %s expect string", message().c_str()));
    }
    auto result = std::string(m_pos, pos);
    it = pos;
    return result;
}

std::int64_t Lexer::getInteger() {
    const_iterator it;
    const auto res = std::strtol(m_pos.base(), const_cast<char **>(&(it.base())), 0);
    m_pos = it;
    return res;
}

bool Lexer::isDelimiter(Lexer::const_iterator pos) {
    return std::isspace(*pos)
           || *pos == ')'
           || *pos == '('
           || *pos == '\0'
           || *pos == '"'
           || *pos == ';'
           || *pos == '/'
           || *pos == '['
           || *pos == ']'
           || *pos == '=';
}
Lexer::const_iterator Lexer::findStringEnd() const noexcept {
    auto pos = m_pos;
    while (true) {
        if (isDelimiter(pos)) {
            break;
        }
        pos += 1;
    }
    return pos;
}

char Lexer::isValidEscape(char ch) {
    auto pos = std::find(escape_chars.begin(), escape_chars.end(), ch);
    if (pos == escape_chars.end()) {
        throw std::runtime_error(format("invalid escape char %c'\'", ch));
    }
    return escape_vals[pos - escape_chars.begin()];
}

Lexer::const_iterator Lexer::findLiteralEnd() const {
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
            throw std::runtime_error(format("in %s unexpected newline in string literal", message().c_str()));
        }
    }
    return m_end;
}

std::string Lexer::getStringLiteral() {
    assert(checkStringLiteral());
    auto end = findLiteralEnd();
    std::string result{};
    for (m_pos = m_pos + 1; m_pos != end; m_pos++) {
        if (*m_pos == '\\') {
            m_pos++;
            if (m_pos == m_end) {
                throw std::runtime_error(format("in %s unfinished escape sequence at the end of the string",
                                                message().c_str()));
            }
            result.push_back(isValidEscape(*m_pos));
        } else {
            result.push_back(*m_pos);
        }
    }
    m_pos++;
    return result;
}

bool Lexer::isInteger() const {
    auto p = m_pos;
    if (*p == '-') {
        p++;
    }
    while(std::isdigit(*p)) {
        p++;
    }
    return isDelimiter(p);
}

bool Lexer::isKeyword(const char *keyword) {
    const_iterator tmp;
    if (getString(tmp) == keyword) {
        m_pos = tmp;
        return true;
    } else {
        return false;
    }
}
