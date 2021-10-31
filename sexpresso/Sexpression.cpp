#include "Sexpression.h"
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <sstream>
#include <array>
#include <iostream>
#include <cassert>

#define shouldNeverReachHere() fprintf(stderr, "Error: Should never reach here " __FILE__ ":%d\n", __LINE__)

//todo todo
static const std::array<char, 11> escape_chars = {'\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};
static const std::array<char, 11> escape_vals = {'\'', '"', '\?', '\\', '\a', '\b', '\f', '\n', '\r', '\t', '\v'};

static std::vector<std::string_view> splitPathString(std::string_view path) {
    std::vector<std::string_view> paths{};
    if (path.empty()) {
        return paths;
    }
    auto start = path.begin();
    for (auto i = path.begin() + 1; i != path.end(); i++) {
        if (*i == '/') {
            paths.emplace_back(start, i);
            start = i + 1;
        }
    }
    paths.emplace_back(start, path.end());
    return paths;
}

static bool isEscapeValue(char c) {
    return std::find(escape_vals.begin(), escape_vals.end(), c) != escape_vals.end();
}

static std::size_t countEscapeValues(std::string_view str) {
    return std::count_if(str.begin(), str.end(), isEscapeValue);
}

static std::string escape(std::string_view str) {
    auto escape_count = countEscapeValues(str);
    if (escape_count == 0) {
        return std::string(str);
    }
    std::string result_str{};
    result_str.reserve(str.size() + escape_count);
    for (auto c: str) {
        auto loc = std::find(escape_vals.begin(), escape_vals.end(), c);
        if (loc == escape_vals.end()) {
            result_str.push_back(c);
        } else {
            result_str.push_back('\\');
            result_str.push_back(escape_chars[loc - escape_vals.begin()]);
        }
    }
    return result_str;
}

static std::string stringValToString(const std::string &s) {
    if (s.empty()) {
        return "\"\"";
    }
    if ((std::find(s.begin(), s.end(), ' ') == s.end()) && countEscapeValues(s) == 0) {
        return s;
    }
    return std::string("\"") + escape(s) + "\"";
}

void Sexpression::toStringIter(std::ostringstream& ostream) const {
    if (m_kind == SKind::STRING) {
        ostream << stringValToString(m_name);
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

std::string Sexpression::toString() const {
    std::ostringstream ostream{};
    toStringIter(ostream);
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
    return {SKind::STRING, escape(string)};
}

Sexpression &Sexpression::operator[](std::string_view path) {
    const auto ch = findChild(path);
    if (ch != end()) {
        return *ch;
    } else {
        return addChild(path);
    }
}
