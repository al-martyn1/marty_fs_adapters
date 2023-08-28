#pragma once


#include <string>
#include <vector>
#include <filesystem>
#include <list>
#include <deque>


#include "encoding/encoding.h"

#include "umba/filename.h"
#include "umba/filesys.h"


namespace marty_fs_adapters {


// Вообще, подразумевалось, что StringType - это тип строки с именами файлов. Но как-то на это подзабилось


template<typename StringType>
struct SimpleFileApi
{

public:

    typedef StringType  string_type_t;


public:

    std::string getFilePath(const std::string &fileName) const
    {
        return umba::filename::getPath(fileName);
    }

    std::string getFileName(const std::string &fileName) const
    {
        return umba::filename::getFileName(fileName);
    }

    char getPathSep() const
    {
        return umba::filename::getNativePathSep<char>();
    }

    std::string appendPath(const std::string &path, const std::string &nameToAppend) const
    {
        return umba::filename::appendPath(path, nameToAppend, getPathSep());
    }

    std::string readFile( const std::string &fileName ) const
    {
        //std::string fileText = readFileHelper( fileName );

        std::string fileText;
        if (!umba::filesys::readFile(fileName, fileText))
        {
            return std::string();
        }

        encoding::EncodingsApi* pApi = encoding::getEncodingsApi();

        size_t bomSize = 0;
        std::string detectRes = pApi->detect( fileText, bomSize );

        if (bomSize)
            fileText.erase(0,bomSize);

        auto cpId = pApi->getCodePageByName(detectRes);

        return pApi->convert( fileText, cpId, encoding::EncodingsApi::cpid_UTF8 );
    }

    bool writeFile( const std::string &fileName, const std::string &data ) const
    {
        return umba::filesys::writeFile( fileName, data, true /* bOverwrite */ );
    }

    bool deleteFile( const std::string &fileName ) const
    {
        return umba::filesys::deleteFile( fileName );
    }

    bool isExistAndReadable(const std::string &fileName) const
    {
        return umba::filesys::isFileReadable( fileName );
        //return isFileReadableHelper(fileName);
    }


    // bool handler(const std::string& path, const std::string& fileName, bool fDirectory)

    template<typename EnumHandler>
    bool enumerateDirectoryEntries( const EnumHandler &enumHandler
                                  , const std::string& path
                                  , bool  bRecursive
                                  , char  pathSep=0 // separator for paths passed to handler
                                  ) const
    {
        if (!pathSep)
        {
            pathSep = umba::filename::getNativePathSep<char>();
        }

        std::deque<std::string> pathList;

        pathList.push_back(umba::filename::stripLastPathSepCopy(umba::filename::normalizePathSeparators(path)));

        namespace fs = std::filesystem;

        bool bFirstPath = true;

        while(!pathList.empty())
        {
            auto curPath = pathList.front();
            pathList.pop_front();

            auto curPathNormalizedSeps = umba::filename::normalizePathSeparators( curPath, pathSep );

            fs::directory_iterator scanPathDirectoryIterator;
            try
            {
                scanPathDirectoryIterator = fs::directory_iterator(curPath);
            }
            catch(...)
            {
                if (bFirstPath)
                {
                    return false; // Возвращаем фалсю только если стартовый путь был некорректным
                }

                continue;
            }

            bFirstPath = false;
            //enumHandler


            for (const auto & entry : scanPathDirectoryIterator)
            {
                // https://en.cppreference.com/w/cpp/filesystem/directory_entry
    
                if (!entry.exists())
                    continue;
    
                std::string entryName = umba::filename::getFileName(entry.path().string());
                if (entryName=="." || entryName=="..") // Пропускаем ссылки на текущий и родительский каталоги
                {
                    continue;
                }
    
                if (entry.is_directory())
                {
                    if (bRecursive)
                    {
                        pathList.push_back(appendPath(curPath, entryName));
                    }

                    if (!enumHandler(curPathNormalizedSeps, entryName, true)) // pass directory entry to handler
                    {
                        return true; // Хоть и досрочно завершилось, но стартовый путь-то был нормасик
                    }

                    continue;
                }
    
                //------------------------------
                if (!entry.is_regular_file())
                {
                    continue; // Какая-то шляпа попалась
                }
    
                //------------------------------
                if (!enumHandler(curPathNormalizedSeps, entryName, false)) // pass file entry to handler
                {
                    return true; // Хоть и досрочно завершилось, но стартовый путь-то был нормасик
                }
                
            } // for (const auto & entry : scanPathDirectoryIterator)

        } // while(!pathList.empty())

        return true;
    
    }


}; // struct SimpleFileApi


} // namespace marty_fs_adapters

