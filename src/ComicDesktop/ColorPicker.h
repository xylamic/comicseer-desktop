/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <Xylasoft.hpp>
#include <QtWidgets/qwidget.h>
#include <qcolor.h>
#include <qevent.h>

/*!
@brief The ColorPicker class allowed the user to select a color and
displays it.
*/
class ColorPicker : public QWidget
{
    Q_OBJECT

public:
    /*!
    @brief Construct the color picker.
    @param parent The parent widget.
    */
    explicit ColorPicker(QWidget *parent = 0);
    
    /*!
    @brief Set the color to display.
    @param color The color to display.
    */
    void SetColor(QColor color);

    /*!
    @brief Get the color displayed.
    @return QColor The color.
    */
    QColor Color();

protected:
    /*!
    @brief The paint event that draws the appropriate color.
    @param e The event arguments.
    */
    virtual void paintEvent(QPaintEvent *e);

    /*!
    @brief The mouse press event that start the trigger.
    @param e The mouse event arguments.
    */
    virtual void mousePressEvent(QMouseEvent *e);

    /*!
    @brief The mouse release event that can trigger the color selector.
    @param e The mouse event arguments.
    */
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    QColor _color;
    bool _mouseDown;
};

#endif // COLORPICKER_H
