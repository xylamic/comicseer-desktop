/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ColorPicker.h"
#include <qpainter.h>
#include <QtWidgets/qcolordialog.h>

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent)
{
    this->setMinimumHeight(20);
    this->setMaximumHeight(20);

    this->_color = QColor("black");
    this->_mouseDown = false;
}

void ColorPicker::SetColor(QColor color)
{
    this->_color = color;
    this->update();
}

QColor ColorPicker::Color()
{
    return this->_color;
}

void ColorPicker::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);

    painter.setBrush(QBrush(this->_color));
    painter.setPen(QPen(Qt::gray));
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    painter.end();

    QWidget::paintEvent(e);
}

void ColorPicker::mousePressEvent(QMouseEvent *e)
{
    this->_mouseDown = true;

    QWidget::mousePressEvent(e);
}

void ColorPicker::mouseReleaseEvent(QMouseEvent *e)
{
    if (this->_mouseDown)
    {
        QColorDialog dlg(this->_color);
        if (dlg.exec())
        {
            this->_color = dlg.currentColor();
            this->update();
        }
    }

    this->_mouseDown = false;

    QWidget::mouseReleaseEvent(e);
}
