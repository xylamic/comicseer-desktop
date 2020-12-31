/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "UpdateButton.h"
#include <qresource.h>
#include "ComicUtils.h"
#include <qbytearray.h>
#include <qtextstream.h>
#include <QSysInfo>
#include <qdesktopservices.h>
#include <qurl.h>
#include <QtWidgets/QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>

UpdateButton::UpdateButton(QWidget* parent /*= NULL*/)
    : QToolButton(parent)
{
    this->_elapsedInterval = 0;
    this->setText("");

    this->_success = false;
    this->_upgradeAvailable = false;

    this->connect(&this->_timer, SIGNAL(timeout()), SLOT(CheckTimerTimeout()));
    this->connect(&_http, SIGNAL(finished(QNetworkReply*)), SLOT(WebRequestFinished(QNetworkReply*)));
    this->connect(this, SIGNAL(clicked()), SLOT(Click()));
    this->connect(&this->_upgradeTimer, SIGNAL(timeout()), SLOT(UpgradeTimerTimeout()));

    // check again every 12 hours while still running
    this->_timer.start(43200000);

    this->CheckTimerTimeout();
}

void UpdateButton::WebRequestFinished(QNetworkReply *reply)
{
    QNetworkReply::NetworkError error = reply->error();
    if (error != 0)
	{
        qDebug(QString("Error in reading from CS website: error [%1] %2")
               .arg(error).arg(reply->errorString()).toLatin1());
		this->DisplayResult(false);
	}
	else
	{
		// read the results
        QByteArray data = reply->readAll();
		QTextStream stream(&data, QIODevice::ReadOnly);

		// get the strings
		QString numberstr = stream.readLine();
		QString versionstr = stream.readLine();

		if (numberstr.isEmpty() || versionstr.isEmpty())
		{
			this->DisplayResult(false);
		}
		else
		{
            bool ok;
            int version = numberstr.toInt(&ok);

            if (ok)
            {
                std::string versionStr(versionstr.toUtf8());

                this->DisplayResult(true, version, versionStr);
            }
            else
            {
                this->DisplayResult(false);
            }
		}
    }
}

void UpdateButton::CheckTimerTimeout()
{
    this->setIcon(QIcon(":/ComicDesktop/Resources/Checking.png"));
    this->setToolTip(tr("Checking for latest version... [Click to visit Comic Seer]"));

    try
    {
#ifdef _WIN32
        QString myOsIndicator = "1";
        QString myOs = QString("%1").arg(QSysInfo::WindowsVersion);
#else
        QString myOsIndicator = "2";
        QString myOs = "-1";
#endif
        QString myVersion = ComicUtils::VersionString;

        QNetworkRequest request;
        request.setUrl(QUrl(QString("http://www.xylasoft.com/appaccess/version.php?OSI=%1&OS=%2&Ver=%3&Elapsed=%4")
                            .arg(myOsIndicator)
                            .arg(myOs)
                            .arg(myVersion)
                            .arg(this->_elapsedInterval)));
        _http.post(request, QString().toUtf8());
    }
    catch (...)
    {
        this->DisplayResult(false);
    }

    this->_elapsedInterval++;
}

void UpdateButton::UpgradeTimerTimeout()
{
    if (this->icon().isNull())
    {
        this->setIcon(QIcon(":/ComicDesktop/Resources/Upgrade.png"));
    }
    else
    {
        this->setIcon(QIcon());
    }
}

void UpdateButton::Click()
{
    bool visitSite = false;
    if (this->_success == false)
    {
        QMessageBox msg(QMessageBox::Information,
                        tr("No information"),
                        tr("The software version could not be checked. Confirm your internet connection. "
                           "Would you like to try to visit the website?"),
                        QMessageBox::Yes | QMessageBox::No);
        if (msg.exec() == QMessageBox::Yes)
        {
            visitSite = true;
        }
    }
    else if (this->_success && this->_upgradeAvailable)
    {
        QMessageBox msg(QMessageBox::Information,
                        tr("Update available"),
                        QString(tr("There is a software update (%1) available. "
                           "Would you like to visit the website?")).arg(this->_upgradeVersion),
                        QMessageBox::Yes | QMessageBox::No);
        if (msg.exec() == QMessageBox::Yes)
        {
            visitSite = true;
        }
    }
    else
    {
        QMessageBox msg(QMessageBox::Information,
                        tr("Version current"),
                        tr("You have the latest software version."),
                        QMessageBox::Ok);
        msg.exec();
    }

    if (visitSite)
    {
        QDesktopServices::openUrl(QUrl("http://comicseer.xylasoft.com"));
    }
}

void UpdateButton::DisplayResult(bool success, int version, std::string versionString)
{
	// if the request was not successful
    if (!success)
	{
        this->setIcon(QIcon(":/ComicDesktop/Resources/Fail.png"));
        this->setToolTip(tr("Failed to check for latest version. Check your internet connection."));
        this->_upgradeTimer.stop();
        this->_success = false;
	}
	// if the request was success, compare the versions
	else
	{
        this->_success = true;

        if (ComicUtils::VersionNumber >= version)
		{
            this->setIcon(QIcon(":/ComicDesktop/Resources/Check.png"));
            this->setToolTip(tr("You have the latest version!"));
            this->_upgradeTimer.stop();
            this->_upgradeAvailable = false;
		}
		else
		{
            this->_upgradeTimer.start(1000);
            this->setIcon(QIcon());
            this->UpgradeTimerTimeout();
            this->_upgradeAvailable = true;
            this->_upgradeVersion = QString::fromUtf8(versionString.c_str());
            this->setToolTip(tr("Update available. Click for information."));
		}
    }
}
