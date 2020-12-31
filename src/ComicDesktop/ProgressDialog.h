/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <Xylasoft.hpp>
#include <QtWidgets/QProgressBar>

/*!
This provides a reusable dialog for showing progress as a modal dialog.
*/
class ProgressDialog : public QProgressBar
{
    Q_OBJECT
    
public:
    /*!
    Construct a progress dialog.
    @param[in] parent This is the parent in position only.
    */
    explicit ProgressDialog(QWidget *parent = 0);
};

#endif // PROGRESSDIALOG_H
