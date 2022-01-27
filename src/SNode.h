#pragma once
#include <vector>
#include <string_view>

class Sexpression;

/** Base class for s-expression nodes. */
class SNode {
public:
    using iterator = std::vector<Sexpression>::iterator;
    using reverse_iterator = std::vector<Sexpression>::reverse_iterator;
    using const_iterator = std::vector<Sexpression>::const_iterator;

public:
    SNode() = default;

    SNode(SNode &) = delete;

    SNode(SNode &&doc) noexcept: m_sexp(std::move(doc.m_sexp)) {}

    explicit SNode(std::vector<Sexpression>&& sexp) : m_sexp(std::move(sexp)) {}

    virtual ~SNode() = default;

public:
    iterator findChild(std::string_view basename) noexcept;

    std::vector<iterator> findAll(std::string_view basename) noexcept;

public:
    [[nodiscard]]
    inline std::size_t size() const noexcept {
        return m_sexp.size();
    }

    iterator begin() noexcept;

    iterator end() noexcept;

    reverse_iterator rbegin() noexcept;

    reverse_iterator rend() noexcept;

protected:
    std::vector<Sexpression> m_sexp{};
};