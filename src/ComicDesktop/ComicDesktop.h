/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICDESKTOP_H
#define COMICDESKTOP_H

#include <Xylasoft.hpp>
#include <QtWidgets/QMainWindow>
#include "ui_ComicDesktop.h"
#include <QtWidgets/qmenu.h>
#include "ComicDockWidget.h"
#include "ComicViewWidget.h"
#include "RecentComicMenu.h"
#include "UpdateButton.h"

/*!
The main window of the application.
*/
class ComicDesktop : public QMainWindow
{
	Q_OBJECT

public:
	/*!
	Construct the comic main window.
	@param[in] parent The parent widget.
	@param[in] flags The widget flags.
	*/
    ComicDesktop(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	/*!
	Destruct the main window.
	*/
	~ComicDesktop();

	/*!
	Get the static comic desktop instance (only 1 per application instance).
	@returns The instance or NULL if not yet created.
	*/
	static ComicDesktop* Instance();

    /*!
    @brief Check if the specified comic is open.
    @param file The file to check.
    @return True if open.
    */
    bool IsComicOpen(const QString &file) const;

    /*!
    @brief Close the specified comic. Closes nothing if
    not found.
    @param file The file to close.
    */
    void CloseComic(const QString &file);

private slots:
	void ViewLibrariesToggled(bool checked);
    void ViewBookmarksToggled(bool checked);
	void ViewTabsToggled(bool checked);
	void OpenTriggered();
	void CloseTriggered();
	void ShowToolbarTriggered();
	void CollapseToolbarTriggered();
	void LibrariesCloseTriggered();
    void BookmarksCloseTriggered();
	void TabCloseRequested(int index);
	void HomeTriggered();
	void TabCurrentChanged(int index);
	void PageFitAllTriggered();
	void PageFitWidthTriggered();
	void PageFitHeightTriggered();
	void PageForwardTriggered();
	void PageBackTriggered();
    void NextTriggered();
    void ComicPageChanged(int page, bool secondVisible);
	void NumberOfComicsChanged(long count);
	void RecentTriggered();
	void RecentItemSelected(std::string path);
    void OptionsClicked();
    void FullscreenClicked();
    void AboutClicked();
    void SingleAppMsgReceived(QString message);
    void CustomContextMenuTriggered(int actionId, int tabIndex);
    void BookmarkActiveComicRequested(QString &comic, int &page);

	// protected methods specific to this class
protected:
	/*!
	Get the currently selected comic widget.
	@returns The widget or NULL if none selected.
	*/
	ComicViewWidget* SelectedComicWidget();

	/*!
	Store the applicaation state.
	*/
	void StoreState();

	/*!
	Load the application state.
	*/
	void LoadState();

	/*!
	Set the comic state information.
	*/
	void SetComicInformation();

	/*!
	Create the status bar content.
	*/
	void CreateStatusBar();

	/*!
	Parse and calculate the number of comics presented in the library.
	@returns The number of comics.
	*/
    long NumberOfComicsInLibrary();

	/*!
	Create the library view and initialize it.
	*/
	void CreateLibraryView();

    /*!
    Create the bookmark view and initialize it.
    */
    void CreateBookmarkView();

	/*!
	Initialize the buttons on the window.
	*/
	void InitializeButtons();

	/*!
	Add a comic view to the application.
	@param[in] path The full path to the comic.
    @param[in] page The page within the comic.
	*/
    void AddComicView(std::string path, int page = -1);

    /*!
    @brief Save the comic viewing session.
    */
    void SaveSession();

    /*!
    @brief Load the last comic viewing session.
    */
    void LoadSession();

    /*!
    @brief Create the home tab.
    */
    void CreateHomeTab();

protected:
	virtual bool event(QEvent* e);
	virtual void paintEvent(QPaintEvent* e);
	virtual void changeEvent(QEvent* e);
	virtual void hideEvent(QHideEvent* e);
	virtual void showEvent(QShowEvent* e);
	virtual void closeEvent(QCloseEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);

    /*!
    @brief Method for receiving the change in full path display signal.
    @param show Whether to show the full path.
    */
    void ShowFullPathChanged(bool);

private:
	ComicDesktop(const ComicDesktop&);
	ComicDesktop& operator=(const ComicDesktop&);

private:
    QMainWindow *_subWindow;
    boost::signals2::scoped_connection _slotShowPathUpdated;
	Ui::ComicDesktopClass ui;
    ComicDockWidget *_libraryView;
    ComicDockWidget *_bookmarkView;
    static ComicDesktop* _instance;
	QLabel* _statusFileName;
	QLabel* _statusPageNumber;
	QLabel* _statusLibraryNumber;
    QLabel* _statusLocaleName;
    UpdateButton* _statusUpdateButton;
    RecentComicMenu* _recentMenu;
};

#endif // COMICDESKTOP_H
