/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_COMICSELECTIONTABWIDGET_H
#define X_COMICSELECTIONTABWIDGET_H

#include <Xylasoft.hpp>
#include <QtWidgets/qtabwidget.h>
#include <qevent.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>

/*!
brief The ComicSelectionTabWidget class inherits from QTabWidget to provide
customized behavior for this application.
 */
class ComicSelectionTabWidget : public QTabWidget
{
	Q_OBJECT

public:
    /*!
    @brief Construct the tab widget.
    @param parent The parent widget.
    */
	ComicSelectionTabWidget(QWidget* parent = NULL);

    /*!
    @brief Set the visibility of the tab bar.
    @param visible Whether the tab bar should be visible.
    */
    void SetTabBarVisibility(bool visible);

    /*!
    @brief Add a menu item to the tab content menu.
    @param actionId The idenifier for the action.
    @param text The text for the menu item.
    */
    void AddCustomContextMenuAction(int actionId, QString& text);

signals:
    /*!
    @brief Custom context menu item was triggered.
    @param actionId The action identifier.
    @param tabIndex The tab index triggered from.
    */
    void CustomContextMenuTriggered(int actionId, int tabIndex);

protected:
    /*!
    @brief The "Close" menu item was clicked.
    @param tabIndex The index of the selected tab.
    */
    void MenuCloseClicked(int tabIndex);

    /*!
    @brief The "Close all other" menu item was clicked.
    @param tabIndex The index of the selected tab.
    */
    void MenuCloseOthersClicked(int tabIndex);

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private:
    QMenu _contextMenu;
    QAction* _contextActionClose;
    QAction* _contextActionCloseOthers;
    bool _additionalMenuItemAdded;
};

#endif
