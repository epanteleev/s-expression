#pragma once

#include <string>
#include <utility>
#include <vector>
#include "SNode.h"

enum class SKind : uint8_t {
    SEXP,
    STRING,
};

class Sexpression final : public SNode {
public:
    using iterator = SNode::iterator;
    using const_iterator = SNode::const_iterator;

public:
    Sexpression(Sexpression &) = delete;

    Sexpression(Sexpression &&sexpression) noexcept:
            SNode(std::move(sexpression)),
            m_kind(sexpression.m_kind),
            m_name(std::move(sexpression.m_name)) {}

    Sexpression operator=(Sexpression &) = delete;
private:
    Sexpression(SKind kind, std::string_view string) :
            m_kind(kind),
            m_name(string) {}

public:
    inline Sexpression& addChild(Sexpression &&sexpression) {
        if (m_kind == SKind::STRING) {
            m_kind = SKind::SEXP;
        }
        return m_sexp.emplace_back(std::move(sexpression));
    }

    inline Sexpression& addChild(std::string_view str) {
        return addChild(makeFromStr(str));
    }

public:
    [[nodiscard]]
    inline const std::string &name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    std::string toString() const;

    Sexpression& operator[](std::string_view path);

    template<SKind k>
    [[nodiscard]]
    inline bool is() const noexcept {
        return m_kind == k;
    }

public:
    bool operator==(const Sexpression &other) const;

    bool operator!=(const Sexpression &other) const {
        return !(*this == other);
    }

private:
    void toStringIter(std::ostringstream &ostream) const;

public:
    static Sexpression make(std::string &&basename) {
        return {SKind::SEXP, std::move(basename)};
    }

    static Sexpression makeFromStr(std::string_view string);

private:
    SKind m_kind;
    std::string m_name;
};
