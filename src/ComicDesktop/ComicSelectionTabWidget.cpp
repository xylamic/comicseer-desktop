/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicSelectionTabWidget.h"
#include <QtWidgets/qtabbar.h>

ComicSelectionTabWidget::ComicSelectionTabWidget(QWidget* parent)
	: QTabWidget(parent)
{
    _contextActionClose = new QAction(tr("&Close"), &this->_contextMenu);
    _contextActionCloseOthers = new QAction(tr("Close &all others"), &this->_contextMenu);

    _contextMenu.addAction(_contextActionClose);
    _contextMenu.addAction(_contextActionCloseOthers);

    _additionalMenuItemAdded = false;
}


void ComicSelectionTabWidget::SetTabBarVisibility(bool visible)
{
    this->tabBar()->setVisible(visible);
}

void ComicSelectionTabWidget::AddCustomContextMenuAction(int actionId, QString &text)
{
    if (!_additionalMenuItemAdded)
    {
        _contextMenu.addSeparator();
        _additionalMenuItemAdded = true;
    }

    QAction* action = _contextMenu.addAction(text);
    action->setData(actionId);
}

void ComicSelectionTabWidget::MenuCloseClicked(int tabIndex)
{
    // request close of the selected
    emit this->tabCloseRequested(tabIndex);
}

void ComicSelectionTabWidget::MenuCloseOthersClicked(int tabIndex)
{
    // iterate over all tabs, requesting close of all but the selected
    for (int index = this->count() - 1; index >= 0; index--)
    {
        if (tabIndex != index)
        {
            emit this->tabCloseRequested(index);
        }
    }
}

void ComicSelectionTabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        int tabIndex = this->tabBar()->tabAt(event->pos());
        if (tabIndex >= 0)
        {
            QPoint global = this->tabBar()->mapToGlobal(event->pos());
            QAction* action = _contextMenu.exec(global);
            if (action != nullptr)
            {
                if (action == this->_contextActionClose)
                {
                    this->MenuCloseClicked(tabIndex);
                }
                else if (action == this->_contextActionCloseOthers)
                {
                    this->MenuCloseOthersClicked(tabIndex);
                }
                else
                {
                    emit CustomContextMenuTriggered(action->data().toInt(), tabIndex);
                }
            }

            return;
        }
    }

    QTabWidget::mousePressEvent(event);
}
