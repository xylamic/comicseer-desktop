/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef ABOUTVIEW_H
#define ABOUTVIEW_H

#include <QtWidgets/QDialog>

namespace Ui {
class AboutView;
}

/*!
\brief The AboutView class provides 'about' functionality for the Comic Seer
application.
*/
class AboutView : public QDialog
{
    Q_OBJECT

public:
    /*!
    @brief Construct the about view.
    @param parent The parent widget.
    */
    explicit AboutView(QWidget *parent = 0);
    ~AboutView();

private slots:
    /*!
    @brief The close button was clicked.
    */
    void on_toolButtonClose_clicked();

private:
    Ui::AboutView *ui;
};

#endif // ABOUTVIEW_H
