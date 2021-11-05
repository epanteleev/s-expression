#pragma once

#include <string_view>
#include <functional>
#include <memory>
#include <cassert>
#include <list>
#include <limits>
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
        using range = std::tuple<std::size_t, std::size_t>;
    public:
        enum class FilterType: char {
            RANGE,
            INDEX,
        };

    public:
        SQueryFilter(FilterType type, range range) :
                m_type(type),
                m_data(std::move(range)) {}

        SQueryFilter(FilterType type, std::int64_t data) :
                m_type(type),
                m_data(data) {}

    public:
        detail::SData apply(detail::SData &doc) override;

    private:
        detail::SData applyName(detail::SData &doc);

        detail::SData applyIndex(detail::SData &doc);

        template<typename Type>
        std::size_t verifyIdx(std::int64_t idx, Type& list) const noexcept {
            if (idx < 0) {
                if (-idx > list.size()) {
                    return std::numeric_limits<std::size_t>::max();
                } else {
                    return idx + list.size();
                }
            } else {
                return idx;
            }
        }

    private:
        FilterType m_type;
        std::variant<range, std::int64_t> m_data;
    };

    class SQueryCombination final : public SQuery {
    public:
        SQueryCombination() = default;

    public:

        inline void append(SQuery::Pointer ptr) noexcept {
            m_commands.emplace_back(std::move(ptr));
        }

        SData apply(detail::SData &doc) override;

    private:
        std::list<SQuery::Pointer> m_commands;
    };
}