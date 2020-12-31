/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QDialog>

namespace Ui {
class AboutBox;
}

class AboutBox : public QDialog
{
    Q_OBJECT
    
public:
    explicit AboutBox(QWidget *parent = 0);
    ~AboutBox();
    
private:
    Ui::AboutBox *ui;
};

#endif // ABOUTBOX_H
