#pragma once

#include <string>
#include <vector>

enum class SexpValueKind : uint8_t {
    SEXP,
    STRING,
};


class Sexpression final {
public:
    Sexpression() : m_kind(SexpValueKind::SEXP) {}

    ~Sexpression() = default;

    Sexpression(Sexpression &) = delete;

    Sexpression(Sexpression &&sexpression) noexcept:
            m_kind(sexpression.m_kind),
            m_sexp(std::move(sexpression.m_sexp)),
            m_str(std::move(sexpression.m_str)) {}

    Sexpression operator=(Sexpression &) = delete;

private:
    explicit Sexpression(const std::string &string);

    explicit Sexpression(std::vector<Sexpression> &&vec) :
            m_kind(SexpValueKind::SEXP),
            m_sexp(std::move(vec)) {}

public:
    void addChild(Sexpression &&sexpression) {
        if (m_kind == SexpValueKind::STRING) {
            m_kind = SexpValueKind::SEXP;
            m_sexp.push_back(Sexpression(m_str));
        } else {
            m_sexp.push_back(std::move(sexpression));
        }
    }

    void addChild(std::string &&str) {
        addChild(Sexpression(str));
    }

    void addExpression(const std::string &string);

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
        return this->m_kind == SexpValueKind::STRING;
    }

    [[nodiscard]]
    inline bool isSexp() const noexcept {
        return this->m_kind == SexpValueKind::SEXP;
    }

    [[nodiscard]]
    bool isNil() const noexcept {
        return this->m_kind == SexpValueKind::SEXP && this->childCount() == 0;
    }

public:
    bool operator==(const Sexpression &other) const;

    bool operator!=(const Sexpression &other) const {
        return !(*this == other);
    }

public:
    using iterator = std::vector<Sexpression>::iterator;
    using const_iterator = std::vector<Sexpression>::const_iterator;

    [[nodiscard]]
    std::size_t nodes() const noexcept {
        auto sz = m_sexp.size();
        if (sz == 0) {
            return 0;
        } else {
            return sz - 1;
        }
    }

    iterator begin() noexcept {
        if (nodes() == 0) {
            return this->end();
        } else {
            return ++(m_sexp.begin());
        }
    }

    iterator end() noexcept {
        return m_sexp.end();
    }

    [[nodiscard]]
    const_iterator begin() const {
        if (nodes() == 0) {
            return end();
        } else {
            return ++(m_sexp.begin());
        }
    }

    [[nodiscard]]
    const_iterator end() const noexcept {
        return m_sexp.end();
    }

private:
    Sexpression &createPath(const std::vector<std::string> &path);

    void addChild(std::vector<Sexpression> &&vec) {
        addChild(Sexpression(std::move(vec)));
    }

    inline Sexpression &getChild(size_t idx) {
        return m_sexp[idx];
    }

    Sexpression *findChild(const std::string &name);

public:
    static Sexpression parse(const std::string &str);

public:
    SexpValueKind m_kind;
    std::vector<Sexpression> m_sexp{};
    std::string m_str;
};