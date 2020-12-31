/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef LIBRARYCHANGEDEVENT_H
#define LIBRARYCHANGEDEVENT_H

#include <Xylasoft.hpp>
#include <qevent.h>

/*!
Event for when the user library has been modified.
*/
class LibraryChangedEvent : public QEvent
{
public:
	/*! Static event registration. */
	static const QEvent::Type AddEvent;
	static const QEvent::Type RemoveEvent;

	/*!
	Construct the event.
	@param[in] added Whether the library was added, removed otherwise.
	*/
	LibraryChangedEvent(bool added);
};

#endif
