#ifndef CTTYPEID_HPP_
#define CTTYPEID_HPP_

#if defined(__clang__)
#  if defined(__ELLCC__)
#    define COMPILER_ELLCC "ellcc"
#  elif defined(__INTEL_LLVM_COMPILER)
#    define COMPILER_ICX "icx"
#  endif
#  define COMPILER_CLANG "clang"
#elif defined(__INTEL_COMPILER)
#  define COMPILER_ICC "icc"
#elif defined(__GNUC__)
#  if defined(__MINGW32__)
#    define COMPILER_MINGW "mingw"
#    define COMPILER_GCC "gcc"
#  elif defined(__NVCOMPILER)
#    define COMPILER_NVCPP "nvc++"
#  else
#    define COMPILER_GCC "gcc"
#  endif
#elif defined(_MSC_VER) || defined(_MSVC_LANG)
#  define COMPILER_MSVC "msvc"
#else
#  define COMPILER_UNKNOWN "unknown"
#  error Your compiler is currently unsupported!
#endif

#if defined(COMPILER_MSVC)
#  if !defined(_MSVC_LANG)
#    define COMPILER_CPP_VERSION __cplusplus
#  else
#    define COMPILER_CPP_VERSION _MSVC_LANG
#  endif
#else
#  define COMPILER_CPP_VERSION __cplusplus
#endif

#if defined(COMPILER_MSVC) && (COMPILER_CPP_VERSION < 202002L)
#  define not !
#  define INTROSPECT_NOT_COMPAT
#endif

#include <array>
#include <iostream>
#include <string_view>
#include <type_traits>

#if defined(COMPILER_CLANG)
#  define INTROSPECT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define INTROSPECT_PRETTY_FUNCTION_START "[T = "
#  define INTROSPECT_PRETTY_FUNCTION_END "]\0"
#elif defined(COMPILER_GCC) || defined(COMPILER_MINGW) || defined(COMPILER_ICC) || defined(COMPILER_NVCPP)
#  define INTROSPECT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define INTROSPECT_PRETTY_FUNCTION_START "[with T = "
#  define INTROSPECT_PRETTY_FUNCTION_END "]\0"
#elif defined(COMPILER_MSVC)
#  define INTROSPECT_PRETTY_FUNCTION __FUNCSIG__
#  define INTROSPECT_PRETTY_FUNCTION_START "pretty_function_array<"
#  define INTROSPECT_PRETTY_FUNCTION_END ">(void)\0"
#else
#  define INTROSPECT_PRETTY_FUNCTION " <null> "
#  define INTROSPECT_PRETTY_FUNCTION_START " "
#  define INTROSPECT_PRETTY_FUNCTION_END " \0"
#endif

template <std::size_t...NN>
constexpr auto substr_to_array(std::string_view str, std::index_sequence<NN...>) noexcept
{
    constexpr std::size_t count = sizeof...(NN) + 1;
    return std::array<char, count> { str[NN]..., '\0' };
};

template <typename T>
constexpr auto pretty_function_array() noexcept
{
    constexpr std::string_view name = std::string_view{ INTROSPECT_PRETTY_FUNCTION };
    constexpr std::string_view prefix = { INTROSPECT_PRETTY_FUNCTION_START };
    constexpr std::string_view suffix = { INTROSPECT_PRETTY_FUNCTION_END };

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
    std::string_view nameof { pretty.data(), pretty.size() - 1 };
};


template <typename T>
constexpr auto cttypeid { cttypeid_impl<T>{} };

#endif // CTTYPEID_HPP_
