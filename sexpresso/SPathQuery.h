#pragma once

#include <string_view>
#include <functional>
#include <memory>
#include <cassert>
#include <variant>
#include "Lexer.h"
#include "Sexpression.h"

class SDocument;

using SPathResponse = std::vector<Sexpression::iterator>;

class SQuery {
public:
    using Pointer = std::unique_ptr<SQuery>;

public:
    SQuery() = default;

    virtual ~SQuery() = default;

public:
    virtual SPathResponse apply(SDocument &) = 0;

public:
    static SQuery::Pointer parse(std::string_view query);
};


class SQueryExpressionByAbsolutePath : public SQuery {
public:
    explicit SQueryExpressionByAbsolutePath(std::vector<std::string> &&path)
            : m_path(std::move(path)) {}

public:
    SPathResponse apply(SDocument &doc) override;

private:
    std::vector<std::string> m_path;
};

class SQueryExpressionByRelativePath : public SQuery {
public:
    explicit SQueryExpressionByRelativePath(std::vector<std::string> &&path)
            : m_path(std::move(path)) {}

public:
    SPathResponse apply(SDocument &doc) override;

private:
    std::vector<std::string> m_path;
};

class SQueryFilter final : public SQuery {
public:
    enum class FilterType: char {
        NAME,
        INDEX,
    };

public:
    SQueryFilter(FilterType type, std::string data) :
        m_type(type),
        m_data(std::move(data)) {}

    SQueryFilter(FilterType type, std::int64_t data) :
            m_type(type),
            m_data(data) {}

public:
    SPathResponse apply(SDocument &doc) override;

private:
    FilterType m_type;
    std::variant<std::string, std::int64_t> m_data;
};