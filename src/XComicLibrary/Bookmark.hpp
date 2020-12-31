/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef BOOKMARK_HPP
#define BOOKMARK_HPP

#include "Xylasoft.hpp"
#include "pugixml.hpp"
#include "ComicFileName.hpp"
#include <boost/uuid/uuid.hpp>
#include <ctime>

namespace ComicSeer
{
    /*!
    @brief The Bookmark class contains information about an individual
    bookmark and its associated properties.
    */
    class Bookmark
    {
    public:
        /*!
        @brief Construct an old bookmark.
        @param id The identifier of the mark.
        @param path The path to the associated file.
        @param[in] created The time the mark was created.
        @param page The page to bookmark.
        @param description The description for the bookmark.
        @param notes The notes for the bookmark.
        */
        Bookmark(boost::uuids::uuid id, const char *path, int page, time_t created, const char *description = NULL, const char *notes = NULL);

        /*!
        @brief Construct a bookmark.
        @param path The path to the associated file.
        @param page The page to bookmark.
        @param description The description for the bookmark.
        @param notes The notes for the bookmark.
        */
        Bookmark(const char *path, int page, const char *description = NULL, const char *notes = NULL);

        /*!
        @brief Copy constructor.
        @param other The bookmark to copy.
        */
        Bookmark(const Bookmark &other);

        /*! Get the identifier for the bookmark. */
        boost::uuids::uuid Identifier() const;

        /*! Get the comic name. */
        ComicFileName Name() const;

        /*! Get the path to the bookmarked file. */
        const char* Path() const;

        /*! Get the bookmarked page. */
        int Page() const;

        /*! Get the time the bookmark was created. */
        time_t Created() const;

        /*! Get the description. */
        const char* Description() const;

        /*!
        @brief Set the description.
        @param description The description to set.
        */
        void SetDescription(const char *description);

        /*! Get the notes. */
        const char* Notes() const;

        /*!
        @brief Set the notes.
        @param notes The notes to set.
        */
        void SetNotes(const char *notes);

    private:
        boost::uuids::uuid _id;
        ComicFileName _name;
        std::string _path;
        int _page;
        std::string _notes;
        std::string _description;
        time_t _created;
    };
}
#endif // BOOKMARK_HPP
