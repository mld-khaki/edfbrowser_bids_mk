/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2024 Teunis van Beelen
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




#ifndef ECGEXPORTCLASS_H
#define ECGEXPORTCLASS_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "viewcurve.h"
#include "utils.h"
#include "filtered_block_read.h"
#include "edf_annot_list.h"




class UI_Mainwindow;
class ViewCurve;



class UI_ECGExport : public QObject
{
  Q_OBJECT

public:
  UI_ECGExport(QWidget *parent);

  UI_Mainwindow *mainwindow;


private:

QDialog      *myobjectDialog;

QListWidget  *list;

QPushButton  *startButton,
             *cancelButton,
             *helpButton;

QGroupBox    *groupBox1;

QVBoxLayout  *vbox1;

QRadioButton *radioButton1,
             *radioButton2,
             *radioButton3;

QCheckBox    *checkBox1,
             *checkBox2;

void load_signalcomps(void);

private slots:

void Export_RR_intervals();
void helpbuttonpressed();

};



#endif




