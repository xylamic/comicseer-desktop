/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ProgressDialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QProgressBar(NULL)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setTextVisible(false);

    this->setValue(0);

    if (parent != NULL)
    {
        QRect parentGeometry = parent->geometry();

        QRect geometry(
            parentGeometry.x() + parentGeometry.width() / 2 - 75,
            parentGeometry.y() + parentGeometry.height() / 2 - 10,
            150, 20);

        this->setGeometry(geometry);
    }

    this->setWindowModality(Qt::WindowModal);
}
