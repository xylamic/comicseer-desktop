/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef LIBRARYVIEWCONTENT_H
#define LIBRARYVIEWCONTENT_H

#include <Xylasoft.hpp>
#include "ui_LibraryView.h"
#include <map>
#include <qdir.h>
#include <qframe.h>
#include <qmenu.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <vector>
#include <ComicLibrary.hpp>
#include <ComicLibraryItem.hpp>

/*!
This widget is the body of the user library management.
*/
class LibraryViewContent : public QFrame
{
    Q_OBJECT
public:
	/*!
	Construct the widget.
	@param[in] parent The parent widget.
	*/
    explicit LibraryViewContent(QWidget *parent = 0);

    /*!
    Focus to the search control.
    */
    void FocusSearch();

    /*!
    @brief Find the next comic in the series.
    @param path The path to the current comic.
    @param found Whether the specified comic was found in a library.
    @return The path to the next comic or empty if not found/none.
    */
    QString FindNextInSeries(const QString& path, bool& found);

signals:
	/*!
	Signal emitted when the number of comics in the library has changed.
	*/
	void NumberOfComicsChanged(long count);
    
private slots:
    /*!
    @brief Event received to copy the comic name to the clipboard.
    */
    void CopyNameToClipboard();

    /*!
    @brief Event received to refresh the libraries.
    */
	void Refresh();

    /*!
    @brief Event received to add a new library.
    */
	void AddLibrary();

    /*!
    @brief Event received to remove the selected library.
    */
	void RemoveLibrary();

    /*!
    @brief Event received to open the specified comic file.
    */
    void Open();

    /*!
    @brief Event received that the selected comic has changed.
    */
	void ComicSelectionChanged();

    /*!
    @brief Event received that a comic was activated in the library view.
    @param item The item that was activated.
    @param column The textual column that was activated.
    */
    void ComicActivated(QTreeWidgetItem* item, int column);

    /*!
    @brief Event received that the search text changed.
    @param text The text it was changed to.
    */
	void SearchTextChanged(const QString& text);

    /*!
    @brief Event received that the user requested to clear any search filter.
    */
	void ClearSearchTriggered();

    /*!
    @brief Event received that the user right-clicked on a library item and needs a context menu.
    @param pos The position at which the user clicked.
    */
    void TreeContextMenuRequested(const QPoint& pos);

    /*!
    @brief Event received that the user requested to export a list of their libraries.
    */
    void Export();

    /*!
    @brief Event received that search is ready to search the library and find matching items.
    */
    void SearchReady();

    /*!
    @brief Event received when the library view selection has changed.
    @param index The index of the view (0 - sort, 1 - files, 2 - recent)
    */
    void on_cboViewType_currentIndexChanged(int index);

    /*!
    @brief Event received to show the comic information.
    */
    void ViewComicInformation();

protected:
    /*!
    @brief Load the libraries into the tree.
    */
	void LoadLibraries();

    void ParseComicFolder(QTreeWidgetItem* parentItem, const ComicSeer::ComicLibrary::ComicFolder &folder);

    /*!
    @brief Set whether to show or hide the folders in the libraries.
    @param isHidden True to hide, false otherwise.
    */
    void SetFoldersHidden(bool isHidden);

    /*!
    @brief Show a library with the given name and path.
    @param name The name of the library.
    @param path The path to the library.
    */
    void ShowLibrary(QString name);

    /*!
    @brief Create the context menus for tree items.
    */
    void CreateMenus();

    /*!
    @brief The hide event was received.
    @param e The hide event.
    */
    virtual void hideEvent(QHideEvent *e);

    /*!
    @brief Get the selected tree widget based
           on the selected view.
    @return The selected tree widget, or NULL for searching.
    */
    QTreeWidget* GetSelectedTreeWidget();

    /*!
    @brief Find the library level item in the specified tree widget.
    @param widget The widget to search.
    @param name The name of the library.
    @return The widget item or NULL if not found.
    */
    QTreeWidgetItem *FindLibraryItem(QTreeWidget *widget, QString &name);

    /*!
    @brief Find the next item following the specified one, in the specified tree..
    @param tree The tree to search within.
    @param name The name of of the item.
    @param path The path to the item.
    @param found Whether the item was located.
    @return The path to the next comic or empty if there is no next comic.
    */
    QString FindNextInSeriesInTree(QTreeWidget *tree, const QString &name, const QString &path, bool &found);

private:
    /*!
    @brief Recursive method for setting whether a folder is hidden or not.
    @param item The item to set and parse.
    @param isHidden Whether the folders should be hidden or not.
    */
    void SetFoldersHiddenRecurse(QTreeWidgetItem *item, bool isHidden);

    /*!
    @brief Create a library level tree item.
    @param name The name for the item.
    @return The new item.
    */
    QTreeWidgetItem *CreateLibraryTreeItem(const std::string &name);

    /*!
    @brief Create a folder level tree item.
    @param parentItem The parent tree item.
    @param name The name for the item.
    @param path The path to the item.
    @return The new item.
    */
    QTreeWidgetItem *CreateFolderTreeItem(QTreeWidgetItem *parentItem, const std::string &name, const std::string &path);

    /*!
    @brief Create a comic level tree item.
    @param parentItem The parent tree item.
    @param name The name of the item.
    @param path The path to the comic.
    @param ctype The comic file type.
    @return The new item.
    */
    QTreeWidgetItem *CreateComicTreeItem(QTreeWidgetItem *parentItem, const std::string &name, const std::string &path, ComicSeer::ComicFile::ComicFileType ctype);

    /*!
    @brief Show the files library.
    @param library The library to show.
    @param name The name of the library.
    */
    void ShowLibrary_Files(const ComicSeer::ComicLibrary &library, std::string &name);

    /*!
    @brief Show the sorted library.
    @param library The library to show.
    @param name The name of the library.
    */
    void ShowLibrary_Sorted(const ComicSeer::ComicLibrary &library, std::string &name);

    /*!
    @brief Show the recent list library.
    @param library The library to show.
    @param name The name of the library.
    */
    void ShowLibrary_Recent(const ComicSeer::ComicLibrary &library, std::string &name);

private:
    Ui::LibraryViewClass ui;
    QString _textNoPreview;
    QMenu* _libraryMenu;
    QMenu* _fileMenu;
    QMenu *_searchItemMenu;
    QMenu* _treeMenu;
    QTimer _searchTimer;
    bool _searching;

	static const int _itemTypeLibrary;
	static const int _itemTypeFolder;
	static const int _itemTypeFile;

	static const int _itemColumnDataType;
	static const int _itemColumnDataPath;
	static const int _itemColumnDataComicType;
};

#endif // LIBRARYVIEW_HPP
