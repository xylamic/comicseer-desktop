/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICUTILITIES_H
#define X_COMICUTILITIES_H

#include <Xylasoft.hpp>
#include <ComicState.h>
#include <XIni.hpp>
#include <qsettings.h>
#include <QKeyEvent>
#include "CommandEnums.h"
#include <qcolor.h>
#include <qpixmap.h>
#include <XByteArray.hpp>
#include <ComicPageTypes.hpp>
#include "qtsingleapplication.h"
#include <ComicFile.hpp>

/*!
brief The global application instance.
 */
extern QtSingleApplication *ApplicationInstance;

/*!
Support of the comic application.
*/
class ComicUtils
{
public:
	/*!
	Load the state of the application.
	*/
	static void LoadState();

	/*!
	Store the state of the application.
	*/
	static void StoreState();

	/*!
	Get the application name.
	*/
	static const char* ApplicationName;

	/*!
	Get the company name.
	*/
	static const char* CompanyName;

	/*!
	Get the version number of the current application.
	*/
	static const int VersionNumber;

    /*!
    @brief Get the version string of the current application.
    */
    static const char *VersionString;

    /*!
    @brief Get the version revision string of the current application.
    */
    static const char *RevisionString;

    /*!
    @brief Flag that indicates whether a custom language file has been loaded.
    */
    static bool CustomLanguageLoaded;

    /*!
    @brief String that describes the necessary file name for a custom language.
    */
    static std::string CustomLanguageFileName;

    /*!
    Check for an identified key combination.
    @param[in] event The key event.
    @returns The identified key command.
    */
    static CommandEnums::KeyCommandType CheckKeyCombination(QKeyEvent* event);

    /*!
    Get the text color for the specified background color.
    @param color The background color to contrast against.
    @return The hex color code for foreground.
    */
    static std::string GetForegroundContrastColor(const char* color);

    /*!
    Convert the raw binary data to a pixel map.
    @param data The raw image data.
    @param scaledWidth The width to scale to or -1 for no scaling.
    @param scaledHeight The height to scale to or -1 for no scaling.
    @return The pixel map.
    */
    static QPixmap ConvertRawImageToPixmap(ComicSeer::XByteArray& data, int scaledWidth = -1, int scaledHeight = -1);

    /*!
    Check whether the given pixmap is a double page.
    @param map The map to review.
    @return True if double, false otherwise.
    */
    static bool IsPixmapDoublePage(const QPixmap& map);

    /*!
    @brief Get the comic page rotation setting.
    @param file The comic file name.
    @param page The comic page.
    @return The rotation to use for the page.
    */
    static ComicSeer::ComicPageTypes::PageRotationType GetComicPageRotation(QString file, int page);

    /*!
    @brief Set the comic page rotation setting.
    @param file The comic file name.
    @param page The comic page.
    @param rotation The page rotation.
    */
    static void SetComicPageRotation(QString& file, int page, ComicSeer::ComicPageTypes::PageRotationType rotation);

    /*!
    @brief Clear the page rotation information for all of the pages in the specified comic.
    @param file The file to clear from.
    */
    static void ClearComicPageRotations(const QString& file);

	/*!
	Get the file configuration path.
	@returns The path, created if not already.
	*/
	static QString FileConfigurationPath();

    /*!
    @brief Get the visible comic path, which is either name or full path dependening on the option.
    @return The path to display.
    */
    static QString GetVisibleComicPath(const QString &path);

    /*!
    @brief Launch comic tagger either directly or indirectly.
    @param file The comic file to have it load.
    */
    static void LaunchComicTagger(const QString &file);

    /*!
    @brief Convert a std::string to QString.
    @param s The string to convert.
    @return The converted string.
    */
    static QString StringToQString(const std::string &s);

    /*!
    @brief Convert a QString to std::string.
    @param s The string to convert.
    @return The converted string.
    */
    static std::string ConvertQStringToStdString(const QString &q);

    /*! Get the ComicTagger website. */
    static QString ComicTaggerWebsite();

    /*!
    @brief Get the visible comic file type.
    @param type The file type.
    @return The visible text.
    */
    static QString GetVisibleFileType(ComicSeer::ComicFile::ComicFileType type);

    /*! Get the user-visible version string. */
    static QString GetVersionString();

    /*!
    @brief Open the specified comic.
    @param path The path of the comic.
    @param[in] page The zero-based specific page in the comic.
    */
    static void OpenComic(const QString &path, int page = -1);

    /*!
    @brief Find the best match for the specified comic.
    @param path The path to the comic.
    */
    static QString FindBestComicMatch(const QString &path);

private:
    static QSettings _rotationSettings;
    static QSettings _viewSettings;
};

#endif
