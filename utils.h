/* \file
   \brief Utils for keyboard identification
 */

#pragma once

#include <string>
#include <vector>
// #include <map>

// #include "enums.h"
// #include "marty_cpp/marty_cpp.h"


namespace marty_fs_adapters {


namespace utils {

template<typename ActualPred, typename ParamType>
struct not_pred
{
    const ActualPred &actualPred; //!< Ссылка на предикат для отрицания его результата
    //! Конструктор предиката отрицания
    not_pred( const ActualPred &a /*!< предикат для отрицания */ ) : actualPred(a) {}
    //! Оператор функционального объекта
    bool operator()( ParamType ch ) const { return !actualPred(ch); }

    // not_pred(const not_pred&) = delete;
    // not_pred(not_pred&&) = delete;
    // not_pred& operator=(const not_pred&) = delete;
    // not_pred& operator=(not_pred&&) = delete;

};

template <typename StringType> inline bool starts_with( const StringType &str, const StringType &prefix )
{
    if (str.size()<prefix.size())
        return false;

    return str.compare( 0, prefix.size(), prefix )==0;
}

template <typename StringType, typename TrimPred> inline void ltrim( StringType &s, const TrimPred &pred )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_pred<TrimPred, typename StringType::value_type>(pred) ));
}

//-----------------------------------------------------------------------------
template <typename StringType, typename TrimPred > inline void rtrim(StringType &s, const TrimPred &pred)
{
    s.erase( std::find_if(s.rbegin(), s.rend(), not_pred<TrimPred,typename StringType::value_type>(pred) ).base(), s.end());
}

//-----------------------------------------------------------------------------
template <typename StringType, typename TrimPred> inline void trim(StringType &s, const TrimPred &pred)
{
    ltrim(s,pred); rtrim(s,pred);
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
inline bool    is_lower( char ch )     { return (ch>='a' && ch<='z'); }
inline bool    is_upper( char ch )     { return (ch>='A' && ch<='Z'); }

inline bool    is_lower( wchar_t ch )  { return (ch>=L'a' && ch<=L'z'); }
inline bool    is_upper( wchar_t ch )  { return (ch>=L'A' && ch<=L'Z'); }

inline char    to_lower( char ch )     { return is_upper(ch) ? ch-'A'+'a' : ch; }
inline char    to_upper( char ch )     { return is_lower(ch) ? ch-'a'+'A' : ch; }

inline wchar_t to_lower( wchar_t ch )  { return (wchar_t)(is_upper(ch) ? ch-L'A'+L'a' : ch); }
inline wchar_t to_upper( wchar_t ch )  { return (wchar_t)(is_lower(ch) ? ch-L'a'+L'A' : ch); }

template< class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT> >
inline std::basic_string< CharT, Traits, Allocator >
to_lower( const std::basic_string< CharT, Traits, Allocator > &str )
{
    std::basic_string< CharT, Traits, Allocator > resStr; resStr.reserve(str.size());
    for( auto it = str.begin(); it != str.end(); ++it )
    {
        resStr.append( 1, to_lower(*it) );
    }

    return resStr;
}

template< class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT> >
inline std::basic_string< CharT, Traits, Allocator >
to_upper( const std::basic_string< CharT, Traits, Allocator > &str )
{
    std::basic_string< CharT, Traits, Allocator > resStr; resStr.reserve(str.size());
    for( auto it = str.begin(); it != str.end(); ++it )
    {
        resStr.append( 1, to_upper(*it) );
    }

    return resStr;
}


} // namespace utils


} // namespace marty_fs_adapters

