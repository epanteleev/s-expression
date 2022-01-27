#pragma once

#include "Sexpression.h"
#include "detail/SQuery.h"
#include <filesystem>

class SDocument : public SNode {
public:
    using iterator = SNode::iterator;
    using const_iterator = SNode::const_iterator;

public:
    SDocument() = default;

    SDocument(SDocument &) = delete;

    SDocument(SDocument &&document) noexcept: SNode(std::move(document)) {}

    explicit SDocument(std::vector<Sexpression> &&doc) : SNode(std::move(doc)) {}

public:
    std::string toString();

    std::string dump();
    /**
     * Get s-expression by given basename.
     * If s-expression doesn't exist, new s-expression is created.
     * @return reference to s-expression.
     */
    Sexpression &operator[](std::string_view basename) noexcept;

    /**
     * Get s-expression by index.
     * @return reference to s-expression.
     */
    Sexpression &operator[](std::size_t idx) noexcept {
        return m_sexp[idx];
    }

    /**
     * Add new string as new s-expression.
     * @return reference to new s-expression.
     */
    Sexpression &addChild(Sexpression &&sexpression) {
        return m_sexp.emplace_back(std::move(sexpression));
    }

    detail::SPathResponse query(std::string_view queryStr) {
        detail::SData data(this);
        return detail::command::SQuery::parse(queryStr)->apply(data).get<detail::SPathResponse>();
    };

public:
    static SDocument parse(std::string_view string);

    static SDocument load(std::filesystem::path& string);
};

inline SDocument operator "" _s(const char *s, std::size_t n) {
    return SDocument::parse(std::string_view(s, n));
}
