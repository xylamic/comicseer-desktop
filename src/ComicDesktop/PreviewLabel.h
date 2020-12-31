/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include <Xylasoft.hpp>
#include <QtWidgets/qlabel.h>
#include <qpixmap.h>
#include <XByteArray.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include "ComicImageReader.h"
#include <queue>
#include <map>

/*!
Label extending for showing preview images.
*/
class PreviewLabel : public QLabel
{
	Q_OBJECT

public:
	/*!
	Construct the label.
	@param[in] fixSize Whether the size of the image is fixed and memory can be saved.
	@param[in] parent The parent object.
	*/
	PreviewLabel(QWidget *parent = 0);

	/*!
	Set a comic to preview. Loads asynchronously.
	@param[in] path The path to the comic file for preview.
	@param[in] file The file to read from the comic, NULL for title.
	*/
	void SetImage(const char* path, const char* file = NULL);

	/*!
	Set the image to preview.
	@param[in] data The data to load for preview.
	*/
	void SetImage(QByteArray& data);

	/*!
	Set the image to preview.
	@param[in] map The pixel map to show.
	*/
    void SetImage(const QPixmap& map);

	/*!
	Add data to the label.
	@param[in] name The data name.
	@param[in] data The data.
	*/
	void AddData(const char* name, const char* data);

	/*!
	Get the data of the given name.
	@param[in] name The data name.
	@returns The data or empty if none found.
	*/
	std::string Data(const char* name);

signals:
	/*!
	Signal for sending a click event.
	@param[in] sender The sender of the event.
	*/
	void Clicked(PreviewLabel* sender);

    /*!
    @brief The comic information has been read.
    @param data The comic information.
    */
    void ComicInformationRead(const ComicSeer::XByteArray &data);

protected slots:
	/*!
	Event received when data has finished reading.
	@param[in] read The comic reader instance.
	@param[in] success The success of the request.
	@param[in] map The pixel map if successful.
	*/
	void ImageReader(ComicImageReader* reader, bool success, QPixmap* map);

protected:
	/*!
	Resize event triggered for the label. Resizes the preview.
	@param[in] e The event arguments.
	*/
	virtual void resizeEvent(QResizeEvent* e);

	/*!
	Mouse down event received.
	@param[in] e The mouse event.
	*/
	virtual void mousePressEvent(QMouseEvent* e);

	/*!
	Mouse up event received.
	@param[in] e The mouse event.
	*/
	virtual void mouseReleaseEvent(QMouseEvent* e);

private:
	bool _mouseDown;
	std::map<std::string, std::string> _data;
	const QString _textPreviewFailed;
	boost::scoped_ptr<QPixmap> _previewImage;
	boost::scoped_ptr<ComicImageReader> _reader;
	std::queue<std::string> _readQueue;
    bool _reading;
};

#endif
