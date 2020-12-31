/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifdef  PDFSUPPORT
#ifndef PDFCOMICFILE_HPP
#define PDFCOMICFILE_HPP

#include <Xylasoft.hpp>
#include <vector>
#include <boost/process.hpp>
#include <XByteArray.hpp>
#include "ComicFile.hpp"

namespace ComicSeer
{
    /*!
    This implementation of the comic file supports reading from PDF format files.
    */
    class PdfComicFile : public ComicFile
    {
    public:
        /*!
        Constructor for PDF comic file.
        @param[in] filepath The path to the comic file.
       */
        PdfComicFile(const char* filepath);

        /*!
        @brief Validate that the specified comic file format is supported.
        throws ComicFileFormatUnsupportedException If unsupported.
        */
        static void ValidateSupported();

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

        static std::string GetGhostscriptLocation();

        static void ReadProcStream(boost::process::pistream& stream, ComicSeer::XByteArray &buffer);

        static void ExecuteGhostScript(std::vector<std::string> &args, ComicSeer::XByteArray& out);
    };
}

#endif // PDFCOMICFILE_HPP
#endif
