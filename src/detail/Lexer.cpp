#include <stdexcept>
#include "Lexer.h"
#include "detail/Escape.h"

namespace detail::lexer {
    std::string Lexer::getString(Lexer::iterator &it) const {
        const auto pos = findStringEnd();
        if (pos == m_pos) {
            throw std::runtime_error(std::format("in {} expect string", message()));
        }
        auto result = std::string(m_pos, pos);
        it = pos;
        return result;
    }

    std::int64_t Lexer::getInteger() {
        iterator it;
        const auto res = std::strtol(&(*m_pos), (char **) &(it), 0);
        m_pos = it;
        return res;
    }

    std::size_t Lexer::getUinteger() {
        iterator it;
        const auto res = std::strtoul(&(*m_pos), (char **) &(it), 0);
        m_pos = it;
        return res;
    }

    bool Lexer::isDelimiter(Lexer::iterator pos) const {
        return pos == m_end || isSpace(pos)
               || *pos == ')'
               || *pos == '('
               || *pos == '\0'
               || *pos == '"'
               || *pos == ';'
               || *pos == '/'
               || *pos == '['
               || *pos == ']'
               || *pos == '{'
               || *pos == '}'
               || *pos == '='
               || *pos == ',';
    }

    Lexer::iterator Lexer::findStringEnd() const noexcept {
        auto pos = m_pos;
        while (true) {
            if (isDelimiter(pos)) {
                break;
            }
            pos += 1;
        }
        return pos;
    }

    Lexer::iterator Lexer::findLiteralEnd() const {
        auto i = m_pos + 1;
        for (; i != m_end; ++i) {
            if (*i == '\\') {
                i++;
                continue;
            } else if (*i == '"') {
                return i;
            }
            if (*i == '\n') {
                throw std::runtime_error(std::format("in {} unexpected newline in string literal", message()));
            }
        }
        throw std::runtime_error(std::format("in {} expect '\"' but found end", message()));
    }

    std::string Lexer::getStringLiteral() {
        assert(checkStringLiteral());
        auto end = findLiteralEnd();
        std::string result = escape::escapeUnwrap({m_pos + 1, end});
        m_pos = end + 1;
        return result;
    }

    bool Lexer::checkDigits(iterator it) const {
        while (std::isdigit(*it)) {
            it++;
        }
        return isDelimiter(it);
    }

    bool Lexer::isUinteger() const {
        return checkDigits(m_pos);
    }

    bool Lexer::isInteger() const {
        auto p = m_pos;
        if (*p == '-') {
            p++;
        }
        return checkDigits(p);
    }

    bool Lexer::isKeyword(const char *keyword) {
        iterator tmp;
        if (getString(tmp) == keyword) {
            m_pos = tmp;
            return true;
        } else {
            return false;
        }
    }
}