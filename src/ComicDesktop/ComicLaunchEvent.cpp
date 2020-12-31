/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicLaunchEvent.h"

const QEvent::Type ComicLaunchEvent::Event = static_cast<QEvent::Type>(QEvent::registerEventType());

ComicLaunchEvent::ComicLaunchEvent(const char* path, int page)
	: QEvent(Event)
{
    this->_page = page;
	this->_path = path;
}


std::string ComicLaunchEvent::Path() const
{
    return this->_path;
}

int ComicLaunchEvent::Page() const
{
    return this->_page;
}
