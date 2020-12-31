/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "BusyIndicator.h"
#include "ui_BusyIndicator.h"

BusyIndicator::BusyIndicator(const QString &text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyIndicator)
{
    ui->setupUi(this);
    ui->labelText->setText(text);
}

BusyIndicator::~BusyIndicator()
{
    delete ui;
}

void BusyIndicator::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QApplication::processEvents();
}
