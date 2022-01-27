#pragma once

#include <vector>
#include <string>
#include <SDocument.h>

namespace detail::actions {

    // Find all available s-expression in SDocument by given path.
    class FindAllExpression final {
    public:
        using iterator = std::vector<std::string>::iterator;

    public:
        explicit FindAllExpression(SData &root, std::vector<std::string> &paths) :
                m_doc(root),
                m_pos(paths.begin()),
                m_end(paths.end()) {}

    public:
        SData apply() noexcept;

    private:
        void findAllExpr(SNode& s) noexcept;

    private:
        SData &m_doc;
        iterator m_pos;
        iterator m_end;
        std::vector<Sexpression::iterator> m_response{};
    };


    // Find all available s-expression in SDocument by given part of path.
    class FindByPartOfPath final {
    public:
        using iterator = std::vector<std::string>::iterator;

    public:
        explicit FindByPartOfPath(SData &root, std::vector<std::string> &paths) :
                m_doc(root),
                m_pos(paths.begin()),
                m_end(paths.end()) {}

    public:
        SData apply() noexcept;

    private:
        void findAllExpr(iterator pos, SNode& s) noexcept;

    private:
        SData &m_doc;
        iterator m_pos;
        iterator m_end;
        std::vector<Sexpression::iterator> m_response{};
    };
}