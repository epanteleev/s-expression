#include "detail/Actions.h"
#include "SQuery.h"
#include "detail/SData.h"
#include "detail/SQueryParser.h"

namespace detail::command {

    SQuery::Pointer SQuery::parse(std::string_view query) {
        return parser::SQueryParser(query).apply();
    }

    detail::SData SQueryExpressionByAbsolutePath::apply(detail::SData &doc) {
        return detail::actions::FindAllExpression(doc, m_path).apply();
    }

    detail::SData SQueryExpressionByRelativePath::apply(detail::SData &doc) {
        return detail::actions::FindByPartOfPath(doc, m_path).apply();
    }

    detail::SData SQueryFilter::apply(detail::SData &doc) {
        if (m_type == FilterType::RANGE) {
            return detail::SData(applyRange(doc));
        } else if (m_type == FilterType::INDEX) {
            return detail::SData(applyIndex(doc));
        } else {
            assert(false);
            return {};
        }
    }

    detail::SData SQueryFilter::applyRange(detail::SData &doc) {
        const auto &[begin, end] = std::get<range>(m_data);
        if (doc.hold<SDocument*>()) {
            auto d = doc.get<SDocument*>();
            return makeRange(*d);

        } else if (doc.hold<SPathResponse>()) {
            auto d = doc.get<SPathResponse>();
            return makeRange(d);

        } else {
            assert(false);
            return {};
        }
    }

    detail::SData SQueryFilter::applyIndex(detail::SData &data) {
        const auto &idx = std::get<std::int64_t>(m_data);
        if (data.hold<SDocument*>()) {
            auto doc = data.get<SDocument *>();

            std::size_t pos = verifyIdx(idx, *doc);
            if (pos == npos) {
                return {};
            } else {
                return detail::SData({doc->begin() + pos});
            }

        } else if (data.hold<SPathResponse>()) {
            auto doc = data.get<SPathResponse>();

            std::size_t pos = verifyIdx(idx, doc);
            if (pos == npos) {
                return {};
            } else {
                auto it = *(doc.begin() + pos);
                return detail::SData({it});
            }

        } else {
            assert(false);
            return {};
        }
    }

    detail::SData SQueryCombination::apply(detail::SData &doc) {
        detail::SData res = std::move(doc);
        for (auto &i: m_commands) {
            res = std::move(i->apply(res));
        }
        return res;
    }
}