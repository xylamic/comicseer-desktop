/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef DESCRIBEDPREVIEWLABEL_H
#define DESCRIBEDPREVIEWLABEL_H

#include <Xylasoft.hpp>
#include <QtWidgets/QWidget>
#include <QPixmap>
#include "ui_DescribedPreviewLabel.h"

/*!
This widget adds a description to a preview label and some
other features.
*/
class DescribedPreviewLabel : public QWidget
{
    Q_OBJECT
    
public:
    /*!
    Construct the preview label.
    @param[in] parent The parent widget.
    */
    explicit DescribedPreviewLabel(QWidget *parent = 0);

    /*!
    Set the image to be shown.
    @param[in] map The pixmap to show.
    */
    void SetPixmap(const QPixmap& map);

    /*!
    Set the text to be displayed with the image.
    @param[in] text The text to display.
    */
    void SetText(std::string text);

    /*!
    Get the text that is displayed.
    */
    std::string Text();

    /*!
    Add data to the label.
    @param[in] name The data name.
    @param[in] data The data.
    */
    void AddData(const char* name, const char* data);

    /*!
    Get the data of the given name.
    @param[in] name The data name.
    @returns The data or empty if none found.
    */
    std::string Data(const char* name);

signals:
    /*!
    Signal for sending a click event.
    @param[in] sender The sender of the event.
    */
    void Clicked(DescribedPreviewLabel* sender);

protected slots:
    /*!
    Event that the preview label was clicked.
    @param[in] sender The sender of the event.
    */
    void ImageClicked(PreviewLabel* sender);

protected:
    /*!
    Event received to paint.
    @param e The paint event.
    */
    virtual void paintEvent(QPaintEvent *e);
    
private:
    Ui::DescribedPreviewLabel ui;
    std::string _data;
};

#endif // DESCRIBEDPREVIEWLABEL_H
