/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICSTATE_HPP
#define COMICSTATE_HPP

#include <Xylasoft.hpp>
#include <list>
#include <map>
#include <XIni.hpp>
#include "ImageState.hpp"
#include "ImageView.hpp"
#include <boost/signals2.hpp>
#include "PageNumberLocation.hpp"
#include <Bookmarkings.hpp>
#include <boost/smart_ptr.hpp>
#include <ComicLibrary.hpp>
#include <boost/tuple/tuple.hpp>

/*!
This manages the state of the comic environment.
*/
class ComicState
{
public:
    /*!
    Construct the state object.
    */
    ComicState();

    /*!
    Get the list of library directories.
    @returns The list of directories.
    */
    std::map<std::string, std::string> LibraryDirectories() const;

    /*!
    Add the specified library directory.
    @param[in] name The name of the library.
    @param[in] path The path to the directory.
    */
    void AddLibraryDirectory(const char* name, const char* path);

    /*!
    Remove the specified library directory.
    @param[in] name The name of the library.
    */
    void RemoveLibraryDirectory(const char* name);

    /*!
    @brief Refresh the content of the specified library.
    @param name The library name.
    */
    void RefreshLibrary(const char *name);

    /*!
    @brief Get the specified comic library.
    @param name The name of the library.
    @return const ComicLibrary The library or NULL if not found.
    */
    const ComicSeer::ComicLibrary* Library(const char *name) const;

    /*!
    @brief Get the number of comics in all libraries.
    @return The total number of comics.
    */
    int NumberOfComicsInLibraries();

    /*!
    Store the state settings to the INI.
    @param[in] ini The INI to write to.
    */
    void StoreState(ComicSeer::XIni& ini) const;

    /*!
    Load the state settings from the INI.
    @param[in] ini The INI to read from.
    */
    void LoadState(ComicSeer::XIni& ini);

    /*!
    Reinitialize the comic state to initial state.
    */
    void Reinitialize(bool clearLibraries = true);

    /*!
    @brief Get the set of user bookmarks. Loads from the file if
    not already loaded.
    @return ComicSeer::Bookmarkings The current bookmarks.
    */
    ComicSeer::Bookmarkings& Bookmarks();

    /*!
    @brief Reload the bookmarks from the user's file.
    */
    void ReloadBookmarks();

    /*!
    @brief Store the current bookmark set.
    */
    void StoreBookmarks();

    /*!
    @brief Signal to notify when background color has changed.
    */
    boost::signals2::signal<void (const char* color)> BackgroundColorUpdated;

    /*!
    @brief Signal to notify when the thumbnail size has changed.
    */
    boost::signals2::signal<void (int size)> ThumbnailSizeUpdated;

    /*!
    @brief Signal to notify when the page numbering scheme has changed.
    */
    boost::signals2::signal<void ()> PageNumberingUpdated;

    /*!
    @brief Signal to notify that path visibility has changed.
    */
    boost::signals2::signal<void (bool show)> ShowFullPathUpdated;

public:
    /*! Get the default image state. */
    ComicSeer::ImageState DefaultImageState() const;

    /*! Set the default image state. */
    void SetDefaultImageState(ComicSeer::ImageState state);

    /*! Get the default image view. */
    ComicSeer::ImageView DefaultImageView() const;

    /*! Set the default image view. */
    void SetDefaultImageView(ComicSeer::ImageView view);

    /*! Get the scaled view zoom. */
    double ScaledZoomFactor() const;

    /*! Set the scaled view zoom. */
    void SetScaledZoom(double value);

    /*! Get the two-page view number. */
    int TwoPageMoveNumber() const;

    /*! Set the two-page view number. */
    void SetTwoPageMoveNumber(int number);

    /*! Set the background color string. */
    void SetBackgroundColor(const char* color);

    /*! Get the background color string. */
    std::string BackgroundColor() const;

    /*! Get whether to auto-detect wide pages in two-page view. */
    bool TwoPageAutodetectWidePage() const;

    /*! Set whether to auto-detect wide pages in two-page view. */
    void SetTwoPageAutodetectWidePage(bool autodetect);

    /*! Get whether to reopen the last session on restart. */
    bool ReopenLastSession() const;

    /*! Set whether to reopen the last session on restart. */
    void SetReopenLastSession(bool reopen);

    /*! Get whether to show the full file path. */
    bool ShowFullFilePath() const;

    /*! Set whether to show the full file path. */
    void SetShowFullFilePath(bool show);

    /*! Get the thumbnail size. */
    int ThumbnailSize() const;

    /*! Set the thumbnail size. */
    void SetThumbnailSize(int size);

    /*! Get whether to show page numbers. */
    bool ShowPageNumbers() const;

    /*! Get the page number location on a page. */
    ComicSeer::PageNumberLocation PageNumberLocation() const;

    /*! Get the page number color. */
    std::string PageNumberColor() const;

    /*!
    @brief Set page number settings.
    @param show Whether to show page numbers.
    @param location The location to show page numbers on a page.
    @param color The color of the page numbers.
    */
    void SetPageNumberSettings(bool show, ComicSeer::PageNumberLocation location = ComicSeer::Bottom, const char *color = "#006600");

    /*!
    @brief Get the degree of magnification for the magnifier.
    @return The magnification (1.0+).
    */
    double MagnifierMagnification() const;

    /*! Get the comic tagger executable path. */
    std::string ComicTaggerPath() const;

    /*! Set the comic tagger executable path. */
    void SetComicTaggerPath(const std::string &path);

    /*! Get whether to allow magnification. */
    bool AllowMagnification() const;

    /*! Set whether to allow magnification. */
    void SetAllowMagnification(bool allow);

public:
    /*!
    Get the static state of the environment. Creates one if it doesn't exist.
    @returns The state.
    */
    static ComicState* State();

private:
    static ComicState* _instance;
    std::map<std::string, boost::shared_ptr<ComicSeer::ComicLibrary>> _libraries;
    ComicSeer::ImageView _defaultView;
    ComicSeer::ImageState _defaultState;
    double _scaledZoom;
    bool _showFilePath;
    int _twoPageMoveNumber;
    std::string _colorString;
    bool _twoPageAutodetectWide;
    int _thumbnailSize;
    bool _reopenLastSession;
    bool _showPageNumbers;
    ComicSeer::PageNumberLocation _pageNumberLocation;
    std::string _pageNumberColorString;
    std::string _comicTaggerPath;
    bool _allowMagnification;
    boost::scoped_ptr<ComicSeer::Bookmarkings> _bookmarks;
};

#endif
