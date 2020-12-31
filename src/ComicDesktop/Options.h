/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <Xylasoft.hpp>
#include <QtWidgets/QDialog>
#include "ui_Options.h"

/*!
This dialog provides the options for configuration in Comic Seer.
 */
class Options : public QDialog
{
    Q_OBJECT

public:
    /*!
    Construct the options dialog.
    @param parent The parent to the dialog.
    */
    Options(QWidget *parent = 0);
    
protected:
    /*!
    @brief Load the options dialog.
    */
    void Load();
    
public slots:
    /*!
    The accept event was received.
    */
    virtual void accept();

private slots:
    /*!
    @brief Event for changing of the default view.
    @param index The selected index.
    */
    void DefaultViewStateChanged(int index);

    /*!
    @brief RestoreDefaults triggered on menu.
    */
    void RestoreDefaults();

private:
    Ui_Options ui;
};

#endif // OPTIONS_H
