/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015 - 2024 Teunis van Beelen
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


#ifndef UI_FMA2EDFFORM_H
#define UI_FMA2EDFFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edflib.h"

#include "../filt/ravg_filter.h"


class UI_Mainwindow;


class UI_FMaudio2EDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_FMaudio2EDFwindow(QWidget *, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow  *mainwindow;

private:

QDialog       *myobjectDialog;

QPushButton   *pushButton1,
              *pushButton2,
              *helpButton;

QLineEdit     *PatientnameLineEdit,
              *RecordingLineEdit;

QDateTimeEdit *StartDatetimeedit;

char *recent_opendir,
     *recent_savedir;

ravgfiltset_t *hpf44,
              *lpf9a,
              *lpf9b,
              *lpf9c,
              *lpf9d,
              *lpf9e,
              *lpf200a,
              *lpf200b,
              *lpf200c,
              *lpf200d,
              *lpf200e;

void enable_widgets(bool);

bool allocateFilters(int, int, int);
void deleteFilters(void);

private slots:

void SelectFileButton();
void helpbuttonpressed();

};




#endif


