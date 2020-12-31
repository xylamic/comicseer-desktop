/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "AboutView.h"
#include "ui_AboutView.h"
#include "ComicUtils.h"

AboutView::AboutView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutView)
{
    ui->setupUi(this);
    ui->labelCopyright->setText(ui->labelCopyright->text().replace("{VERSION}", ComicUtils::GetVersionString()));
}

AboutView::~AboutView()
{
    delete ui;
}

void AboutView::on_toolButtonClose_clicked()
{
    this->close();
}
