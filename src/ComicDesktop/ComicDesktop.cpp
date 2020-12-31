/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicDesktop.h"
#include "ComicLaunchEvent.h"
#include <XUtilities.hpp>
#include <qresource.h>
#include <ComicState.h>
#include "ComicUtils.h"
#include "LibraryViewContent.h"
#include "BrowserContent.h"
#include <qfiledialog.h>
#include "ComicUtilities.hpp"
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <QMessageBox>
#include "Options.h"
#include <ComicExceptions.hpp>
#include <PdfComicFile.hpp>
#include <qdesktopservices.h>
#include "HomePage.h"
#include <ComicFileName.hpp>
#include "qtsingleapplication.h"
#include <qdebug.h>
#include <qlocale.h>
#include "AboutView.h"
#include "BookmarkViewContent.h"
#include "BusyIndicator.h"

using namespace ComicSeer;
using namespace std;

ComicDesktop* ComicDesktop::_instance = NULL;

const int CustomActionComicViewResetId = 1;
const int CustomActionComicRotationResetId = 2;

ComicDesktop* ComicDesktop::Instance()
{
    return _instance;
}

bool ComicDesktop::IsComicOpen(const QString &file) const
{
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        ComicViewWidget* view = dynamic_cast<ComicViewWidget*>(ui.tabWidget->widget(i));
        if (view != nullptr)
        {
            if (file.compare(QString::fromUtf8(view->Path().c_str()), Qt::CaseInsensitive) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

void ComicDesktop::CloseComic(const QString &file)
{
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        ComicViewWidget* view = dynamic_cast<ComicViewWidget*>(ui.tabWidget->widget(i));
        if (view != nullptr)
        {
            if (file.compare(QString::fromUtf8(view->Path().c_str()), Qt::CaseInsensitive) == 0)
            {
                this->TabCloseRequested(i);
                return;
            }
        }
    }
}

ComicDesktop::ComicDesktop(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
	_instance = this;

	ui.setupUi(this);

    // move the central widget to a sub-window
    // this will allow the left/right dock panels to stretch
    // to the maximum height of the window
    _subWindow = new QMainWindow(this);
    _subWindow->setCentralWidget(ui.tabWidget);
    _subWindow->setWindowFlags(Qt::Widget);
    _subWindow->setContextMenuPolicy(Qt::NoContextMenu);
    this->setCentralWidget(_subWindow);

    // set the version string
    QString title = this->windowTitle();
    title.replace("{VERSION}", ComicUtils::GetVersionString());
#ifdef _DEBUG
    title.append(" ##### DEBUG #####");
#endif
    this->setWindowTitle(title);

	// create the status bar content
	this->CreateStatusBar();

	// initialize buttons/content
	this->InitializeButtons();

	// create the library view
    this->CreateLibraryView();

    // create the bookmark view
    this->CreateBookmarkView();

    // create the home page
    this->CreateHomeTab();

    // initialize all of the slots
    this->connect(ui.action_Libraries, SIGNAL(toggled(bool)), SLOT(ViewLibrariesToggled(bool)));
    this->connect(ui.action_Bookmarks, SIGNAL(toggled(bool)), SLOT(ViewBookmarksToggled(bool)));
	this->connect(ui.action_Close, SIGNAL(triggered()), SLOT(CloseTriggered()));
	this->connect(ui.action_Recent, SIGNAL(triggered()), SLOT(RecentTriggered()));
	this->connect(ui.action_Open, SIGNAL(triggered()), SLOT(OpenTriggered()));
	this->connect(ui.action_Show, SIGNAL(triggered()), SLOT(ShowToolbarTriggered()));
	this->connect(ui.action_Collapse, SIGNAL(triggered()), SLOT(CollapseToolbarTriggered()));
	this->connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(TabCloseRequested(int)));
	this->connect(ui.action_Home, SIGNAL(triggered()), SLOT(HomeTriggered()));
	this->connect(ui.tabWidget, SIGNAL(currentChanged(int)), SLOT(TabCurrentChanged(int)));
	this->connect(ui.action_FitAll, SIGNAL(triggered()), SLOT(PageFitAllTriggered()));
	this->connect(ui.action_FitHeight, SIGNAL(triggered()), SLOT(PageFitHeightTriggered()));
	this->connect(ui.action_FitWidth, SIGNAL(triggered()), SLOT(PageFitWidthTriggered()));
	this->connect(ui.action_PageBack, SIGNAL(triggered()), SLOT(PageBackTriggered()));
	this->connect(ui.action_PageFoward, SIGNAL(triggered()), SLOT(PageForwardTriggered()));
    this->connect(ui.action_Next, SIGNAL(triggered()), SLOT(NextTriggered()));
	this->connect(this->_libraryView, SIGNAL(closeTriggered()), SLOT(LibrariesCloseTriggered()));
    this->connect(this->_bookmarkView, SIGNAL(closeTriggered()), SLOT(BookmarksCloseTriggered()));
	this->connect(this->_recentMenu, SIGNAL(RecentClicked(std::string)), SLOT(RecentItemSelected(std::string)));
    this->connect(ui.actionOptions, SIGNAL(triggered()), SLOT(OptionsClicked()));
    this->connect(ui.action_FullScreen, SIGNAL(triggered()), SLOT(FullscreenClicked()));
    this->connect(ui.action_About, SIGNAL(triggered()), SLOT(AboutClicked()));
    this->connect(ui.tabWidget, SIGNAL(CustomContextMenuTriggered(int,int)), SLOT(CustomContextMenuTriggered(int,int)));
    this->_slotShowPathUpdated = ComicState::State()->ShowFullPathUpdated.connect([this](bool show) { this->ShowFullPathChanged(show); });
    this->connect(static_cast<BookmarkViewContent*>(this->_bookmarkView->ContainedWidget()),
                  SIGNAL(ActiveComicRequested(QString&,int&)), SLOT(BookmarkActiveComicRequested(QString&,int&)));

    // connect secondary application slot
    this->connect(ApplicationInstance, SIGNAL(messageReceived(QString)), SLOT(SingleAppMsgReceived(QString)));

    // restore previous state and geometry
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    this->restoreState(setting.value("MainWindow/State").toByteArray());
    this->restoreGeometry(setting.value("MainWindow/Geometry").toByteArray());
    this->_subWindow->restoreState(setting.value("MainWindow/SubState").toByteArray());
    this->_subWindow->restoreGeometry(setting.value("MainWindow/SubGeometry").toByteArray());
    if (!setting.value("MainWindow/MenuFull", true).toBool()) ui.action_Collapse->trigger();

    // set view for comic
    this->SetComicInformation();

    // load a previous session if needed
    this->LoadSession();
}

ComicDesktop::~ComicDesktop()
{

}

void ComicDesktop::CreateBookmarkView()
{
    BookmarkViewContent* bookmarkView = new BookmarkViewContent();
    this->_bookmarkView = new ComicDockWidget(bookmarkView, tr("Bookmarks"), _subWindow, Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    _subWindow->addDockWidget(Qt::BottomDockWidgetArea, this->_bookmarkView);
    ui.action_Bookmarks->setChecked(false);
    this->_bookmarkView->setVisible(false);
}

void ComicDesktop::CreateLibraryView()
{
	LibraryViewContent* libraryView = new LibraryViewContent();
    this->_libraryView = new ComicDockWidget(libraryView, tr("Libraries"), this);
	this->connect(libraryView, SIGNAL(NumberOfComicsChanged(long)), SLOT(NumberOfComicsChanged(long)));
    this->NumberOfComicsChanged(ComicState::State()->NumberOfComicsInLibraries());
	this->addDockWidget(Qt::LeftDockWidgetArea, this->_libraryView);
	ui.action_Libraries->setChecked(true);
}

void ComicDesktop::InitializeButtons()
{
	// add a spacer to move the close/collapse to the right side
	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui.mainToolBar->insertWidget(ui.action_Show, spacer);

	// set the recent menu
	this->_recentMenu = new RecentComicMenu(ui.mainToolBar);
}

void ComicDesktop::CreateStatusBar()
{
	QStatusBar* status = this->statusBar();

	// create the widgets
	_statusFileName = new QLabel(status);
	_statusPageNumber = new QLabel(status);
	_statusLibraryNumber = new QLabel(status);
    _statusLocaleName = new QLabel(status);
    _statusUpdateButton = new UpdateButton(status);

	// format the widgets
	_statusFileName->setMargin(3);
	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	policy.setHorizontalStretch(1);
	_statusFileName->setSizePolicy(policy);
	_statusPageNumber->setMargin(3);
    _statusLibraryNumber->setMargin(3);
    _statusLocaleName->setMargin(3);
    _statusUpdateButton->setMaximumSize(20, 20);

	// attach the widgets
	status->addWidget(_statusFileName);
    status->addWidget(_statusUpdateButton);
	status->addWidget(_statusPageNumber);
    status->addWidget(_statusLibraryNumber);
    status->addWidget(_statusLocaleName);

    // set the locale name
    _statusLocaleName->setText(QLocale::system().name());
    if (ComicUtils::CustomLanguageLoaded)
    {
        _statusLocaleName->setToolTip(QString(tr("Custom language file (%1.qm) has been loaded."))
                                      .arg(QString::fromUtf8(ComicUtils::CustomLanguageFileName.c_str())));
    }
    else
    {
        _statusLocaleName->setToolTip(QString(tr("No custom language file (%1.qm) found."))
                                      .arg(QString::fromUtf8(ComicUtils::CustomLanguageFileName.c_str())));
    }
}

void ComicDesktop::ViewLibrariesToggled(bool checked)
{
    this->_libraryView->setVisible(checked);
}

void ComicDesktop::ViewBookmarksToggled(bool checked)
{
    this->_bookmarkView->setVisible(checked);
}

void ComicDesktop::ViewTabsToggled(bool checked)
{
    ui.tabWidget->SetTabBarVisibility(checked);
}

void ComicDesktop::OptionsClicked()
{
    Options dlg(this);
    dlg.exec();
}

void ComicDesktop::FullscreenClicked()
{
    ComicViewWidget* widget = this->SelectedComicWidget();
    if (widget != NULL)
    {
        widget->FullScreen();
    }
}

void ComicDesktop::RecentTriggered()
{
	QRect rect = ui.mainToolBar->actionGeometry(ui.action_Recent);
	QPoint globalPoint = ui.mainToolBar->mapToGlobal(rect.bottomLeft());
	this->_recentMenu->popup(globalPoint, ui.action_Recent);
}

void ComicDesktop::RecentItemSelected(std::string path)
{
	this->AddComicView(path);
}

void ComicDesktop::CloseTriggered()
{
	this->close();
}

void ComicDesktop::LibrariesCloseTriggered()
{
	if (this->ui.action_Libraries->isChecked())
	{
		this->ui.action_Libraries->setChecked(false);
	}
}

void ComicDesktop::BookmarksCloseTriggered()
{
    if (this->ui.action_Bookmarks->isChecked())
    {
        this->ui.action_Bookmarks->setChecked(false);
    }
}

void ComicDesktop::AboutClicked()
{
    AboutView view(this);
    view.exec();
}

void ComicDesktop::SingleAppMsgReceived(QString message)
{
    qDebug() << "Received message from another app instance: " << message;

    QStringList argList = message.split("**", QString::SkipEmptyParts);
    for (auto it = argList.begin(); it != argList.end(); it++)
    {
        QString &arg = *it;

        qDebug() << "Launching: " << arg;

        ComicUtils::OpenComic(arg);
    }
}

void ComicDesktop::CustomContextMenuTriggered(int actionId, int tabIndex)
{
    switch (actionId)
    {
    case CustomActionComicViewResetId:
        break;
    case CustomActionComicRotationResetId:
        break;
    }
}

void ComicDesktop::BookmarkActiveComicRequested(QString &comic, int &page)
{
    ComicViewWidget *view = this->SelectedComicWidget();
    if (view == NULL)
    {
        return;
    }

    comic = QString::fromUtf8(view->Path().c_str());
    bool secondPage;
    page = view->CurrentPage(secondPage) + 1;
}

void ComicDesktop::AddComicView(std::string path, int page)
{
    // start the busy indicator
    //BusyIndicator indicator(QString(tr("Opening...")), this);
    //indicator.show();

	// get the file type
    ComicFile::ComicFileType ctype = ComicFile::ClassifyFileExtension(XUtilities::GetFileExtension(path.c_str()).c_str());
	if (ctype == ComicFile::Unknown)
	{
        QMessageBox msg(QMessageBox::Critical, tr("Open"), tr("The specified file is not a recognized comic file."), QMessageBox::Ok, this);
        msg.exec();

		return;
	}
#ifdef PDFSUPPORT
    else if (ctype == ComicFile::Pdf)
    {
        try
        {
            PdfComicFile::ValidateSupported();
        }
        catch (std::exception& ex)
        {
            string msg = "The PDF file could not be opened, error: \n";
            msg.append(ex.what());

            msg.append("\n\nFor information on using Ghostscript with Comic Seer\n");
            msg.append("to open PDFs, visit the website's blog at \n");
            msg.append("comicseer.xylasoft.com/blog");

            QMessageBox msgbox(QMessageBox::Critical, tr("Open PDF"), QString::fromUtf8(msg.c_str()), QMessageBox::Ok, this);
            msgbox.exec();

            return;
        }
    }
#endif

	// get the file information
    QFileInfo info(QString::fromUtf8(path.c_str()));
	if (!info.exists() || !info.isFile())
	{
        QMessageBox msg(QMessageBox::Critical, tr("Open"), tr("The file no longer exists."), QMessageBox::Ok, this);
        msg.exec();

		return;
	}

	QIcon icon;
	switch (ctype)
	{
    case ComicSeer::ComicFile::Zip:
		icon = QIcon(":/ComicDesktop/Resources/FileZip.png");
		break;
    case ComicSeer::ComicFile::Rar:
		icon = QIcon(":/ComicDesktop/Resources/FileRar.png");
        break;
#ifdef PDFSUPPORT
    case ComicSeer::ComicFile::Pdf:
        icon = QIcon(":/ComicDesktop/Resources/FilePdf.png");
        break;
#endif
	default:
		icon = QIcon(":/ComicDesktop/Resources/FileImage.png");
		break;
	}

	bool found = false;
    QString qpath(QString::fromUtf8(path.c_str()));
	for (int i = 0; i < ui.tabWidget->count(); i++)
	{
		ComicViewWidget* view = dynamic_cast<ComicViewWidget*>(ui.tabWidget->widget(i));
		if (view != nullptr)
		{
            if (qpath.compare(QString::fromUtf8(view->Path().c_str()), Qt::CaseInsensitive) == 0)
			{
				ui.tabWidget->setCurrentIndex(i);
                view->SetCurrentPage(page);
				found = true;
				break;
			}
		}
	}

	// if not already existing, create the tab
	if (!found)
	{
		ComicViewWidget* widget = new ComicViewWidget(path.c_str(), this);
        this->connect(widget, SIGNAL(PageChanged(int, bool)), SLOT(ComicPageChanged(int, bool)));
        int index = ui.tabWidget->addTab(widget, icon, QString::fromUtf8(ComicFileName(info.fileName().toUtf8().constData()).PrettyName()));
		ui.tabWidget->setCurrentIndex(index);
        widget->SetCurrentPage(page);
	}

	// add to recent list
    this->_recentMenu->AddComic(path);

    // close the indicator
    //indicator.close();
}

void ComicDesktop::SaveSession()
{
    QStringList pathList;
    int index = ui.tabWidget->currentIndex();
    if (index < 0) index = 0;
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        QWidget* widget = ui.tabWidget->widget(i);
        if (widget != NULL)
        {
            ComicViewWidget* comicWidget = dynamic_cast<ComicViewWidget*>(widget);
            if (comicWidget != NULL)
            {
                string path = comicWidget->Path();
                pathList.append(QString::fromUtf8(path.c_str()));
            }
        }
    }

    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    setting.setValue("LastSessionComics", pathList);
    setting.setValue("LastSessionIndex", index);
}

void ComicDesktop::LoadSession()
{
    ComicState* state = ComicState::State();
    if (!state->ReopenLastSession())
    {
        return;
    }

    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    QStringList comicList = setting.value("LastSessionComics", QStringList()).toStringList();
    int index = setting.value("LastSessionIndex", 0).toInt();

    for(auto it = begin(comicList); it != end(comicList); it++)
    {
        QString &path = *it;
        ComicUtils::OpenComic(path);
    }

    if (index >= 0 && index < ui.tabWidget->count())
    {
        ui.tabWidget->setCurrentIndex(index);
    }
}

void ComicDesktop::CreateHomeTab()
{
    ui.tabWidget->insertTab(0, new HomePage(ui.tabWidget), QIcon(":/ComicDesktop/Resources/Home.png"), tr("Home"));
    ui.tabWidget->setCurrentIndex(0);
}

bool ComicDesktop::event(QEvent* e)
{
	if (e->type() == ComicLaunchEvent::Event)
	{
		ComicLaunchEvent* cle = dynamic_cast<ComicLaunchEvent*>(e);
        this->AddComicView(cle->Path(), cle->Page());
	}

	return QMainWindow::event(e);
}

void ComicDesktop::ShowToolbarTriggered()
{
    ui.mainToolBar->setIconSize(QSize(46, 20));
	ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.action_Show->setVisible(false);
	ui.action_Collapse->setVisible(true);
}

void ComicDesktop::CollapseToolbarTriggered()
{
	ui.mainToolBar->setIconSize(QSize(16,16));
	ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	ui.action_Show->setVisible(true);
	ui.action_Collapse->setVisible(false);
}

void ComicDesktop::paintEvent(QPaintEvent* e)
{
	QMainWindow::paintEvent(e);
}

void ComicDesktop::OpenTriggered()
{
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    QString initialDir = setting.value("Desktop/OpenDirectory", QString()).toString();

    QString comicFilePath("Comic Files (*.zip *.cbz *.rar *.cbr *.jpg *.png)");
#ifdef PDFSUPPORT
    comicFilePath.append(".pdf");
#endif
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Comic"), initialDir, comicFilePath);
    if (!filename.isEmpty())
    {
        ComicUtils::OpenComic(filename);

        QFileInfo file(filename);
        setting.setValue("Desktop/OpenDirectory", file.dir().path());
	}
}

void ComicDesktop::TabCloseRequested(int index)
{
	QWidget* widget = ui.tabWidget->widget(index);
    ui.tabWidget->removeTab(index);
    delete widget;
}

void ComicDesktop::NumberOfComicsChanged(long count)
{
	QString libraryNumber(tr("Library: %1 comic(s)"));
	this->_statusLibraryNumber->setText(libraryNumber.arg(count));
}

void ComicDesktop::HomeTriggered()
{
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        QWidget* widget = ui.tabWidget->widget(i);
        if (widget != NULL)
        {
            ComicViewWidget* comicWidget = dynamic_cast<ComicViewWidget*>(widget);
            if (comicWidget == NULL)
            {
                ui.tabWidget->setCurrentIndex(i);
                return;
            }
        }
    }

    this->CreateHomeTab();
}

void ComicDesktop::TabCurrentChanged(int)
{
	this->SetComicInformation();

    ComicViewWidget* widget = this->SelectedComicWidget();
    if (widget != NULL)
    {
        widget->setFocus();
    }
}

void ComicDesktop::SetComicInformation()
{
	ComicViewWidget* widget = this->SelectedComicWidget();

	// if there is not comic selection made, disable buttons
	if (widget == NULL)
	{
		ui.action_FitAll->setEnabled(false);
		ui.action_FitHeight->setEnabled(false);
		ui.action_FitWidth->setEnabled(false);
		ui.action_PageBack->setEnabled(false);
        ui.action_Next->setEnabled(false);
		ui.action_PageFoward->setEnabled(false);
        ui.action_FullScreen->setEnabled(false);

        this->_statusFileName->setText(tr("Not viewing a comic"));
        this->_statusPageNumber->setText(tr("---"));

		return;
	}

	// activate page fitting
	ui.action_FitAll->setEnabled(true);
	ui.action_FitHeight->setEnabled(true);
	ui.action_FitWidth->setEnabled(true);
    ui.action_FullScreen->setEnabled(true);

	// set comic information
    bool secondVisible;
    int currentPage = widget->CurrentPage(secondVisible);
	int totalPages = widget->NumberOfPages();
	ui.action_PageBack->setEnabled(currentPage > 0);
	ui.action_PageFoward->setEnabled(currentPage < totalPages - 1);
    ui.action_Next->setEnabled(true);

	// set the status
    this->_statusFileName->setText(QString(tr("File: %1"))
                                   .arg(ComicUtils::GetVisibleComicPath(QString::fromUtf8(widget->Path().c_str()))));
    QString secondPageStr;
    if (secondVisible)
    {
        secondPageStr = QString("/%1").arg(currentPage + 2);
    }
    this->_statusPageNumber->setText(QString(tr("Page: %1%2 of %3")).arg(currentPage + 1).arg(secondPageStr).arg(widget->NumberOfPages()));
}

void ComicDesktop::PageFitAllTriggered()
{
	ComicViewWidget* widget = this->SelectedComicWidget();
	if (widget != NULL)
	{
        widget->setState(ComicSeer::FitWidthHeight);
	}
}

void ComicDesktop::PageFitWidthTriggered()
{
	ComicViewWidget* widget = this->SelectedComicWidget();
	if (widget != NULL)
	{
        widget->setState(ComicSeer::FitWidth);
	}
}

void ComicDesktop::PageFitHeightTriggered()
{
	ComicViewWidget* widget = this->SelectedComicWidget();
	if (widget != NULL)
	{
        widget->setState(ComicSeer::FitHeight);
	}
}

ComicViewWidget* ComicDesktop::SelectedComicWidget()
{
	if (ui.tabWidget->count() == 0)
	{
		return NULL;
	}

	QWidget* widget = ui.tabWidget->currentWidget();
	if (widget == NULL)
	{
		return NULL;
	}

	ComicViewWidget* comicWidget = dynamic_cast<ComicViewWidget*>(widget);
	return comicWidget;
}

void ComicDesktop::PageForwardTriggered()
{
	ComicViewWidget* widget = this->SelectedComicWidget();
	if (widget != NULL)
	{
		widget->PageForward();
	}
}

void ComicDesktop::PageBackTriggered()
{
	ComicViewWidget* widget = this->SelectedComicWidget();
	if (widget != NULL)
	{
		widget->PageBack();
    }
}

void ComicDesktop::NextTriggered()
{
    ComicViewWidget* widget = this->SelectedComicWidget();
    if (widget == NULL)
    {
        return;
    }

    LibraryViewContent* content = static_cast<LibraryViewContent*>(this->_libraryView->ContainedWidget());
    assert(content != NULL);

    QString path = QString::fromUtf8(widget->Path().c_str());

    bool found;
    QString nextPath = content->FindNextInSeries(path, found);

    if (!found)
    {
        QMessageBox msg(QMessageBox::Information,
                        tr("Next"),
                        tr("The comic was not found in your libraries.\n\nTo traverse the series, add them to your libraries."));
        msg.exec();
    }
    else
    {
        if (nextPath.isEmpty())
        {
            QMessageBox msg(QMessageBox::Information,
                            tr("Next"),
                            tr("There was no comic next in series."));
            msg.exec();
        }
        else
        {
            ComicUtils::OpenComic(nextPath);
        }
    }
}

void ComicDesktop::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
}

void ComicDesktop::hideEvent(QHideEvent* e)
{
	QMainWindow::hideEvent(e);
}

void ComicDesktop::showEvent(QShowEvent* e)
{
	QMainWindow::showEvent(e);

    // restore the button state for docked widgets
    ui.action_Libraries->setChecked(this->_libraryView->isVisible());
    ui.action_Bookmarks->setChecked(this->_bookmarkView->isVisible());
}

void ComicDesktop::closeEvent(QCloseEvent* e)
{
    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
    setting.setValue("MainWindow/State", this->saveState());
    setting.setValue("MainWindow/Geometry", this->saveGeometry());
    setting.setValue("MainWindow/MenuFull", ui.action_Collapse->isVisible());
    setting.setValue("MainWindow/SubState", this->_subWindow->saveState());
    setting.setValue("MainWindow/SubGeometry", this->_subWindow->saveGeometry());
    this->SaveSession();

    BookmarkViewContent *bmView = static_cast<BookmarkViewContent*>(this->_bookmarkView->ContainedWidget());
    bmView->CheckForChanges();

	QMainWindow::closeEvent(e);
}

void ComicDesktop::keyPressEvent(QKeyEvent *e)
{
    CommandEnums::KeyCommandType cmd = ComicUtils::CheckKeyCombination(e);

    if (cmd == CommandEnums::UNIDENTIFIED)
    {
        QMainWindow::keyPressEvent(e);
    }
    else
    {
        switch (cmd)
        {
        case CommandEnums::FULLSCREEN:
            ui.action_FullScreen->activate(QAction::Trigger);
            break;
        case CommandEnums::FINDINLIBRARY:
        {
            LibraryViewContent* content = static_cast<LibraryViewContent*>(this->_libraryView->ContainedWidget());
            content->FocusSearch();
            break;
        }
        case CommandEnums::FITALL:
            ui.action_FitAll->activate(QAction::Trigger);
            break;
        case CommandEnums::FITHEIGHT:
            ui.action_FitHeight->activate(QAction::Trigger);
            break;
        case CommandEnums::FITWIDTH:
            ui.action_FitWidth->activate(QAction::Trigger);
            break;
        case CommandEnums::LIBRARY:
            ui.action_Libraries->activate(QAction::Trigger);
            break;
        case CommandEnums::OPEN:
            ui.action_Open->activate(QAction::Trigger);
            break;
        case CommandEnums::PAGEBACKWARD:
            ui.action_PageBack->activate(QAction::Trigger);
            break;
        case CommandEnums::PAGEFORWARD:
            ui.action_PageFoward->activate(QAction::Trigger);
            break;
        case CommandEnums::TABCHANGE:
        {
            int index = ui.tabWidget->currentIndex();
            if (index >= 0)
            {
                index++;
                if (index > ui.tabWidget->count())
                {
                    index = 0;
                }
                ui.tabWidget->setCurrentIndex(index);
            }
            break;
        }
        case CommandEnums::QUIT:
            ui.action_Close->activate(QAction::Trigger);
            break;
        case CommandEnums::HOME:
            ui.action_Home->activate(QAction::Trigger);
            break;
        case CommandEnums::ONEPAGE:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->setView(ComicSeer::OnePage);
            break;
        case CommandEnums::TWOPAGE:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->setView(ComicSeer::TwoPage);
            break;
        case CommandEnums::FIRSTPAGE:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->FirstPage();
            break;
        case CommandEnums::LASTPAGE:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->LastPage();
            break;
        case CommandEnums::ZOOMIN:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->ZoomIn();
            break;
        case CommandEnums::ZOOMOUT:
            if (this->SelectedComicWidget() != NULL) this->SelectedComicWidget()->ZoomOut();
            break;
        case CommandEnums::RECENTLIST:
            this->RecentTriggered();
            break;
        case CommandEnums::OPTIONS:
            this->OptionsClicked();
            break;
        case CommandEnums::CLOSECOMIC:
        {
            int index = ui.tabWidget->currentIndex();
            if (index >= 0)
            {
                this->TabCloseRequested(index);
            }
            break;
        }
        }
    }
}

void ComicDesktop::ShowFullPathChanged(bool)
{
    this->SetComicInformation();
}

void ComicDesktop::ComicPageChanged(int page, bool secondVisible)
{
	this->SetComicInformation();
}
