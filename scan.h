/* \file
   \brief Scan filesystem for files 
 */

#pragma once

#include <string>
#include <vector>

#include "umba/filename.h"



namespace marty_fs_adapters {


template<typename FileSysApi>
std::vector<typename FileSysApi::string_type_t> scanFilesystem( const FileSysApi &fsApi
                                      , const typename FileSysApi::string_type_t &rootPath
                                      , bool bRecurse
                                      , int dirFileOrAny // -1 - any, 0 - file, >0 - dir
                                      )
{
    std::vector<typename FileSysApi::string_type_t> resVec;

    auto fsEnumHandler = [&](const typename FileSysApi::string_type_t& path, const typename FileSysApi::string_type_t& entryName, bool fDirectory)
    {
        if (dirFileOrAny<0 || fDirectory==(dirFileOrAny?true:false))
        {
            resVec.emplace_back(fsApi.appendPath(path, entryName));
        }

        return true;
    };

    fsApi.enumerateDirectoryEntries( fsEnumHandler, rootPath, bRecurse );

    return resVec;

}


template<typename FileSysApi>
std::vector<typename FileSysApi::string_type_t> scanFilesystem( const FileSysApi &fsApi
                                      , const std::vector<typename FileSysApi::string_type_t> &rootPaths
                                      , bool bRecurse
                                      , int dirFileOrAny // -1 - any, 0 - file, >0 - dir
                                      )
{
    std::vector<typename FileSysApi::string_type_t> resVec;

    for(const auto &rootPath : rootPaths)
    {
        auto tmpRes = scanFilesystem(fsApi, rootPath, bRecurse, dirFileOrAny);
        resVec.insert(resVec.end(), tmpRes.begin(), tmpRes.end());
    }

    return resVec;
}


} // namespace marty_fs_adapters {



