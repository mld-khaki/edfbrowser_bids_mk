/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2024 Teunis van Beelen
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




#ifndef ACTIVEFILESAVERFORM1_H
#define ACTIVEFILESAVERFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Mainwindow;



class UI_activeFileChooserWindow : public QObject
{
  Q_OBJECT

public:
  UI_activeFileChooserWindow(int *, QWidget *);

private:

QDialog      *chooserDialog;

QPushButton  *okButton,
             *cancelButton;

QListWidget  *filelist;

int *file_num;

UI_Mainwindow *mainwindow;


private slots:

void okButtonClicked();

};



#endif


