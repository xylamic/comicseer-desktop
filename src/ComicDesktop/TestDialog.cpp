/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include <Xylasoft.hpp>
#include "TestDialog.h"
#include "ui_TestDialog.h"
#include <fstream>
#include <QFile>
#include <wchar.h>
#include <XUtilities.hpp>

using namespace std;
using namespace ComicSeer;

TestDialog::TestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestDialog)
{
    ui->setupUi(this);
}

TestDialog::~TestDialog()
{
    delete ui;
}

void TestDialog::on_pushButtonCycleText_clicked()
{
    QString str = ui->lineEditCycleTextInput->text();
    ui->lineEditCycleTextAscii->setText(QString::fromLatin1(string(str.toLatin1()).c_str()));
    ui->lineEditCycleTextUtf8->setText(QString::fromUtf8(string(str.toUtf8()).c_str()));
    ui->lineEditCycleTextLocal8->setText(QString::fromLocal8Bit(string(str.toLocal8Bit()).c_str()));
    ui->lineEditCycleTextWideChar->setText(QString::fromWCharArray(wstring(str.toStdWString()).c_str()));
}

void TestDialog::on_pushButtonReadFile_clicked()
{
    QString str = ui->lineEditInputFile->text();

    string input;

    QString status;

    QFile f(str);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray a = f.read(4);
        f.close();

        status.append(QString("[Qt]%1 ").arg(QString::fromLatin1(a)));
    }
    else
    {
        status.append(QString("[Qt]Fail "));
    }

    ifstream ascii(str.toLatin1());
    if (ascii.is_open())
    {
        ascii >> input;
        ascii.close();

        status.append(QString("[ASCII]%1 ").arg(input.c_str()));
    }
    else
    {
        status.append(QString("[ASCII]Fail "));
    }

    ifstream local8(str.toLocal8Bit());
    if (local8.is_open())
    {
        local8 >> input;
        local8.close();

        status.append(QString("[Local8]%1 ").arg(input.c_str()));
    }
    else
    {
        status.append(QString("[Local8]Fail "));
    }

    ifstream utf8(str.toUtf8());
    if (utf8.is_open())
    {
        utf8 >> input;
        utf8.close();

        status.append(QString("[UTF8]%1 ").arg(input.c_str()));
    }
    else
    {
        status.append(QString("[UTF8]Fail "));
    }

    wstring file = XUtilities::ConvertUtf8ToWString(string(str.toUtf8()));
    char buffer[4096];
    ::wcstombs(buffer, file.c_str(), 4096);

    ifstream wstr(buffer);
    if (wstr.is_open())
    {
        wstr >> input;
        wstr.close();

        status.append(QString("[WStr]%1 ").arg(input.c_str()));
    }
    else
    {
        status.append(QString("[WStr]Fail "));
    }

    ui->labelFileReadStatus->setText(status);
}
