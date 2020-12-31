/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICVIEWWIDGET_H
#define X_COMICVIEWWIDGET_H

#include <Xylasoft.hpp>
#include <QtWidgets/qwidget.h>
#include "ui_ComicViewer.h"
#include "ComicView.h"

/*!
Widget that contains the comic viewing part of the application.
*/
class ComicViewWidget : public QWidget
{
	Q_OBJECT

public:
	/*!
	Construct the comic viewer.
	@param[in] path The path of the comic.
	@param[in] parent The parent widget.
	*/
	ComicViewWidget(const char* path, QWidget* parent = NULL);

	/*!
	Get the current page.
    @param[out] secondVisible Whether a second page is visible.
	@returns The page number.
	*/
    int CurrentPage(bool &secondVisible);

	/*!
	Get the number of pages in the comic.
	@returns The number.
	*/
	int NumberOfPages();

	/*!
	Move to the next page.
	@returns True if moved.
	*/
	bool PageForward();

	/*!
	Move to the previous page.
	@returns True if moved.
	*/
	bool PageBack();

    /*!
    Move to the first page.
    */
    void FirstPage();

    /*!
    Move to the last page.
    */
    void LastPage();

    /*!
    Zoom in on the page by a single step.
    */
    void ZoomIn();

    /*!
    Zoom out on the page by a single step.
    */
    void ZoomOut();

    /*!
	Set the scale factor.
	@param[in] value The scale factor greater than 1.0.
	*/
	void setScaleFactor(double value);

	/*!
	Get the scale factor.
	@returns The factor.
	*/
	double ScaleFactor();

	/*!
	Set the image state.
	@param[in] state The state to set.
	*/
    void setState(ComicSeer::ImageState state);

	/*!
	Get the image state.
	@returns The state.
	*/
    ComicSeer::ImageState State();

    /*!
    Set the image view.
    @param[in] view The image view.
    */
    void setView(ComicSeer::ImageView view);

    /*!
    Get the image view.
    @returns The current view.
    */
    ComicSeer::ImageView View();

	/*!
	Get the path of the comic.
	@returns The full path.
	*/
	std::string Path();

    /*!
    Show the comic view in full screen.
    */
    void FullScreen();

    /*!
    @brief Set the current page.
    @param page The page to set.
    */
    void SetCurrentPage(int page);

signals:
	/*!
	The page has changed.
	@param[in] page The page number.
    @param[in] secondVisible Whether a second page is visible following the selected.
	*/
    void PageChanged(int page, bool secondVisible);

protected slots:
    /*!
    The one page button was clicked to view a single page.
    */
    void OnePageClicked();

    /*!
    The two page button was clicked to view two pages at once.
    */
    void TwoPageClicked();

	/*!
	The scale was changed on the comic view.
	@param[in] scale The new scale factor.
	*/
	void ScaleChanged(double scale);

	/*!
	The user changed the slider value, the zoom.
	@param[in] value The value changed to.
	*/
	void SlideValueChanged(int value);

	/*!
	The browse button was toggled.
	@param[in] checked The check value.
	*/
	void BrowseToggled(bool checked);

	/*!
	Event that a page was selected from the browser.
	@param[in] page The page number (1+).
	*/
	void BrowserPageSelected(int page);

	/*!
	The user made a page change directly on the view.
	*/
	void ViewUserPageChange();

    /*!
    Event received that a comic view was closed.
    */
    void ComicViewClosed();

    /*!
    Event received that the comic view's view was changed.
    @param[in] view The new view.
    */
    void ComicViewChanged(ComicSeer::ImageView view);

    /*!
    @brief The magnify button was toggled.
    @param checked True if active.
    */
    void MagnifyToggled(bool checked);

    /*!
    @brief The magnify was toggled on/off from the comic view.
    @param magnify
    */
    void ComicViewMagnifyToggled(bool magnify);

    /*!
    @brief Rotate the current page right.
    */
    void RotateRight();

    /*!
    @brief Rotate the current page left.
    */
    void RotateLeft();

protected:
	/*!
	Read the page setting for the path.
	@returns The page setting or 0 if not found/invalid.
	*/
	int ReadPageSetting();

	/*!
	Update the current page.
	*/
    void UpdatePage();

private slots:
    void InformationClicked();

private:
	std::string _path;
	Ui::ComicViewer ui;
    ComicView* _fullView;
    int _comicViewLayoutIndex;
};

#endif
