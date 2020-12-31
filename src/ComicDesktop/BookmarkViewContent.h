/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef BOOKMARKVIEWCONTENT_H
#define BOOKMARKVIEWCONTENT_H

#include <Xylasoft.hpp>
#include <QtWidgets/QFrame>
#include <Bookmarkings.hpp>
#include <QtWidgets/QMenu>

namespace Ui {
class BookmarkViewContent;
}

/*!
@brief The BookmarkViewContent displays bookmarks and the editing of them.
*/
class BookmarkViewContent : public QFrame
{
    Q_OBJECT

public:
    /*!
    @brief Construct the bookmark view.
    @param[in] parent The parent widget.
    */
    explicit BookmarkViewContent(QWidget *parent = 0);

    /*!
    @brief Destroy the bookmark view.
    */
    ~BookmarkViewContent();

    /*!
    @brief Check for unsaved changes and confirm if needed.
    */
    void CheckForChanges();

signals:
    /*!
    @brief Request from a listener the active comic to use if requested to create
    a bookmark.
    @param[out] comic The path to the comic file. Empty if none.
    @param[ou] page The page in the comic. Must be filled if comic is not empty.
    */
    void ActiveComicRequested(QString &comic, int &page);

protected:
    /*!
    @brief Load bookmarks from user preferences.
    */
    void LoadBookmarks();

    /*!
    @brief Add a row to the bookmark table.
    @param mark The bookmark to append.
    */
    void AddRow(const ComicSeer::Bookmark &mark);

    /*!
    @brief Get the identifier for the specified row.
    @param row The row to retrieve.
    @return boost::uuids::uuid The row identifier.
    */
    boost::uuids::uuid GetIdForRow(int row);

    /*!
    @brief Show the specified row.
    @param id The row to show.
    */
    void ShowRow(boost::uuids::uuid id);

    /*!
    @brief Open the specified bookmark.
    @param row The bookmark row.
    */
    void OpenComicBookmark(int row);

    /*!
    @brief Open the comic information
    @param row The bookmark row.
    */
    void OpenComicInformation(int row);

    /*!
    @brief Get the comic file for the specified bookmark.
    @param id The bookmark identifier.
    @return QString The path if valid, empty otherwise.
    */
    QString GetBookmarkComic(int row);

    /*!
    @brief Get the first selected row.
    @return The row index.
    */
    int GetFirstSelectedRow();

    /*!
    @brief The window was shown.
    @param e The show event arguments.
    */
    virtual void showEvent(QShowEvent *e);

    /*!
    @brief The window was hidden.
    @param e The hide event arguments.
    */
    virtual void hideEvent(QHideEvent *e);

protected slots:
    /*!
    @brief Add a bookmark based on the currently shown page.
    */
    void AddBookmarkClicked();

    /*!
    @brief Remove the selected bookmark(s).
    */
    void RemoveSelectedBookmarks();

    /*!
    @brief Save button was clicked.
    */
    void SaveClicked();

    /*!
    @brief Open button was clicked.
    */
    void OpenClicked();

    /*!
    @brief Revert button was clicked.
    */
    void RevertClicked();

    /*!
    @brief Import button was clicked.
    */
    void ImportClicked();

    /*!
    @brief Export button was clicked.
    */
    void ExportClicked();

    /*!
    @brief Update button was clicked.
    */
    void UpdateClicked();

    /*!
    @brief Mark the bookmarks dirty.
    */
    void MarkDirty(bool dirty = true);

    /*!
    @brief The table requested a context menu.
    @param point The point where it was requested.
    */
    void TableContextMenuRequested(QPoint point);

    /*!
    @brief The table selectiion has changed.
    */
    void TableSelectionChanged();

    /*!
    @brief The table header geometries changed.
    */
    void TableHeaderGeoChanged();

private:
    Ui::BookmarkViewContent *ui;
    bool _dirty;
    QMenu *_contextMenu;
};

#endif // BOOKMARKVIEWCONTENT_H
