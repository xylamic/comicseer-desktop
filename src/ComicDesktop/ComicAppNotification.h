/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICAPPNOTIFICATION_H
#define COMICAPPNOTIFICATION_H

#include <QtWidgets/QDialog>

namespace Ui {
class ComicAppNotification;
}

/*!
The ComicAppNotification class provides a notification to applicable users on
using the new Comic Seer App.
*/
class ComicAppNotification : public QDialog
{
    Q_OBJECT

public:
    /*!
    @brief Construct the dialog.
    @param[in] parent The parent widget
    */
    explicit ComicAppNotification(QWidget *parent = 0);
    ~ComicAppNotification();

    /*!
    @brief Show the notification dialog if needed.
    @param[in] parent The parent widget
    */
    static bool TryShowDialog(QWidget *parent);

private slots:
    /*!
    @brief Navigate to the Comic Seer App.
    */
    void on_btnGo_clicked();

    /*!
    @brief Close and don't remind about the app.
    */
    void on_btnDontRemind_clicked();

    /*!
    @brief Close and remind me about the app.
    */
    void on_btnRemindMe_clicked();

private:
    Ui::ComicAppNotification *ui;
};

#endif // COMICAPPNOTIFICATION_H
