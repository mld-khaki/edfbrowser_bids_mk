/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2024 Teunis van Beelen
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


#ifndef UI_EDFD2EDFCFORM_H
#define UI_EDFD2EDFCFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "check_edf_file.h"
#include "edf_annotations.h"
#include "utils.h"
#include "edf_annot_list.h"


class UI_Mainwindow;


class UI_EDFDwindow : public QObject
{
  Q_OBJECT

public:

  UI_EDFDwindow(QWidget *parent, char *, char *);

  UI_Mainwindow *mainwindow;

private:

QPushButton  *pushButton1,
             *pushButton2;

QTextEdit    *textEdit1;

QDialog      *myobjectDialog;

char *recent_opendir,
     *recent_savedir;

long long get_datarecord_timestamp(char *);
void write_values_to_hdr(FILE *, long long, int, edfhdrblck_t *);

private slots:

void SelectFileButton();

};




#endif


