#include <stdexcept>
#include "Lexer.h"

std::string Lexer::getString() {
    assert(checkString());

    const auto pos = findStringEnd();
    if (pos == m_pos) {
        throw std::runtime_error(format("in %s expect string", message().c_str()));
    }
    auto result = std::string(m_pos, pos);
    m_pos = pos;
    return result;
}

std::string::const_iterator Lexer::findStringEnd() const noexcept {
    auto pos = m_pos;
    while (true) {
        if (std::isspace(*pos)
            || *pos == ')'
            || *pos == '('
            || *pos == '\0'
            || *pos == '"'
            || *pos == ';'
            || *pos == '/') {
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

std::string::const_iterator Lexer::findLiteralEnd() const {
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
