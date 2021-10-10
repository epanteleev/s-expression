#pragma once

#include "sexpresso.hpp"

class SDocument final {
public:
    SDocument(SDocument &) = delete;

    SDocument(SDocument &&doc) noexcept: m_sexp(std::move(doc.m_sexp)) {}

    std::string toString();

private:
    explicit SDocument(std::vector<Sexpression> &&sexpr) : m_sexp(std::move(sexpr)) {}

public:
    static SDocument parse(const std::string &string);

private:
    std::vector<Sexpression> m_sexp{};
};