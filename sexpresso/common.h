#pragma once

template<typename ... Args>
std::string format(const std::string &format, Args ... args) noexcept {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...);
    assert(size_s > 0);
    auto size = static_cast<std::size_t>(size_s) + 1;
    std::vector<char> buf(size);
    std::snprintf(buf.data(), size, format.c_str(), args ...);
    return std::string{buf.begin(), buf.end()};
}

