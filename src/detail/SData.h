#pragma once
#include <variant>

class SDocument;

namespace detail {
    using SPathResponse = std::vector<Sexpression::iterator>;

    class SData final {
    public:
        SData() : m_data(SPathResponse()) {}

        explicit SData(SDocument* doc) : m_data(doc) {}

        explicit SData(std::vector<Sexpression::iterator>&& data) : m_data(data) {}

    public:
        template<typename Type>
        inline constexpr decltype(auto) get() {
            return std::get<Type>(m_data);
        }

        template<typename Type>
        inline constexpr Type* getIf() {
            return std::get_if<Type>(&m_data);
        }

        template<typename Type>
        inline constexpr bool hold() noexcept {
            return std::holds_alternative<Type>(m_data);
        }
    private:
        std::variant<std::vector<Sexpression::iterator>, SDocument*> m_data;
    };
}