/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_TEXTBROWSERUPDATE_H
#define X_TEXTBROWSERUPDATE_H

#include <Xylasoft.hpp>
#include <QtWidgets/qtoolbutton.h>
#include <QNetworkAccessManager>
#include <qtimer.h>

/*!
This widget contains information related to the version of the applcation availability.
*/
class UpdateButton : public QToolButton
{
	Q_OBJECT

public:
	/*!
	Construct the browser.
	@param[in] parent The parent widget.
	*/
    UpdateButton(QWidget* parent = NULL);

protected slots:
	/*!
	The http requested completed.
	@param[in] id The ID of the request.
	@param[in] error True if error occurred.
	*/
    void WebRequestFinished(QNetworkReply *reply);

    /*!
    @brief Event received that the version check timeout has elapsed.
    */
    void CheckTimerTimeout();

    /*!
    @brief Event received that the upgrade timers need to flash.
    */
    void UpgradeTimerTimeout();

    /*!
    @brief The button was clicked. Go to the website.
    */
    void Click();

private:
	/*!
	Display the result.
	@param[in] success True if successfully tested version.
	@parma[in] version The version integer.
	@param[in] versionString The user friendly version string.
	*/
	void DisplayResult(bool success, int version = -1, std::string versionString = "");

private:
    QNetworkAccessManager _http;
    QTimer _timer;
    int _elapsedInterval;
    QTimer _upgradeTimer;

    bool _success;
    bool _upgradeAvailable;
    QString _upgradeVersion;
};

#endif
