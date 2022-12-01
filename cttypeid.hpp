#ifndef CTTYPEID_HPP_
#define CTTYPEID_HPP_

#include <array>
#include <iostream>
#include <string_view>
#include <type_traits>

template <std::size_t...Indices>
constexpr auto substr_to_array(std::string_view str, std::index_sequence<Indices...>) noexcept
{
    return std::array { str[Indices]..., '\0' };
};

#if defined(__clang__)
#  define TEFLON_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define TEFLON_PRETTY_FUNCTION_START "[T = "
#  define TEFLON_PRETTY_FUNCTION_END "]\0"
#elif defined(__GNUC__) && defined(__GNUG__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#  define TEFLON_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define TEFLON_PRETTY_FUNCTION_START "[with T = "
#  define TEFLON_PRETTY_FUNCTION_END "]\0"
#elif defined(_MSC_VER)
#  define TEFLON_PRETTY_FUNCTION __FUNCSIG__
#  define TEFLON_PRETTY_FUNCTION_START "pretty_function_array<"
#  define TEFLON_PRETTY_FUNCTION_END ">(void)\0"
#else
#endif


template <typename T>
constexpr auto pretty_function_array() noexcept
{
    constexpr std::string_view name = std::string_view{ TEFLON_PRETTY_FUNCTION };
    constexpr std::string_view prefix = { TEFLON_PRETTY_FUNCTION_START };
    constexpr std::string_view suffix = { TEFLON_PRETTY_FUNCTION_END };

    constexpr std::size_t start = name.find(prefix) + prefix.size();
    constexpr std::size_t end = name.rfind(suffix);

    static_assert(start < end, "Invalid arguments.");

    constexpr std::string_view pname = name.substr(start, (end - start));
    return substr_to_array(pname, std::make_index_sequence<pname.size()>{});
};


template <typename T> 
struct prettyparse {
protected: static inline constexpr auto pretty_name = pretty_function_array<T>();
};

template <typename T>
struct cttypeid_impl : prettyparse<T>
{
    constexpr std::string_view name() const noexcept { return this->nameof; }
    friend std::ostream& operator << (std::ostream& os, const cttypeid_impl& id) { 
        return os << id.name(); 
    }
private:
    static constexpr auto pretty { prettyparse<T>::pretty_name };
    std::string_view nameof = { pretty.data(), pretty.size() };
};


template <typename T>
constexpr auto cttypeid { cttypeid_impl<T>{} };

#endif // CTTYPEID_HPP_
