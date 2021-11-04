#include <numeric>
#include "SNode.h"
#include "Sexpression.h"

SNode::iterator SNode::findChild(std::string_view basename) noexcept {
    auto pred = [&basename](Sexpression &s) {
        return s.name() == basename;
    };
    return std::find_if(m_sexp.begin(), m_sexp.end(), pred);
}

std::vector<SNode::iterator> SNode::findAll(std::string_view basename) noexcept {
    std::vector<Sexpression::iterator> result;
    auto fn = [&](Sexpression& s) {
        if (s.name() == basename)
            result.emplace_back(&s);
    };

    std::for_each(m_sexp.begin(),m_sexp.end(), fn);
    return result;
}