/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef X_RECENTCOMICMENU_H
#define X_RECENTCOMICMENU_H

#include <Xylasoft.hpp>
#include <QtWidgets/qmenu.h>
#include <qsignalmapper.h>
#include <ComicState.h>

/*!
This is a menu that always contains information about the recent comic list, including
the persistence of it to load on the next instance.
*/
class RecentComicMenu : public QMenu
{
	Q_OBJECT

public:
	/*!
	Construct the menu.
	@param[in] parent The parent widget.
	*/
	RecentComicMenu(QWidget* parent = nullptr);

	/*!
	Add a comic to the recent list.
	@param[in] path The path for the recent list.
	*/
	void AddComic(std::string path);

signals:
	/*!
	Signal that a recent was requested for opening.
	@param[in] path The path to the comic requested.
	*/
	void RecentClicked(std::string path);

protected slots:
	/*!
	One of the recent items where triggered.
	@param[in] map The mapped text.
	*/
	void Triggered(const QString& map);

protected:
	/*!
	Build the recent menu and clear any preivous items.
	*/
	void BuildRecentMenu();

    /*!
    @brief Method for receiving the change in full path display signal.
    @param show Whether to show the full path.
    */
    void ShowFullPathChanged(bool show);

private:
    boost::signals2::scoped_connection _slotShowPathUpdated;
	QStringList _recentList;
	QSignalMapper* _mapper;
};

#endif
