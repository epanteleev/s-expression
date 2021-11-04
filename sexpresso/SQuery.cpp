#include <detail/Actions.h>
#include "SQuery.h"
#include "Lexer.h"
#include "SDocument.h"
#include "detail/SData.h"

namespace utils {
    std::vector<std::string> parseRelativePath(Lexer &lex) {
        std::vector<std::string> paths;

        while (!lex.eof()) {
            if (lex.is<Tok::STRING>()) {
                paths.emplace_back(lex.peek<Tok::STRING>());
            } else if (lex.is<Tok::OPEN_SQUARE_BRACKET>()) {
                assert(false);
            }
            if (!lex.is<Tok::SLASH>()) {
                break;
            }
        }

        return paths;
    }

    detail::command::SQuery::Pointer parseFilter(Lexer &lex) {
        using namespace detail::command;
        lex.skipSpaces();
        if (lex.is<Tok::DOLLAR>()) {
            if (lex.is<Tok::NAME>() && lex.is<Tok::EQUALITY>()) { // $name=<name>
                if (lex.is<Tok::STRING>()) {
                    return std::make_unique<SQueryFilter>(SQueryFilter::FilterType::NAME, lex.peek<Tok::STRING>());
                } else {
                    throw std::runtime_error(format("in %s expect name", lex.message().c_str()));
                }
            } else if (lex.is<Tok::INDEX>() && lex.is<Tok::EQUALITY>()) { // $idx=<index>
                if (lex.is<Tok::INTEGER>()) {
                    return std::make_unique<SQueryFilter>(SQueryFilter::FilterType::INDEX, lex.peek<Tok::INTEGER>());
                } else {
                    throw std::runtime_error(format("in %s expect name", lex.message().c_str()));
                }
            } else {
                throw std::runtime_error(format("in %s undefined command", lex.message().c_str()));
            }
        } else {
            throw std::runtime_error(format("in %s expect '$'", lex.message().c_str()));
        }
    }
};

namespace detail::command {

    SQuery::Pointer SQuery::parse(std::string_view query) {
        Lexer lex(query);
        if (lex.is<Tok::SLASH>()) {
            auto paths = utils::parseRelativePath(lex);
            return std::make_unique<SQueryExpressionByAbsolutePath>(std::move(paths));
        } else if (lex.is<Tok::STRING>()) {
            auto paths = utils::parseRelativePath(lex);
            return std::make_unique<SQueryExpressionByRelativePath>(std::move(paths));
        } else if (lex.is<Tok::OPEN_SQUARE_BRACKET>()) {
            auto command = utils::parseFilter(lex);
            lex.skipSpaces();
            if (!lex.is<Tok::CLOSE_SQUARE_BRACKET>()) {
                throw std::runtime_error(format("in %s expect ']'", lex.message().c_str()));
            }
            return command;
        } else {
            throw std::runtime_error(format("in %s incorrect query", lex.message().c_str()));
        }
    }


    detail::SData SQueryExpressionByAbsolutePath::apply(detail::SData &doc) {
        return detail::actions::FindAllExpression(doc, m_path).apply();
    }

    detail::SData SQueryExpressionByRelativePath::apply(detail::SData &doc) {
        return detail::actions::FindByPartOfPath(doc, m_path).apply();
    }

    detail::SData SQueryFilter::apply(detail::SData &doc) {
        if (m_type == FilterType::NAME) {
            return detail::SData(applyName(doc));
        } else if (m_type == FilterType::INDEX) {
            return detail::SData(applyIndex(doc));
        } else {
            assert(false);
        }
    }

    detail::SData SQueryFilter::applyName(detail::SData &doc) {
        auto d = doc.get<SDocument *>();
        const auto &name = std::get<std::string>(m_data);
        return detail::SData(d->findAll(name));
    }

    detail::SData SQueryFilter::applyIndex(detail::SData &data) {
        const auto &idx = std::get<std::int64_t>(m_data);
        auto doc = data.get<SDocument *>();

        std::int64_t pos;
        if (idx < 0) {
            if (-idx > doc->size()) {
                return {};
            } else {
                pos = idx + doc->size();
            }
        } else {
            pos = idx;
        }
        return detail::SData({Sexpression::iterator(&(*doc)[pos])});
    }

    detail::SData SQueryCombination::apply(detail::SData &doc) {
        detail::SData res = std::move(doc);
        for (auto &i: m_commands) {
            res = std::move(i->apply(res));
        }
        return res;
    }
}