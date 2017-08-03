#ifndef __STL_EXTENSION__
#define __STL_EXTENSION__


#undef NDEBUG
#include <list>
#include <vector>
#include <assert.h>
#include <type_traits>
#include <string>
#include <memory>
#include <stdio.h>
#include <memory>
#include <stddef.h>
#include <boost/optional.hpp>
#include <type_traits>
#include "Qt-Utils/json.hpp"
#include "Qt-Utils/tinyxml2.h"
#include <chrono>
#include <iso646.h>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

constexpr unsigned long long qt_utils_version = 20160801L;


using boost::none;
using boost::optional;
using namespace std::string_literals;
using namespace std::chrono_literals;

using std::unique_ptr;
using std::to_string;
using std::shared_ptr;
using std::weak_ptr;
using std::make_unique;
using std::make_shared;
using std::string;
using std::vector;
using std::pair;




/*---------------------------------------error handle----------------------------------------------*/






/*--------------------------------------------end--------------------------------------------------*/

namespace krys
{

template<typename T>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<std::vector<T>> : std::true_type {};

template<typename T>
struct is_basic_string : std::false_type {};

template<typename T>
struct is_basic_string<std::basic_string<T>> : std::true_type {};

template<typename T>
struct is_std_array : std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename T>
struct is_buffer
{
    static constexpr bool value = (is_vector<T>::value or is_basic_string<T>::value or is_std_array<T>::value) and std::is_pod<typename T::value_type>::value;
};


/*improvement for C style file IO*/

struct fclose_deleter
{
    void operator () (FILE* fp)
    {
        fclose (fp);
    }
};

using file_ptr = std::unique_ptr<FILE, fclose_deleter>;

inline file_ptr file_open (const char* filename, const char* modes) noexcept
{
    return file_ptr{::fopen (filename, modes)};
}


template<typename T = string>
inline optional<T> read_all (const string& filename)
{
    static_assert (is_vector<T>::value or is_basic_string<T>::value, "类型错误,非法缓冲区类型");
    using data_type = typename std::decay<T>::type ::value_type;
    static_assert (std::is_pod<data_type>::value, "值类型不是POD类型");

    file_ptr fp = file_open (filename.data (), "rb");
    if (fp == nullptr)
    {
        return none;
    }

    T buffer_ret;

    if (::fseek (fp.get(), 0, SEEK_END) != 0)
    {
        return none;
    }

    signed length = ftell (fp.get());
    if (length == -1)
    {
        return none;
    }

    if (::fseek (fp.get(), 0, SEEK_SET) != 0)
    {
        return none;
    }

    buffer_ret.resize (length / sizeof (typename T::value_type));

    if (::fread (static_cast<void*> (const_cast<data_type*>(buffer_ret.data())),
                 sizeof (data_type), buffer_ret.size (), fp.get()) != static_cast<unsigned>(buffer_ret.size ()))
    {
        return none;
    }
    return buffer_ret;
}

enum class append_flag
{
    yes, no
};

template<typename T = const string>
inline bool write_buffer (const string& filename,  T&& buffer, append_flag flag = append_flag::no)
{
    static_assert (is_buffer<typename std::decay<T>::type>::value, "buffer type error");
    using data_type = typename std::decay<T>::type ::value_type;

    file_ptr fp = file_open (filename.data (), flag == append_flag::yes ? "ab+" : "wb");
    if (fp == nullptr) return false;

    if (::fwrite (static_cast<void*>(const_cast<data_type*> (buffer.data())), sizeof (data_type) , buffer.size(), fp.get()) != static_cast<unsigned>(buffer.size()))
    {
        return false;
    }

    return true;
}


}

#endif
