/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QtWidgets/QWidget>
#include "ui_HomePage.h"

/*!
brief The HomePage class provides the central widget to the applications for news and help.
 */
class HomePage : public QWidget
{
    Q_OBJECT
    
public:
    /*!
    @brief Construct the homepage.
    @param parent The parent widget.
    */
    explicit HomePage(QWidget *parent = 0);

    /*!
    Destruct the homepage.
    */
    ~HomePage();
    
private:
    Ui::HomePage *ui;
    QWidget* _centralWidget;
};

#endif // HOMEPAGE_H
