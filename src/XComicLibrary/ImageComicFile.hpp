/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef IMAGECOMICFILE_HPP
#define IMAGECOMICFILE_HPP

#include <Xylasoft.hpp>
#include "ComicFile.hpp"

namespace ComicSeer
{
    /*!
    This implementation of the comic file supports access to individual image files.
    */
    class ImageComicFile : public ComicFile
    {
    public:
        /*!
        Constructor for CBZ comic file.
        @param[in] filepath The path to the comic file.
       */
        ImageComicFile(const char* filepath);

        /*!
        @brief Get the comic file type.
        @return The file type.
        */
        virtual ComicFileType GetFileType() const;

    protected:
        /*!
        Get the list of files within the comic file.
        @returns The list of file names.
       */
        virtual std::list<std::string> GetFileList() const;

        /*!
        Read the specified file.
        @param[in] name The name of the file.
        @param[out] data The array to write the content to.
       */
        virtual void ReadFile(const char* name, XByteArray& data) const;
    };
}

#endif // IMAGECOMICFILE_HPP
