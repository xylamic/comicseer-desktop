/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicImageReader.h"
#include <ComicFile.hpp>
#include <XByteArray.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <qbuffer.h>
#include <qimagereader.h>
#include <qbytearray.h>
#include <ComicUtilities.hpp>

using namespace ComicSeer;

ComicImageReader::ComicImageReader(const char* path, const char* file, int width, int height)
{
	this->_width = width;
	this->_height = height;
	this->_path = path;
	if (file != NULL)
	{
		this->_file = file;
	}
}

std::string ComicImageReader::Path()
{
	return this->_path;
}

std::string ComicImageReader::File()
{
	return this->_file;
}

void ComicImageReader::run()
{
	// preview the file
	try
	{
		// create the comic
		boost::scoped_ptr<ComicFile> file(ComicFile::CreateComicFile(this->_path.c_str()));
		file->Initialize();

		// read the image
		XByteArray data;
		if (this->_file.empty())
		{
			file->GetTitleImage(data);
		}
		else
		{
			file->GetFile(this->_file.c_str(), data);
		}

		// convert the image data
		QByteArray qdata(data.ConstData(), data.Size());

		// parse the image data
		QBuffer buffer(&qdata);
		QImageReader imageReader(&buffer);
		imageReader.setAutoDetectImageFormat(true);

		// resize if requested a fixed size
		if (this->_height > 0 && this->_width > 0)
		{
			QSize size = imageReader.size();

			double scale = ComicUtilities::ScaleKeepAspectRatio(size.width(), size.height(), this->_width, this->_height);

			// if the scale was less 1.0, do the resize, otherwise it is already smaller
			if (scale < 1.0)
			{
				size.setWidth(size.width() * scale);
				size.setHeight(size.height() * scale);

				imageReader.setScaledSize(size);
			}
		}

		// save and display the image
		QPixmap* map = new QPixmap(QPixmap::fromImageReader(&imageReader));

		// send the event
		emit ReadComplete(this, true, map);
	}
	catch (...)
	{
        emit ReadComplete(this, false, NULL);
	}
}
