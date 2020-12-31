/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicInfoViewer.h"
#include "ui_ComicInfoViewer.h"
#include <ComicFile.hpp>
#include <ComicInfo.hpp>
#include "ComicUtils.h"
#include <QPixmap>
#include <QProcess>
#include <QFileInfo>
#include "ComicDesktop.h"

using namespace ComicSeer;
using namespace std;

QMap<QString, ComicInfoViewer*> ComicInfoViewer::_windows;

ComicInfoViewer::ComicInfoViewer(const QString &file, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComicInfoViewer)
{
    ui->setupUi(this);
    _file = file;

    this->setWindowTitle(QString(tr("Comic Information - %1")).arg(ComicUtils::GetVisibleComicPath(file)));

    ui->labelStatus->setText(tr("Loading file..."));
}

void ComicInfoViewer::SetFailureStatus(const string &message)
{
    ui->labelStatus->setText(QString(tr("Failed to load - %1")).arg(QString::fromUtf8(message.c_str())));
}

ComicInfoViewer::~ComicInfoViewer()
{
    delete ui;
}

void ComicInfoViewer::OpenComicInfoViewer(const QString &file)
{
    ComicInfoViewer *viewer = NULL;
    if (_windows.contains(file))
    {
        viewer = _windows[file];
        if (!viewer->isVisible())
        {
            delete viewer;
            _windows.remove(file);
        }
        else
        {
            viewer->activateWindow();
            return;
        }
    }

    if (!_windows.contains(file))
    {
        ComicInfoViewer *viewer = new ComicInfoViewer(file, ComicDesktop::Instance());
        _windows.insert(file, viewer);
        viewer->show();
        viewer->Load();
    }
}

void ComicInfoViewer::Load()
{
    ui->labelStatus->setText(tr("Loading file..."));
    QApplication::processEvents();

    ComicFile *cf = NULL;

    try
    {
        // load the comic file
        cf = ComicFile::CreateComicFile(this->_file.toUtf8());
        cf->Initialize();

        // show the file status
        QFileInfo fileInfo(this->_file);
        ui->labelFileSize->setText(QString(tr("%1 MB, %2 pages, %3 format"))
                                   .arg(fileInfo.size() / 1024.0 / 1024.0, 0, 'f', 2)
                                   .arg(cf->NumberOfPages())
                                   .arg(ComicUtils::GetVisibleFileType(cf->GetFileType())));

        // get the cover image
        XByteArray titleData;
        cf->GetTitleImage(titleData);

        // show the cover image
        ui->labelPreview->SetImage(ComicUtils::ConvertRawImageToPixmap(titleData));

        // check if this is an image file
        if (cf->GetFileType() == ComicFile::Image)
        {
            ui->labelStatus->setText(tr("Individual images cannot have comic information."));
        }
        // check for a comic information file
        else if (!cf->HasComicInformationFile())
        {
            ui->labelStatus->setText(tr("File contains no comic information"));
        }
        else
        {
            // create the comic information file
            ComicInfo ci = cf->GetComicInformation();

            // populate the UI
            ui->lineEditTitle->setText(QString::fromUtf8(ci.Title().c_str()));
            ui->lineEditSeries->setText(QString::fromUtf8(ci.Series().c_str()));
            ui->lineEditNumber->setText(QString::fromUtf8(ci.Number().c_str()));
            ui->lineEditVolume->setText(QString("%1").arg(ci.Volume()));
            ui->textEditSummary->setText(QString::fromUtf8(ci.Summary().c_str()));
            ui->lineEditYear->setText(QString("%1").arg(ci.Year()));
            ui->lineEditMonth->setText(QString("%1").arg(ci.Month()));
            ui->lineEditWriter->setText(QString::fromUtf8(ci.Writer().c_str()));
            ui->lineEditPenciller->setText(QString::fromUtf8(ci.Penciller().c_str()));
            ui->lineEditInker->setText(QString::fromUtf8(ci.Inker().c_str()));
            ui->lineEditColorer->setText(QString::fromUtf8(ci.Colorist().c_str()));
            ui->lineEditLetterer->setText(QString::fromUtf8(ci.Letterer().c_str()));
            ui->lineEditCoverArtist->setText(QString::fromUtf8(ci.CoverArtist().c_str()));
            ui->lineEditEditor->setText(QString::fromUtf8(ci.Editor().c_str()));
            ui->lineEditPublisher->setText(QString::fromUtf8(ci.Publisher().c_str()));
            ui->lineEditImprint->setText(QString::fromUtf8(ci.Imprint().c_str()));
            ui->lineEditGenre->setText(QString::fromUtf8(ci.Genre().c_str()));
            ui->lineEditPages->setText(QString("%1").arg(ci.PageCount()));
            ui->lineEditLanguage->setText(QString::fromUtf8(ci.LanguageISO().c_str()));
            ui->lineEditFormat->setText(QString::fromUtf8(ci.Format().c_str()));
            ui->checkBoxBlackAndWhite->setChecked(ci.IsBlackAndWhite() == ComicInfo::YES);
            ui->checkBoxManga->setChecked(ci.IsManga() == ComicInfo::YES);

            if (!ci.Web().empty())
            {
                ui->labelLink->setText(QString("<a href=\"%1\">Link</a>").arg(ComicUtils::StringToQString(ci.Web())));
                ui->labelLink->setToolTip(ComicUtils::StringToQString(ci.Web()));
            }
            else
            {
                ui->labelLink->setText(tr("&lt;no link&gt;"));
            }

            ui->labelStatus->setText(tr("Loaded"));
        }
    }
    catch (std::exception &ex)
    {
        this->SetFailureStatus(ex.what());
        return;
    }
    catch (...)
    {
        this->SetFailureStatus(string((tr("Unknown read error")).toUtf8()));
        return;
    }

    ComicFile::DisposeComicFile(cf);
}

void ComicInfoViewer::on_buttonComicTagger_clicked()
{
    ComicUtils::LaunchComicTagger(this->_file);
}

void ComicInfoViewer::on_toolButtonRefresh_clicked()
{
    this->Load();
}

void ComicInfoViewer::on_toolButtonClose_clicked()
{
    this->close();
}
