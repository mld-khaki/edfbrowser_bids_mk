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




#ifndef IMPORTANNOTATIONSFORM1_H
#define IMPORTANNOTATIONSFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utc_date_time.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"
#include "xml.h"
#include "edflib.h"
#include "check_edf_file.h"
#include "edf_annotations.h"



class UI_Mainwindow;



class UI_ImportAnnotationswindow : public QObject
{
  Q_OBJECT

public:

UI_ImportAnnotationswindow(QWidget *);

UI_Mainwindow  *mainwindow;

private:

int mal_formatted_lines,
    mal_formatted_line_nrs[32];

QDialog        *ImportAnnotsDialog;

QTabWidget     *tabholder;

QWidget        *tab[5];

QLabel         *DCEventSignalLabel,
               *DCEventBitTimeLabel,
               *DCEventTriggerLevelLabel,
               *SampleTimeLabel,
               *importStandardLabel,
               *importAuxLabel;

QLineEdit      *SeparatorLineEdit,
               *DescriptionLineEdit;

QSpinBox       *OnsetColumnSpinBox,
               *DurationColumnSpinBox,
               *StopColumnSpinBox,
               *DescriptionColumnSpinBox,
               *DatastartSpinbox,
               *BitTimeSpinbox,
               *SampleTimeSpinbox;

QDoubleSpinBox *DCEventTriggerLevelSpinBox;

QComboBox      *RelativeTimeComboBox,
               *text_encoding_combobox,
               *DCEventSignalComboBox;

QRadioButton   *DescriptionColumnRadioButton,
               *UseManualDescriptionRadioButton;

QCheckBox      *IgnoreConsecutiveCheckBox,
               *DurationCheckBox,
               *StopTimeCheckBox,
               *importStandardCheckBox,
               *importAuxCheckBox,
               *equalFilenameCheckBox;

QPushButton    *CloseButton,
               *ImportButton,
               *helpButton;

int tab_index_array[5];

int import_from_xml(void);
int import_from_ascii(void);
int import_from_edfplus(void);
int import_from_dcevent(void);
int import_from_mitwfdb(void);
int get_samplefreq_inf(void);

int get_onset_time_from_ascii(const char *, long long *, long long *, int);

private slots:

void ImportButtonClicked();
void descriptionRadioButtonClicked(bool);
void DCEventSignalChanged(int);
void DurationCheckBoxChanged(int);
void StopTimeCheckBoxChanged(int);
void equalFilenameCheckBoxChanged(int);
void TabChanged(int);
void helpbuttonpressed();

};



#endif




