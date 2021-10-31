#include <detail/Actions.h>
#include "SPathQuery.h"
#include "Lexer.h"
#include "SDocument.h"

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

    SQuery::Pointer parseFilter(Lexer &lex) {
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


SPathResponse SQueryExpressionByAbsolutePath::apply(SDocument &doc) {
    return detail::FindAllExpression(doc, m_path).apply();
}

SPathResponse SQueryExpressionByRelativePath::apply(SDocument &doc) {
    return detail::FindByPartOfPath(doc, m_path).apply();
}

SPathResponse SQueryFilter::apply(SDocument &doc) {
    if (m_type == FilterType::NAME) {
        const auto& name = std::get<std::string>(m_data);
        auto list = doc.findAll(name);
        auto pred = [&](Sexpression::iterator i) {
            return name == i->name();
        };
        std::erase_if(list, pred);
        return list;
    } else if (m_type == FilterType::INDEX) {
        const auto& idx = std::get<std::int64_t>(m_data);

        std::int64_t pos;
        if (idx < 0) {
            pos = idx + doc.size();
        } else {
            pos = idx;
        }

        auto it = doc.begin();
        for (auto i = 0; i < pos; i++) {
            it++;
        }
        return {it};
    } else {
        assert(false);
    }
}
