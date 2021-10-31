#pragma once

#include "Sexpression.h"
#include "SPathQuery.h"

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

    Sexpression &addChild(Sexpression &&sexpression) {
        return m_sexp.emplace_back(std::move(sexpression));
    }

    std::vector<Sexpression::iterator> query(std::string_view q) {
        return SQuery::parse(q)->apply(*this);
    };

public:
    static SDocument parse(std::string_view string);
};

inline SDocument operator "" _s(const char* s, std::size_t n) {
    return SDocument::parse(std::string_view(s, n));
}
