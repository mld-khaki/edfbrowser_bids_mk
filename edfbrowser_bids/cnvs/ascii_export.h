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




#ifndef ASCIIEXPORTFORM1_H
#define ASCIIEXPORTFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"




class UI_Mainwindow;



class UI_AsciiExportwindow : public QObject
{
  Q_OBJECT

public:
  UI_AsciiExportwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *AsciiExportDialog;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *ExportButton;


struct asciiedfparamblock
{
  int smp_per_record;
  int smp_written;
  int dig_min;
  int dig_max;
  double offset;
  int buf_offset;
  double phys_min;
  double phys_max;
  long long time_step;
  double sense;
} *edfparamascii;


private slots:

  void ExportButtonClicked();

};



#endif // ASCIIEXPORTFORM1_H


