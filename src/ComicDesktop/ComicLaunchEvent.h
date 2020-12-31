/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICLAUNCHEVENT_H
#define X_COMICLAUNCHEVENT_H

#include <Xylasoft.hpp>
#include <qevent.h>
#include <ComicFile.hpp>

/*!
Event for requesting the launch of a comic.
*/
class ComicLaunchEvent : public QEvent
{
public:
	/*! Static event registration. */
	static const QEvent::Type Event;

	/*!
	Construct the event.
	@param[in] path The path to the comic to launch.
    @param[in] page The requested zero-based page in the comic, -1 for none.
	*/
    ComicLaunchEvent(const char* path, int page = -1);

	/*!
	Get the path of the comic.
	@returns The file path.
	*/
	std::string Path() const;

    /*!
    @brief Get the requested page number.
    @return int The page number.
    */
    int Page() const;

private:
	std::string _path;
    int _page;
};

#endif
