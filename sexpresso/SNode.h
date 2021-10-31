#pragma once
#include <vector>
#include <string_view>

class Sexpression;

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
    iterator findChild(std::string_view basename);

    std::vector<iterator> findAll(std::string_view basename);

public:
    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_sexp.size();
    }

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

    reverse_iterator rbegin() noexcept {
        return m_sexp.rbegin();
    }

    reverse_iterator rend() noexcept {
        return m_sexp.rend();
    }

protected:
    std::vector<Sexpression> m_sexp{};

};
