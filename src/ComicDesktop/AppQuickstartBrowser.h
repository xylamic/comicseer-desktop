/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef APPQUICKSTARTBROWSER_H
#define APPQUICKSTARTBROWSER_H

#include <Xylasoft.hpp>
#include <QtWidgets/qtextbrowser.h>
#include <QNetworkAccessManager>
#include <qtimer.h>

/*!
brief The AppQuickstartBrowser class provides a text browser that displays the
quick start information for a user.
 */
class AppQuickstartBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    /*!
    @brief Construct the browser and load the quickstart information.
    @param parent The parent widget.
    */
    AppQuickstartBrowser(QWidget* parent = NULL);

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
    void TimerTimeout();

protected:
    /*!
    Display the result.
    @param[in] success True if successfully tested version.
    @param[in] text The received text.
    */
    void DisplayResult(bool success, QString text = "");

private:
    QString _homepageText;
    QNetworkAccessManager _http;
    QTimer _timer;
    int _elapsedInterval;
};

#endif // APPQUICKSTARTBROWSER_H
