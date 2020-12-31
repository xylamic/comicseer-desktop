/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICIMAGEREADER_H
#define X_COMICIMAGEREADER_H

#include <Xylasoft.hpp>
#include <qthread.h>
#include <qpixmap.h>

/*!
Thread for reading comic image files asynchronously.
*/
class ComicImageReader : public QThread
{
	Q_OBJECT

public:
	/*!
	Construct the image reader.
	@param[in] path The path to the comic file.
	@param[in] file The file to read or NULL for the title image.
	*/
	ComicImageReader(const char* path, const char* file = NULL, int width = -1, int height = -1);

	/*!
	Get the path.
	*/
	std::string Path();

	/*!
	Get the file.
	*/
	std::string File();

protected:
	/*!
	Run the read operation.
	*/
	virtual void run();

signals:
	/*!
	Signal emitted when the read is complete with either success or failure.
	@param[in] reader The comic reader instance.
	@param[in] success The success of the operation. False if failed.
	@param[in] map The pixel map that was generated.
	*/
	void ReadComplete(ComicImageReader* reader, bool success, QPixmap* map);

private:
	std::string _path;
	std::string _file;
	int _width;
	int _height;
};

#endif
