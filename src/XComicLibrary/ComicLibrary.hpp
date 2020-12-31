/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef LIBRARYCONTAINER_HPP
#define LIBRARYCONTAINER_HPP

#include <Xylasoft.hpp>
#include "ComicLibraryItem.hpp"
#include <vector>
#include <map>
#include <boost/smart_ptr.hpp>

namespace ComicSeer
{
    /*!
    @brief The ComicLibrary class manages a library of comics. It provides tools for
    retrieval of comics in multiple ways.
    */
    class ComicLibrary
    {
    public:
        /*!
        @brief Construct a library from the specified path.
        @param path The path to the library.
        */
        ComicLibrary(const char* path);

        /*!
        @brief Refresh the library from the source path.
        */
        void RefreshLibrary();

        /*!
        @brief Get the number of comics in the library.
        */
        long GetComicNumber() const;

        /*!
        @brief Get the library path.
        @return The path.
        */
        const char *Path() const;

    public:
        /*!
        @brief The ComicFolder struct contains information about a particular
        folder within a library.
        */
        struct ComicFolder
        {
            /*! The name of the folder. */
            std::string name;

            /*! The path to the folder. */
            std::string path;

            /*! The list of folders within the folder. */
            std::vector<ComicFolder> Folders;

            /*! The list of comics within in the folder. */
            std::vector<boost::shared_ptr<ComicLibraryItem> > Comics;
        };

    public:
        /*!
        @brief Get the library by folder (physical) structure.
        */
        const ComicFolder& GetLibraryComics() const;

        /*!
        @brief Get the complete list of comics as a flat list.
        @return The list of comics.
        */
        const std::vector<boost::shared_ptr<ComicLibraryItem> > &GetCompleteComicList() const;

        /*!
        @brief Get the list of comics grouped by their 'base name' (effective series).
        @return The grouped list of comics.
        */
        const std::vector<ComicLibrary::ComicFolder> GetGroupedComicList() const;

        /*!
        @brief Get the complete list of comics ordered by most recently modified.
        @return The list of comics.
        */
        const std::vector<boost::shared_ptr<ComicLibraryItem> > &GetModifiedTimeComicList() const;

    protected:
        /*!
        @brief Parse the specified folder (recursive).
        @param folder The folder to parse.
        */
        void ParseFolder(ComicFolder &folder);

    private:
        std::string _path;
        ComicFolder _folder;
        long _itemNumber;
        std::vector<boost::shared_ptr<ComicLibraryItem> > _flatComicList;
        std::vector<boost::shared_ptr<ComicLibraryItem> > _recentComicList;
    };
}

#endif // LIBRARYCONTAINER_HPP
