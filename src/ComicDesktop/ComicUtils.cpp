/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicUtils.h"
#include <qstring.h>
#include <qdesktopservices.h>
#include <qfileinfo.h>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <qsettings.h>
#include <QtWidgets/QMessageBox>
#include <QDebug>
#include <QColor>
#include <cmath>
#include <qbuffer.h>
#include <qimagereader.h>
#include <ComicFileName.hpp>
#include <QDir>
#include <QtWidgets/QFileDialog>
#include <QProcess>
#include "ComicDesktop.h"
#include "ComicLaunchEvent.h"

using namespace ComicSeer;
using namespace std;

const char* ComicUtils::CompanyName = "Xylasoft";
const char* ComicUtils::ApplicationName = "ComicSeer";

QtSingleApplication *ApplicationInstance = nullptr;

// 1 - v0.8b
// 2 - v0.9b
// 3 - v0.91b
// 4 - v0.95
// 5 - v1.00
// 6 - v1.05
// 7 - v1.10
// 8 - v1.15
// 9 - v1.20
// 10 - v1.21
// 11 - v1.25
// 12 - v1.26
// 13 - v1.30
// 14 - v1.31
// 15 - v1.32
// 16 - v1.40
// 17 - v1.45
// 18 - v2.00
// 19 - v2.10
// 20 - v2.20
// 21 - v2.21
// 22 - v2.30
// 23 - v2.50
// 24 - v2.51
// 25 - v2.51-4

const int ComicUtils::VersionNumber = 25;
const char* ComicUtils::VersionString = "v2.51";
const char *ComicUtils::RevisionString = "4";
bool ComicUtils::CustomLanguageLoaded = false;
std::string ComicUtils::CustomLanguageFileName = "<unset>";

QSettings ComicUtils::_rotationSettings(FileConfigurationPath().append("PageRotation.ini"), QSettings::IniFormat);
QSettings ComicUtils::_viewSettings(FileConfigurationPath().append("PageView.ini"), QSettings::IniFormat);

QString ComicUtils::FileConfigurationPath()
{
    QApplication::setApplicationName(ApplicationName);
    QApplication::setOrganizationName(CompanyName);

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
            QString("/%1/%2").arg(CompanyName).arg(ApplicationName);

    if (!path.endsWith("/") && !path.endsWith("\\"))
	{
#ifdef WIN32
        path.append("\\");
#else
        path.append("/");
#endif
    }

	QDir dir;
	if (!dir.exists(path))
	{
		dir.mkpath(path);
	}

    return path;
}

QString ComicUtils::GetVisibleComicPath(const QString &path)
{
    bool showPath = ComicState::State()->ShowFullFilePath();
    if (showPath)
    {
        return path;
    }
    else
    {
        ComicFileName name(path.toUtf8());
        return QString::fromUtf8(name.PrettyName());
    }
}

QString ComicUtils::GetVisibleFileType(ComicFile::ComicFileType type)
{
    switch (type)
    {
        case ComicFile::Rar:
            return "RAR";
        case ComicFile::Zip:
            return "ZIP";
        case ComicFile::Image:
            return "RAW";
        default:
            return "Unknown";
    }

    return "";
}

QString ComicUtils::GetVersionString()
{
    return QString(VersionString).append("-").append(RevisionString);
}

void ComicUtils::OpenComic(const QString &path, int page)
{
    ComicLaunchEvent cle(path.toUtf8(), page);
    QApplication::sendEvent(ComicDesktop::Instance(), &cle);
}

QString ComicUtils::FindBestComicMatch(const QString &path)
{
    // if the file already exists, return it
    QFileInfo matchFile(path);
    if (matchFile.exists())
    {
        return path;
    }

    // get the file name
    QString filename = matchFile.fileName();

    // iterate over all libraries and look for a matching file name
    map<string, string> libraryDirectories = ComicState::State()->LibraryDirectories();
    for (auto itlib = libraryDirectories.begin(); itlib != libraryDirectories.end(); itlib++)
    {
        const ComicLibrary *library = ComicState::State()->Library(itlib->first.c_str());
        const vector<boost::shared_ptr<ComicLibraryItem> > &comics = library->GetCompleteComicList();

        for (auto ititem = comics.begin(); ititem != comics.end(); ititem++)
        {
            ComicLibraryItem *comic = ititem->get();
            QFileInfo file(QString::fromUtf8(comic->Path()));

            // if the file names match
            if (file.fileName().compare(filename, Qt::CaseInsensitive) == 0 && file.exists())
            {
                return QString::fromUtf8(comic->Path());
            }
        }
    }

    return QString();
}

void ComicUtils::LaunchComicTagger(const QString &file)
{
#ifdef WIN32
    QString caption = QObject::tr("Comic Tagger Path");

    // set the file filter and default directory for windows and linux
    QString filter = "ComicTagger (comictagger.exe)";
    QString defaultDir = ::getenv("PROGRAMFILES");
    if (QDir(defaultDir + "\\ComicTagger").exists())
    {
        defaultDir = defaultDir + "\\ComicTagger";
    }

    // get any existing path
    string path = ComicState::State()->ComicTaggerPath();

    // if the path is undefined (never selected)
    if (path.empty())
    {
        // explain to the user what needs to be done
        QMessageBox msg(QMessageBox::Information,
                        QObject::tr("ComicTagger"),
                        QObject::tr("ComicTagger must be installed separately. It can be downloaded from "
                        "<a href=\"%1\">%1</a>.If already installed, click OK to "
                        "proceed with selecting the install location.\n\n")
                        .arg(ComicTaggerWebsite()),
                        QMessageBox::Ok | QMessageBox::Cancel);

        // if the user chooses not to continue, return
        if (msg.exec() != QMessageBox::Ok)
        {
            return;
        }

        // select the location for comic tagger
        QString exePath = QFileDialog::getOpenFileName(NULL,
                                                       caption,
                                                       defaultDir,
                                                       filter);

        // if the user did not select a path, return
        if (exePath.isEmpty())
        {
            return;
        }

        // update the configuration for the selected path
        path = exePath.toUtf8();
        ComicState::State()->SetComicTaggerPath(path);
        StoreState();
    }
    // if the path had been defined, but the file no longer exists
    else if (!QFileInfo(path.c_str()).exists())
    {
        // ask the user whether they want to select another path
        QMessageBox msg(QMessageBox::Warning,
                        QObject::tr("Not found"),
                        QObject::tr("ComicTagger was not found in the previously selected location, select another?\n\n"),
                        QMessageBox::Yes | QMessageBox::No);

        // if they cancel, return
        if (msg.exec() != QMessageBox::Yes)
        {
            return;
        }

        // prompt for a new path
        QString exePath = QFileDialog::getOpenFileName(
                    NULL,
                    caption,
                    defaultDir,
                    filter);

        // if no path was selected, return
        if (exePath.isEmpty())
        {
            return;
        }

        // store the updated path
        path = exePath.toUtf8();
        ComicState::State()->SetComicTaggerPath(path);
        StoreState();
    }

    // check if the comic that is selected is open within comic seer
    if (ComicDesktop::Instance()->IsComicOpen(file))
    {
        // ask the user to close the open comic
        QMessageBox msg(
                    QMessageBox::Question,
                    QObject::tr("Comic open"),
                    QObject::tr("The selected comic is currently open. It may not be able to be edited while open, would "
                                "you like to close it?\n\n"),
                    QMessageBox::Yes | QMessageBox::No);

        // if they agree, close the comic
        if (msg.exec() == QMessageBox::Yes)
        {
            ComicDesktop::Instance()->CloseComic(file);
        }
    }

    // create the arguments
    QStringList arguments(file);

    // start comic tagger and check for failure
    if (!QProcess::startDetached(QString::fromUtf8(path.c_str()), arguments))
    {
        // launch failed, ask the user if they want to clear their comic tagger path
        QMessageBox msg(QMessageBox::Critical,
                        QObject::tr("ComicTagger"),
                        QObject::tr("ComicTagger could not be launched. Do you want to remove "
                                    "your selected path for ComicTagger?"),
                        QMessageBox::Yes | QMessageBox::No);

        // if agree, remove the path
        if (msg.exec() == QMessageBox::Yes)
        {
            ComicState::State()->SetComicTaggerPath("");
            StoreState();
        }
    }
#else
    // create the arguments
    QStringList arguments(file);

    // start comic tagger and check for failure
    if (!QProcess::startDetached("comictagger.py", arguments))
    {
        // launch failed, notify the user how to correct
        QMessageBox msg(QMessageBox::Critical,
                        QObject::tr("ComicTagger"),
                        QObject::tr("ComicTagger could not be launched. Visit "
                                    "<a href=\"%1\">%1</a> to "
                                    "get instructions on installation. If installed, "
                                    "verify that comictagger.py is in your path.\n\n")
                        .arg(ComicTaggerWebsite()),
                        QMessageBox::Ok);

        msg.exec();
    }
#endif
}

QString ComicUtils::StringToQString(const string &s)
{
    return QString::fromUtf8(s.c_str());
}

string ComicUtils::ConvertQStringToStdString(const QString &q)
{
    return string(q.toUtf8());
}

QString ComicUtils::ComicTaggerWebsite()
{
    return "https://code.google.com/p/comictagger/";
}

void ComicUtils::LoadState()
{
    ComicState* state = ComicState::State();

	// try to load the file, if it fails, continue
	try
	{
		QString path = FileConfigurationPath();
		QFileInfo configFile = path + "ComicSeer.ini";

        // if the configuration file does not exist, try to read
        if (configFile.exists())
        {
            XIni ini(configFile.absoluteFilePath().toUtf8());

            state->LoadState(ini);
        }
	}
    catch (...)
	{
        QMessageBox msg(QMessageBox::Critical, QObject::tr("Load"), QObject::tr("Failed to load personal settings. Using defaults."), QMessageBox::Ok);
        msg.exec();

        state->Reinitialize();
	}

    // try to resave for any missing fields
    StoreState();
}

void ComicUtils::StoreState()
{
    try
    {
        QString path = FileConfigurationPath();
        QFileInfo configFile = path + "ComicSeer.ini";

        boost::scoped_ptr<XIni> ini;

        // if the file exists, try to load it
        if (configFile.exists())
        {
            try
            {
                ini.reset(new XIni(configFile.absoluteFilePath().toUtf8()));
            }
            catch (...) { }
        }

        if (ini.get() == NULL)
        {
            ini.reset(new XIni());
        }

        ComicState* state = ComicState::State();
        state->StoreState(*ini);

        ini->Save(configFile.absoluteFilePath().toUtf8());
    }
    catch (...)
    {
        QMessageBox msg(QMessageBox::Critical, QObject::tr("Save"), QObject::tr("Failed to save personal settings. Continuing without saving."), QMessageBox::Ok);
        msg.exec();
    }
}

std::string ComicUtils::GetForegroundContrastColor(const char* color)
{
    QColor qcolor(color);

    return (qcolor.red() > 128 || qcolor.green() > 128 || qcolor.blue() >= 128) ? "#000000" : "#FFFFFF";
}

QPixmap ComicUtils::ConvertRawImageToPixmap(XByteArray& data, int scaledWidth, int scaledHeight)
{
    QByteArray qdata(data.ConstData(), data.Size());

    QBuffer buffer(&qdata);
    QImageReader reader(&buffer);
    reader.setAutoDetectImageFormat(true);

    QPixmap map = QPixmap::fromImageReader(&reader);

    if (scaledWidth > 0 && scaledHeight > 0)
    {
        map = map.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return map;
}

bool ComicUtils::IsPixmapDoublePage(const QPixmap& map)
{
    if (map.isNull())
    {
        return false;
    }
    else if (map.height() > map.width())
    {
        return false;
    }
    else
    {
        return true;
    }
}

ComicPageTypes::PageRotationType ComicUtils::GetComicPageRotation(QString file, int page)
{
    _rotationSettings.beginGroup(file.toLower());
    ComicPageTypes::PageRotationType rotation = static_cast<ComicPageTypes::PageRotationType>(
                _rotationSettings.value(QString("%1").arg(page), static_cast<int>(ComicPageTypes::NONE)).toInt());
    _rotationSettings.endGroup();
    return rotation;
}

void ComicUtils::SetComicPageRotation(QString &file, int page, ComicPageTypes::PageRotationType rotation)
{
    QString key = QString("%1").arg(page);

    _rotationSettings.beginGroup(file.toLower());
    if (rotation == ComicPageTypes::NONE)
    {
        _rotationSettings.remove(key);
    }
    else
    {
        _rotationSettings.setValue(key, static_cast<int>(rotation));
    }
    _rotationSettings.endGroup();
}

void ComicUtils::ClearComicPageRotations(const QString &file)
{
    _rotationSettings.remove(file);
}

CommandEnums::KeyCommandType ComicUtils::CheckKeyCombination(QKeyEvent* event)
{
    qDebug() << "Analyzing key press with count " << event->count() << " and text " << event->text() << "...";

    if (event->count() == 1 && !(event->modifiers() & Qt::ControlModifier))
    {
        qDebug() << "No modifier with key " << event->key();
        switch (event->key())
        {
        case Qt::Key_F11:
            return CommandEnums::FULLSCREEN;
        case Qt::Key_BracketRight:
        case Qt::Key_Forward:
            return CommandEnums::PAGEFORWARD;
        case Qt::Key_BracketLeft:
        case Qt::Key_Back:
            return CommandEnums::PAGEBACKWARD;
        case Qt::Key_Home:
        case Qt::Key_F1:
            return CommandEnums::HOME;
        case Qt::Key_Minus:
            return CommandEnums::ZOOMOUT;
        case Qt::Key_Equal:
        case Qt::Key_Plus:
            return CommandEnums::ZOOMIN;
        }
    }
    else if (event->count() == 1 && (event->modifiers() & Qt::ControlModifier))
    {
        qDebug() << "CTRL modifier with key " << event->key();
        switch (event->key())
        {
        case Qt::Key_L:
            return CommandEnums::LIBRARY;
        case Qt::Key_F:
            return CommandEnums::FINDINLIBRARY;
        case Qt::Key_O:
            return CommandEnums::OPEN;
        case Qt::Key_Tab:
            return CommandEnums::TABCHANGE;
        case Qt::Key_Q:
            return CommandEnums::QUIT;
        case Qt::Key_H:
            return CommandEnums::HOME;
        case Qt::Key_R:
            return CommandEnums::RECENTLIST;
        case Qt::Key_P:
            return CommandEnums::OPTIONS;
        case Qt::Key_C:
            return CommandEnums::CLOSECOMIC;
        case Qt::Key_Home:
            return CommandEnums::FIRSTPAGE;
        case Qt::Key_End:
            return CommandEnums::LASTPAGE;
        case Qt::Key_1:
            return CommandEnums::ONEPAGE;
        case Qt::Key_2:
            return CommandEnums::TWOPAGE;
        case Qt::Key_6:
            return CommandEnums::FITALL;
        case Qt::Key_5:
            return CommandEnums::FITHEIGHT;
        case Qt::Key_4:
            return CommandEnums::FITWIDTH;
        }
    }

    return CommandEnums::UNIDENTIFIED;
}
