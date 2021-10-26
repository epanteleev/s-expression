#pragma once

#include "Sexpression.h"

class SDocument final {
public:
    SDocument(SDocument &) = delete;

    SDocument(SDocument &&doc) noexcept: m_sexp(std::move(doc.m_sexp)) {}

public:
    std::string toString();

    Sexpression* operator[](std::string_view name) noexcept;

    inline Sexpression& addChild(Sexpression &&sexpression) {
        return m_sexp.emplace_back(std::move(sexpression));
    }

private:
    explicit SDocument(std::vector<Sexpression> &&sexpr) : m_sexp(std::move(sexpr)) {}

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

public:
    static SDocument parse(const std::string &string);

private:
    std::vector<Sexpression> m_sexp{};
};