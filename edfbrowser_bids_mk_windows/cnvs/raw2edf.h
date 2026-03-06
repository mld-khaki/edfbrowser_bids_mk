/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012 - 2024 Teunis van Beelen
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


#ifndef UI_RAW2EDFFORM_H
#define UI_RAW2EDFFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xml.h"
#include "utils.h"
#include "edflib.h"
#include "global.h"
#include "utils.h"

typedef struct
{
  int sf;
  int chns;
  int phys_max;
  int straightbinary;
  int endianness;
  int samplesize;
  int offset;
  int skipblocksize;
  int skipbytes;
  char phys_dim[16];
} raw2edf_var_t;

#include "mainwindow.h"


class UI_Mainwindow;


class UI_RAW2EDFapp : public QObject
{
  Q_OBJECT

public:
  UI_RAW2EDFapp(QWidget *w_parent, raw2edf_var_t *, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow *mainwindow;

private:

QDialog       *raw2edfDialog;

QLineEdit     *PatientnameLineEdit,
              *RecordingLineEdit,
              *PhysicalDimensionLineEdit,
              *variableTypeLineEdit;

QSpinBox      *SignalsSpinbox,
              *OffsetSpinbox,
              *SamplefreqSpinbox,
              *skipblocksizeSpinbox,
              *skipbytesSpinbox,
              *PhysicalMaximumSpinbox;

QComboBox     *EncodingCombobox,
              *EndiannessCombobox,
              *SampleSizeCombobox;

QDateTimeEdit *StartDatetimeedit;

QPushButton   *GoButton,
              *CloseButton,
              *SaveButton,
              *LoadButton,
              *helpButton;

char *recent_opendir,
     *recent_savedir;

int edfsignals,
    offset,
    samplefrequency;

raw2edf_var_t *raw2edf_var;

private slots:

void gobuttonpressed();
void savebuttonpressed();
void loadbuttonpressed();
void PhysicalDimensionLineEdited(QString);
void sampleTypeChanged(int);
void helpbuttonpressed();

};




#endif


