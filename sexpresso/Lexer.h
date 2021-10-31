#pragma once

#include <string>
#include <array>
#include <cassert>
#include <vector>
#include "common.h"

enum class Tok: char {
    STRING = 0,
    INTEGER,
    OPEN_PAREN = '(',
    CLOSE_PAREN = ')',
    OPEN_SQUARE_BRACKET = '[',
    CLOSE_SQUARE_BRACKET = ']',
    STRING_LITERAL,
    DOLLAR = '$',
    SEMICOLON = ';',
    SLASH = '/',
    EQUALITY = '=',
    // Keywords
    NAME,
    INDEX,
};

namespace keyword {
    static const char* NAME = "name";
    static const char* INDEX = "idx";
}

static const std::array<char, 11> escape_chars = {'\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};
static const std::array<char, 11> escape_vals = {'\'', '"', '\?', '\\', '\a', '\b', '\f', '\n', '\r', '\t', '\v'};

class Lexer final {
public:
    using iterator = std::string::iterator;
    using const_iterator = std::string::const_iterator;
public:
    explicit Lexer(std::string_view string) :
            m_pos(string.begin()),
            m_end(string.end()),
            m_lineBegin(string.begin()) {}

private:

    [[nodiscard]]
    const_iterator findStringEnd() const noexcept;

    std::string getString(const_iterator& it) const;

    std::int64_t getInteger();

    [[nodiscard]]
    bool isInteger() const {
        auto p = m_pos;
        if (*p == '-') {
            p++;
        }
        while(std::isdigit(*p)) {
            p++;
        }
        return isDelimiter(p);
    }

    [[nodiscard]]
    const_iterator findLiteralEnd() const;

    static char isValidEscape(char ch);

    inline bool checkStringLiteral() {
        return *m_pos == '"';
    }

    std::string getStringLiteral();

    inline void get() noexcept {
        m_pos++;
    }

    inline bool isKeyword(const char* keyword) {
        const_iterator tmp;
        if (getString(tmp) == keyword) {
            m_pos = tmp;
            return true;
        } else {
            return false;
        }
    }

    static bool isDelimiter(const_iterator pos);

public:
    template<Tok t>
    [[nodiscard]]
    inline bool is() noexcept {
        if (*m_pos == static_cast<char>(t)) {
            get();
            return true;
        } else {
            return false;
        }
    }

    template<Tok t>
    requires (t == Tok::STRING)
    [[nodiscard]]
    inline bool is() noexcept {
        return std::isalnum(*m_pos);
    }

    template<Tok t>
    requires (t == Tok::INTEGER)
    [[nodiscard]]
    inline bool is() noexcept {
        return isInteger();
    }

    template<Tok t>
    requires (t == Tok::STRING_LITERAL)
    [[nodiscard]]
    inline bool is() noexcept {
        return *m_pos == '"';
    }

    template<Tok t>
    requires (t == Tok::NAME)
    [[nodiscard]]
    inline bool is() noexcept {
        return isKeyword(keyword::NAME);
    }

    template<Tok t>
    requires (t == Tok::INDEX)
    [[nodiscard]]
    inline bool is() noexcept {
        return isKeyword(keyword::INDEX);
    }

    template<Tok t>
    requires (t == Tok::STRING)
    inline decltype(auto) peek() {
        assert(is<Tok::STRING>());
        return getString(m_pos);
    }

    template<Tok t>
    requires (t == Tok::INTEGER)
    inline decltype(auto) peek() {
        assert(is<Tok::INTEGER>());
        return getInteger();
    }

    template<Tok t>
    requires (t == Tok::STRING_LITERAL)
    inline decltype(auto) peek() {
        assert(is<Tok::STRING_LITERAL>());
        return getStringLiteral();
    }

    inline void skipSpaces() {
        auto pos = m_pos;
        while (std::isspace(*pos)) {
            if (*pos == '\n') {
                m_lineCount++;
                m_lineBegin = pos + 1;
            }
            pos++;
        }
        m_pos = pos;
    }

    void skipComment() {
        while (*m_pos != '\n') {
            m_pos++;
        }
        m_lineCount++;
        m_pos++;
        m_lineBegin = m_pos;
    }

    [[nodiscard]]
    inline bool eof() const noexcept {
        return m_pos == m_end;
    }

    [[nodiscard]]
    inline std::size_t lines() const noexcept {
        return m_lineCount;
    }

    [[nodiscard]]
    inline std::size_t pos() const noexcept {
        return m_pos - m_lineBegin;
    }

    [[nodiscard]]
    std::string message() const noexcept {
        return format("%d:%d '%s'", lines(), pos(), std::string(m_pos, m_end).c_str());
    }

private:
    const_iterator m_pos;
    const_iterator m_end;
    std::size_t m_lineCount{};
    const_iterator m_lineBegin{};
};

