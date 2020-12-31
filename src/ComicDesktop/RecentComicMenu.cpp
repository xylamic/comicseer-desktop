/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "RecentComicMenu.h"
#include <qsettings.h>
#include "ComicUtils.h"
#include <algorithm>
#include <qfileinfo.h>
#include <QtWidgets/qmessagebox.h>

using namespace std;

RecentComicMenu::RecentComicMenu(QWidget* parent /* = nullptr */)
    : QMenu(parent)
{
	// create mapper
	this->_mapper = new QSignalMapper(this);
	connect(this->_mapper, SIGNAL(mapped(const QString &)), this, SLOT(Triggered(const QString &)));

	// load the recent list
	QSettings settings(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
	QVariant recent = settings.value("RecentComics", QVariant(QStringList()));
	_recentList = recent.toStringList();

	this->BuildRecentMenu();

    // set the full path updated connection
    this->_slotShowPathUpdated = ComicState::State()->ShowFullPathUpdated.connect([this](bool show) { this->ShowFullPathChanged(show); });
}

void RecentComicMenu::BuildRecentMenu()
{
	QList<QAction*> actions = this->actions();
	for_each(begin(actions), end(actions), [this](QAction* action)
	{
		this->removeAction(action);
		delete action;
	});

	if (this->_recentList.size() == 0)
	{
		QAction* action = this->addAction(QIcon(":/ComicDesktop/Resources/Check.png"), tr("No recent items"));
		action->setEnabled(false);
	}
	else
	{
		for_each(begin(this->_recentList), end(this->_recentList), [this](QString path)
		{
            QAction* action = this->addAction(QIcon(":/ComicDesktop/Resources/Check.png"),
                                              ComicUtils::GetVisibleComicPath(path));
            action->setData(path);
            this->_mapper->connect(action, SIGNAL(triggered()), SLOT(map()));
			this->_mapper->setMapping(action, path);
		});
    }
}

void RecentComicMenu::ShowFullPathChanged(bool show)
{
    this->BuildRecentMenu();
}

void RecentComicMenu::Triggered(const QString& map)
{
    // get the file information
    QFileInfo info(map);

    // if the file does not exist, ask to remove it
    if (!info.exists() || !info.isFile())
    {
        QMessageBox msg;
        msg.setText(tr("The file no longer exists."));
        msg.setInformativeText(tr("Would you like to remove from the recent list?"));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setDefaultButton(QMessageBox::Yes);
        msg.setIcon(QMessageBox::Warning);
        msg.setWindowTitle(tr("Open"));

        if (msg.exec() == QMessageBox::Yes)
        {
            this->_recentList.removeOne(map);

            // save the new list
            QSettings settings(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
            settings.setValue("RecentComics", this->_recentList);

            // build the new menu
            this->BuildRecentMenu();
        }

        return;
    }
    // else, open it
    else
    {
        emit RecentClicked(string(map.toUtf8()));
    }
}

void RecentComicMenu::AddComic(std::string path)
{
	// add the new item to the front of the list and delete all others
    this->_recentList.removeAll(QString::fromUtf8(path.c_str()));
    this->_recentList.insert(0, QString::fromUtf8(path.c_str()));

	// chop down to 10 items max
	while (this->_recentList.size() > 10)
	{
		this->_recentList.removeLast();
	}

	// save the new list
	QSettings settings(QSettings::UserScope, ComicUtils::CompanyName, ComicUtils::ApplicationName);
	settings.setValue("RecentComics", this->_recentList);

	// build the new menu
	this->BuildRecentMenu();
}
