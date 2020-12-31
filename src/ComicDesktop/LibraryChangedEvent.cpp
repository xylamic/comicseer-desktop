/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "LibraryChangedEvent.h"

const QEvent::Type LibraryChangedEvent::AddEvent = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type LibraryChangedEvent::RemoveEvent = static_cast<QEvent::Type>(QEvent::registerEventType());

LibraryChangedEvent::LibraryChangedEvent(bool added)
	: QEvent(static_cast<QEvent::Type>(added ? AddEvent : RemoveEvent))
{
}
