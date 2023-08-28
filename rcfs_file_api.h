#pragma once


#include <string>
#include <vector>
#include <utility>

#include "encoding/encoding.h"

#include "umba/filename.h"
#include "marty_rcfs/rcfs.h"
#include "marty_rcfs/rcfs_enumerate.h"


namespace marty_fs_adapters {


// Вообще, подразумевалось, что StringType - это тип строки с именами файлов. Но как-то на это подзабилось


template<typename StringType>
struct RcfsFileApi
{

public:

    typedef StringType  string_type_t;

protected:

    marty_rcfs::ResourceFileSystem   *pRcfs;


public:

    RcfsFileApi() = delete;
    RcfsFileApi(marty_rcfs::ResourceFileSystem *p) : pRcfs(p) {}
    RcfsFileApi(const RcfsFileApi &other) : pRcfs(other.pRcfs) {}
    RcfsFileApi(RcfsFileApi &&other) : pRcfs(std::move(other.pRcfs)) {}

    RcfsFileApi& operator=(const RcfsFileApi &other)
    {
        pRcfs = other.pRcfs;
        return *this;
    }
    
    RcfsFileApi& operator=(RcfsFileApi &&other)
    {
        pRcfs = std::move(other.pRcfs);
        return *this;
    }


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
        return '/';
    }

    std::string appendPath(const std::string &path, const std::string &nameToAppend) const
    {
        return umba::filename::appendPath(path, nameToAppend, getPathSep());
    }

    std::string readFile( const std::string &fileName ) const
    {
        marty_rcfs::AutoFileHandle fh = pRcfs;
        if (!fh.open(fileName))
        {
            return std::string();
        }

        std::string fileText;
        if (!fh.read(fileText))
        {
            return std::string();
        }

        // return fileText;

        // По идее, в RCFS лежат файлы, уже перекодированные в UTF-8. Но, для надежности попробуем и тут, 
        // хуже не будет, и времени много не займёт

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
        throw std::runtime_error("Not implemented: writeFile not implemented for marty_fs_adapters::RcfsFileApi");
    }

    bool deleteFile( const std::string &fileName ) const
    {
        throw std::runtime_error("Not implemented: deleteFile not implemented for marty_fs_adapters::RcfsFileApi");
    }

    bool isExistAndReadable(const std::string &fileName) const
    {
        marty_rcfs::AutoFileHandle fh = pRcfs;
        return fh.open(fileName);
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
            pathSep = '/';
        }

        // bool EnumerateHandler(const std::string &dirPath, const marty_rcfs::FileInfo &fileInfo)
        // struct FileInfo
        // {
        //     FileAttrs   attrs = FileAttrs::FileAttrsDefault;
        //     std::string name;
        // };
        // enum class FileAttrs : std::uint32_t
        // {
        //     FileAttrsDefault        = 0,
        //     Directory               = 1,
        //     FlagDirectory           = 1,
        //     DirectoryAttrsDefault   = 1
        //  
        // }; // enum class FileAttrs : std::uint32_t


        auto proxyHandler = [&](const std::string &dirPath, const marty_rcfs::FileInfo &fileInfo)
        {
            bool isDir = (fileInfo.attrs & marty_rcfs::FileAttrs::FlagDirectory) != 0;

            return enumHandler(dirPath, fileInfo.name, isDir);
        };

        return marty_rcfs::enumerateDirectoryItems( pRcfs, path, proxyHandler, bRecursive );


#if 0
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
#endif

    
    }



}; // struct RcfsFileApi


} // namespace marty_fs_adapters

