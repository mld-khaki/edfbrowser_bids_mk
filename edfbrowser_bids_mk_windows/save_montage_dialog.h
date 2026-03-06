/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2024 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/




#ifndef SAVEMONTAGEFORM1_H
#define SAVEMONTAGEFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"
#include "spectrum_dock.h"
#include "utils.h"

#include "filt/filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/math_func.h"


class UI_Mainwindow;


class UI_SaveMontagewindow : public QObject
{
  Q_OBJECT

public:
  UI_SaveMontagewindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *SaveMontageDialog;

  QListWidget  *filelist;

  QRadioButton *label_radio_button,
               *index_radio_button;

  QGroupBox    *radio_group_box;

  QPushButton  *CloseButton,
               *SaveButton;


private slots:

  void SaveButtonClicked();

};



#endif // SAVEMONTAGEFORM1_H


