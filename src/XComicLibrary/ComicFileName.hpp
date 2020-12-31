/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICFILENAME_HPP
#define COMICFILENAME_HPP

#include <Xylasoft.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <XUtilities.hpp>

namespace ComicSeer
{
    /*!
    @brief The ComicName struct provides the information retrieved when parsing
    a file name.
    */
    class ComicFileName
    {
    public:
        /*!
        @brief Construct the comic file name information.
        @param pathOrFilename The path or file name string.
        */
        ComicFileName(const char* filename);

        /*!
        @brief Copy constructor.
        @param other The name to copy.
        */
        ComicFileName(const ComicFileName &other);

        /*!
        @brief Assignment.
        @param other The name to copy from.
        @return The local instance.
        */
        ComicFileName& operator=(const ComicFileName &other);

        /*! Get the original file name. */
        inline const char* FileName() const { return this->_filename.c_str(); }

        /*! Get the base file name after stripping attributes. */
        inline const char* BaseName() const { return this->_basename.c_str(); }

        /*! Get the simplified user visible name. */
        inline const char* PrettyName() const { return this->_prettyname.c_str(); }

        /*! Get the volume number in the name, NULL if none. */
        inline const int* VolumeNumber() const { return this->_volNumber.get(); }

        /*! Get the part number in the name, NULL if none. */
        inline const int* PartNumber() const { return this->_partNumber.get(); }

        /*! Get the issue number in the name, NULL if none. */
        inline const double* IssueNumber() const { return this->_issueNumber.get(); }

        /*! Get a comparison of another comic file to this one and see if it not the same. */
        bool operator<(const ComicFileName& other) const;

        /*!
        @brief Get the comic name regular expression.
        @return The regex string.
        */
        static const char *NameRegex();

    private:
        std::string _filename;
        std::string _basename;
        std::string _prettyname;
        boost::scoped_ptr<int> _volNumber;
        boost::scoped_ptr<int> _partNumber;
        boost::scoped_ptr<double> _issueNumber;

        const static std::string _regex;
    };
}

#endif // COMICFILENAME_HPP
