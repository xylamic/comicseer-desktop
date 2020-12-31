/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef BUSYINDICATOR_H
#define BUSYINDICATOR_H

#include <Xylasoft.hpp>
#include <QtWidgets/QDialog>

namespace Ui {
    class BusyIndicator;
}

/*!
@brief The BusyIndicator provides a simple dialog for work indication.
*/
class BusyIndicator : public QDialog
{
    Q_OBJECT

public:
    /*!
    @brief Construct the busy indicator.
    @param text The text to display.
    @param parent The parent widget.
    */
    explicit BusyIndicator(const QString &text, QWidget *parent = 0);

    /*!
    @brief Destructor.
    */
    ~BusyIndicator();

protected:
    /*!
    @brief The dialog is being shown.
    @param e The show event.
    */
    virtual void showEvent(QShowEvent *e);

private:
    Ui::BusyIndicator *ui;
};

#endif // BUSYINDICATOR_H
