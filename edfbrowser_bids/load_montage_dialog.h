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




#ifndef LOADMONTAGEFORM1_H
#define LOADMONTAGEFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"
#include "utc_date_time.h"
#include "utils.h"
#include "spectrum_dock.h"
#include "edf_helper.h"

#include "filt/filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"
#include "filt/spike_filter.h"

#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;


class UI_LoadMontagewindow : public QObject
{
  Q_OBJECT

public:
  UI_LoadMontagewindow(QWidget *parent, char *path = NULL);

  UI_Mainwindow *mainwindow;


private:

  QDialog      *LoadMontageDialog;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *LoadButton;

  char mtg_path[MAX_PATH_LENGTH];

  int format_error(const char *, int , sigcompblck_t *, xml_hdl_t *);

private slots:

  int LoadButtonClicked();

};



#endif // LOADMONTAGEFORM1_H


