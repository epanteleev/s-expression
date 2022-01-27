#include "Actions.h"

namespace detail::actions {
    SData FindAllExpression::apply() noexcept {
        findAllExpr(*static_cast<SNode *>(m_doc.get<SDocument*>()));
        return SData(std::move(m_response));
    }

    void FindAllExpression::findAllExpr(SNode &s) noexcept {
        if (m_pos == m_end) {
            return;
        }

        auto list = s.findAll(*m_pos);
        if (list.empty()) {
            return;
        }
        if (m_pos + 1 == m_end) {
            m_response.insert(m_response.end(),
                              std::make_move_iterator(list.begin()),
                              std::make_move_iterator(list.end()));
        } else {
            m_pos += 1;
            for (const auto &i: list) {
                findAllExpr(*i); //todo check that thi call is tail
            }
        }
    }

    SData FindByPartOfPath::apply() noexcept {
        findAllExpr(m_pos, *static_cast<SNode *>(m_doc.get<SDocument*>()));
        return SData(std::move(m_response));
    }

    void FindByPartOfPath::findAllExpr(iterator pos, SNode &s) noexcept {
        if (pos == m_end) {
            return;
        }
        auto list = s.findAll(*pos);
        if (pos + 1 == m_end) {
            m_response.insert(m_response.end(),
                              std::make_move_iterator(list.begin()),
                              std::make_move_iterator(list.end()));
        }
        for (const auto &i: list) {
            findAllExpr(pos + 1, *i); //todo too
        }
        for (auto &i: s) {
            findAllExpr(pos, i); //todo too
        }
    }
}