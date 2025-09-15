#pragma once

#include <boost/pfr.hpp>

template <typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

// Get param hash
template <typename Param>
constexpr size_t GetParamHash(const Param& param) {
    size_t hash = 0xcbf29ce484222325; // FNV-1a
    boost::pfr::for_each_field(param,
        [&] <typename Member>(const Member & member) {
            hash ^= std::hash<std::decay_t<Member>>{}(member);
            hash *= 0x100000001b3;  // FNV-1a
    });

    return hash;
}

template<typename Param>
concept ParamWithHasher = requires(Param param) {
    param.GetHash();
};

template <ParamWithHasher Param>
constexpr size_t GetParamHash() {
    return Param{}.GetHash();
}

// Get default param hash
template <typename Param>
constexpr size_t GetParamHash() {
    return GetParamHash(Param{});
}

template <size_t N>
constexpr size_t GetStringHash(const char(&arr)[N]) {
    return std::hash<std::string_view>{}(std::string_view(arr, std::strlen(arr)));
}
