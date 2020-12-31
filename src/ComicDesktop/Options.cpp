/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Options.h"
#include <ComicState.h>
#include <ComicUtils.h>
#include <qcolor.h>
#include <qmessagebox.h>

using namespace ComicSeer;

Options::Options(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    this->connect(ui.comboBoxImageState, SIGNAL(currentIndexChanged(int)), SLOT(DefaultViewStateChanged(int)));
    this->connect(ui.buttonOk, SIGNAL(clicked()), SLOT(accept()));
    this->connect(ui.buttonCancel, SIGNAL(clicked()), SLOT(reject()));
    this->connect(ui.buttonRestoreDefaults, SIGNAL(clicked()), SLOT(RestoreDefaults()));

    this->Load();
}

void Options::Load()
{
    ComicState* state = ComicState::State();

    ui.comboBoxImageState->setCurrentIndex(static_cast<int>(state->DefaultImageState()));
    ui.comboBoxImageView->setCurrentIndex(static_cast<int>(state->DefaultImageView()));
    ui.spinBoxTwoPageMove->setValue(state->TwoPageMoveNumber());
    ui.widgetColor->SetColor(QColor(state->BackgroundColor().c_str()));
    ui.checkBoxAutodetectWide->setChecked(state->TwoPageAutodetectWidePage());
    ui.spinBoxThumbnailSize->setValue(state->ThumbnailSize());
    ui.checkBoxReopenSession->setChecked(state->ReopenLastSession());
    ui.chkShowPageNumber->setChecked(state->ShowPageNumbers());
    ui.cboPageLocation->setCurrentIndex(static_cast<int>(state->PageNumberLocation()));
    ui.widgetPageColor->SetColor(QColor(state->PageNumberColor().c_str()));
    ui.checkBoxShowPath->setChecked(state->ShowFullFilePath());
    ui.sliderScaledZoom->setValue(static_cast<int>(state->ScaledZoomFactor()));
    ui.checkBoxAllowMagnification->setChecked(state->AllowMagnification());
}

void Options::accept()
{
    ComicState* state = ComicState::State();

    state->SetDefaultImageState(static_cast<ImageState>(ui.comboBoxImageState->currentIndex()));
    state->SetDefaultImageView(static_cast<ImageView>(ui.comboBoxImageView->currentIndex()));
    state->SetTwoPageMoveNumber(ui.spinBoxTwoPageMove->value());
    state->SetBackgroundColor(ui.widgetColor->Color().name().toUtf8());
    state->SetTwoPageAutodetectWidePage(ui.checkBoxAutodetectWide->isChecked());
    state->SetThumbnailSize(ui.spinBoxThumbnailSize->value());
    state->SetReopenLastSession(ui.checkBoxReopenSession->isChecked());
    state->SetPageNumberSettings(ui.chkShowPageNumber->isChecked(),
                                 static_cast<PageNumberLocation>(ui.cboPageLocation->currentIndex()),
                                 ui.widgetPageColor->Color().name().toUtf8());
    state->SetShowFullFilePath(ui.checkBoxShowPath->isChecked());
    state->SetScaledZoom(ui.sliderScaledZoom->value());
    state->SetAllowMagnification(ui.checkBoxAllowMagnification->isChecked());

    ComicUtils::StoreState();

    QDialog::accept();
}

void Options::DefaultViewStateChanged(int index)
{
    ImageState state = static_cast<ImageState>(index);
    if (state == Manual)
    {
        ui.sliderScaledZoom->setEnabled(true);
    }
    else
    {
        ui.sliderScaledZoom->setEnabled(false);
    }
}

void Options::RestoreDefaults()
{
    QMessageBox msg(QMessageBox::Question,
                    tr("Restore defaults"),
                    tr("Are you sure you want to restore defaults?"),
                    QMessageBox::Yes | QMessageBox::No);
    if (msg.exec() != QMessageBox::Yes)
    {
        return;
    }

    ComicState::State()->Reinitialize(false);
    ComicUtils::StoreState();
    this->Load();
}
