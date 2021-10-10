#pragma once

#include <string>
#include <utility>
#include <vector>

enum class SexpValueKind : uint8_t {
    SEXP,
    STRING,
};


class Sexpression final {
public:
    Sexpression(Sexpression &) = delete;

    Sexpression(Sexpression &&sexpression) noexcept:
            m_kind(sexpression.m_kind),
            m_sexp(std::move(sexpression.m_sexp)),
            m_str(std::move(sexpression.m_str)) {}

    Sexpression operator=(Sexpression &) = delete;

private:
    explicit Sexpression(std::vector<Sexpression> &&vec) :
            m_kind(SexpValueKind::SEXP),
            m_sexp(std::move(vec)) {}

    Sexpression(SexpValueKind kind, std::string string) :
            m_kind(kind),
            m_str(std::move(string)) {}

public:
    inline void addChild(Sexpression &&sexpression) {
        m_sexp.push_back(std::move(sexpression));
    }

    inline void addChild(std::string &&str) {
        addChild(makeFromStr(str));
    }

public:
    [[nodiscard]]
    size_t childCount() const;

    [[nodiscard]]
    inline const std::string &getString() const noexcept {
        return m_str;
    }

    Sexpression *getChild(const std::string &path);

    Sexpression &createPath(const std::string &path);

    [[nodiscard]]
    std::string toString() const;

    [[nodiscard]]
    inline bool isString() const noexcept {
        return m_kind == SexpValueKind::STRING;
    }

    [[nodiscard]]
    inline bool isSexp() const noexcept {
        return m_kind == SexpValueKind::SEXP;
    }

    [[nodiscard]]
    inline bool isNil() const noexcept {
        return m_kind == SexpValueKind::SEXP && childCount() == 0;
    }

public:
    bool operator==(const Sexpression &other) const;

    bool operator!=(const Sexpression &other) const {
        return !(*this == other);
    }

public:
    using iterator = std::vector<Sexpression>::iterator;
    using const_iterator = std::vector<Sexpression>::const_iterator;

    iterator begin() noexcept {
        return m_sexp.begin();
    }

    iterator end() noexcept {
        return m_sexp.end();
    }

    [[nodiscard]]
    const_iterator begin() const {
        return m_sexp.begin();
    }

    [[nodiscard]]
    const_iterator end() const noexcept {
        return m_sexp.end();
    }

private:
    Sexpression &createPath(const std::vector<std::string> &path);

    inline Sexpression &getChild(size_t idx) {
        return m_sexp[idx];
    }

    Sexpression *findChild(const std::string &name);

public:
    static Sexpression make(std::string &&basename) {
        return {SexpValueKind::SEXP, std::move(basename)};
    }

    static Sexpression makeFromStr(const std::string &string);

private:
    SexpValueKind m_kind;
    std::vector<Sexpression> m_sexp{};
    std::string m_str;
};
