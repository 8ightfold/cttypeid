#ifndef CTTYPEID_HPP_
#define CTTYPEID_HPP_

#include <iostream>

#if defined(__clang__)
#  define TEFLON_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define TEFLON_PRETTY_FUNCTION_START "Pointer_wrapper pretty_function_array() [Ty = "
#  define TEFLON_PRETTY_FUNCTION_END "]\0"
#elif defined(__GNUC__) && defined(__GNUG__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#  define TEFLON_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define TEFLON_PRETTY_FUNCTION_START "Pointer_wrapper pretty_function_array() [with Ty = "
#  define TEFLON_PRETTY_FUNCTION_END "]\0"
#elif defined(_MSC_VER)
#  define TEFLON_PRETTY_FUNCTION __FUNCSIG__
#  define TEFLON_PRETTY_FUNCTION_START "struct Pointer_wrapper __cdecl pretty_function_array<"
#  define TEFLON_PRETTY_FUNCTION_END ">(void)\0"
#else
#endif

struct Pointer_wrapper {
    const char* data;
    std::size_t count;
    constexpr const char* get_offset() const { return data; }
    constexpr std::size_t get_count() const { return count; }
};

template <typename Ty>
Pointer_wrapper pretty_function_array()
{
    return Pointer_wrapper { 
        TEFLON_PRETTY_FUNCTION + sizeof(TEFLON_PRETTY_FUNCTION_START) - 1, 
        sizeof(TEFLON_PRETTY_FUNCTION) - 
        sizeof(TEFLON_PRETTY_FUNCTION_START) -
        sizeof(TEFLON_PRETTY_FUNCTION_END) + 2
    };
}

template <typename Ty>
struct cttypeid
{
    constexpr cttypeid() {}
    std::string name() const { 
        return std::string { pretty_name.get_offset(), pretty_name.get_count() }; 
    }
    friend std::ostream& operator << (std::ostream& os, const cttypeid id) { return os << id.name(); }

private:
    decltype(pretty_function_array<Ty>()) pretty_name = pretty_function_array<Ty>();
};

#endif // CTTYPEID_HPP_
