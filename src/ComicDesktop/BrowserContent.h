/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_BROWSERCONTENT_H
#define X_BROWSERCONTENT_H

#include <Xylasoft.hpp>
#include <QtWidgets/qwidget.h>
#include <map>
#include <list>
#include <QtWidgets/qlabel.h>
#include "DescribedPreviewLabel.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <QtWidgets/qscrollarea.h>
#include <boost/signals2.hpp>

/*!
The browser for scrolling the content of a comic.
*/
class BrowserContent : public QScrollArea
{
	Q_OBJECT

public:
	/*!
	Construct the browser.
	*/
    BrowserContent(QWidget* parent = NULL);

	/*!
	Set a comic for browsing.
	@param[in] path The path to the comic to browse.
	*/
	void SetComic(const char* path);

	/*!
	Select the specified comic page.
	@param[in] page The page number (1+).
    @param[in] nextVisible Whether the following page is visible.
	*/
    void SelectPage(int page, bool nextVisible);

    /*!
    Load the comic.
    */
    void LoadComic();

signals:
	/*!
	Event that a page was selected from the browser.
	@param[in] page The page number (1+).
	*/
	void PageSelected(int page);

protected:
	/*!
	Event received to show the widget.
	@param[in] e The show event.
	*/
    virtual void showEvent(QShowEvent* e);

    /*!
    Event received to paint.
    @param e The paint event.
    */
    virtual void paintEvent(QPaintEvent *e);

    /*!
    @brief Run the loading queue if not already running.
    @param initialSize The initial size of the queue before modification.
    */
    void RunLoadQueue(int initialSize);

    /*!
    @brief Ensure the selected pages are visible.
    */
    void EnsureSelectedVisibility();

protected slots:
	/*!
	Event received when data has finished reading.
	@param[in] read The comic reader instance.
	@param[in] success The success of the request.
	@param[in] map The pixel map if successful.
	*/
	void ImageReader(ComicImageReader* reader, bool success, QPixmap* map);

	/*!
	A preview clicked was clicked.
	@param[in] label The clicked.
	*/
    void PreviewClicked(DescribedPreviewLabel* label);

private:
    QWidget *_widget;
    std::map<int, DescribedPreviewLabel*> _labels;
    DescribedPreviewLabel* _selected;
    DescribedPreviewLabel* _altSelected;
    std::queue<boost::tuple<std::string, std::string, DescribedPreviewLabel*> > _loadQueue;
	bool _loaded;
	std::string _path;
    boost::signals2::scoped_connection _slotBackUpdated;
    boost::signals2::scoped_connection _slotThumbnailSizeUpdated;
    int _currentSize;

	static const char* _previewDataNameFile;
	static const char* _previewDataNamePage;
    static const char* _previewDataNameText;
};

#endif
