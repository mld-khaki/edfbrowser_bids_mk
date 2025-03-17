/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2023 - 2024 Teunis van Beelen
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


#ifndef UI_CARDIOTRAK2EDFFORM_H
#define UI_CARDIOTRAK2EDFFORM_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "qt_headers.h"

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edflib.h"



class UI_Mainwindow;


class UI_CardiotrakEDFwindow : public QObject
{
  Q_OBJECT

  public:

  UI_CardiotrakEDFwindow(QWidget *, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow  *mainwindow;

  private:

  QDialog *ct2edf_dialog;

  QPlainTextEdit *logger;

  QPushButton *select_file_button,
              *close_button;

  char *recent_opendir,
       *recent_savedir;

  void logger_append_txt(const char *);

  private slots:

  void select_file_button_pressed();
};




#endif


