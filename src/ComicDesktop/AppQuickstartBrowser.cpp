/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "AppQuickstartBrowser.h"
#include "ComicUtils.h"
#include <qresource.h>
#include <qtextstream.h>
#include <boost/algorithm/string.hpp>
#include <QNetworkReply>
#include <QNetworkRequest>

using namespace std;

AppQuickstartBrowser::AppQuickstartBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
    this->setOpenExternalLinks(true);

    QString homepageHeader = tr("Thank you for installing! I appreciate any feedback, whether problems or suggestions. "
                                "Please send e-mails to <a href=\"mailto:comicseer@xylasoft.com\">comicseer@xylasoft.com</a>.");

    QString quickStartHeader = tr("Quick Start Guide");

    QString quickStartNotes = tr("Quick notes and keyboard/mouse commands.");

    QString addOpenEcomic = tr("<h3 style=\"color:#004F00;\">Add your library and open an eComic</h3>"
                               "<ol>"
                               "<li>Click the '+' icon on the libraries view.</li>"
                               "<li>Select your library folder and then select a name for the library.</li>"
                               "<li>The library will be added and browsable in the library viewer.</li>"
                               "<li>Browse the contents of your library and find an eComic to read.</li>"
                               "<li>Double-click the eComic to open (any number of comics can be opened at one time).</li>"
                               "</ol>");

    QString openEcomic = tr("<h3 style=\"color:#004F00;\">Open an eComic directly</h3>"
                            "<ul>"
                            "<li>From Windows Explorer/Nautilus, you can open an eComic file to launch Comic Seer.</li>"
                            "<li>With Comic Seer open, the 'Open' button can be clicked to select an eComic file.</li>"
                            "</ul>");

    QString viewEcomic = tr("<h3 style=\"color:#004F00;\">Viewing an eComic once opened</h3>"
                            "<ul>"
                            "<li>Slide the zoom scroller on the left side of the comic to zoom in and out.</li>"
                            "<li>Click the 'Browse' button above the zoom scroll to view the pages of the comic.</li>"
                            "<li>Click and drag the mouse (or finger) around a page to move the image.</li>"
                            "<li>Change the page by clicking the page back/forward buttons in the menu, clicking a page in the browser, or clicking on the current page.</li>"
                            "<li>Magnify part of a page by clicking the magnify button or right-clicking on a page.</li>"
                            "<li>Rotate the image with the left/right arrows.</li>"
                            "</ul>");

    QString bookmarkEcomic = tr("<h3 style=\"color:#004F00;\">Bookmarking comics</h3>"
                                "<ul>"
                                "<li>Open the bookmark panel with the Bookmarks button on the menu.</li>"
                                "<li>When viewing a page that should be marked, click the plus button to mark it.</li>"
                                "<li>When a bookmark needs updating, click the change button to update it.</li>"
                                "<li>Click on a bookmark and click the open button (or right-click) to launch the bookmark.</li>"
                                "<li>Export and import your bookmarks to other computers with the in/out buttons.</li>"
                                "</ul>");

    QString commands = tr("<h2 style=\"color:#004F00;\">Key &amp; Mouse Commands</h2>"
                          "Viewing"
                          "<ul>"
                          "<li>Click on comic: Page forward (left side), Page back (right side)</li>"
                          "<li>Page forward, left bracket '[': Page forward</li>"
                          "<li>Page backward, right bracket ']': Page back</li>"
                          "<li>Ctrl+Home: First page</li>"
                          "<li>Ctrl+End: Last page</li>"
                          "<li>Arrow keys, Page up/dn: Pan page</li>"
                          "<li>+/-: Zoom in/out</li>"
                          "<li>Right click on comic: Magnify</li>"
                          "<li>F11: Enter/exit full screen</li>"
                          "<li>Esc: Exit full screen [Full screen only]</li>"
                          "<li>Ctrl+1: One page view</li>"
                          "<li>Ctrl+2: Two page horizontal view</li>"
                          "<li>Ctrl+4: Fit to width</li>"
                          "<li>Ctrl+5: Fit to height</li>"
                          "<li>Ctrl+6: Fit to width and height</li>"
                          "</ul>"
                          "Windows"
                          "<ul>"
                          "<li>Ctrl+O: Open a comic file directly</li>"
                          "<li>Ctrl+R: Open the recent list</li>"
                          "<li>Ctrl+F: Search your library</li>"
                          "<li>Ctrl+L: Show/hide your library</li>"
                          "<li>Ctrl+Tab: Switch between comic tabs</li>"
                          "<li>Ctrl+P: Show the options dialog</li>"
                          "<li>Ctrl+C: Close the selected comic</li>"
                          "<li>Ctrl+Q: Quit</li>"
                          "<li>Home: Home</li>"
                          "<li>F1: Home</li>"
                          "</ul>");

    this->_homepageText = QString("<html><body><h1><img src=\":/ComicDesktop/Resources/XylasoftComicsLogoSmall.png\" height=\"32\" width=\"32\" />"
                                  "&nbsp;&nbsp;&nbsp;<font color=\"DarkGreen\">Comic Seer %1</font></h1>%2<br />"
                                  "<table border=\"1\" cellpadding=\"15\"><tr><td valign=\"top\" width=\"50%\">"
                                  "    <h2 style=\"color:#004F00;\">%3</h2>"
                                  "    %4%5%6%7%8%9"
                                  "    </td><td valign=\"top\" width=\"50%\">"
                                  "    <h2 style=\"color:#004F00;\">News</h2>"
                                  "    {NEWS}"
                                  "    </td></tr></table>"
                                  "</body>"
                                  "</html>")
                               .arg(ComicUtils::VersionString)
                               .arg(homepageHeader)
                               .arg(quickStartHeader)
                               .arg(quickStartNotes)
                               .arg(addOpenEcomic)
                               .arg(openEcomic)
                               .arg(viewEcomic)
                               .arg(bookmarkEcomic)
                               .arg(commands);

    QString text = _homepageText;
    text.replace("{NEWS}", tr("Checking for news..."));
    this->setHtml(text);

    this->_elapsedInterval = 0;
    this->setOpenExternalLinks(true);

    this->connect(&this->_timer, SIGNAL(timeout()), SLOT(TimerTimeout()));
    this->connect(&_http, SIGNAL(finished(QNetworkReply*)), SLOT(WebRequestFinished(QNetworkReply*)));

    // check again every 12 hours while still running
    this->_timer.start(43200000);

    this->TimerTimeout();
}

void AppQuickstartBrowser::WebRequestFinished(QNetworkReply *reply)
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

        this->DisplayResult(true, stream.readAll());
    }
}

void AppQuickstartBrowser::TimerTimeout()
{
    try
    {
        QNetworkRequest request;
        request.setUrl(QUrl("http://www.xylasoft.com/appaccess/appnews.php"));
        _http.post(request, QString().toUtf8());
    }
    catch (...)
    {
        this->DisplayResult(false);
    }

    this->_elapsedInterval++;
}

void AppQuickstartBrowser::DisplayResult(bool success, QString text)
{
    // if the request was not successful
    if (!success)
    {
        QString hpText = _homepageText;
        hpText.replace("{NEWS}", tr("Failed to contact server for news. Check for internet connection."));
        this->setHtml(hpText);
    }
    // if the request was success, compare the versions
    else
    {
        QString hpText = _homepageText;
        hpText.replace("{NEWS}", text);
        this->setHtml(hpText);
    }
}
