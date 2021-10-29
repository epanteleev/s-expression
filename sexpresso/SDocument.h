#pragma once

#include "Sexpression.h"
#include "SPathQuery.h"

class SDocument : public SNode  {
public:
    using iterator = std::vector<Sexpression>::iterator;
    using const_iterator = std::vector<Sexpression>::const_iterator;

public:
    SDocument(SDocument &) = delete;

    SDocument(SDocument &&doc) noexcept: m_sexp(std::move(doc.m_sexp)) {}

public:
    std::string toString();

    iterator operator[](std::string_view name) noexcept;

    SPathResponce query(std::string_view q) {
        assert(false);
        auto command = SQuery::parse(q);
        return {};
    };

    Sexpression& addChild(Sexpression &&sexpression) override {
        sexpression._setParent(this);
        return m_sexp.emplace_back(std::move(sexpression));
    }

private:
    explicit SDocument(std::vector<Sexpression> &&sexpr) : m_sexp(std::move(sexpr)) {}

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

public:
    static SDocument parse(std::string_view string);

private:
    std::vector<Sexpression> m_sexp{};
};