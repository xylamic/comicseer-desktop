/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicDesktop.h"
#include <qapplication.h>
#include "ComicUtils.h"
#include <ComicFile.hpp>
#include <qfileinfo.h>
#include "ComicUtilities.hpp"
#include <qtextstream.h>
#include <TestDialog.h>
#include <locale>
#include <qsplashscreen.h>
#include <qtranslator.h>
#include <qlibraryinfo.h>
#include <qdebug.h>
#include "qtsingleapplication.h"
#include "ComicAppNotification.h"
using namespace ComicSeer;
using namespace std;

void loadStyleSheet() {
    QFile data(":/ComicDesktop/style.qss");

    QString style;
    if(data.open(QFile::ReadOnly)) {
        /* QTextStream... */
        QTextStream styleIn(&data);
        /* ...read file to a string. */
        style = styleIn.readAll();
        data.close();
        /* We'll use qApp macro to get the QApplication pointer
        and set the style sheet application wide. */
        qApp->setStyleSheet(style);
    }
}

void validateOnlyRunningInstance(QtSingleApplication &a, int argc, char *argv[])
{
    // check if the application is already running and act appropriately
    if (a.isRunning())
    {
        // for each argument, put into a list for the application that is already running
        QString argList;
        for (int i = 1; i < argc; i++)
        {
            string path = argv[i];
            QString qpath = QString::fromUtf8(path.c_str());
            argList.append(qpath);
            argList.append("**");
        }

        if (!argList.isEmpty())
        {
            a.sendMessage(argList);
        }

        exit(0);
    }
}

void LoadTranslator(QTranslator& translator, QApplication& app, const QString& localeName)
{
    QString filename = "comicseer_" + localeName;
    QString path = ComicUtils::FileConfigurationPath();

    ComicUtils::CustomLanguageFileName = string(QString(path + "comicseer_" + localeName).replace("\\", "/").toUtf8());

    if (translator.load(QString::fromUtf8(ComicUtils::CustomLanguageFileName.c_str())))
    {
        app.installTranslator(&translator);
        qDebug() << "Loaded and installed Comic Seer translation for" << localeName << "from" << path;
        ComicUtils::CustomLanguageLoaded = true;
    }
    else if (translator.load(filename))
    {
        app.installTranslator(&translator);
        ComicUtils::CustomLanguageFileName = string(filename.toUtf8());
        qDebug() << "Loaded and installed Comic Seer translation for" << localeName << "from local directory";
        ComicUtils::CustomLanguageLoaded = true;
    }
    else
    {
        qDebug() << "Comic Seer translation" << QString::fromUtf8(ComicUtils::CustomLanguageFileName.c_str()) <<
                    "not found at" << path << ", using default of en-us";
        ComicUtils::CustomLanguageLoaded = false;
    }
}

int main(int argc, char *argv[])
{

    // load the application
    QtSingleApplication a(argc, argv);
    ApplicationInstance = &a;

    // load the translaters
    QString localeName = QLocale::system().name();
    qDebug() << "Locale name:" << localeName;
    QTranslator translator;
    if (translator.load("qt_" + localeName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        a.installTranslator(&translator);
        qDebug() << "Loaded and installed Qt translation for" << localeName << ".";
    }
    else
    {
        qDebug() << "Qt translation not found, using default.";
    }

    LoadTranslator(translator, a, localeName);

    // validate only one instance is running
    validateOnlyRunningInstance(a, argc, argv);

    // set the application icon
    a.setWindowIcon(QIcon(":/ComicDesktop/Resources/XylasoftComicsLogo.png"));

    /* TestDialog dlg; dlg.exec(); */

    loadStyleSheet();

    // load the splash screen
    QPixmap pixmap(":/ComicDesktop/Resources/ComicSeerSplash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    // load comic state information from the file
    ComicUtils::LoadState();

    // show the main window
    ComicDesktop w;
    w.show();

    // for each argument
    for (int i = 1; i < argc; i++)
    {
        // get the file info
        string path = argv[i];
        QFileInfo file(QString(path.c_str()));

        // if the file exists and is a file
        if (file.exists() && file.isFile())
        {
            ComicUtils::OpenComic(file.absoluteFilePath());
        }
    }

    ComicAppNotification::TryShowDialog(NULL);

    // wait for the window to show
    splash.finish(&w);

    // set the single application focus
    a.setActivationWindow(&w);

    // enter the event loop
    return a.exec();
}
