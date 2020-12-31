/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicAppNotification.h"
#include "ui_ComicAppNotification.h"
#include <QSettings>
#include <QDateTime>
#include <QDesktopServices>
#include "ComicUtils.h"
#include "ComicUtilities.hpp"
#include <QUrl>

#define SETTINGS_LASTSHOWN "Windows8LastShown"
#define SETTINGS_TRAVERSED "Windows8Navigated"

ComicAppNotification::ComicAppNotification(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComicAppNotification)
{
    ui->setupUi(this);
}

ComicAppNotification::~ComicAppNotification()
{
    delete ui;
}

bool ComicAppNotification::TryShowDialog(QWidget* parent = NULL)
{
    bool versionApplicable = false;

#ifdef WIN32
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS8_1)
    {
        versionApplicable = true;
    }
#endif

    if (!versionApplicable)
    {
        return false;
    }

    QSettings settings(ComicUtils::CompanyName, ComicUtils::ApplicationName);
    bool traversed = settings.value(SETTINGS_TRAVERSED, false).toBool();
    QDateTime lastShown = settings.value(SETTINGS_LASTSHOWN, QDateTime::currentDateTimeUtc().addYears(-1)).toDateTime();

    // if it was already traversed, ignore request
    if (traversed)
    {
        return false;
    }

    // if greater than 5 days has elapsed, shown again
    if (lastShown < QDateTime::currentDateTimeUtc().addDays(-5))
    {
        ComicAppNotification dlg(parent);
        dlg.setWindowFlags(Qt::Dialog);
        dlg.exec();

        return true;
    }

    return false;
}

void ComicAppNotification::on_btnGo_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("http://www.xylasoft.com/appaccess/renavigate.php?page=cs_in_win_store&origin=comic_desktop")));

    on_btnDontRemind_clicked();
}

void ComicAppNotification::on_btnDontRemind_clicked()
{
    QSettings settings(ComicUtils::CompanyName, ComicUtils::ApplicationName);
    settings.setValue(SETTINGS_TRAVERSED, true);
    settings.setValue(SETTINGS_LASTSHOWN, QDateTime::currentDateTimeUtc());

    QDialog::accept();
}

void ComicAppNotification::on_btnRemindMe_clicked()
{
    QSettings settings(ComicUtils::CompanyName, ComicUtils::ApplicationName);
    settings.setValue(SETTINGS_TRAVERSED, false);
    settings.setValue(SETTINGS_LASTSHOWN, QDateTime::currentDateTimeUtc());

    QDialog::accept();
}
