#pragma once

#include <string>
#include <array>
#include <cassert>
#include <vector>
#include <format>

namespace detail::lexer {
    enum class Tok : char {
        STRING = 0,
        INTEGER,
        UINTEGER,
        OPEN_PAREN = '(',
        CLOSE_PAREN = ')',
        OPEN_BRACE = '{',
        CLOSE_BRACE = '}',
        COMMA = ',',
        OPEN_SQUARE_BRACKET = '[',
        CLOSE_SQUARE_BRACKET = ']',
        STRING_LITERAL,
        DOLLAR = '$',
        SEMICOLON = ';',
        SLASH = '/',
        EQUALITY = '=',
        // Keywords
        RANGE,
        INDEX,
    };

    namespace keyword {
        static const char *RANGE = "range";
        static const char *INDEX = "idx";
    }

    class Lexer final {
    public:
        using iterator = const char*;

    public:
        explicit Lexer(std::string_view string) :
                m_pos(string.data()),
                m_end(string.data() + string.length()),
                m_lineBegin(string.data()) {}

    private:

        static inline bool isSpace(iterator p) {
            return *p == ' ' || *p == '\n' || *p == '\t' || *p == '\0';
        }

        [[nodiscard]]
        iterator findStringEnd() const noexcept;

        std::string getString(iterator &it) const;

        std::int64_t getInteger();

        std::size_t getUinteger();

        bool checkDigits(iterator it) const;

        [[nodiscard]]
        bool isInteger() const;

        [[nodiscard]]
        bool isUinteger() const;

        [[nodiscard]]
        iterator findLiteralEnd() const;

        inline bool checkStringLiteral() {
            return *m_pos == '"';
        }

        std::string getStringLiteral();

        inline void get() noexcept {
            assert(!eof());
            m_pos++;
        }

        bool isKeyword(const char *keyword);

        bool isDelimiter(iterator pos) const;

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
        requires (t == Tok::UINTEGER)
        [[nodiscard]]
        inline bool is() noexcept {
            return isUinteger();
        }

        template<Tok t>
        requires (t == Tok::STRING_LITERAL)
        [[nodiscard]]
        inline bool is() noexcept {
            return *m_pos == '"';
        }

        template<Tok t>
        requires (t == Tok::RANGE)
        [[nodiscard]]
        inline bool is() noexcept {
            return isKeyword(keyword::RANGE);
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
            assert(!eof());
            return getString(m_pos);
        }

        template<Tok t>
        requires (t == Tok::INTEGER)
        inline decltype(auto) peek() {
            assert(is<Tok::INTEGER>());
            assert(!eof());
            return getInteger();
        }

        template<Tok t>
        requires (t == Tok::UINTEGER)
        inline decltype(auto) peek() {
            assert(is<Tok::UINTEGER>());
            return getUinteger();
        }

        template<Tok t>
        requires (t == Tok::STRING_LITERAL)
        inline decltype(auto) peek() {
            assert(is<Tok::STRING_LITERAL>());
            return getStringLiteral();
        }

        inline bool skipSpaces() {
            auto pos = m_pos;
            while (pos != m_end && isSpace(pos)) {
                if (*pos == '\n') {
                    m_lineCount++;
                    m_lineBegin = pos + 1;
                }
                pos++;
            }
            m_pos = pos;
            return true;
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
            return std::format("{}:{} '{}'", lines(), pos(), std::string(m_pos, m_end));
        }

    private:
        iterator m_pos;
        iterator m_end;
        std::size_t m_lineCount{};
        iterator m_lineBegin{};
    };
}