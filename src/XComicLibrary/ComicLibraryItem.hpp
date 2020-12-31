/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef LIBRARYITEM_HPP
#define LIBRARYITEM_HPP

#include <Xylasoft.hpp>
#include <ComicFileName.hpp>
#include <ComicFile.hpp>
#include <boost/shared_ptr.hpp>
#include <time.h>
#include <XUtilities.hpp>

namespace ComicSeer
{
    /*!
    @brief A comic library item contains information about an individual comic and the creation of a comic file.
    */
    class ComicLibraryItem : public ComicFileName
    {
    public:
        /*!
        @brief Contruct the comic library item.
        @param path The path to the file.
        */
        ComicLibraryItem(const char* path);

        /*!
        @brief Construct the comic library item from a file description.
        @param fileDesc The file description.
        */
        ComicLibraryItem(const XUtilities::XFileDesc& fileDesc);

        /*!
        @brief The file type of the current comic file.
        @return The file tyoe.
        */
        inline ComicFile::ComicFileType FileType() const
        {
            return ComicFile::ClassifyFileExtension(this->_fileext.c_str());
        }

        /*!
        @brief Get the path to the file.
        @return The file path.
        */
        inline const char* Path() const
        {
            return this->_filepath.c_str();
        }

        /*!
        @brief Get the modification date/time.
        @return The modification date/time.
       */
        inline time_t ModificationDate() const
        {
            return _modDateTime;
        }

        /*!
        @brief Create a comic file from the specified comic path.
        @return The comic file object.
        */
        inline boost::shared_ptr<ComicFile> CreateComicFile()
        {
            return boost::shared_ptr<ComicFile>(ComicFile::CreateComicFile(this->_filepath.c_str()));
        }

    private:
        std::string _filepath;
        std::string _fileext;
        time_t _modDateTime;
    };
}

#endif // LIBRARYITEM_HPP
