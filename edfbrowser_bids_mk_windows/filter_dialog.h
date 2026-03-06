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




#ifndef ADD_FILTERFORM1_H
#define ADD_FILTERFORM1_H



#include "qt_headers.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "filtercurve.h"
#include "utils.h"

#include "filt/filter.h"
#include "filt/ravg_filter.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;



class UI_FilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_FilterDialog(QWidget *parent=0);
  ~UI_FilterDialog();

  UI_Mainwindow *mainwindow;


private:

QDialog        *filterdialog;

QPushButton    *CancelButton,
               *ApplyButton;

QListWidget    *signals_list;

QComboBox      *typebox,
               *modelbox;

QDoubleSpinBox *freq1box,
               *ripplebox,
               *freq2box;

QSpinBox       *orderbox;

QLabel         *typeboxlabel,
               *freqbox1label,
               *freqbox2label,
               *orderboxlabel,
               *modelboxlabel,
               *listlabel,
               *orderlabel,
               *ordervaluelabel;

FilterCurve    *curve1;

double *array,
       *array_pha,
       last_ripple;

int last_order,
    last_qfactor,
    last_model,
    last_samples;

void updatecurve(void);

char *filter_spec,
     spec_str_1[256],
     spec_str_2[256];


private slots:

void ApplyButtonClicked();
void freq1boxvaluechanged(double);
void orderboxvaluechanged(int);
void filtertypeboxvaluechanged(int);
void filtermodelboxvaluechanged(int);
void rippleboxvaluechanged(double);
void freq2boxvaluechanged(double);
void selected_signals_changed();

};



#endif // ADD_FILTERFORM1_H


