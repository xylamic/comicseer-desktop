/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicView.h"
#include <XException.hpp>
#include <qbytearray.h>
#include <qimagereader.h>
#include <qbuffer.h>
#include <QtWidgets/qgraphicsitem.h>
#include <math.h>
#include <qevent.h>
#include <qpen.h>
#include <ComicState.h>
#include <QKeyEvent>
#include "ComicUtils.h"
#include <boost/signals2.hpp>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qpen.h>
#include <ComicPageTypes.hpp>
#include <qdebug.h>
#include <QtWidgets/QMessageBox>

using namespace ComicSeer;
using namespace std;
using namespace boost;
using namespace boost::signals2;

#define PADDING 5

ComicView::ComicView(QWidget* parent)
    : QGraphicsView(parent), _scaleFactor(1.75), _mousePress(-1, -1), _secondPageVisible(false), _zoomPixmap(NULL),
      _magnified(false), _changingScale(false)
{
	// set the scene
    this->setScene(&this->_scene);

    // set the back updated connection
    this->_slotBackgroundColorUpdated = ComicState::State()->BackgroundColorUpdated.connect([this](const char*) { this->DrawImage(); });

    // set the page numbering updated connection
    this->_slotPageNumberingUpdated = ComicState::State()->PageNumberingUpdated.connect([this]() { this->SetPage(this->CurrentPage()); });
}

void ComicView::Initialize(const char* path, int page)
{
    // intialize the page
    this->_page = -1;

    // read the view defaults
    ComicState* state = ComicState::State();
    this->_state = state->DefaultImageState();
    this->_scaleFactor = state->ScaledZoomFactor();
    this->_view = state->DefaultImageView();

	try
	{
		// create the comic file
		_comic.reset(ComicFile::CreateComicFile(path));
		_comic->Initialize();

		// get the list of images
		list<string> files = _comic->ImageList();

		// gather the file list
		int count = 0;
		for (list<string>::iterator it = files.begin(); it != files.end(); it++)
		{
			this->_comicPages[count] = *it;
			count++;
        }

		// if the specified page is invalid, reset it
		if (this->_comicPages.find(page) == this->_comicPages.end())
		{
			this->_page = 0;
		}
		else
		{
			this->_page = page;
        }
    }
	catch (...)
	{
        _comic.reset();
	}

    // draw the image
    this->DrawImage();
}

int ComicView::CurrentPage()
{
	return this->_page;
}

int ComicView::NumberOfPages()
{
	return this->_comicPages.size();
}

void ComicView::PageForward()
{
    int pagesToMove = 1;
    if (this->_view == TwoPage)
    {
        // if not using auto-detect or the current page is not double width, then get the two page move number
        if (!ComicState::State()->TwoPageAutodetectWidePage() || !ComicUtils::IsPixmapDoublePage(*this->_currentPixmap))
        {
            pagesToMove = ComicState::State()->TwoPageMoveNumber();
            if (pagesToMove < 1 || pagesToMove > 2)
            {
                pagesToMove = 2;
            }
        }
    }

    scoped_ptr<QPixmap> upOne, upTwo;
    this->PopulatePixmap(this->_page + 1, this->_altPixmap);
    this->PopulatePixmap(this->_page + 2, upOne);
    if (pagesToMove > 1)
    {
        this->PopulatePixmap(this->_page + 3, upTwo);
    }

    if (this->_altPixmap.get() == NULL)
    {
        return;
    }

    // if we need to move one page or two pages, but two ahead in not present
    if (pagesToMove == 1 || (pagesToMove > 1 && upOne.get() == NULL))
    {
        this->_currentPixmap.swap(this->_altPixmap);
        this->_altPixmap.swap(upOne);

        this->_page++;
    }
    else
    {
        // if either of the two next pages are double width and auto-detect is ON, then only move a single page
        if (ComicState::State()->TwoPageAutodetectWidePage() && ComicUtils::IsPixmapDoublePage(*this->_altPixmap))
        {
            this->_currentPixmap.swap(this->_altPixmap);
            this->_altPixmap.swap(upOne);

            this->_page++;
        }
        // this is the standard 2 page move
        else
        {
            this->_altPixmap.swap(upTwo);
            this->_currentPixmap.swap(upOne);

            this->_page = this->_page + 2;
        }
    }

    // draw the new layout
    this->DrawImage();
}

void ComicView::PageBack()
{
    int pagesToMove = 1;
    if (this->_view == TwoPage)
    {
        pagesToMove = ComicState::State()->TwoPageMoveNumber();
        if (pagesToMove < 1 || pagesToMove > 2)
        {
            pagesToMove = 2;
        }
    }

    scoped_ptr<QPixmap> backOne, backTwo;
    this->PopulatePixmap(this->_page - 1, backOne);
    if (pagesToMove > 1)
    {
        this->PopulatePixmap(this->_page - 2, backTwo);
    }

    if (backOne.get() == NULL)
    {
        return;
    }

    // if we need to move one page or two pages, but back pages back is not present
    if (pagesToMove == 1 || (pagesToMove > 1 && backTwo.get() == NULL))
    {
        this->_altPixmap.swap(this->_currentPixmap);
        this->_currentPixmap.swap(backOne);

        this->_page--;
    }
    else
    {
        // if either of the two previous pages are double width and auto-detect is ON, then only move a single page
        if (ComicState::State()->TwoPageAutodetectWidePage() && (ComicUtils::IsPixmapDoublePage(*backOne) || ComicUtils::IsPixmapDoublePage(*backTwo)))
        {
            this->_altPixmap.swap(this->_currentPixmap);
            this->_currentPixmap.swap(backOne);

            this->_page--;
        }
        // this is the standard 2 page move
        else
        {
            this->_altPixmap.swap(backOne);
            this->_currentPixmap.swap(backTwo);

            this->_page = this->_page - 2;
        }
    }

    // draw the new layout
    this->DrawImage();
}

void ComicView::FirstPage()
{
    int nPages = this->NumberOfPages();
    if (nPages > 0)
    {
        SetPage(0);
        emit UserPageChange();
    }
}

void ComicView::LastPage()
{
    int nPages = this->NumberOfPages();
    if (nPages > 0)
    {
        SetPage(nPages - 1);
        emit UserPageChange();
    }
}

void ComicView::ZoomIn()
{
    double scale = this->_scaleFactor * 1.20;
    if (scale > 4.0)
        scale = 4.0;

    this->SetScaleFactor(scale);
}

void ComicView::ZoomOut()
{
    double scale = this->_scaleFactor * .80;
    if (scale < 1.0)
        scale = 1.0;

    this->SetScaleFactor(scale);
}

void ComicView::PopulatePixmap(int page, boost::scoped_ptr<QPixmap>& pmap)
{
    try
    {
        // get the file information
        QFileInfo file(QString::fromUtf8(this->_comic->FilePath().c_str()));

        // find the comic page
        map<int, std::string>::iterator it = this->_comicPages.find(page);

        // if the file no longer exists or the page does not exist
        if (it == this->_comicPages.end() || !file.exists() || !file.isFile())
        {
            pmap.reset();
        }
        else if (pmap.get() == NULL)
        {
            XByteArray pageData;
            this->_comic->GetFile(it->second.c_str(), pageData);
            pmap.reset(new QPixmap(ComicUtils::ConvertRawImageToPixmap(pageData)));

            // check page settings
            ComicPageTypes::PageRotationType rotation = ComicUtils::GetComicPageRotation(
                        file.fileName(), page);
            switch (rotation)
            {
            case ComicPageTypes::DEGREE90:
            {
                QTransform t = QTransform().rotate(90);
                QPixmap* map = new QPixmap(pmap->transformed(t, Qt::SmoothTransformation));
                pmap.reset(map);
            }
                break;
            case ComicPageTypes::DEGREE180:
            {
                QTransform t = QTransform().rotate(180);
                QPixmap* map = new QPixmap(pmap->transformed(t, Qt::SmoothTransformation));
                pmap.reset(map);
            }
                break;
            case ComicPageTypes::DEGREE270:
            {
                QTransform t = QTransform().rotate(270);
                QPixmap* map = new QPixmap(pmap->transformed(t, Qt::SmoothTransformation));
                pmap.reset(map);
            }
                break;
            }
            qDebug() << QString("Page %1 loaded with rotation %2.").arg(page).arg(rotation);

            if (ComicState::State()->ShowPageNumbers())
            {
                QString pageText = QString("%1").arg(page + 1);
                QPainter p(pmap.get());
                QFont f("Courier", pmap->height() * .015, QFont::Bold);
                QFontMetrics metric(f);

                p.setPen(QPen(QColor(ComicState::State()->PageNumberColor().c_str())));
                p.setFont(f);

                if (ComicState::State()->PageNumberLocation() == Top)
                {
                    p.drawText(pmap->width() - 10 - metric.width(pageText), metric.height(), pageText);
                }
                else
                {
                    p.drawText(pmap->width() - 10 - metric.width(pageText), pmap->height() - 10, pageText);
                }
                p.end();
            }
        }
    }
    catch (...)
    {
        pmap.reset();
    }
}

void ComicView::ClearPixmaps()
{
    this->_currentPixmap.reset();
    this->_altPixmap.reset();
}

void ComicView::DrawMagnifierFrame(QPixmap &map)
{
    QPainter magPainter(&map);

    QPen pen(Qt::SolidLine);
    pen.setColor(Qt::darkGreen);
    pen.setWidth(7);
    magPainter.setPen(pen);
    magPainter.setBrush(Qt::NoBrush);

    magPainter.drawRect(1, 1, map.width() - 3, map.height() - 3);

    magPainter.end();
}

void ComicView::MagnifyPoint(const QPoint &point)
{
    static QPixmap *lastTargetMap = NULL;
    static QPixmap *lastTargetMagnifyMap = NULL;
    static QPixmap *lastTargetMapAlt = NULL;
    static QPixmap *lastTargetMagnifyMapAlt = NULL;

    // clear any existing zoom image
    delete this->_zoomPixmap;
    this->_zoomPixmap = NULL;

    // check if magnification is active
    if (!this->_magnified)
    {
        return;
    }

    // get the comic state
    ComicState* state = ComicState::State();

    // find the scene coordinate for the point
    QPointF scenePoint = this->mapToScene(point.x(), point.y());

    // get the graphics item selected and if none, then don't magnify
    QGraphicsItem *graphicItem = this->_scene.itemAt(scenePoint, QTransform());
    if (graphicItem == NULL)
    {
        return;
    }

    // find the image being moused over and grab the pixel area
    QPointF pixmapPoint = scenePoint;
    QPixmap *targetMap = this->_currentPixmap.get();
    bool isAlt = false;
    if (scenePoint.x() >= this->_currentPixmap->width())
    {
        // if no second page is visible, don't magnify
        if (!this->_secondPageVisible)
        {
            return;
        }

        pixmapPoint.setX(pixmapPoint.x() - this->_currentPixmap->width());
        targetMap = this->_altPixmap.get();
        isAlt = true;
    }

    // get the properties of the magnifier
    double magnification = state->MagnifierMagnification();
    int magSize = this->_scene.height() / 4;
    int magHalfSize = magSize / 2;

    // get the graphics dimensions
    double graphicItemWidth = targetMap->width() * graphicItem->scale();
    double graphicItemHeight = targetMap->height() * graphicItem->scale();
    int magItemScaleWidth = graphicItemWidth * magnification;
    int magItemScaleHeight = graphicItemHeight * magnification;

    // scale up the selected pixmap
    QPixmap *targetMagMap = NULL;
    if (!isAlt)
    {
        if (lastTargetMap != targetMap || lastTargetMagnifyMap == NULL)
        {
            delete lastTargetMagnifyMap;
            lastTargetMagnifyMap = new QPixmap(targetMap->scaled(magItemScaleWidth, magItemScaleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            lastTargetMap = targetMap;
        }

        targetMagMap = lastTargetMagnifyMap;
    }
    else
    {
        if (lastTargetMapAlt != targetMap || lastTargetMagnifyMapAlt == NULL)
        {
            delete lastTargetMagnifyMapAlt;
            lastTargetMagnifyMapAlt = new QPixmap(targetMap->scaled(magItemScaleWidth, magItemScaleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            lastTargetMapAlt = targetMap;
        }

        targetMagMap = lastTargetMagnifyMapAlt;
    }

    // copy the pixels and draw the map
    int copyMapX = pixmapPoint.x() * magnification - magHalfSize;
    int copyMapY = pixmapPoint.y() * magnification - magHalfSize;
    if (copyMapX < 0) copyMapX = 0;
    if (copyMapY < 0) copyMapY = 0;
    if (copyMapX + magSize > targetMagMap->width()) copyMapX = targetMagMap->width() - magSize;
    if (copyMapY + magSize > targetMagMap->height()) copyMapY = targetMagMap->height() - magSize;
    QPixmap magMap = targetMagMap->copy(copyMapX, copyMapY, magSize, magSize);
    this->DrawMagnifierFrame(magMap);
    this->_zoomPixmap = this->_scene.addPixmap(magMap);

    // move to appropriate position
    int destX = scenePoint.x() - magHalfSize;
    int destY = scenePoint.y() - magHalfSize;
    if (destX - graphicItem->x() < 0) destX = graphicItem->x();
    if (destY - graphicItem->y() < 0) destY = graphicItem->y();
    if (destX + magSize > graphicItemWidth + graphicItem->x()) destX = graphicItemWidth - magSize + graphicItem->x();
    if (destY + magSize > graphicItemHeight + graphicItem->y()) destY = graphicItemHeight - magSize + graphicItem->y();

    this->_zoomPixmap->setX(destX);
    this->_zoomPixmap->setY(destY);
}

void ComicView::SetCursor(bool mouseDown)
{
    if (this->_magnified)
    {
        this->viewport()->setCursor(Qt::BlankCursor);
    }
    else
    {
        if (mouseDown)
        {
            this->viewport()->setCursor(Qt::ClosedHandCursor);
        }
        else
        {
            this->viewport()->setCursor(Qt::OpenHandCursor);
        }
    }
}

void ComicView::TriggerScaleChange(double factor)
{
    this->_changingScale = true;
    emit scaleChanged(factor);
    this->_changingScale = false;
}

void ComicView::DrawImage()
{
	// reset the view
	this->_scene.clear();
    this->_zoomPixmap = NULL;
    QColor backColor(ComicState::State()->BackgroundColor().c_str());
    this->_scene.setBackgroundBrush(QBrush(backColor));

	// create the new image
	this->CreateCurrentImage();

	// resize to fit the current image
	this->_scene.setSceneRect(this->_scene.itemsBoundingRect());

	// transform the scene
    this->TransformImage();

	// move to upper-left
	this->centerOn(QPoint(0, 0));
}

void ComicView::CreateCurrentImage()
{
	try
	{
        this->_secondPageVisible = false;

        // check the comic file
        if (this->_comic.get() == NULL)
        {
            throw FileException(tr("The file could not be read.").toUtf8());
        }

        // verify pages exist
        if (this->_comicPages.size() == 0)
        {
            throw FileException(tr("No pages exist in the file.").toUtf8());
        }

        // get the image data
        this->PopulatePixmap(this->_page, this->_currentPixmap);

        // store the image
        QGraphicsPixmapItem* currentImage = NULL;
        if (this->_currentPixmap.get() == NULL)
        {
            QMessageBox msg(QMessageBox::Critical, tr("Image error"), tr("The image could not be read from the file."));
            msg.exec();
            currentImage = this->_scene.addPixmap(QPixmap());
        }
        else
        {
            currentImage = this->_scene.addPixmap(*this->_currentPixmap);
        }
        currentImage->setTransformationMode(Qt::SmoothTransformation);

        if (_view == TwoPage)
        {
            if (ComicState::State()->TwoPageAutodetectWidePage() &&
                    (this->_currentPixmap.get() == NULL || ComicUtils::IsPixmapDoublePage(*this->_currentPixmap)))
            {
                // if the current page is double width with auto-detect ON, no additional work needed
            }
            else
            {
                QPixmap next;
                if (this->_page + 1 >= static_cast<int>(this->_comicPages.size()))
                {
                    // if the next page doesn't exist, fill with an empty pixmap
                    next = QPixmap(this->_currentPixmap->width(), this->_currentPixmap->height());
                    next.fill(QColor(ComicState::State()->BackgroundColor().c_str()));
                }
                else
                {
                    this->PopulatePixmap(this->_page + 1, this->_altPixmap);

                    if (this->_altPixmap.get() == NULL)
                    {
                        QMessageBox msg(QMessageBox::Critical, tr("Image error"), tr("The second image could not be read from the file."));
                        msg.exec();
                    }

                    // if the alternate page is double width and auto-detect is ON, fill with empty
                    if (ComicState::State()->TwoPageAutodetectWidePage() &&
                            (this->_altPixmap.get() == NULL || ComicUtils::IsPixmapDoublePage(*this->_altPixmap)))
                    {
                        next = QPixmap(this->_currentPixmap->width(), this->_currentPixmap->height());
                        next.fill(QColor(ComicState::State()->BackgroundColor().c_str()));
                    }
                    else
                    {
                        next = *this->_altPixmap;
                        this->_secondPageVisible = true;
                    }
                }

                QGraphicsPixmapItem* altImage = this->_scene.addPixmap(next);
                altImage->setTransformationMode(Qt::SmoothTransformation);

                // move the image
                QPointF point = currentImage->pos();
                altImage->setPos(currentImage->boundingRect().width(), point.ry());
            }
        }
	}
	catch (XException& ex)
	{
        this->ClearPixmaps();
        this->_errorMessage = ex.what();
	}
	catch (...)
	{
        this->ClearPixmaps();
        this->_errorMessage = string(tr("The file could not be read.").toUtf8());
	}

    if (this->_currentPixmap.get() == NULL && !this->_errorMessage.empty())
	{
        QGraphicsTextItem* textItem = this->_scene.addText(QString::fromUtf8(this->_errorMessage.c_str()), QFont("Comic Sans"));
		textItem->setDefaultTextColor(Qt::white);
	}
}

void ComicView::resizeEvent(QResizeEvent* e)
{
	this->TransformImage();

	QGraphicsView::resizeEvent(e);
}

void ComicView::keyPressEvent(QKeyEvent* event)
{
    // only trigger key events from here if we are running as full screen
    if (this->windowModality() == Qt::ApplicationModal)
    {
        CommandEnums::KeyCommandType cmd = ComicUtils::CheckKeyCombination(event);
        if (cmd == CommandEnums::FULLSCREEN || event->key() == Qt::Key_Escape)
        {
            this->close();
        }
        else if (cmd == CommandEnums::PAGEFORWARD)
        {
            this->PageForward();
            emit UserPageChange();
        }
        else if (cmd == CommandEnums::PAGEBACKWARD)
        {
            this->PageBack();
            emit UserPageChange();
        }
        else if (cmd == CommandEnums::FITWIDTH)
        {
            this->SetState(ComicSeer::FitWidth);
        }
        else if (cmd == CommandEnums::FITHEIGHT)
        {
            this->SetState(ComicSeer::FitHeight);
        }
        else if (cmd == CommandEnums::FITALL)
        {
            this->SetState(ComicSeer::FitWidthHeight);
        }
        else if (cmd == CommandEnums::ONEPAGE)
        {
            this->SetView(ComicSeer::OnePage);
            emit ViewChanged(this->View());
        }
        else if (cmd == CommandEnums::TWOPAGE)
        {
            this->SetView(ComicSeer::TwoPage);
            emit ViewChanged(this->View());
        }
        else if (cmd == CommandEnums::FIRSTPAGE)
        {
            this->FirstPage();
        }
        else if (cmd == CommandEnums::LASTPAGE)
        {
            this->LastPage();
        }
        else if (cmd == CommandEnums::ZOOMIN)
        {
            this->ZoomIn();
        }
        else if (cmd == CommandEnums::ZOOMOUT)
        {
            this->ZoomOut();
        }
        else
        {
            QGraphicsView::keyPressEvent(event);
        }
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}

void ComicView::closeEvent(QCloseEvent* event)
{
    event->ignore();

    emit ViewClosed();
}

void ComicView::paintEvent(QPaintEvent *e)
{
    QGraphicsView::paintEvent(e);
}

void ComicView::SetScaleFactor(double value)
{
    if (value < 1.0 || value > 4.0 || (this->_scaleFactor - 0.1 < value && this->_scaleFactor + 0.1 > value))
    {
        return;
    }

	this->_scaleFactor = value;

    if (!this->_changingScale)
    {
        this->_state = ComicSeer::Manual;
    }

	this->TransformImage();

	emit scaleChanged(this->_scaleFactor);
}

double ComicView::ScaleFactor()
{
    if (this->_state == ComicSeer::FitWidthHeight)
	{
		return 1.0;
	}
	else
	{
		return this->_scaleFactor;
	}
}

void ComicView::TransformImage()
{
	// verify that we have an image to prevent damage
    if (this->_currentPixmap.get() == NULL)
	{
		return;
	}

	this->resetTransform();

    QRectF imageRect = this->_scene.itemsBoundingRect();
	double widthScale = (this->width() - PADDING) / imageRect.width();
	double heightScale = (this->height() - PADDING) / imageRect.height();
    this->_baseScaleFactor = min(widthScale, heightScale);

	// if scale is unset or set to fitting both width and height, use the base factor
    if (this->_state == ComicSeer::FitWidthHeight)
	{
		this->_scaleFactor = 1.0;
		this->scale(this->_baseScaleFactor, this->_baseScaleFactor);

        this->TriggerScaleChange(1.0);
	}
	// if set to manual mode, use the scale only
    else if (this->_state == ComicSeer::Manual)
	{
		double scale = this->_scaleFactor * this->_baseScaleFactor;
		this->scale(scale, scale);
	}
	// if set to fit width, reset the scale appropriately
    else if (this->_state == ComicSeer::FitWidth)
	{
		this->_scaleFactor = widthScale / this->_baseScaleFactor;
		this->scale(widthScale, widthScale);

        this->TriggerScaleChange(this->_scaleFactor);
	}
	// if set to fit height, reset the scale appropriately
	else
	{
		this->_scaleFactor = heightScale / this->_baseScaleFactor;
		this->scale(heightScale, heightScale);

        this->TriggerScaleChange(this->_scaleFactor);
	}
}

void ComicView::SetState(ComicSeer::ImageState state)
{
	this->_state = state;
	this->TransformImage();
}

ComicSeer::ImageState ComicView::State()
{
	return this->_state;
}

void ComicView::SetView(ComicSeer::ImageView view)
{
    if (this->_view != view)
    {
        this->_view = view;
        this->DrawImage();
    }
}

ComicSeer::ImageView ComicView::View()
{
    return this->_view;
}

bool ComicView::SecondPageVisible()
{
    return this->_secondPageVisible;
}

bool ComicView::Magnified()
{
    return this->_magnified;
}

void ComicView::SetMagnified(bool magnified)
{
    this->_magnified = magnified;
    delete this->_zoomPixmap;
    this->_zoomPixmap = NULL;
}

void ComicView::SetPage(int page)
{
    if (page < 0)
    {
        return;
    }

	// verify the page exists
	map<int, string>::iterator it = this->_comicPages.find(page);

	// if the page exists
	if (it != this->_comicPages.end())
	{
        this->_page = page;
        this->ClearPixmaps();

		this->DrawImage();

        emit this->UserPageChange();
	}
}

void ComicView::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		_mousePress = e->pos();
        this->SetCursor(true);
    }
	else
	{
        _mousePress = QPoint(-1, -1);

        if (e->button() == Qt::RightButton && ComicState::State()->AllowMagnification())
        {

            this->SetMagnified(!this->_magnified);
            emit MagnificationChanged(this->_magnified);
            this->MagnifyPoint(e->pos());
        }
	}

    QGraphicsView::mousePressEvent(e);
}

void ComicView::mouseReleaseEvent(QMouseEvent* e)
{
	// if this was a click, take action
	if (_mousePress.x() >= 0 && _mousePress.y() >= 0 && _mousePress == e->pos())
	{
		// if left side of screen
		if (_mousePress.x() < this->width() / 2)
		{
			this->PageBack();
		}
		else
		{
			this->PageForward();
		}

        // draw magnifier
        this->MagnifyPoint(e->pos());

		emit UserPageChange();
	}
	else
	{
		_mousePress = QPoint(-1, -1);
	}

    QGraphicsView::mouseReleaseEvent(e);

    this->SetCursor(false);
}

void ComicView::enterEvent(QEvent *e)
{
    QGraphicsView::enterEvent(e);
    this->SetCursor(false);
}

void ComicView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    this->MagnifyPoint(event->pos());
}
