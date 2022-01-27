#pragma once

#include <string>
#include <utility>
#include <vector>
#include "SNode.h"

enum class SKind : uint8_t {
    SEXP,
    STRING,
};

/** Represent S-expression. **/
class Sexpression final : public SNode {
public:
    using iterator = SNode::iterator;

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
    /**
     * Add new s-expression as child.
     * @return reference to new s-expression.
     */
    inline Sexpression& addChild(Sexpression &&sexpression) {
        if (m_kind == SKind::STRING) {
            m_kind = SKind::SEXP;
        }
        return m_sexp.emplace_back(std::move(sexpression));
    }

    /**
     * Add new string as new s-expression.
     * @return reference to new s-expression.
     */
    inline Sexpression& addChild(std::string_view string) {
        return addChild(makeFromStr(string));
    }

public:
    [[nodiscard]]
    inline const std::string &name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    std::string toString() const;

    [[nodiscard]]
    std::string dump() const;

    /**
     * Get s-expression by given basename.
     * If s-expression doesn't exist, new s-expression is created.
     * @return reference to s-expression.
     */
    Sexpression& operator[](std::string_view basename);

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

    void dumpIter(std::size_t tab, std::size_t depth, std::ostringstream &ostream) const;

public:
    /** Fabric method for creating new s-expression. */
    static Sexpression make(std::string &&basename) {
        return {SKind::SEXP, std::move(basename)};
    }

    /** Fabric method for creating new s-expression as string data. */
    static Sexpression makeFromStr(std::string_view string);

private:
    SKind m_kind;
    std::string m_name;
};