/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008 - 2024 Teunis van Beelen
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


#ifndef UI_ASCII2EDFFORM_H
#define UI_ASCII2EDFFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "xml.h"
#include "utils.h"
#include "mainwindow.h"



class UI_Mainwindow;



class UI_ASCII2EDFapp : public QObject
{
  Q_OBJECT

public:
  UI_ASCII2EDFapp(QWidget *w_parent, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow *mainwindow;

private:

QDialog       *ascii2edfDialog;

QRadioButton  *edfButton,
              *bdfButton;

QGroupBox     *groupbox1;

QLineEdit     *SeparatorLineEdit,
              *PatientnameLineEdit,
              *RecordingLineEdit;

QSpinBox      *NumcolumnsSpinbox,
              *DatastartSpinbox;

QDoubleSpinBox *SamplefreqSpinbox;

QDateTimeEdit *StartDatetimeedit;

QCheckBox     *autoPhysicalMaximumCheckbox;

QTableWidget  *SignalsTablewidget;

QPushButton   *GoButton,
              *CloseButton,
              *SaveButton,
              *LoadButton,
              *helpButton,
              *setAllButton,
              *setAllCheckedButton,
              *setAllUncheckedButton;

char separator,
     *recent_opendir,
     *recent_savedir;

int edfsignals,
    startline,
    columns,
    column_enabled[MAXSIGNALS],
    autoPhysicalMaximum,
    edf_format;

double samplefrequency,
       datrecduration,
       sensitivity[MAXSIGNALS],
       value[MAXSIGNALS],
       physmax[MAXSIGNALS];


int check_input(void);

private slots:

void numofcolumnschanged(int);
void gobuttonpressed();
void savebuttonpressed();
void loadbuttonpressed();
void helpbuttonpressed();
void setallbuttonpressed();
void setallcheckedbuttonpressed();
void setalluncheckedbuttonpressed();
void autoPhysicalMaximumCheckboxChanged(int);
void signalCheckboxChanged(int);
};




#endif


