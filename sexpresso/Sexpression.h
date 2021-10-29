#pragma once

#include <string>
#include <utility>
#include <vector>
#include "SNode.h"

enum class SKind : uint8_t {
    SEXP,
    STRING,
    NIL,
};

class Sexpression : public SNode {
public:
    using iterator = std::vector<Sexpression>::iterator;
    using const_iterator = std::vector<Sexpression>::const_iterator;

public:
    Sexpression(Sexpression &) = delete;

    Sexpression(Sexpression &&sexpression) noexcept:
            m_kind(sexpression.m_kind),
            m_sexp(std::move(sexpression.m_sexp)),
            m_name(std::move(sexpression.m_name)) {}

    Sexpression operator=(Sexpression &) = delete;

private:
    explicit Sexpression(std::vector<Sexpression> &&vec) :
            m_kind(SKind::SEXP),
            m_sexp(std::move(vec)) {}

    Sexpression(SKind kind, std::string_view string) :
            m_kind(kind),
            m_name(string) {}

public:
    inline Sexpression& addChild(Sexpression &&sexpression) override {
        if (m_kind == SKind::STRING) {
            m_kind = SKind::SEXP;
        }
        sexpression.m_parent = this;
        return m_sexp.emplace_back(std::move(sexpression));
    }

    inline Sexpression& addChild(const std::string &str) {
        return addChild(makeFromStr(str));
    }

public:
    [[nodiscard]]
    size_t childCount() const;

    [[nodiscard]]
    inline const std::string &name() const noexcept {
        return m_name;
    }

    iterator getChild(std::string_view path);

    iterator findChild(std::string_view basename);

    std::vector<iterator> findAll(std::string_view basename);

    Sexpression &createPath(const std::string &path);

    [[nodiscard]]
    std::string toString() const;

    template<SKind k>
    [[nodiscard]]
    inline bool is() const noexcept {
        return m_kind == k;
    }

    template<SKind k>
    requires (k == SKind::NIL)
    [[nodiscard]]
    inline bool is() const noexcept {
        return m_kind == SKind::SEXP && childCount() == 0;
    }

    void _setParent(const SNode* parent) noexcept {
        m_parent = parent;
    }
public:
    bool operator==(const Sexpression &other) const;

    bool operator!=(const Sexpression &other) const {
        return !(*this == other);
    }

public:
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
    Sexpression &createPath(const std::vector<std::string_view> &path);

    void toStringIter(std::ostringstream &ostream) const;

public:
    static Sexpression make(std::string &&basename) {
        return {SKind::SEXP, std::move(basename)};
    }

    static Sexpression makeFromStr(const std::string &string);

private:
    SKind m_kind;
    std::string m_name;
    std::vector<Sexpression> m_sexp{};
    const SNode* m_parent{};
};
