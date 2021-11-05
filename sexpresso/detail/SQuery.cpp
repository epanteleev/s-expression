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
            return detail::SData(applyName(doc));
        } else if (m_type == FilterType::INDEX) {
            return detail::SData(applyIndex(doc));
        } else {
            assert(false);
        }
    }

    detail::SData SQueryFilter::applyName(detail::SData &doc) {
        const auto &[begin, end] = std::get<range>(m_data);
        if (doc.hold<SDocument*>()) {
            auto d = doc.get<SDocument*>();

            if (begin > end) {
                return {};
            }
            if (d->size() < begin) {
                return {};
            }
            auto beginPos = d->begin() + begin;
            Sexpression::iterator endPos{};
            if (d->size() < end) {
                endPos = d->end();
            } else {
                endPos = d->begin() + end + 1;
            }

            SPathResponse response{};
            auto fn = [&](Sexpression& s) {
                response.emplace_back(Sexpression::iterator(&s));
            };
            std::for_each(beginPos, endPos, fn); // Todo coping

            return detail::SData(std::move(response));

        } else if (doc.hold<SPathResponse>()) {
//            auto d = doc.get<SPathResponse>();
//            SPathResponse response{};
//
//            for (auto& i: d) {
//                auto list = i->findAll(name);
//                response.insert(response.end(), list.begin(), list.end());
//            }
            return {}; // detail::SData(std::move(response));

        } else {
            assert(false);
        }
    }

    detail::SData SQueryFilter::applyIndex(detail::SData &data) {
        const auto &idx = std::get<std::int64_t>(m_data);
        if (data.hold<SDocument*>()) {
            auto doc = data.get<SDocument *>();

            std::size_t pos = verifyIdx(idx, *doc);
            return detail::SData({Sexpression::iterator(&(*doc)[pos])});

        } else if (data.hold<SPathResponse>()) {
            auto doc = data.get<SPathResponse>();

            std::size_t pos = verifyIdx(idx, doc);
            auto it = *(doc.begin() + pos);
            return detail::SData({it});
        } else {
            assert(false);
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