/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicDockWidget.h"
#include <qevent.h>
#include <qpainter.h>

ComicDockWidget::ComicDockWidget(QWidget* child, QString name, QWidget *parent, Qt::DockWidgetAreas areas) :
    QDockWidget(name, parent)
{
	_view = child;
	this->setObjectName(name);
	this->setWidget(_view);

    //this->setStyleSheet("QDockWidget::title { background:#DBFA7D; text-align:center; padding:2px; border:1px solid black; }");

	// this window can be moved/docked/closed
	this->setFeatures(DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable);

    // set the dock limitations
    this->setAllowedAreas(areas);
}

void ComicDockWidget::closeEvent(QCloseEvent* e)
{
	QDockWidget::closeEvent(e);

	emit closeTriggered();
}

QWidget* ComicDockWidget::ContainedWidget()
{
	return this->_view;
}
