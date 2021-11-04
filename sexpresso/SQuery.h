#pragma once

#include <string_view>
#include <functional>
#include <memory>
#include <cassert>
#include <variant>
#include <list>
#include "Lexer.h"
#include "detail/SData.h"
#include "Sexpression.h"

namespace detail::command {
    class SQuery {
    public:
        using Pointer = std::unique_ptr<SQuery>;

    public:
        SQuery() = default;

        virtual ~SQuery() = default;

    public:
        virtual detail::SData apply(detail::SData &) = 0;

    public:
        static SQuery::Pointer parse(std::string_view query);
    };


    class SQueryExpressionByAbsolutePath : public SQuery {
    public:
        explicit SQueryExpressionByAbsolutePath(std::vector<std::string> &&path)
                : m_path(std::move(path)) {}

    public:
        detail::SData apply(detail::SData &doc) override;

    private:
        std::vector<std::string> m_path;
    };

    class SQueryExpressionByRelativePath : public SQuery {
    public:
        explicit SQueryExpressionByRelativePath(std::vector<std::string> &&path)
                : m_path(std::move(path)) {}

    public:
        detail::SData apply(detail::SData &doc) override;

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
        detail::SData apply(detail::SData &doc) override;

    private:
        detail::SData applyName(detail::SData &doc);

        detail::SData applyIndex(detail::SData &doc);
    private:
        FilterType m_type;
        std::variant<std::string, std::int64_t> m_data;
    };

    class SQueryCombination final : public SQuery {
    public:
        SQueryCombination() = default;

    public:
        SData apply(detail::SData &doc) override;

    private:
        std::list<SQuery::Pointer> m_commands;
    };
}