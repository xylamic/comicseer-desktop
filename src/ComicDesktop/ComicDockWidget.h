/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICDOCKWIDGET_H
#define X_COMICDOCKWIDGET_H

#include <Xylasoft.hpp>
#include <QtWidgets/qdockwidget.h>

/*!
Dockable pane for displayed the library information.
*/
class ComicDockWidget : public QDockWidget
{
    Q_OBJECT
public:
	/*!
	Construct the widget.
	@param[in] child The child widget to insert.
	@param[in] name The name of the dock.
	@param[in] parent The parent widget.
    @param[in] areas The areas allowed to dock.
	*/
    explicit ComicDockWidget(QWidget* child, QString name, QWidget *parent = 0,
                             Qt::DockWidgetAreas areas = Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	/*!
	Get the contained widget.
	@returns The comic widget.
	*/
    QWidget* ContainedWidget();

signals:
	/*!
	Signal that the user clicked the close on the panel.
	*/
	void closeTriggered();

protected:
	/*!
	Event received that the user clicked the close on the panel.
	@param[in] e The close event.
	*/
    virtual void closeEvent(QCloseEvent* e);

private:
    QWidget* _view;
};

#endif
