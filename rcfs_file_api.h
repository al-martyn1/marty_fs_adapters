#pragma once


#include <string>
#include <vector>
#include <utility>

#include "encoding/encoding.h"

#include "umba/filename.h"
#include "marty_rcfs/rcfs.h"


namespace marty_fs_adapters {


// Вообще, подразумевалось, что StringType - это тип строки с именами файлов. Но как-то на это подзабилось


template<typename StringType>
struct RcfsFileApi
{

protected:

    ResourceFileSystem    *pRcfs;


public:

    RcfsFileApi() = delete;
    RcfsFileApi(ResourceFileSystem *p) : pRcfs(p) {}
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
        return umba::::filename::getFileName(fileName);
    }

    std::string appendPath(const std::string &path, const std::string &nameToAppend) const
    {
        return umba::filename::appendPath(path, nameToAppend);
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

    bool isExistAndReadable(const std::string &fileName) const
    {
        marty_rcfs::AutoFileHandle fh = pRcfs;
        return fh.open(fileName);
    }


}; // struct RcfsFileApi


} // namespace marty_fs_adapters

