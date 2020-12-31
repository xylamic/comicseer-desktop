/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "BookmarkViewContent.h"
#include "ui_BookmarkViewContent.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDateTime>
#include <Bookmarkings.hpp>
#include "ComicState.h"
#include <QMessageBox>
#include <QFileDialog>
#include <ComicUtils.h>
#include <ComicInfoViewer.h>
#include <ComicFileName.hpp>

#define DISABLE_GREEN QColor(0, 255, 0, 20)
#define TABLECOL_ID 0
#define TABLECOL_COMIC 1
#define TABLECOL_PAGE 2
#define TABLECOL_CREATED 3
#define TABLECOL_DESC 4
#define TABLECOL_NOTES 5

using namespace ComicSeer;
using namespace std;
using namespace boost::uuids;

BookmarkViewContent::BookmarkViewContent(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BookmarkViewContent)
{
    ui->setupUi(this);
    this->setFrameStyle(QFrame::StyledPanel);
    this->_dirty = false;

    // make the table auto-resize
    ui->tableWidgetMarks->setColumnWidth(TABLECOL_COMIC, 350);
    ui->tableWidgetMarks->setColumnWidth(TABLECOL_PAGE, 100);
    ui->tableWidgetMarks->setColumnWidth(TABLECOL_CREATED, 150);
    ui->tableWidgetMarks->setColumnWidth(TABLECOL_DESC, 250);
    ui->tableWidgetMarks->setColumnWidth(TABLECOL_NOTES, 350);
    ui->tableWidgetMarks->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidgetMarks->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // hide the ID column
    ui->tableWidgetMarks->setColumnHidden(0, true);
    ui->tableWidgetMarks->sortByColumn(TABLECOL_COMIC, Qt::AscendingOrder);

    // connect the signals
    this->connect(ui->toolButtonAdd, SIGNAL(clicked()), SLOT(AddBookmarkClicked()));
    this->connect(ui->toolButtonRemove, SIGNAL(clicked()), SLOT(RemoveSelectedBookmarks()));
    this->connect(ui->toolButtonExport, SIGNAL(clicked()), SLOT(ExportClicked()));
    this->connect(ui->toolButtonImport, SIGNAL(clicked()), SLOT(ImportClicked()));
    this->connect(ui->toolButtonRevert, SIGNAL(clicked()), SLOT(RevertClicked()));
    this->connect(ui->toolButtonOpen, SIGNAL(clicked()), SLOT(OpenClicked()));
    this->connect(ui->toolButtonSave, SIGNAL(clicked()), SLOT(SaveClicked()));
    this->connect(ui->toolButtonUpdate, SIGNAL(clicked()), SLOT(UpdateClicked()));
    this->connect(ui->tableWidgetMarks, SIGNAL(cellChanged(int,int)), SLOT(MarkDirty()));
    this->connect(ui->tableWidgetMarks, SIGNAL(customContextMenuRequested(QPoint)), SLOT(TableContextMenuRequested(QPoint)));
    this->connect(ui->tableWidgetMarks, SIGNAL(itemSelectionChanged()), SLOT(TableSelectionChanged()));
    this->connect(ui->tableWidgetMarks->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), SLOT(TableHeaderGeoChanged()));

    // create context menu
    this->_contextMenu = new QMenu(this);
    this->_contextMenu->addAction(ui->action_Open);
    this->_contextMenu->addAction(ui->action_ComicInformation);
    this->_contextMenu->addSeparator();
    this->_contextMenu->addAction(ui->action_RemoveSelected);

    this->LoadBookmarks();
}

BookmarkViewContent::~BookmarkViewContent()
{
    delete ui;
}

void BookmarkViewContent::LoadBookmarks()
{
    // clear any existing information
    while (ui->tableWidgetMarks->rowCount() > 0)
    {
        ui->tableWidgetMarks->removeRow(0);
    }

    // populate the table
    Bookmarkings bm = ComicState::State()->Bookmarks();
    const vector<const Bookmark*> marks = bm.Bookmarks();
    for (auto it = marks.begin(); it != marks.end(); it++)
    {
        this->AddRow(**it);
    }

    this->MarkDirty(false);
}

void BookmarkViewContent::AddRow(const Bookmark &mark)
{
    ui->tableWidgetMarks->setSortingEnabled(false);

    int row = ui->tableWidgetMarks->rowCount();

    ui->tableWidgetMarks->insertRow(row);

    ui->tableWidgetMarks->setItem(row, TABLECOL_ID, new QTableWidgetItem(XUtilities::BoostUuidToString(mark.Identifier()).c_str()));

    ui->tableWidgetMarks->setItem(row, TABLECOL_COMIC, new QTableWidgetItem(QString::fromUtf8(mark.Name().PrettyName())));
    ui->tableWidgetMarks->item(row, TABLECOL_COMIC)->setFlags(ui->tableWidgetMarks->item(row, TABLECOL_COMIC)->flags() ^ Qt::ItemIsEditable);
    ui->tableWidgetMarks->item(row, TABLECOL_COMIC)->setBackgroundColor(DISABLE_GREEN);

    ui->tableWidgetMarks->setItem(row, TABLECOL_PAGE, new QTableWidgetItem(QString("%1").arg(mark.Page())));
    ui->tableWidgetMarks->item(row, TABLECOL_PAGE)->setFlags(ui->tableWidgetMarks->item(row, TABLECOL_PAGE)->flags() ^ Qt::ItemIsEditable);
    ui->tableWidgetMarks->item(row, TABLECOL_PAGE)->setBackgroundColor(DISABLE_GREEN);
    ui->tableWidgetMarks->item(row, TABLECOL_PAGE)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidgetMarks->setItem(row, TABLECOL_CREATED, new QTableWidgetItem(QDateTime::fromTime_t(mark.Created()).toString("yyyy-MM-dd")));
    ui->tableWidgetMarks->item(row, TABLECOL_CREATED)->setFlags(ui->tableWidgetMarks->item(row, TABLECOL_CREATED)->flags() ^ Qt::ItemIsEditable);
    ui->tableWidgetMarks->item(row, TABLECOL_CREATED)->setBackgroundColor(DISABLE_GREEN);
    ui->tableWidgetMarks->item(row, TABLECOL_CREATED)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidgetMarks->setItem(row, TABLECOL_DESC, new QTableWidgetItem(QString::fromUtf8(mark.Description())));

    ui->tableWidgetMarks->setItem(row, TABLECOL_NOTES, new QTableWidgetItem(QString::fromUtf8(mark.Notes())));

    ui->tableWidgetMarks->setSortingEnabled(true);
}

void BookmarkViewContent::CheckForChanges()
{
    if (this->_dirty)
    {
        QMessageBox box(QMessageBox::Question,
                        tr("Changes pending"),
                        tr("There are unsaved bookmark changes. Save the changes?"),
                        QMessageBox::Yes | QMessageBox::No);
        if (box.exec() == QMessageBox::Yes)
        {
            this->SaveClicked();
        }
    }
}

void BookmarkViewContent::AddBookmarkClicked()
{
    QString path;
    int page;

    emit ActiveComicRequested(path, page);

    if (path.isEmpty() || page < 0)
    {
        QMessageBox msg(QMessageBox::Warning, tr("No comic"), tr("No comic is selected. "
                                                                 "Open a comic to the page you would like bookmarked."));
        msg.exec();
        return;
    }

    Bookmark mark(path.toUtf8(), page);
    ComicState::State()->Bookmarks().AddBookmark(mark);

    this->AddRow(mark);
    this->MarkDirty();

    this->ShowRow(mark.Identifier());
}

void BookmarkViewContent::RemoveSelectedBookmarks()
{
    // get the selected items (rows where the comic name is selected)
    map<uuid, uuid> selectedIds;
    for (int i = 0; i < ui->tableWidgetMarks->rowCount(); i++)
    {
        if (ui->tableWidgetMarks->item(i, TABLECOL_COMIC)->isSelected())
        {
            uuid id = this->GetIdForRow(i);
            selectedIds[id] = id;
        }
    }

    // verify a selection is made
    if (selectedIds.size() <= 0)
    {
        QMessageBox msg(QMessageBox::Information, tr("None selected"), tr("Select bookmark row(s) to remove."));
        msg.exec();
        return;
    }
    // ask the user to confirm
    else
    {
        QMessageBox msg(QMessageBox::Question, tr("Confirm removal"),
                        QString(tr("Are you sure you want to remove %1 bookmark(s)?")).arg(selectedIds.size()),
                        QMessageBox::Yes | QMessageBox::No);

        if (msg.exec() != QMessageBox::Yes)
        {
            return;
        }
    }

    // remove the bookmarks
    ui->tableWidgetMarks->setSortingEnabled(false);
    int i = 0;
    while (i < ui->tableWidgetMarks->rowCount())
    {
        uuid rowId = this->GetIdForRow(i);
        if (selectedIds.find(rowId) != selectedIds.end())
        {
            // erase it from future searched
            selectedIds.erase(rowId);
            // remove it from the table
            ui->tableWidgetMarks->removeRow(i);
            // remove it from the bookmarks
            ComicState::State()->Bookmarks().RemoveBookmark(rowId);
        }
        else
        {
            i++;
        }
    }
    this->MarkDirty();
    ui->tableWidgetMarks->setSortingEnabled(true);
}

void BookmarkViewContent::SaveClicked()
{
    try
    {
        for (int i = 0; i < ui->tableWidgetMarks->rowCount(); i++)
        {
            uuid id = this->GetIdForRow(i);
            QString desc = ui->tableWidgetMarks->item(i, TABLECOL_DESC)->text();
            QString notes = ui->tableWidgetMarks->item(i, TABLECOL_NOTES)->text();

            ComicState::State()->Bookmarks().UpdateBookmarkDescriptionNotes(id,
                                                                            desc.toUtf8(),
                                                                            notes.toUtf8());
        }

        ComicState::State()->StoreBookmarks();
        this->MarkDirty(false);
    }
    catch (...)
    {
        QMessageBox msg(QMessageBox::Critical, tr("Failed"), tr("Failed to save the bookmark file. Check permissions."));
        msg.exec();
    }
}

void BookmarkViewContent::OpenClicked()
{
    int row = this->GetFirstSelectedRow();
    if (row < 0)
    {
        return;
    }

    this->OpenComicBookmark(row);
}

void BookmarkViewContent::RevertClicked()
{
    QMessageBox msg(QMessageBox::Question,
                    tr("Confirm"),
                    tr("Are you sure you want to revert? You will lose any pending changes."),
                    QMessageBox::Yes | QMessageBox::No);
    if (msg.exec() != QMessageBox::Yes)
    {
        return;
    }

    ComicState::State()->ReloadBookmarks();
    this->LoadBookmarks();
}

void BookmarkViewContent::ImportClicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Append bookmarks"), QString(), tr("Bookmarks (*.xml)"));
    if (path.isEmpty())
    {
        return;
    }

    QMessageBox msg(QMessageBox::Question,
                    tr("Overwrite"),
                    tr("Do you want to overwrite any pre-existing bookmarks?"),
                    QMessageBox::Yes | QMessageBox::No);
    bool overwrite = msg.exec() == QMessageBox::Yes;

    Bookmarkings bm;
    try
    {
        bm.Load(path.toUtf8());
    }
    catch (...)
    {
        QMessageBox msg(QMessageBox::Critical, tr("Failed"), tr("Failed to read the bookmark file."));
        msg.exec();
        return;
    }

    const vector<const Bookmark*> newMarks = bm.Bookmarks();
    ComicState::State()->Bookmarks();
    int appended = 0, overwritten = 0, skipped = 0;
    for (int i = 0; i < newMarks.size(); i++)
    {
        const Bookmark *mark = newMarks[i];

        if (ComicState::State()->Bookmarks().BookmarkById(mark->Identifier()) == NULL)
        {
            ComicState::State()->Bookmarks().AddBookmark(*mark);
            appended++;
        }
        else if (overwrite)
        {
            ComicState::State()->Bookmarks().RemoveBookmark(mark->Identifier());
            ComicState::State()->Bookmarks().AddBookmark(*mark);
            overwritten++;
        }
        else
        {
            skipped++;
        }
    }

    // reload the view
    this->LoadBookmarks();

    // verify the table is marked dirty
    this->MarkDirty();

    QMessageBox msg2(QMessageBox::Information,
                    tr("Success"),
                    QString(tr("Successfully added %1, overwrote %2, and skipped %3 bookmarks."))
                    .arg(appended).arg(overwritten).arg(skipped));
    msg2.exec();
}

void BookmarkViewContent::ExportClicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save bookmarks"), QString(), tr("Bookmarks (*.xml)"));
    if (path.isEmpty())
    {
        return;
    }

    try
    {
        ComicState::State()->Bookmarks().Store(path.toUtf8());

        QMessageBox msg(QMessageBox::Information, tr("Success"), tr("Successfully wrote bookmark file."));
        msg.exec();
    }
    catch (...)
    {
        QMessageBox msg(QMessageBox::Critical, tr("Failure"), tr("Failed to save the bookmark file. Check permissions."));
        msg.exec();
    }
}

void BookmarkViewContent::UpdateClicked()
{
    // query for the active page
    QString path;
    int page;
    emit ActiveComicRequested(path, page);

    // validate we got the information
    if (path.isEmpty() || page < 0)
    {
        QMessageBox msg(QMessageBox::Warning, tr("No comic"), tr("No comic is selected. "
                                                                 "Open a comic to the page you would like bookmarked."));
        msg.exec();
        return;
    }

    // get the selected row
    int row = this->GetFirstSelectedRow();
    if (row < 0)
    {
        return;
    }

    // get the old bookmark information
    uuid id = this->GetIdForRow(row);
    const Bookmark *oldBm = ComicState::State()->Bookmarks().BookmarkById(id);
    string desc = oldBm->Description();
    string notes = oldBm->Notes();
    assert(oldBm != NULL);

    // remove the bookmark
    ui->tableWidgetMarks->removeRow(row);
    ComicState::State()->Bookmarks().RemoveBookmark(id);

    // create the new bookmark
    Bookmark mark(id, path.toUtf8(), page, ::time(NULL), desc.c_str(), notes.c_str());
    ComicState::State()->Bookmarks().AddBookmark(mark);
    this->AddRow(mark);
    this->ShowRow(mark.Identifier());

    // mark the table dirty
    this->MarkDirty();
}

void BookmarkViewContent::MarkDirty(bool dirty)
{
    if (dirty && !this->_dirty)
    {
        this->_dirty = true;
        ui->toolButtonSave->setEnabled(true);
        ui->toolButtonRevert->setEnabled(true);
    }
    else if (!dirty)
    {
        this->_dirty = false;
        ui->toolButtonSave->setEnabled(false);
        ui->toolButtonRevert->setEnabled(false);
    }
}

void BookmarkViewContent::TableContextMenuRequested(QPoint point)
{
    int row = ui->tableWidgetMarks->rowAt(point.y());
    if (row < 0)
    {
        return;
    }

    ui->tableWidgetMarks->item(row, TABLECOL_COMIC)->setSelected(true);
    QAction *action = this->_contextMenu->exec(ui->tableWidgetMarks->viewport()->mapToGlobal(point));
    if (action == NULL)
    {
        return;
    }

    if (action == ui->action_Open)
    {
        this->OpenComicBookmark(row);
    }
    else if (action == ui->action_ComicInformation)
    {
        this->OpenComicInformation(row);
    }
    else if (action == ui->action_RemoveSelected)
    {
        this->RemoveSelectedBookmarks();
    }
}

void BookmarkViewContent::TableSelectionChanged()
{
    int comicsSelected = 0;
    for (int i = 0; i < ui->tableWidgetMarks->rowCount(); i++)
    {
        if (ui->tableWidgetMarks->item(i, TABLECOL_COMIC)->isSelected())
        {
            comicsSelected++;
        }
    }

    if (comicsSelected != 1)
    {
        ui->toolButtonOpen->setEnabled(false);
        ui->toolButtonUpdate->setEnabled(false);
    }
    else
    {
        ui->toolButtonOpen->setEnabled(true);
        ui->toolButtonUpdate->setEnabled(true);
    }
}

void BookmarkViewContent::TableHeaderGeoChanged()
{
    ui->tableWidgetMarks->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void BookmarkViewContent::OpenComicBookmark(int row)
{
    QString path = this->GetBookmarkComic(row);
    int page = ::atoi(ui->tableWidgetMarks->item(row, TABLECOL_PAGE)->text().toUtf8());
    if (path.isEmpty())
    {
        return;
    }
    else
    {
        ComicUtils::OpenComic(path, page - 1);
    }
}

void BookmarkViewContent::OpenComicInformation(int row)
{
    QString path = this->GetBookmarkComic(row);
    if (path.isEmpty())
    {
        return;
    }
    else
    {
        ComicInfoViewer::OpenComicInfoViewer(path);
    }
}

QString BookmarkViewContent::GetBookmarkComic(int row)
{
    uuid id = this->GetIdForRow(row);

    const Bookmark *bm = ComicState::State()->Bookmarks().BookmarkById(id);
    assert(bm != NULL);

    QString bmPath = QString::fromUtf8(bm->Path());
    QFileInfo file(bmPath);

    // if the file exists, launch it
    if (file.exists())
    {
        return bmPath;
    }
    // try to find the file
    else
    {
        // try to automatically locate a match
        QString newPath = ComicUtils::FindBestComicMatch(bmPath);

        // if it could not be matched
        if (newPath.isEmpty())
        {
            QMessageBox msg(QMessageBox::Warning,
                            tr("File not found"),
                            tr("The comic does not exist and could not be matched in the libraries. "
                               "Would you like to select a new path for this comic?"),
                            QMessageBox::Yes | QMessageBox::No);
            if (msg.exec() != QMessageBox::Yes)
            {
                return QString();
            }

            newPath = QFileDialog::getOpenFileName(this,
                                                        tr("Bookmark comic"),
                                                        QString(),
                                                        tr("Comic Files (*.zip *.cbz *.rar *.cbr *.jpg *.png)"));
            if (newPath.isEmpty())
            {
                return QString();
            }
        }

        // update path
        ComicState::State()->Bookmarks().UpdateBookmarkPath(id, newPath.toUtf8());
        ui->tableWidgetMarks->item(row, TABLECOL_COMIC)->setText(ComicFileName(newPath.toUtf8()).PrettyName());
        return newPath;
    }
}

int BookmarkViewContent::GetFirstSelectedRow()
{
    for (int i = 0; i < ui->tableWidgetMarks->rowCount(); i++)
    {
        if (ui->tableWidgetMarks->item(i, TABLECOL_COMIC)->isSelected())
        {
            return i;
        }
    }

    return -1;
}

void BookmarkViewContent::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);

    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    ui->tableWidgetMarks->horizontalHeader()->restoreState(setting.value("Bookmarkings/TableState").toByteArray());

    ui->tableWidgetMarks->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void BookmarkViewContent::hideEvent(QHideEvent *e)
{
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    setting.setValue("Bookmarkings/TableState", ui->tableWidgetMarks->horizontalHeader()->saveState());
}

uuid BookmarkViewContent::GetIdForRow(int row)
{
    assert(row < ui->tableWidgetMarks->rowCount());

    QTableWidgetItem *idItem = ui->tableWidgetMarks->item(row, 0);
    QString idStr = idItem->text();
    uuid id = XUtilities::StringToBoostUuid(idStr.toUtf8());

    return id;
}

void BookmarkViewContent::ShowRow(uuid id)
{
    for (int i = 0; i < ui->tableWidgetMarks->rowCount(); i++)
    {
        uuid rowId = this->GetIdForRow(i);
        if (rowId == id)
        {
            ui->tableWidgetMarks->setCurrentCell(i, TABLECOL_COMIC);
            return;
        }
    }
}
