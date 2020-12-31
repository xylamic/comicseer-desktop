/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "DescribedPreviewLabel.h"
#include <qpainter.h>
#include <qstyleoption.h>

DescribedPreviewLabel::DescribedPreviewLabel(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    this->connect(ui.labelImage, SIGNAL(Clicked(PreviewLabel*)), SLOT(ImageClicked(PreviewLabel*)));
}

void DescribedPreviewLabel::SetPixmap(const QPixmap& map)
{
    ui.labelImage->SetImage(map);
}

void DescribedPreviewLabel::SetText(std::string text)
{
    ui.labelName->setText(QString::fromUtf8(text.c_str()));
}

std::string DescribedPreviewLabel::Text()
{
    return std::string(ui.labelName->text().toUtf8());
}

void DescribedPreviewLabel::AddData(const char* name, const char* data)
{
    ui.labelImage->AddData(name, data);
}

std::string DescribedPreviewLabel::Data(const char* name)
{
    return ui.labelImage->Data(name);
}

void DescribedPreviewLabel::ImageClicked(PreviewLabel* sender)
{
    emit Clicked(this);
}

void DescribedPreviewLabel::paintEvent(QPaintEvent *e)
 {
     QStyleOption opt;
     opt.init(this);

     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

     QWidget::paintEvent(e);
 }
