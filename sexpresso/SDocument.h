#pragma once

#include "Sexpression.h"
#include "SQuery.h"

class SDocument : public SNode  {
public:
    using iterator = SNode::iterator;
    using const_iterator = SNode::const_iterator;

public:
    SDocument(SDocument &) = delete;

    SDocument(SDocument&& document) noexcept : SNode(std::move(document)) {}

    explicit SDocument(std::vector<Sexpression>&& doc) : SNode(std::move(doc)) {}

public:
    std::string toString();

    Sexpression& operator[](std::string_view basename) noexcept;

    Sexpression& operator[](std::size_t idx) noexcept {
        return m_sexp[idx];
    }

    Sexpression &addChild(Sexpression &&sexpression) {
        return m_sexp.emplace_back(std::move(sexpression));
    }

    detail::SPathResponse query(std::string_view q) {
        detail::SData data(this);
        return detail::command::SQuery::parse(q)->apply(data).get<detail::SPathResponse>();
    };

public:
    static SDocument parse(std::string_view string);
};

inline SDocument operator "" _s(const char* s, std::size_t n) {
    return SDocument::parse(std::string_view(s, n));
}
