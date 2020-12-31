/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "PreviewLabel.h"
#include <ComicFile.hpp>
#include <qbuffer.h>
#include <qimagereader.h>
#include <qdebug.h>
#include <qevent.h>

#define DBGFLD qDebug() << "PreviewLabel"

using namespace ComicSeer;
using namespace std;

PreviewLabel::PreviewLabel(QWidget *parent)
	: QLabel(parent), _textPreviewFailed(tr("Preview Failed")), _mouseDown(false)
{
    this->_reading = false;
}

void PreviewLabel::SetImage(const char* path, const char* file)
{
	DBGFLD << "Setting image: " << path << ", " << (file == NULL ? "NULL" : file);

	this->_readQueue.push(path);

    // run reader if not reading
    if (!this->_reading)
	{
        // clear any others waiting on the queue
		while (this->_readQueue.size() > 1)
        {
			this->_readQueue.pop();
		}

		this->_reader.reset(new ComicImageReader(this->_readQueue.front().c_str(), file));
		this->connect(this->_reader.get(), SIGNAL(ReadComplete(ComicImageReader*, bool, QPixmap*)), SLOT(ImageReader(ComicImageReader*, bool, QPixmap*)));
		this->_reader->start();

        this->_reading = true;
	}
}

void PreviewLabel::SetImage(QByteArray& data)
{
	// parse the image data
	QBuffer buffer(&data);
	QImageReader imageReader(&buffer);
	imageReader.setAutoDetectImageFormat(true);

	// save and display the image
	this->_previewImage.reset(new QPixmap(QPixmap::fromImageReader(&imageReader)));
	this->setPixmap(this->_previewImage->scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PreviewLabel::SetImage(const QPixmap& map)
{
	this->_previewImage.reset(new QPixmap(map));
	this->setPixmap(this->_previewImage->scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PreviewLabel::resizeEvent(QResizeEvent* e)
{
	if (this->_previewImage != NULL)
	{
		this->setPixmap(this->_previewImage->scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
}

void PreviewLabel::mousePressEvent(QMouseEvent* e)
{
	this->_mouseDown = true;

	QLabel::mousePressEvent(e);
}

void PreviewLabel::mouseReleaseEvent(QMouseEvent* e)
{
	if (this->_mouseDown)
	{
		emit Clicked(this);
	}

	this->_mouseDown = false;

	QLabel::mouseReleaseEvent(e);
}

void PreviewLabel::ImageReader(ComicImageReader* reader, bool success, QPixmap* map)
{
    try
    {
        DBGFLD << "Read completed for image: " << reader->Path().c_str() << ", " << reader->File().c_str() << " with result " << success;

        // if the read failed
        if (!success || map == NULL)
        {
            this->setText(this->_textPreviewFailed);
            this->_reading = false;
            return;
        }

        // set the image retrieved
        this->SetImage(*map);

        // delete the buffer
        delete map;

        // drop the front of the queue
        this->_readQueue.pop();

        // wait for the thread to end
        if (this->_reader->wait(200) && this->_readQueue.size() > 0)
        {
            while (this->_readQueue.size() > 1)
            {
                DBGFLD << "Popped past read [Thread] for " << this->_readQueue.front().c_str();
                this->_readQueue.pop();
            }

            DBGFLD << "Starting reader [Thread] for " << this->_readQueue.front().c_str();
            this->_reader.reset(new ComicImageReader(this->_readQueue.front().c_str()));
            this->connect(this->_reader.get(), SIGNAL(ReadComplete(ComicImageReader*, bool, QPixmap*)), SLOT(ImageReader(ComicImageReader*, bool, QPixmap*)));
            this->_reader->start();

            // make sure reading flag is set
            this->_reading = true;
        }
        else
        {
            this->_reading = false;
        }
    }
    catch (...)
    {
        this->_reading = false;
    }
}

void PreviewLabel::AddData(const char* name, const char* data)
{
	this->_data[name] = data;
}

std::string PreviewLabel::Data(const char* name)
{
	map<string, string>::iterator item = this->_data.find(name);
	if (item == this->_data.end())
	{
		return string();
	}
	else
	{
		return item->second;
	}
}
