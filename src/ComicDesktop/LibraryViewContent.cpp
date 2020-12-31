/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "LibraryViewContent.h"
#include <ComicState.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qaction.h>
#include <qicon.h>
#include <ComicFile.hpp>
#include <qpixmap.h>
#include <qimagereader.h>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <XByteArray.hpp>
#include <qbuffer.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <ComicExceptions.hpp>
#include "ComicDesktop.h"
#include "ComicUtils.h"
#include <iostream>
#include <qdebug.h>
#include "ComicDesktop.h"
#include <ComicUtilities.hpp>
#include <qclipboard.h>
#include <ComicFileName.hpp>
#include <qsplashscreen.h>
#include "ComicInfoViewer.h"

using namespace std;
using namespace ComicSeer;

const int LibraryViewContent::_itemTypeLibrary = 0;
const int LibraryViewContent::_itemTypeFolder = 1;
const int LibraryViewContent::_itemTypeFile = 2;

const int LibraryViewContent::_itemColumnDataType = 0;
const int LibraryViewContent::_itemColumnDataPath = 1;
const int LibraryViewContent::_itemColumnDataComicType = 2;

#define LIBRARY_VIEWTYPE_SORTED 0
#define LIBRARY_VIEWTYPE_FILES 1
#define LIBRARY_VIEWTYPE_RECENT 2

LibraryViewContent::LibraryViewContent(QWidget *parent) :
    QFrame(parent)
{
	ui.setupUi(this);
    _searching = false;

    this->setFrameStyle(QFrame::StyledPanel);

    this->_textNoPreview = tr("No Preview");
    ui.labelPreview->setText(this->_textNoPreview);
    ui.toolButtonViewMetaData->setEnabled(false);

    ui.lblProcessing->setVisible(false);

	this->LoadLibraries();

    this->CreateMenus();

    this->connect(ui.action_RefreshLibraries, SIGNAL(triggered()), SLOT(Refresh()));
    this->connect(ui.action_AddLibrary, SIGNAL(triggered()), SLOT(AddLibrary()));
    this->connect(ui.action_RemoveLibrary, SIGNAL(triggered()), SLOT(RemoveLibrary()));
    this->connect(ui.action_ExportLibraryList, SIGNAL(triggered()), SLOT(Export()));
    this->connect(ui.action_OpenFile, SIGNAL(triggered()), SLOT(Open()));
    this->connect(ui.action_CopyComicName, SIGNAL(triggered()), SLOT(CopyNameToClipboard()));
    this->connect(ui.action_ComicInformation, SIGNAL(triggered()), SLOT(ViewComicInformation()));
	this->connect(ui.treeWidgetLibraries, SIGNAL(itemSelectionChanged()), SLOT(ComicSelectionChanged()));
    this->connect(ui.treeWidgetLibraries, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(ComicActivated(QTreeWidgetItem*,int)));
    this->connect(ui.treeWidgetLibraries, SIGNAL(customContextMenuRequested(QPoint)), SLOT(TreeContextMenuRequested(QPoint)));
    ui.treeWidgetLibraries->setContextMenuPolicy(Qt::CustomContextMenu);
    this->connect(ui.treeWidgetList, SIGNAL(itemSelectionChanged()), SLOT(ComicSelectionChanged()));
    this->connect(ui.treeWidgetList, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(ComicActivated(QTreeWidgetItem*,int)));
    this->connect(ui.treeWidgetList, SIGNAL(customContextMenuRequested(QPoint)), SLOT(TreeContextMenuRequested(QPoint)));
    ui.treeWidgetList->setContextMenuPolicy(Qt::CustomContextMenu);
    this->connect(ui.treeWidgetRecent, SIGNAL(itemSelectionChanged()), SLOT(ComicSelectionChanged()));
    this->connect(ui.treeWidgetRecent, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(ComicActivated(QTreeWidgetItem*,int)));
    this->connect(ui.treeWidgetRecent, SIGNAL(customContextMenuRequested(QPoint)), SLOT(TreeContextMenuRequested(QPoint)));
    ui.treeWidgetRecent->setContextMenuPolicy(Qt::CustomContextMenu);
    this->connect(ui.treeWidgetSearch, SIGNAL(itemSelectionChanged()), SLOT(ComicSelectionChanged()));
    this->connect(ui.treeWidgetSearch, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(ComicActivated(QTreeWidgetItem*,int)));
    this->connect(ui.treeWidgetSearch, SIGNAL(customContextMenuRequested(QPoint)), SLOT(TreeContextMenuRequested(QPoint)));
    ui.treeWidgetSearch->setContextMenuPolicy(Qt::CustomContextMenu);
	this->connect(ui.lineEditSearch, SIGNAL(textChanged(const QString&)), SLOT(SearchTextChanged(const QString&)));
	this->connect(ui.toolButtonClearSearch, SIGNAL(clicked()), SLOT(ClearSearchTriggered()));
    this->connect(&this->_searchTimer, SIGNAL(timeout()), SLOT(SearchReady()));
    this->connect(ui.toolButtonViewMetaData, SIGNAL(clicked()), SLOT(ViewComicInformation()));

    // set default visibility
    ui.treeWidgetLibraries->setVisible(false);
    ui.treeWidgetRecent->setVisible(false);
    ui.treeWidgetSearch->setVisible(false);

    // restore previous sizing
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    ui.splitter->restoreState(setting.value("Library/SplitFactor").toByteArray());
    ui.cboViewType->setCurrentIndex(setting.value("Library/ViewType", QVariant(0)).toInt());
}

void LibraryViewContent::LoadLibraries()
{
	ui.treeWidgetLibraries->clear();
    ui.treeWidgetList->clear();
    ui.treeWidgetRecent->clear();
    ui.treeWidgetSearch->clear();

	ComicState* state = ComicState::State();

	map<string, string> directories = state->LibraryDirectories();
	for (map<string, string>::const_iterator it = directories.begin(); it != directories.end(); it++)
	{
        this->ShowLibrary(QString::fromUtf8(it->first.c_str()));
	}

    ui.treeWidgetLibraries->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.treeWidgetList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.treeWidgetRecent->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.treeWidgetSearch->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    emit NumberOfComicsChanged(ComicState::State()->NumberOfComicsInLibraries());
}

QTreeWidgetItem* LibraryViewContent::CreateLibraryTreeItem(const string &name)
{
    QTreeWidgetItem* libraryItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(NULL), QStringList(QString::fromUtf8(name.c_str())));
    libraryItem->setIcon(0, QIcon(":/ComicDesktop/Resources/Library.png"));
    libraryItem->setData(this->_itemColumnDataType, Qt::UserRole, this->_itemTypeLibrary);

    return libraryItem;
}

QTreeWidgetItem* LibraryViewContent::CreateFolderTreeItem(QTreeWidgetItem *parentItem, const string &name, const string &path)
{
    QTreeWidgetItem* fileItem = new QTreeWidgetItem(parentItem, QStringList(QString::fromUtf8(name.c_str())));
    fileItem->setIcon(0, QIcon(":/trolltech/styles/commonstyle/images/dirclosed-32.png"));
    fileItem->setData(this->_itemColumnDataType, Qt::UserRole, this->_itemTypeFolder);
    fileItem->setData(this->_itemColumnDataPath, Qt::UserRole, QVariant(QString::fromUtf8(path.c_str())));
    parentItem->addChild(fileItem);

    return fileItem;
}

QTreeWidgetItem* LibraryViewContent::CreateComicTreeItem(QTreeWidgetItem *parentItem, const string &name, const string &path, ComicFile::ComicFileType ctype)
{
    QTreeWidgetItem* fileItem = NULL;

    // get the type of file it is
    switch (ctype)
    {
    case ComicFile::Zip:
        fileItem = new QTreeWidgetItem(parentItem, QStringList(QString::fromUtf8(name.c_str())));
        fileItem->setIcon(0, QIcon(":/ComicDesktop/Resources/FileZip.png"));
        break;
    case ComicFile::Rar:
        fileItem = new QTreeWidgetItem(parentItem, QStringList(QString::fromUtf8(name.c_str())));
        fileItem->setIcon(0, QIcon(":/ComicDesktop/Resources/FileRar.png"));
        break;
    case ComicFile::Image:
        fileItem = new QTreeWidgetItem(parentItem, QStringList(QString::fromUtf8(name.c_str())));
        fileItem->setIcon(0, QIcon(":/ComicDesktop/Resources/FileImage.png"));
        break;
#ifdef PDFSUPPORT
    case ComicFile::Pdf:
        fileItem = new QTreeWidgetItem(parentItem, QStringList(QString::fromUtf8(name.c_str())));
        fileItem->setIcon(0, QIcon(":/ComicDesktop/Resources/FilePdf.png"));
        break;
#endif
    default:
        break;
    }

    fileItem->setData(this->_itemColumnDataType, Qt::UserRole, this->_itemTypeFile);
    fileItem->setData(this->_itemColumnDataPath, Qt::UserRole, QVariant(QString::fromUtf8(path.c_str())));
    fileItem->setData(this->_itemColumnDataComicType, Qt::UserRole, static_cast<int>(ctype));

    if (parentItem != nullptr)
    {
        parentItem->addChild(fileItem);
    }

    return fileItem;
}

void LibraryViewContent::ShowLibrary_Sorted(const ComicLibrary &library, string &name)
{
    QTreeWidgetItem* libraryItem = CreateLibraryTreeItem(name);
    ui.treeWidgetList->addTopLevelItem(libraryItem);

    // parse the sorted list
    const vector<ComicLibrary::ComicFolder> &comicGroups = library.GetGroupedComicList();
    for (auto it = comicGroups.begin(); it != comicGroups.end(); it++)
    {
        const ComicLibrary::ComicFolder& folder = *it;

        QTreeWidgetItem* folderItem = this->CreateFolderTreeItem(libraryItem, folder.name, folder.path);

        for (auto it2 = folder.Comics.begin(); it2 != folder.Comics.end(); it2++)
        {
            const ComicLibraryItem& item = **it2;

            this->CreateComicTreeItem(folderItem, item.PrettyName(), item.Path(), item.FileType());
        }
    }

    libraryItem->setExpanded(true);
}

void LibraryViewContent::ShowLibrary_Files(const ComicLibrary &library, string &name)
{
    QTreeWidgetItem* libraryItem = CreateLibraryTreeItem(name);
    ui.treeWidgetLibraries->addTopLevelItem(libraryItem);

    // parse the library files
    const ComicLibrary::ComicFolder &folder = library.GetLibraryComics();
    this->ParseComicFolder(libraryItem, folder);

    libraryItem->setExpanded(true);
}

void LibraryViewContent::ShowLibrary_Recent(const ComicLibrary &library, string &name)
{
    QTreeWidgetItem* libraryItem = CreateLibraryTreeItem(name);
    ui.treeWidgetRecent->addTopLevelItem(libraryItem);

    // parse the sorted list
    const vector<boost::shared_ptr<ComicLibraryItem> > &recentItems = library.GetModifiedTimeComicList();
    for (auto it = recentItems.begin(); it != recentItems.end(); it++)
    {
        const ComicLibraryItem &item = (**it);

        this->CreateComicTreeItem(libraryItem, item.PrettyName(), item.Path(), item.FileType());
    }

    libraryItem->setExpanded(true);
}

void LibraryViewContent::ShowLibrary(QString name)
{
    string u8name(name.toUtf8());

    try
    {
        // read the library
        ComicState::State()->RefreshLibrary(u8name.c_str());
        const ComicLibrary *library = ComicState::State()->Library(u8name.c_str());

        this->ShowLibrary_Files(*library, u8name);

        this->ShowLibrary_Sorted(*library, u8name);

        this->ShowLibrary_Recent(*library, u8name);
    }
    catch (FileException&)
    {
        QMessageBox msg(QMessageBox::Warning,
                        tr("Library"),
                        QString(tr("The library '%1' could not be read. \n\nDo you want to remove it?\n")).arg(name),
                        QMessageBox::Yes | QMessageBox::No, this);

        if (msg.exec() == QMessageBox::Yes)
        {
            ComicState::State()->RemoveLibraryDirectory(name.toUtf8());
            ComicUtils::StoreState();

            QMessageBox msg(QMessageBox::Information,
                            tr("Library"),
                            QString(tr("The library was removed.\n")),
                            QMessageBox::Ok, this);
            msg.exec();
        }
    }
}

void LibraryViewContent::CreateMenus()
{
    this->_fileMenu = new QMenu(this);
    this->_treeMenu = new QMenu(this);
    this->_libraryMenu = new QMenu(this);
    this->_searchItemMenu = new QMenu(this);

    // open
    this->_fileMenu->addAction(ui.action_OpenFile);
    this->_fileMenu->addAction(ui.action_CopyComicName);
    this->_fileMenu->addAction(ui.action_ComicInformation);
    this->_searchItemMenu->addAction(ui.action_OpenFile);
    this->_searchItemMenu->addAction(ui.action_CopyComicName);
    this->_searchItemMenu->addAction(ui.action_ComicInformation);

    // separator
    this->_fileMenu->addSeparator();
    this->_searchItemMenu->addSeparator();

    // remove library
    this->_fileMenu->addAction(ui.action_RemoveLibrary);
    this->_treeMenu->addAction(ui.action_RemoveLibrary);
    this->_libraryMenu->addAction(ui.action_RemoveLibrary);

    // separator
    this->_fileMenu->addSeparator();
    this->_treeMenu->addSeparator();
    this->_libraryMenu->addSeparator();

    // add library
    this->_fileMenu->addAction(ui.action_AddLibrary);
    this->_treeMenu->addAction(ui.action_AddLibrary);
    this->_libraryMenu->addAction(ui.action_AddLibrary);

    // refresh library
    this->_fileMenu->addAction(ui.action_RefreshLibraries);
    this->_treeMenu->addAction(ui.action_RefreshLibraries);
    this->_libraryMenu->addAction(ui.action_RefreshLibraries);
    this->_searchItemMenu->addAction(ui.action_RefreshLibraries);
}

void LibraryViewContent::hideEvent(QHideEvent *e)
{
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    setting.setValue("Library/SplitFactor", ui.splitter->saveState());
    setting.setValue("Library/ViewType", ui.cboViewType->currentIndex());

    QFrame::hideEvent(e);
}

void LibraryViewContent::SearchReady()
{
    QString text = ui.lineEditSearch->text();

    this->_searchTimer.stop();

    // clear any selection
    ui.treeWidgetLibraries->setCurrentItem(NULL);

    // validate the search results is loaded
    if (ui.treeWidgetSearch->topLevelItemCount() <= 0)
    {
        // show processing indicator if not already shown
        bool previouslyVisibleLabel = ui.lblProcessing->isVisible();
        ui.lblProcessing->setVisible(true);
        ApplicationInstance->processEvents();

        // create the complete list
        vector<const ComicLibraryItem*> totalItems;

        // populate the list from all libraries
        map<string, string> libraryDirectories = ComicState::State()->LibraryDirectories();
        for (auto it = libraryDirectories.begin(); it != libraryDirectories.end(); it++)
        {
            const ComicLibrary *library = ComicState::State()->Library(it->first.c_str());

            const vector<boost::shared_ptr<ComicLibraryItem> > itemList = library->GetCompleteComicList();
            for (auto itemit = itemList.begin(); itemit != itemList.end(); itemit++)
            {
                totalItems.push_back(itemit->get());
            }
        }

        // sort the lists
        std::sort(totalItems.begin(), totalItems.end(), [](const ComicLibraryItem *i1, const ComicLibraryItem *i2)
        {
            return *i1 < *i2;
        });

        // populate the search tree
        for (auto it = totalItems.begin(); it != totalItems.end(); it++)
        {
            const ComicLibraryItem &item = **it;
            ui.treeWidgetSearch->addTopLevelItem(this->CreateComicTreeItem(nullptr, item.PrettyName(), item.Path(), item.FileType()));
        }

        // clear processing indicator if need be
        ApplicationInstance->processEvents();
        ui.lblProcessing->setVisible(previouslyVisibleLabel);
    }

    if (text.isEmpty())
    {
        ui.cboViewType->setEnabled(true);
        ui.toolButtonClearSearch->setEnabled(false);
        ui.toolButtonAddLibrary->setEnabled(true);
        ui.toolButtonRemoveLibrary->setEnabled(true);
        ui.treeWidgetSearch->setVisible(false);
        this->on_cboViewType_currentIndexChanged(ui.cboViewType->currentIndex());

        this->_searching = false;
    }
    else
    {
        ui.cboViewType->setEnabled(false);
        ui.toolButtonClearSearch->setEnabled(true);
        ui.toolButtonAddLibrary->setEnabled(false);
        ui.toolButtonRemoveLibrary->setEnabled(false);

        ui.treeWidgetSearch->setVisible(true);
        ui.treeWidgetLibraries->setVisible(false);
        ui.treeWidgetList->setVisible(false);
        ui.treeWidgetRecent->setVisible(false);

        this->_searching = true;

        for (int i = 0; i < ui.treeWidgetSearch->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *item = ui.treeWidgetSearch->topLevelItem(i);
            if (item->text(0).contains(ui.lineEditSearch->text(), Qt::CaseInsensitive))
            {
                item->setHidden(false);
            }
            else
            {
                item->setHidden(true);
            }
        }
    }

//    // if search text is empty, clear any search results
//    if (text.isEmpty())
//    {
//        ui.treeWidgetSearch->clear();
//        ui.treeWidgetLibraries->items()
//        for_each(begin(this->_libraryItems), end(this->_libraryItems), [&](pair<string, vector<TreeComicItem> > kvp)
//        {
//            for_each(begin(kvp.second), end(kvp.second), [&](TreeComicItem& item)
//            {
//                // make sure this item's parent is set to the root library
//                if (item.Item->parent() != item.FolderParent)
//                {
//                    item.LibraryParent->removeChild(item.Item);
//                    item.FolderParent->addChild(item.Item);
//                }
//                item.Item->setHidden(false);
//            });
//        });

//        this->SetFoldersHidden(false);
//        ui.toolButtonClearSearch->setEnabled(false);
//    }
//    // if not empty, procede with the search
//    else
//    {
//        for_each(begin(this->_libraryItems), end(this->_libraryItems), [&text](pair<string, vector<TreeComicItem> > kvp)
//        {
//            QString searchText = text;
//            for_each(begin(kvp.second), end(kvp.second), [&searchText](TreeComicItem& item)
//            {
//                // if this item's parent is not the library node, make the library node its parent and remove from the folder
//                if (item.Item->parent() != item.LibraryParent)
//                {
//                    item.FolderParent->removeChild(item.Item);
//                    item.LibraryParent->addChild(item.Item);
//                }

//                // if the search text matches this item, make it visible, otherwise invisible
//                if (item.Item->text(0).contains(searchText, Qt::CaseInsensitive))
//                {
//                    item.Item->setHidden(false);
//                }
//                else
//                {
//                    item.Item->setHidden(true);
//                }
//            });
//        });

//        this->SetFoldersHidden(true);
//        ui.toolButtonClearSearch->setEnabled(true);
//    }
}

void LibraryViewContent::FocusSearch()
{
    ui.lineEditSearch->setFocus();
}

QString LibraryViewContent::FindNextInSeries(const QString &path, bool &found)
{
    // get the pretty name to compare by
    ComicFileName name(path.toUtf8());

    // find in the smart-sort list
    QString next = FindNextInSeriesInTree(ui.treeWidgetList, QString::fromUtf8(name.PrettyName()), path, found);

    // if the item was not found or a match was already located, return
    if (!found || !next.isEmpty())
    {
        return next;
    }

    // find the folder list
    next = FindNextInSeriesInTree(ui.treeWidgetLibraries, QString::fromUtf8(name.FileName()), path, found);

    return next;
}

QString LibraryViewContent::FindNextInSeriesInTree(QTreeWidget *tree, const QString &name, const QString &path, bool &found)
{
    // find all items that match the name
    QList<QTreeWidgetItem*> items = tree->findItems(name, Qt::MatchFixedString | Qt::MatchRecursive);

    // if the item was not found at all, then mark
    if (items.count() == 0)
    {
        found = false;
    }
    else
    {
        found = true;
    }

    // iterate over the matches
    QString nextPath;
    for (QList<QTreeWidgetItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
        // try to get the parent of the found node
        QTreeWidgetItem *parent = (*it)->parent();
        if (parent == NULL)
        {
            continue;
        }

        // try to get the index of the found child
        int index = parent->indexOfChild(*it);
        if (index < 0)
        {
            continue;
        }

        // if the series has additional comics, then look for the next item
        if (parent->childCount() > index + 1)
        {
            QTreeWidgetItem *next = parent->child(index + 1);
            nextPath = next->data(this->_itemColumnDataPath, Qt::UserRole).toString();

            // if the next item was found and the original path matches the one passed, then
            // return immediatley because we know it is the correct one
            if ((*it)->data(this->_itemColumnDataPath, Qt::UserRole).toString().compare(path, Qt::CaseInsensitive) == 0)
            {
                return nextPath;
            }

            // if we get here, we found a match that will work, but should continue looking for next option if there is
            // one in case the user has multiple copies in their library
        }
    }

    return nextPath;
}

void LibraryViewContent::ParseComicFolder(QTreeWidgetItem* parentItem, const ComicLibrary::ComicFolder &folder)
{
    // for each folder in the folder
    for (auto it = folder.Folders.begin(); it != folder.Folders.end(); it++)
    {
        QTreeWidgetItem* fileItem = this->CreateFolderTreeItem(parentItem, it->name, it->path);

        this->ParseComicFolder(fileItem, *it);

        // check to make sure there are files/folders in this folder
        if (fileItem->childCount() == 0)
        {
            delete fileItem;
        }
    }

    // for each file in the folder
    for (auto it = folder.Comics.begin(); it != folder.Comics.end(); it++)
    {
        ComicLibraryItem *item = (*it).get();
        this->CreateComicTreeItem(parentItem, item->FileName(), item->Path(), item->FileType());
    }
}

void LibraryViewContent::SetFoldersHidden(bool isHidden)
{
    for (int i = 0; i < ui.treeWidgetLibraries->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = ui.treeWidgetLibraries->topLevelItem(i);

        this->SetFoldersHiddenRecurse(item, isHidden);
    }
}

void LibraryViewContent::SetFoldersHiddenRecurse(QTreeWidgetItem *item, bool isHidden)
{
    if (item->data(this->_itemColumnDataType, Qt::UserRole).toInt() == this->_itemTypeFolder && item->isHidden() != isHidden)
    {
        item->setHidden(isHidden);
    }

    for (int i = 0; i < item->childCount(); i++)
    {
        QTreeWidgetItem* childItem = item->child(i);

        this->SetFoldersHiddenRecurse(childItem, isHidden);
    }
}

void LibraryViewContent::Refresh()
{
    // show the label and process the events
    ui.lblProcessing->setVisible(true);
    ApplicationInstance->processEvents();

    // reload the libraries
	this->LoadLibraries();

    // reset any filter that may have been applied (job000022)
    this->SearchReady();

    // validate all events are processed and close the label
    ApplicationInstance->processEvents();
    ui.lblProcessing->setVisible(false);
}

void LibraryViewContent::AddLibrary()
{
	do
    {
        const string initialLibDirPreference = "InitialLibraryDirectory";
        QSettings dirSetting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
        QString initialDir = dirSetting.value(QString::fromUtf8(initialLibDirPreference.c_str()), QString()).toString();
        QString directory = QFileDialog::getExistingDirectory(this, tr("Comic Library Folder"), initialDir);

		// if the cancel was pushed
        if (directory.isEmpty())
		{
			return;
        }
        else
        {
            dirSetting.setValue(QString::fromUtf8(initialLibDirPreference.c_str()), directory);
        }

		// select the name
        QString name;
		do
		{
            bool ok;
            name = QInputDialog::getText(
                this,
				tr("Library Name"), 
                QString(),
				QLineEdit::Normal, 
                QFileInfo(directory).baseName(),
				&ok, 
				Qt::Tool);

			if (!ok)
			{
				return;
			}

			if (name.isEmpty())
			{
				QMessageBox box(
					tr("Name Empty"),
					tr("<br />The name cannot be empty.<br />"),
					QMessageBox::Warning,
					QMessageBox::Ok, 0, 0,
					this,
					Qt::Tool);

				box.exec();
			}
			else
			{
				break;
			}
		}
		while (true);

		// store the library
		try
		{
			ComicState* state = ComicState::State();
            state->AddLibraryDirectory(name.toUtf8(), directory.toUtf8());
            ComicUtils::StoreState();

            // create the new library folder
            this->ShowLibrary(name);

            // reset search if necessary
            this->SearchTextChanged(ui.lineEditSearch->text());

			// emit that the number of files has changed
            emit NumberOfComicsChanged(ComicState::State()->NumberOfComicsInLibraries());

			break;
		}
		catch (ComicLibraryAlreadyExistsException&)
		{
			QMessageBox box(
				tr("Library Already Exists"), 
                tr("<br />The name or path is already in use. Do you want to select another?<br />"),
				QMessageBox::Warning, 
				QMessageBox::Yes, QMessageBox::No, 0, 
				this, 
				Qt::Tool);

			box.setDefaultButton(QMessageBox::Yes);

			if (box.exec() != QMessageBox::Yes)
			{
				break;
			}
		}
	} while (true);
}

QTreeWidget* LibraryViewContent::GetSelectedTreeWidget()
{
    if (this->_searching)
    {
        return ui.treeWidgetSearch;
    }

    switch (ui.cboViewType->currentIndex())
    {
    case LIBRARY_VIEWTYPE_SORTED:
        return ui.treeWidgetList;
    case LIBRARY_VIEWTYPE_FILES:
        return ui.treeWidgetLibraries;
    case LIBRARY_VIEWTYPE_RECENT:
        return ui.treeWidgetRecent;
    }

    return NULL;
}

void LibraryViewContent::RemoveLibrary()
{
    QTreeWidget *treeWidget = this->GetSelectedTreeWidget();
    if (treeWidget == nullptr)
    {
        return;
    }

    QList<QTreeWidgetItem*> selected = treeWidget->selectedItems();
	if (selected.size() != 1)
	{
		QMessageBox box(
			tr("Invalid Selection"), 
			tr("Select a single library or file."), 
			QMessageBox::Warning, 
			QMessageBox::Ok, 0, 0, 
			this, 
			Qt::Tool);

		box.exec();
		return;
	}

	QTreeWidgetItem* selectedItem = selected[0];
	while (selectedItem != NULL && selectedItem->data(this->_itemColumnDataType, Qt::UserRole).toInt() != this->_itemTypeLibrary)
	{
		selectedItem = selectedItem->parent();
	}

	if (selectedItem == NULL)
	{
		QMessageBox box(
			tr("Invalid Selection"), 
			tr("Select a single library or file."), 
			QMessageBox::Warning, 
            QMessageBox::Ok, 0, 0,
			this, 
			Qt::Tool);

		box.exec();
		return;
	}

	QMessageBox box(
			tr("Remove Library"), 
			(tr("Are you sure you want to remove %1?")).arg(selectedItem->text(0)), 
			QMessageBox::Warning, 
			QMessageBox::Yes, QMessageBox::No, 0, 
			this, 
			Qt::Tool);
	box.setDefaultButton(QMessageBox::No);

	if (box.exec() != QMessageBox::Yes)
	{
		return;
	}

	// remove the library
	ComicState* state = ComicState::State();
    state->RemoveLibraryDirectory(selectedItem->text(0).toUtf8());
    ComicUtils::StoreState();

    QString selectedItemText = selectedItem->text(0);
    delete this->FindLibraryItem(ui.treeWidgetLibraries, selectedItemText);
    delete this->FindLibraryItem(ui.treeWidgetList, selectedItemText);
    delete this->FindLibraryItem(ui.treeWidgetRecent, selectedItemText);

    emit NumberOfComicsChanged(ComicState::State()->NumberOfComicsInLibraries());
}

QTreeWidgetItem* LibraryViewContent::FindLibraryItem(QTreeWidget *widget, QString& name)
{
    int totalItems = widget->topLevelItemCount();

    for (int i = 0; i < totalItems; i++)
    {
        QTreeWidgetItem *item = widget->topLevelItem(i);
        if (item != NULL)
        {
            if (item->text(0).compare(name) == 0)
            {
                return item;
            }
        }
    }

    return NULL;
}

void LibraryViewContent::Open()
{
    QTreeWidgetItem* item = this->GetSelectedTreeWidget()->currentItem();
    if (item != NULL)
    {
        this->ComicActivated(item, 0);
    }
}

void LibraryViewContent::ComicSelectionChanged()
{
	// get selections
    QList<QTreeWidgetItem*> items = this->GetSelectedTreeWidget()->selectedItems();

	// if 0 or >1 items selected
	if (items.size() != 1)
	{
		qDebug() << "Selection changed to non-file";
        ui.labelPreview->setText(this->_textNoPreview);
        ui.toolButtonViewMetaData->setEnabled(false);

		return;
	}

	// get the item type
	QTreeWidgetItem* item = items[0];
	QVariant type = item->data(this->_itemColumnDataType, Qt::UserRole);
	QVariant path = item->data(this->_itemColumnDataPath, Qt::UserRole);

	// if type is not a file
	if (type.toInt() != this->_itemTypeFile)
	{
        ui.labelPreview->setText(this->_textNoPreview);
        ui.toolButtonViewMetaData->setEnabled(false);
		return;
	}

	qDebug() << "Selection changed to: " << path;

    // preview the file
    ui.labelPreview->SetImage(path.toString().toUtf8());
    ui.toolButtonViewMetaData->setEnabled(true);
}

void LibraryViewContent::ComicActivated(QTreeWidgetItem* item, int)
{
	QVariant type = item->data(this->_itemColumnDataType, Qt::UserRole);
	int itype = type.toInt();

	if (itype != this->_itemTypeFile)
	{
		return;
	}

	// get the path
	QVariant path = item->data(this->_itemColumnDataPath, Qt::UserRole);
    std::string spath(path.toString().toUtf8());

    ComicUtils::OpenComic(QString::fromUtf8(spath.c_str()));
}

void LibraryViewContent::ClearSearchTriggered()
{
    ui.lineEditSearch->setText("");
    this->_searchTimer.start(10);
}

void LibraryViewContent::TreeContextMenuRequested(const QPoint& pos)
{
    QTreeWidgetItem* item = this->GetSelectedTreeWidget()->itemAt(pos);
    QPoint globalPos = this->GetSelectedTreeWidget()->mapToGlobal(pos);

    if (item == NULL)
    {
        this->_treeMenu->popup(globalPos);
    }
    else
    {
        int typeId = item->data(this->_itemColumnDataType, Qt::UserRole).toInt();
        switch (typeId)
        {
        case _itemTypeFile:
            if(this->_searching)
            {
                this->_searchItemMenu->popup(globalPos);
            }
            else
            {
                this->_fileMenu->popup(globalPos);
            }
            break;
        case _itemTypeFolder:
        case _itemTypeLibrary:
            this->_libraryMenu->popup(globalPos);
            break;
        }
    }
}

void LibraryViewContent::Export()
{
    try
    {
        const QString initialLibDirPreference = "InitialExportLibraryDirectory";
        QSettings dirSetting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
        QString initialDir = dirSetting.value(initialLibDirPreference, QString()).toString();

        QString filepath = QFileDialog::getSaveFileName(this, tr("Export Comic List"), initialDir, "Text Files (*.txt)");
        if (filepath.isEmpty())
        {
            return;
        }
        else
        {
            dirSetting.setValue(initialLibDirPreference, QFileInfo(filepath).path());
        }

        QMessageBox msg(QMessageBox::Question,
                     tr("Include path"),
                     tr("Do you want to include the smart-sorted series in the name of the comic?"),
                     QMessageBox::Yes | QMessageBox::No);
        bool includeSeries = msg.exec() == QMessageBox::Yes;

        map<string, vector<string> > items;

        map<string, string> libraryDirectories = ComicState::State()->LibraryDirectories();
        for (auto itlib = libraryDirectories.begin(); itlib != libraryDirectories.end(); itlib++)
        {
            string name = itlib->first;
            const ComicLibrary *library = ComicState::State()->Library(name.c_str());

            const vector<boost::shared_ptr<ComicLibraryItem> > &comics = library->GetCompleteComicList();

            for (auto itfile = comics.begin(); itfile != comics.end(); itfile++)
            {
                ComicLibraryItem *item = itfile->get();

                if (includeSeries)
                {
                    string cname = item->BaseName();
                    cname.append("/");
                    cname.append(item->PrettyName());
                    items[itlib->first].push_back(cname);
                }
                else
                {
                    items[itlib->first].push_back(item->PrettyName());
                }
            }
        }

        ComicUtilities::SaveComicList(items, string(filepath.toUtf8()).c_str());

        QMessageBox box(
            tr("Export Completed"),
            tr("<br />The comic file list save was successful.<br />"),
            QMessageBox::Information,
            QMessageBox::Ok, 0, 0,
            this,
            Qt::Tool);
        box.exec();
    }
    catch (...)
    {
        QMessageBox box(
            tr("Export Failed"),
            tr("<br />The file failed to be saved.<br />"),
            QMessageBox::Critical,
            QMessageBox::Ok, 0, 0,
            this,
            Qt::Tool);
        box.exec();
    }
}

void LibraryViewContent::SearchTextChanged(const QString& text)
{
    this->_searchTimer.start(1000);
}


void LibraryViewContent::CopyNameToClipboard()
{
    QTreeWidgetItem* item = this->GetSelectedTreeWidget()->currentItem();
    if (item != NULL)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(item->text(0));
    }
}

void LibraryViewContent::on_cboViewType_currentIndexChanged(int index)
{
    ui.treeWidgetLibraries->setVisible(index == LIBRARY_VIEWTYPE_FILES);
    ui.treeWidgetList->setVisible(index == LIBRARY_VIEWTYPE_SORTED);
    ui.treeWidgetRecent->setVisible(index == LIBRARY_VIEWTYPE_RECENT);
}

void LibraryViewContent::ViewComicInformation()
{
    QTreeWidgetItem* item = this->GetSelectedTreeWidget()->currentItem();
    if (item != NULL)
    {
        ComicInfoViewer::OpenComicInfoViewer(item->data(_itemColumnDataPath, Qt::UserRole).toString());
    }
}
