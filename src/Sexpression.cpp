#include "Sexpression.h"
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <sstream>
#include <array>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "detail/Escape.h"

#define shouldNeverReachHere() fprintf(stderr, "Error: Should never reach here " __FILE__ ":%d\n", __LINE__)

void Sexpression::toStringIter(std::ostringstream& ostream) const {
    if (m_kind == SKind::STRING) {
        ostream << detail::escape::escapeWrap(m_name);
    } else if (m_kind == SKind::SEXP) {
        ostream << '(' << m_name;
        if (!m_sexp.empty()) {
            ostream << ' ';
        }
        for (auto i = m_sexp.begin(); i != m_sexp.end(); i++) {
            i->toStringIter(ostream);
            if (i != m_sexp.end() - 1) {
                ostream << ' ';
            }
        }
        ostream << ')';
    } else {
        shouldNeverReachHere();
    }
}

std::string tabs(std::size_t tab, std::size_t depth) {
    return std::string(depth * tab, ' '); // todo
}

void Sexpression::dumpIter(std::size_t tab, std::size_t depth, std::ostringstream &ostream) const {
    if (m_kind == SKind::STRING) {
        ostream << detail::escape::escapeWrap(m_name);
    } else if (m_kind == SKind::SEXP) {
        ostream << '(' << m_name;
        if (!m_sexp.empty()) {
            ostream << std::endl << tabs(tab, depth);
        } else {
            ostream << ')';
            return;
        }
        for (const auto & i : m_sexp) {
            i.dumpIter(tab, depth + 1, ostream);
            ostream << std::endl << tabs(tab, depth);
        }
        ostream << ')';
    } else {
        shouldNeverReachHere();
    }
}

std::string Sexpression::toString() const {
    std::ostringstream ostream{};
    toStringIter(ostream);
    return ostream.str();
}

std::string Sexpression::dump() const {
    std::ostringstream ostream{};
    dumpIter(2, 1, ostream);
    return ostream.str();
}

bool Sexpression::operator==(const Sexpression &other) const {
    if (m_kind != other.m_kind) {
        return false;
    }
    switch (m_kind) {
        case SKind::SEXP: {
            if (m_name != other.m_name || m_sexp.size() != other.m_sexp.size()) {
                return false;
            }
            for (std::size_t i = 0; i < m_sexp.size(); ++i) {
                if (m_sexp[i] != other.m_sexp[i]) {
                    return false;
                }
            }
            return true;
        }

        case SKind::STRING:
            return m_name == other.m_name;
    }
    shouldNeverReachHere();
    return false;
}

Sexpression Sexpression::makeFromStr(std::string_view string) {
    return {SKind::STRING, std::string(string)};
}

Sexpression &Sexpression::operator[](std::string_view basename) {
    const auto ch = findChild(basename);
    if (ch != end()) {
        return *ch;
    } else {
        return addChild(basename);
    }
}
