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


#ifndef UI_BIOSEMI2BDFFORM_H
#define UI_BIOSEMI2BDFFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edflib.h"
#include "edf_annot_list.h"
#include "utils.h"



class UI_Mainwindow;



class UI_BIOSEMI2BDFPLUSwindow : public QObject
{
  Q_OBJECT

public:

  UI_BIOSEMI2BDFPLUSwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

QLineEdit    *lineEdit1[16];

QRadioButton *radioButton1,
             *radioButton2;

QCheckBox    *checkBox1;

QPushButton  *selectButton,
             *closeButton;


QDialog      *myobjectDialog;


char  inputpath[MAX_PATH_LENGTH],
      outputpath[MAX_PATH_LENGTH],
      *recent_opendir,
      *recent_savedir;


private slots:

void SelectFileButton();

};




#endif


