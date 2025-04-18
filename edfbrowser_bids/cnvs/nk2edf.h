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


#ifndef UI_NK2EDFFORM_H
#define UI_NK2EDFFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"


class UI_Mainwindow;


class UI_NK2EDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_NK2EDFwindow(QWidget *parent, char *recent_dir=NULL);

  UI_Mainwindow *mainwindow;

private:


QPushButton  *pushButton1,
             *pushButton2;

QTextEdit    *textEdit1;

QDialog      *myobjectDialog;

QCheckBox    *checkBox1;

int total_elapsed_time;

char  *recent_opendir,
      labels[256][17];

int check_device(char *);

int read_21e_file(char *);

private slots:

void SelectFileButton();
int convert_nk2edf(FILE *, FILE *, FILE *, int, int, int, char *, int);

};




#endif


