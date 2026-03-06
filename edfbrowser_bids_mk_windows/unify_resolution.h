/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2021 - 2024 Teunis van Beelen
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




#ifndef UNIFYRESOLUTIONFORM1_H
#define UNIFYRESOLUTIONFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "active_file_chooser.h"

class UI_Mainwindow;

class UI_unify_resolution : public QObject
{
  Q_OBJECT

public:
  UI_unify_resolution(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

int file_num;

char  inputpath[MAX_PATH_LENGTH],
      outputpath[MAX_PATH_LENGTH],
      *recent_savedir;

edfhdrblck_t *edfhdr;

  QDialog      *myobjectDialog;

  QLabel       *file_path_label;

  QTableWidget *signals_list;

  QPushButton  *select_button,
               *deselect_button,
               *select_file_button,
               *CloseButton,
               *SaveButton,
               *helpButton;

  QDoubleSpinBox *phys_max_spinbox;

private slots:

  void SaveButtonClicked();
  void select_button_clicked();
  void deselect_button_clicked();
  void select_file_button_clicked();
  void helpbuttonpressed();

};



#endif




