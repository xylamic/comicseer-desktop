/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicViewWidget.h"
#include "qsettings.h"
#include "ComicUtils.h"
#include <QKeyEvent>
#include <qfileinfo.h>
#include <ComicInfoViewer.h>

using namespace ComicSeer;

ComicViewWidget::ComicViewWidget(const char* path, QWidget* parent)
	: QWidget(parent), _path(path)
{
    ui.setupUi(this);
    this->_fullView = NULL;
    this->_comicViewLayoutIndex = ui.horizontalLayout->indexOf(ui.graphicsView);

	int page = this->ReadPageSetting();

	ui.graphicsView->Initialize(path, page);
	ui.widgetBrowser->SetComic(path);
	ui.widgetBrowser->setVisible(false);

    // initialize view buttons
    ui.toolButton1->setChecked(ui.graphicsView->View() == OnePage);
    ui.toolButton2->setChecked(ui.graphicsView->View() != OnePage);

	this->connect(ui.graphicsView, SIGNAL(scaleChanged(double)), SLOT(ScaleChanged(double)));
    this->connect(ui.graphicsView, SIGNAL(ViewClosed()), SLOT(ComicViewClosed()));
	this->connect(ui.sliderZoom, SIGNAL(valueChanged(int)), SLOT(SlideValueChanged(int)));
	this->connect(ui.toolButtonBrowser, SIGNAL(toggled(bool)), SLOT(BrowseToggled(bool)));
	this->connect(ui.widgetBrowser, SIGNAL(PageSelected(int)), SLOT(BrowserPageSelected(int)));
	this->connect(ui.graphicsView, SIGNAL(UserPageChange()), SLOT(ViewUserPageChange()));
    this->connect(ui.toolButton1, SIGNAL(clicked()), SLOT(OnePageClicked()));
    this->connect(ui.toolButton2, SIGNAL(clicked()), SLOT(TwoPageClicked()));
    this->connect(ui.toolButtonRotateLeft, SIGNAL(clicked()), SLOT(RotateLeft()));
    this->connect(ui.toolButtonRotateRight, SIGNAL(clicked()), SLOT(RotateRight()));
    this->connect(ui.graphicsView, SIGNAL(ViewChanged(ComicSeer::ImageView)), SLOT(ComicViewChanged(ComicSeer::ImageView)));
    this->connect(ui.toolButtonMagnify, SIGNAL(toggled(bool)), SLOT(MagnifyToggled(bool)));
    this->connect(ui.graphicsView, SIGNAL(MagnificationChanged(bool)), SLOT(ComicViewMagnifyToggled(bool)));
    this->connect(ui.toolButtonInfo, SIGNAL(clicked()), SLOT(InformationClicked()));

	this->ScaleChanged(ui.graphicsView->ScaleFactor());

    this->setFocusProxy(this->ui.graphicsView);

    if (ui.graphicsView->View() == ComicSeer::TwoPage)
    {
        ui.toolButtonRotateLeft->setEnabled(false);
        ui.toolButtonRotateRight->setEnabled(false);
    }
}

int ComicViewWidget::ReadPageSetting()
{
    QString settingPath = QString::fromUtf8(_path.c_str());
	settingPath.replace(' ', '_').replace('/', '_').replace('\\', '_');

    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);

	QVariant pageSetting;
	pageSetting = setting.value(QString("PageMemory/%1").arg(settingPath), QVariant(static_cast<int>(0)));

	int page = pageSetting.toInt();

	return page;
}

void ComicViewWidget::UpdatePage()
{
	int page = ui.graphicsView->CurrentPage();

    QString settingPath = QString::fromUtf8(_path.c_str());
	settingPath.replace(' ', '_').replace('/', '_').replace('\\', '_');

    QSettings setting(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);

	QVariant value(page);
	setting.setValue(QString("PageMemory/%1").arg(settingPath), value);
}

void ComicViewWidget::ScaleChanged(double scale)
{
	int existingValue = ui.sliderZoom->value();

	int newValue = static_cast<int>(scale * 100);

	if (existingValue != newValue)
	{
        ui.sliderZoom->setValue(newValue);
	}
}

void ComicViewWidget::SlideValueChanged(int value)
{
    if (!this->isVisible())
    {
        return;
    }

	double scaleFactor = ui.sliderZoom->value() / 100.0;
    ui.graphicsView->SetScaleFactor(scaleFactor);
}

void ComicViewWidget::OnePageClicked()
{
    this->setView(ComicSeer::OnePage);
}

void ComicViewWidget::FullScreen()
{
    ui.graphicsView->hide();
    ui.graphicsView->setParent(NULL);
    ui.graphicsView->setWindowModality(Qt::ApplicationModal);
    ui.graphicsView->showFullScreen();
}

void ComicViewWidget::SetCurrentPage(int page)
{
    ui.graphicsView->SetPage(page);
}

void ComicViewWidget::TwoPageClicked()
{
    this->setView(ComicSeer::TwoPage);
}

void ComicViewWidget::BrowseToggled(bool checked)
{
    if (checked)
    {
        ui.widgetBrowser->LoadComic();
        ui.widgetBrowser->show();
        ui.widgetBrowser->SelectPage(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
    }
    else
    {
        ui.widgetBrowser->hide();
    }
}

int ComicViewWidget::NumberOfPages()
{
	return ui.graphicsView->NumberOfPages();
}

bool ComicViewWidget::PageForward()
{
	int currentPage = ui.graphicsView->CurrentPage();

	ui.graphicsView->PageForward();

	if (currentPage != ui.graphicsView->CurrentPage())
	{
        ui.widgetBrowser->SelectPage(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
		this->UpdatePage();
        emit PageChanged(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
		return true;
	}
	else
	{
		return false;
	}
}

bool ComicViewWidget::PageBack()
{
	int currentPage = ui.graphicsView->CurrentPage();

	ui.graphicsView->PageBack();

	if (currentPage != ui.graphicsView->CurrentPage())
	{
        ui.widgetBrowser->SelectPage(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
		this->UpdatePage();
        emit PageChanged(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
		return true;
	}
	else
	{
		return false;
	}
}

void ComicViewWidget::setScaleFactor(double value)
{
    ui.graphicsView->SetScaleFactor(value);
}

double ComicViewWidget::ScaleFactor()
{
	return ui.graphicsView->ScaleFactor();
}

void ComicViewWidget::setState(ComicSeer::ImageState state)
{
    ui.graphicsView->SetState(state);
}

ComicSeer::ImageState ComicViewWidget::State()
{
	return ui.graphicsView->State();
}

void ComicViewWidget::FirstPage()
{
    ui.graphicsView->FirstPage();
}

void ComicViewWidget::LastPage()
{
    ui.graphicsView->LastPage();
}

void ComicViewWidget::ZoomIn()
{
    ui.graphicsView->ZoomIn();
}

void ComicViewWidget::ZoomOut()
{
    ui.graphicsView->ZoomOut();
}

void ComicViewWidget::setView(ComicSeer::ImageView view)
{
    if (ui.graphicsView->View() != view)
    {
        ui.graphicsView->SetView(view);
    }

    if (view == ComicSeer::OnePage)
    {
        ui.toolButton1->setChecked(true);
        ui.toolButton2->setChecked(false);
        ui.toolButtonRotateLeft->setEnabled(true);
        ui.toolButtonRotateRight->setEnabled(true);
    }
    else
    {
        ui.toolButton1->setChecked(false);
        ui.toolButton2->setChecked(true);
        ui.toolButtonRotateLeft->setEnabled(false);
        ui.toolButtonRotateRight->setEnabled(false);
    }
}

ComicSeer::ImageView ComicViewWidget::View()
{
    return ui.graphicsView->View();
}

void ComicViewWidget::ComicViewChanged(ComicSeer::ImageView view)
{
    this->setView(view);
}

void ComicViewWidget::MagnifyToggled(bool checked)
{
    ui.graphicsView->SetMagnified(checked);
}

void ComicViewWidget::ComicViewMagnifyToggled(bool magnify)
{
    ui.toolButtonMagnify->setChecked(magnify);
}

void ComicViewWidget::RotateRight()
{
    QString key = QFileInfo(QString::fromUtf8(this->_path.c_str())).fileName();

    bool secondVisible;
    ComicPageTypes::PageRotationType rotation = ComicUtils::GetComicPageRotation(key, this->CurrentPage(secondVisible));
    int val = static_cast<int>(rotation);
    val += 1;
    if (val > 3)
        rotation = ComicPageTypes::NONE;
    else
        rotation = static_cast<ComicPageTypes::PageRotationType>(val);

    ComicUtils::SetComicPageRotation(key, this->CurrentPage(secondVisible), rotation);
    ui.graphicsView->SetPage(this->CurrentPage(secondVisible));
}

void ComicViewWidget::RotateLeft()
{
    QString key = QFileInfo(QString::fromUtf8(this->_path.c_str())).fileName();

    bool secondVisible;
    ComicPageTypes::PageRotationType rotation = ComicUtils::GetComicPageRotation(key, this->CurrentPage(secondVisible));
    int val = static_cast<int>(rotation);
    val -= 1;
    if (val < 0)
        rotation = ComicPageTypes::DEGREE270;
    else
        rotation = static_cast<ComicPageTypes::PageRotationType>(val);

    ComicUtils::SetComicPageRotation(key, this->CurrentPage(secondVisible), rotation);
    ui.graphicsView->SetPage(this->CurrentPage(secondVisible));
}

void ComicViewWidget::BrowserPageSelected(int page)
{
	ui.graphicsView->SetPage(page);
    ui.widgetBrowser->SelectPage(page, ui.graphicsView->SecondPageVisible());
	this->UpdatePage();
    emit PageChanged(page, ui.graphicsView->SecondPageVisible());
}

void ComicViewWidget::ViewUserPageChange()
{
	UpdatePage();
    ui.widgetBrowser->SelectPage(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
    emit PageChanged(ui.graphicsView->CurrentPage(), ui.graphicsView->SecondPageVisible());
}

void ComicViewWidget::ComicViewClosed()
{
    ui.graphicsView->setParent(this);
    ui.horizontalLayout->insertWidget(this->_comicViewLayoutIndex, ui.graphicsView);
    ui.graphicsView->setWindowModality(Qt::NonModal);
    ui.graphicsView->showNormal();
}

int ComicViewWidget::CurrentPage(bool& secondVisible)
{
    secondVisible = ui.graphicsView->SecondPageVisible();
	return ui.graphicsView->CurrentPage();
}

std::string ComicViewWidget::Path()
{
	return this->_path;
}

void ComicViewWidget::InformationClicked()
{
    ComicInfoViewer::OpenComicInfoViewer(ComicUtils::StringToQString(this->_path));
}
