#include <numeric>
#include <algorithm>
#include "Sexpression.h"

SNode::iterator SNode::findChild(std::string_view basename) noexcept {
    auto pFunction = [&basename](Sexpression &s) {
        return s.name() == basename;
    };
    return std::find_if(m_sexp.begin(), m_sexp.end(), pFunction);
}

std::vector<SNode::iterator> SNode::findAll(std::string_view basename) noexcept {
    std::vector<Sexpression::iterator> result;
    for (auto i = m_sexp.begin(); i != m_sexp.end(); i++) {
        if (i->name() == basename)
            result.emplace_back(i);
    }

    return result;
}

SNode::reverse_iterator SNode::rbegin() noexcept {
    return m_sexp.rbegin();
}

SNode::reverse_iterator SNode::rend() noexcept {
    return m_sexp.rend();
}

SNode::iterator SNode::end() noexcept {
    return m_sexp.end();
}

SNode::iterator SNode::begin() noexcept {
    return m_sexp.begin();
}