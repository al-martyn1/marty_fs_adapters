#pragma once


#include <string>
#include <vector>


#include "encoding/encoding.h"

#include "umba/filename.h"
#include "umba/filesys.h"


namespace marty_fs_adapters {


// Вообще, подразумевалось, что StringType - это тип строки с именами файлов. Но как-то на это подзабилось


template<typename StringType>
struct SimpleFileApi
{
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

    bool isExistAndReadable(const std::string &fileName) const
    {
        return umba::filesys::isFileReadable( fileName );
        //return isFileReadableHelper(fileName);
    }


}; // struct SimpleFileApi


} // namespace marty_fs_adapters

