#include "SNode.h"
#include "Sexpression.h"

SNode::iterator SNode::findChild(std::string_view basename) {
    auto pred = [&basename](Sexpression &s) {
        return s.name() == basename;
    };
    return std::find_if(m_sexp.begin(), m_sexp.end(), pred);
}

std::vector<SNode::iterator> SNode::findAll(std::string_view basename) {
    std::vector<Sexpression::iterator> result;
    for (auto i = m_sexp.begin(); i != m_sexp.end(); i++) {
        if (i->name() == basename) {
            result.push_back(i);
        }
    }
    return result;
}
