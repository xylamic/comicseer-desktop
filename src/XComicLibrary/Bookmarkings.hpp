/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef BOOKMARKINGS_HPP
#define BOOKMARKINGS_HPP

#include "Xylasoft.hpp"
#include "pugixml.hpp"
#include "Bookmark.hpp"
#include <map>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace ComicSeer
{
    /*!
    @brief The Bookmarkings class manages the bookmarks within the application.
    */
    class Bookmarkings
    {
    public:
        /*!
        @brief Construct a new bookmarkings.
        */
        Bookmarkings();

        /*!
        @brief Add a bookmark to the list.
        @param mark The mark to add.
        */
        void AddBookmark(const Bookmark& mark);

        /*!
        @brief Remove the bookmark.
        @param id The identifier of the bookmark to add.
        */
        void RemoveBookmark(boost::uuids::uuid id);

        /*!
        @brief Load bookmarks from the specified file.
        @param path The path to the file to load.
        */
        void Load(const char *path);

        /*!
        @brief Store bookmarks to the specified file.
        @param path The path to store the file to.
        */
        void Store(const char *path);

        /*!
        @brief Get the specific bookmark.
        @param id The ID of the bookmark.
        @return The bookmark if found, NULL othersise.
        */
        const Bookmark* BookmarkById(boost::uuids::uuid id) const;

        /*!
           \brief Update the bookmark's description and notes.
           \param id The bookmark identifier.
           \param desc The new description.
           \param notes The new notes.
         */
        void UpdateBookmarkDescriptionNotes(boost::uuids::uuid id,
                                            const char *desc, const char *notes);

        /*!
        @brief Update the bookmark's file path.
        @param id The bookmark identifier.
        @param path The updated path.
        */
        void UpdateBookmarkPath(boost::uuids::uuid id, const char *path);

        /*!
        @brief Get the list of booksmarks.
        @return The list of bookmarks.
        */
        const std::vector<const Bookmark *> Bookmarks() const;

    private:
        std::map<boost::uuids::uuid, boost::shared_ptr<Bookmark>> _marks;
    };
}
#endif // BOOKMARKINGS_HPP
