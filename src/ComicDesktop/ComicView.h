/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICVIEW_H
#define X_COMICVIEW_H

#include <Xylasoft.hpp>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>
#include <ComicFile.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <map>
#include <qpixmap.h>
#include <ImageState.hpp>
#include <ImageView.hpp>
#include <boost/signals2.hpp>

/*!
This is the widget that allows viewing of comic pages.
*/
class ComicView : public QGraphicsView
{
	Q_OBJECT

public:
	/*!
	Create the comic view with the specified comic file.
	*/
	ComicView(QWidget* parent = NULL);

	/*!
	Initialize the view of the widget.
	@param[in] path The path of the file to show.
    @param[in] page The page to initialize to.
	*/
    void Initialize(const char* path, int page = 0);

	/*!
	Get the current page.
	@returns The page number.
	*/
	int CurrentPage();

	/*!
	Get the number of pages.
	@returns The number.
	*/
	int NumberOfPages();

	/*!
	Set the page being shown.
	@param[in] page The page number.
	*/
	void SetPage(int page);

	/*!
	Move to the next page.
	*/
	void PageForward();

	/*!
	Move to the previous page.
	*/
	void PageBack();

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
    void SetScaleFactor(double value);

	/*!
	Get the scale factor.
	@returns The factor.
	*/
	double ScaleFactor();

	/*!
	Set the image state.
	@param[in] state The state to set.
	*/
    void SetState(ComicSeer::ImageState state);

	/*!
	Get the image state.
	@returns The state.
	*/
    ComicSeer::ImageState State();

    /*!
    Set the image view.
    @param[in] view The view to set.
    */
    void SetView(ComicSeer::ImageView view);

    /*!
    Get the image view.
    @returns The view.
    */
    ComicSeer::ImageView View();

    /*!
    Get whether a second page is visible.
    @return True if visible.
    */
    bool SecondPageVisible();

    /*!
    @brief Get whether the view is magnified.
    @return True if magnified.
    */
    bool Magnified();

    /*!
    @brief Set whether the view is magnified.
    @param magnified True if magnified.
    */
    void SetMagnified(bool magnified);

signals:
	/*!
	Signal that the scale factor has changed.
	@param[in] scale The updated scale.
	*/
	void scaleChanged(double scale);

	/*!
	The user triggered a page change directly on the view.
	*/
	void UserPageChange();

    /*!
    Signal that the view was closed.
    */
    void ViewClosed();

    /*!
    Signal that the view was changed.
    @param[in] view The new view.
    */
    void ViewChanged(ComicSeer::ImageView view);

    /*!
    @brief Signal that magnification has been turned on/off.
    @param magnified True for magnification, false otherwise.
    */
    void MagnificationChanged(bool magnified);

protected:
	/*!
	Draw the image to the canvas.
	*/
	void DrawImage();

	/*!
	Create the current image.
	*/
	void CreateCurrentImage();

    /*!
    Reset and transform the image.
    */
    void TransformImage();

    /*!
    Verify the pixmap is populated. If not, populate it.
    @param page The page to retrieve and populate with.
    @param pmap The pixel map to populate.
    */
    void PopulatePixmap(int page, boost::scoped_ptr<QPixmap>& pmap);

    /*!
    Clear the pixmaps.
    */
    void ClearPixmaps();

    /*!
    @brief Draw the magnifier frame.
    @param map The magnifier frame to draw upon.
    */
    void DrawMagnifierFrame(QPixmap& map);

    /*!
    @brief Magnify the specified point
    @param point The point to magnify.
    */
    void MagnifyPoint(const QPoint& point);

    /*!
    @brief Set the current cursor.
    @param mouseDown Whether the mouse is currently down.
    */
    void SetCursor(bool mouseDown);

    /**
     * @brief Trigger a scale change and protect against changing the mode.
     * @param factor The scale factor.
     */
    void TriggerScaleChange(double factor);

/* Protected virtual overriden methods */
protected:
	/*!
	Resize event received.
	@param[in] e The resize event.
	*/
    virtual void resizeEvent(QResizeEvent* e);

	/*!
	Mouse press event received.
	@param[in] e The mouse event.
	*/
	virtual void mousePressEvent(QMouseEvent* e);

	/*!
	Mouse release event received.
	@param[in] e The mouse event.
	*/
    virtual void mouseReleaseEvent(QMouseEvent* e);

    /*!
    @brief Mouse entered the widget.
    @param e The event arguments.
    */
    virtual void enterEvent(QEvent* e);

    /*!
    @brief Event received that the mouse has moved.
    @param event The mouse move event.
    */
    virtual void mouseMoveEvent(QMouseEvent *event);

    /*!
    Key press event received.
    @param[in] event The key event.
    */
    virtual void keyPressEvent(QKeyEvent* event);

    /*!
    Event received that the window was closed.
    @param[in] event The close event.
    */
    virtual void closeEvent(QCloseEvent* event);

    /*!
    @brief Event recieved to repaint the widget.
    @param e The paint event.
    */
    virtual void paintEvent(QPaintEvent *e);

private:
    /* current user state */
	QPoint _mousePress;
    bool _secondPageVisible;

    /* current view state */
    int _page;
    ComicSeer::ImageState _state;
    ComicSeer::ImageView _view;
    double _scaleFactor;
    double _baseScaleFactor;
    bool _magnified;

    /* data variables */
	boost::scoped_ptr<ComicSeer::ComicFile> _comic;
	std::map<int, std::string> _comicPages;
    QGraphicsScene _scene;
    boost::signals2::scoped_connection _slotBackgroundColorUpdated;
    boost::signals2::scoped_connection _slotPageNumberingUpdated;
    boost::scoped_ptr<QPixmap> _currentPixmap;
    boost::scoped_ptr<QPixmap> _altPixmap;
	std::string _errorMessage;
    QGraphicsPixmapItem *_zoomPixmap;
    QCursor _cursor;
    bool _changingScale;
};

#endif
