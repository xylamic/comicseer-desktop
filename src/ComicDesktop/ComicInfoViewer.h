/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICINFOVIEWER_H
#define COMICINFOVIEWER_H

#include <QtWidgets/QDialog>
#include <QMap>

namespace Ui {
class ComicInfoViewer;
}

class ComicInfoViewer : public QDialog
{
    Q_OBJECT

public:
    ~ComicInfoViewer();

    static void OpenComicInfoViewer(const QString &file);

    void Load();

protected:
    explicit ComicInfoViewer(const QString &file, QWidget *parent = 0);

    void SetFailureStatus(const std::string &message);

private slots:
    void on_buttonComicTagger_clicked();

    void on_toolButtonRefresh_clicked();

    void on_toolButtonClose_clicked();

private:
    Ui::ComicInfoViewer *ui;
    QString _file;

    static QMap<QString, ComicInfoViewer*> _windows;
};

#endif // COMICINFOVIEWER_H
