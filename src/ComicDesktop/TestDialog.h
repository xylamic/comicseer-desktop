/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QtWidgets/QDialog>

namespace Ui {
class TestDialog;
}

class TestDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestDialog(QWidget *parent = 0);
    ~TestDialog();
    
private slots:
    void on_pushButtonCycleText_clicked();

    void on_pushButtonReadFile_clicked();

private:
    Ui::TestDialog *ui;
};

#endif // TESTDIALOG_H
