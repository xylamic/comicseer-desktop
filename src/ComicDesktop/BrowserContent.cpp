/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "BrowserContent.h"
#include <ComicFile.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <list>
#include <ComicImageReader.h>
#include <qlist.h>
#include <qobject.h>
#include <qboxlayout.h>
#include <qlayoutitem.h>
#include <ComicImageReader.h>
#include <qlabel.h>
#include <ComicState.h>
#include "ComicUtils.h"
#include <qpainter.h>
#include <qstyleoption.h>

#define THUMBNAIL_PADDING 40

using namespace ComicSeer;
using namespace boost;
using namespace std;

const char* BrowserContent::_previewDataNameFile = "File";
const char* BrowserContent::_previewDataNamePage = "Page";
const char* BrowserContent::_previewDataNameText = "Text";

BrowserContent::BrowserContent(QWidget* parent)
    : QScrollArea(parent), _loaded(false)
{
    this->_selected = NULL;
    this->_altSelected = NULL;
    this->_currentSize = ComicState::State()->ThumbnailSize();

    // create the content widget
    this->_widget = new QWidget(this);
    this->setWidget(this->_widget);
    this->setMinimumWidth(this->_currentSize + THUMBNAIL_PADDING);
    this->setMaximumWidth(this->_currentSize + THUMBNAIL_PADDING);

    // set scroll area properties
    this->setWidgetResizable(true);
    this->setFrameShape(QFrame::NoFrame);
    this->setFrameShadow(QFrame::Plain);
    this->setLineWidth(0);

	// create the layout in the container
    QVBoxLayout* layout = new QVBoxLayout(this->widget());
	layout->setMargin(0);
	layout->setSpacing(5);
	layout->setAlignment(Qt::AlignCenter);
    this->_widget->setLayout(layout);

	// adjust the scroll size
    this->_widget->adjustSize();

    // set the background and connect to the change signal
    ComicState *state = ComicState::State();
    string color = state->BackgroundColor();
    string forecolor = ComicUtils::GetForegroundContrastColor(color.c_str());
    this->_widget->setStyleSheet(QString("background-color: %1; color: %2").arg(color.c_str()).arg(forecolor.c_str()));
    this->_slotBackUpdated = state->BackgroundColorUpdated.connect([this](const char* color)
    {
        string forecolor = ComicUtils::GetForegroundContrastColor(color);
        QString styleString = QString("background-color: %1; color: %2").arg(color).arg(forecolor.c_str());

        this->_widget->setStyleSheet(styleString);
    });

    // create listener for thumbnail size changes
    this->_slotThumbnailSizeUpdated = state->ThumbnailSizeUpdated.connect([this](int size)
    {
        if (this->_currentSize != size)
        {
            this->_currentSize = size;
        }
        else
        {
            return;
        }

        this->setMinimumWidth(ComicState::State()->ThumbnailSize() + THUMBNAIL_PADDING);
        this->setMaximumWidth(ComicState::State()->ThumbnailSize() + THUMBNAIL_PADDING);
        if (this->_loaded)
        {
            // get the queue length
            size_t queueLength = this->_loadQueue.size();

            for (std::map<int, DescribedPreviewLabel*>::iterator it = this->_labels.begin(); it != this->_labels.end(); it++)
            {
                string text = it->second->Data(this->_previewDataNameText);
                string file = it->second->Data(this->_previewDataNameFile);
                it->second->setFixedSize(size, size);
                it->second->SetText(text.c_str());
                this->_loadQueue.push(boost::tuple<string, string, DescribedPreviewLabel*>(this->_path.c_str(), file.c_str(), it->second));
            }

            this->RunLoadQueue(queueLength);

            this->adjustSize();
            this->EnsureSelectedVisibility();
        }
    });
}

void BrowserContent::RunLoadQueue(int initialSize)
{
    // if queue was 0 and now is greater, start a reader
    if (initialSize == 0 && this->_loadQueue.size() > 0)
    {
        boost::tuple<string, string, DescribedPreviewLabel*> front = this->_loadQueue.front();
        ComicImageReader* reader = new ComicImageReader(front.get<0>().c_str(), front.get<1>().c_str(), ComicState::State()->ThumbnailSize(), ComicState::State()->ThumbnailSize());
        this->connect(reader, SIGNAL(ReadComplete(ComicImageReader*, bool, QPixmap*)), SLOT(ImageReader(ComicImageReader*, bool, QPixmap*)));
        reader->start();
    }
}

void BrowserContent::EnsureSelectedVisibility()
{
    if (this->_selected != NULL)
    {
        this->ensureWidgetVisible(this->_selected);
    }

    if (this->_altSelected != NULL)
    {
        this->ensureWidgetVisible(this->_altSelected);
    }

    if (this->_selected != NULL)
    {
        this->ensureWidgetVisible(this->_selected);
    }
}

void BrowserContent::SetComic(const char* path)
{
	this->_path = path;
}

void BrowserContent::LoadComic()
{
    if (this->_loaded)
    {
        return;
    }

	try
	{
		// initialize the comic
		scoped_ptr<ComicFile> file(ComicFile::CreateComicFile(this->_path.c_str()));
		file->Initialize();
		
		// read the list of files
		list<string> files = file->ImageList();

		// get the queue length
		size_t queueLength = this->_loadQueue.size();

		// launch the readers
        int count = 0;
		for (list<string>::iterator it = files.begin(); it != files.end(); it++)
		{
			QString pageNum = QString("%1").arg(count + 1);

			// create the image label
            DescribedPreviewLabel* label = new DescribedPreviewLabel(this);
            label->setFixedSize(ComicState::State()->ThumbnailSize(), ComicState::State()->ThumbnailSize());
            label->AddData(this->_previewDataNameFile, it->c_str());
            label->AddData(this->_previewDataNamePage, QString("%1").arg(count).toUtf8());
            label->AddData(this->_previewDataNameText, pageNum.toUtf8());
            label->SetText(string(pageNum.toUtf8()));
            this->connect(label, SIGNAL(Clicked(DescribedPreviewLabel*)), SLOT(PreviewClicked(DescribedPreviewLabel*)));
            this->_widget->layout()->addWidget(label);
            this->_labels[count] = label;

			// add to queue for loading
            this->_loadQueue.push(boost::tuple<string, string, DescribedPreviewLabel*>(this->_path.c_str(), it->c_str(), label));

			count++;
		}

        // adjust the scroll size
        this->_widget->adjustSize();

        this->RunLoadQueue(queueLength);
	}
	catch (...)
	{
	}

	this->_loaded = true;
}

void BrowserContent::SelectPage(int page, bool nextVisible)
{
	if (this->_selected != NULL)
	{
		this->_selected->setStyleSheet("");
	}
    if (this->_altSelected != NULL)
    {
        this->_altSelected->setStyleSheet("");
    }

    map<int, DescribedPreviewLabel*>::iterator prev = this->_labels.find(page);
	if (prev != this->_labels.end())
	{
        prev->second->setStyleSheet("DescribedPreviewLabel { border: 3px solid green; }");
		this->_selected = prev->second;
	}

    if (nextVisible)
    {
        map<int, DescribedPreviewLabel*>::iterator prev = this->_labels.find(page + 1);
        if (prev != this->_labels.end())
        {
            prev->second->setStyleSheet("DescribedPreviewLabel { border: 3px solid green; }");
            this->_altSelected = prev->second;
        }
    }

    this->EnsureSelectedVisibility();
}

void BrowserContent::paintEvent(QPaintEvent *e)
 {
     QStyleOption opt;
     opt.init(this);

     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

     QWidget::paintEvent(e);
 }

void BrowserContent::ImageReader(ComicImageReader* reader, bool success, QPixmap* map)
{
	// find the label and pop the queue item
    boost::tuple<string, string, DescribedPreviewLabel*> front = this->_loadQueue.front();
    DescribedPreviewLabel* label = NULL;
	if (strcmp(front.get<0>().c_str(), reader->Path().c_str()) == 0 && strcmp(front.get<1>().c_str(), reader->File().c_str()) == 0)
	{
		label = front.get<2>();
		this->_loadQueue.pop();
	}

	// add a smart pointer to the map so it is deleted on return
	boost::scoped_ptr<QPixmap> smart_map(map);

	// delete the reader
	if (reader->wait(3000))
	{
		delete reader;
	}

	// if all the stars align, show the image
	if (success && smart_map.get() != NULL && label != NULL)
	{
        label->SetPixmap(*map);
	}

	// load the next queue item if one waits
    this->RunLoadQueue(0);
}

void BrowserContent::PreviewClicked(DescribedPreviewLabel* label)
{
	string pagestr = label->Data(this->_previewDataNamePage);
	if (pagestr.empty())
	{
		return;
	}

    int page = ::atoi(pagestr.c_str());
	emit PageSelected(page);
}

void BrowserContent::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);

    this->LoadComic();

    this->EnsureSelectedVisibility();
}
