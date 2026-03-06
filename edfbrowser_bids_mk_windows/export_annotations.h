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




#ifndef EXPORTANNOTATIONSFORM1_H
#define EXPORTANNOTATIONSFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edflib.h"
#include "utc_date_time.h"
#include "edf_annot_list.h"
#include "utils.h"



class UI_Mainwindow;



class UI_ExportAnnotationswindow : public QObject
{
  Q_OBJECT

public:
  UI_ExportAnnotationswindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *ExportAnnotsDialog;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *ExportButton;

  QRadioButton *CSVRadioButton,
               *EDFplusRadioButton,
               *XMLRadioButton,
               *mergeRadioButton,
               *selectRadioButton,
               *asciiSecondsRadioButton,
               *asciiISOtimeRadioButton,
               *asciiISOtimedateRadioButton,
               *asciiISOtimeFractionRadioButton,
               *asciiISOtimedateFractionRadioButton;

  QGroupBox    *formatGroupBox,
               *fileGroupBox,
               *asciiSettingsGroupBox;

  QComboBox    *separatorBox,
               *text_encoding_combobox;

  QCheckBox    *durationCheckBox;

private slots:

  void ExportButtonClicked();
  void outputformatRadioButtonClicked(bool);

};



#endif




