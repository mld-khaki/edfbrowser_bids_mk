/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 - 2024 Teunis van Beelen
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




#ifndef VIEWSESSIONFORM1_H
#define VIEWSESSIONFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"
#include "utils.h"

#include "filt/filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"
#include "filt/spike_filter.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;



class UI_ViewSessionwindow : public QObject
{
  Q_OBJECT

public:
  UI_ViewSessionwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;


private:

  QDialog      *ViewSessionDialog;

  QPushButton  *CloseButton,
               *SelectButton;

  QBoxLayout   *box;

  QHBoxLayout  *hbox;

  QTreeView    *tree;

  QStandardItemModel *t_model;

  char session_path[MAX_PATH_LENGTH],
       session_dir[MAX_PATH_LENGTH];

int view_session_format_error(const char *, int, xml_hdl_t *);

private slots:

  void SelectButtonClicked();

};



#endif // VIEWSESSIONFORM1_H


