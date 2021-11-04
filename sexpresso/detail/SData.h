#pragma once

class SDocument;

namespace detail {
    using SPathResponse = std::vector<Sexpression::iterator>;

    class SData final {
    public:
        SData() = default;

        explicit SData(SDocument* doc) : m_data(doc) {}

        explicit SData(std::vector<Sexpression::iterator>&& data) : m_data(data) {}

    public:
        template<typename Type>
        inline constexpr decltype(auto) get() {
            return std::get<Type>(m_data);
        }
    private:
        std::variant<std::vector<Sexpression::iterator>, SDocument*> m_data;
    };
}