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




#ifndef EDFCOMPATFORM1_H
#define EDFCOMPATFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Mainwindow;



class UI_EDFCompatwindow : public QObject
{
  Q_OBJECT

public:
  UI_EDFCompatwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *EDFCompatDialog;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *CheckButton;

  int is_duration_number(char *);
  int is_onset_number(char *);
  int is_integer_number(char *);
  int is_number(char *);

private slots:

  void CheckButtonClicked();
  void CheckButtonClicked_prv();

};
int is_onset_number_free(char *str);
int is_duration_number_free(char *str);
int check_edf_compatibility(edfhdrblck_t *hdr, FILE *inputfile, char *errmsg, int errmsg_len);


#endif // EDFCOMPATFORM1_H


