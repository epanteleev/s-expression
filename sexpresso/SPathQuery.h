#pragma once

#include <string_view>
#include <functional>
#include <memory>
#include <cassert>

class SDocument;

class SPathResponce final {
public:

};

class SQuery {
public:
    using ptr = std::unique_ptr<SQuery>;

public:
    SQuery() = default;

    virtual ~SQuery() = default;

public:
    virtual SPathResponce apply(SDocument &) = 0;

public:
    static SQuery::ptr parse(std::string_view query);
};


class SQueryExpr : public SQuery {
public:
    explicit SQueryExpr(std::vector<std::string_view> &&path)
            : m_path(std::move(path)) {}

public:
    SPathResponce apply(SDocument &) override {
        assert(false);
        return {};
    }

private:
    std::vector<std::string_view> m_path;
};

