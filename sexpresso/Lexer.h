#pragma once

#include <string>
#include <array>
#include <cassert>
#include <vector>
#include "common.h"

enum class Tok: char {
    STRING = 0,
    OPEN_PAREN = '(',
    CLOSE_PAREN = ')',
    STRING_LITERAL,
    SEMICOLON = ';',
    SLASH = '/'
};

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
    inline bool checkString() {
        return isalnum(*m_pos);
    }

    [[nodiscard]]
    const_iterator findStringEnd() const noexcept;

    std::string getString();

    [[nodiscard]]
    const_iterator findLiteralEnd() const;

    static char isValidEscape(char ch);

    inline bool checkStringLiteral() {
        return *m_pos == '"';
    }

    std::string getStringLiteral();

public:
    template<Tok t>
    [[nodiscard]]
    inline bool is() const noexcept {
        return *m_pos == static_cast<char>(t);
    }

    template<Tok t>
    requires (t == Tok::STRING)
    [[nodiscard]]
    inline bool is() const noexcept {
        return isalnum(*m_pos);
    }

    template<Tok t>
    requires (t == Tok::STRING_LITERAL)
    [[nodiscard]]
    inline bool is() const noexcept {
        return *m_pos == '"';
    }

    template<Tok t>
    requires (t == Tok::STRING)
    inline decltype(auto) peek() {
        return getString();
    }

    template<Tok t>
    requires (t == Tok::STRING_LITERAL)
    inline decltype(auto) peek() {
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

    inline void get() noexcept {
        m_pos++;
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

