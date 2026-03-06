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


#ifndef UI_RENAMEANNOTSFORM_H
#define UI_RENAMEANNOTSFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edf_annot_list.h"


class UI_Mainwindow;


class UI_rename_annots_dialog : public QObject
{
  Q_OBJECT

public:
  UI_rename_annots_dialog(QWidget *, int);

private:

  UI_Mainwindow  *mainwindow;

  int replacements_cnt,
      delete_annots;

  QDialog *rename_dialog;

  QLineEdit *line_edit1,
            *line_edit2;

  QPushButton *cancel_button,
              *rename_button;

private slots:

  void rename_all_func();
  void delete_all_func();

};

#endif










