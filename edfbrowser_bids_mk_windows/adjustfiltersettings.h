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




#ifndef ADJUSTFILTERSETTINGS_H
#define ADJUSTFILTERSETTINGS_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "viewcurve.h"
#include "utils.h"

#include "filt/ravg_filter.h"

#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;
class ViewCurve;



class AdjustFilterSettings : public QObject
{
  Q_OBJECT

public:

AdjustFilterSettings(sigcompblck_t *, QWidget *);


private:

int filter_nr,
    filter_cnt,
    type,
    model,
    order,
    size,
    brand[MAXFILTERS * 2];

double frequency1,
       frequency2,
       ripple;

sigcompblck_t * signalcomp;

UI_Mainwindow  *mainwindow;

ViewCurve      *maincurve;

QFormLayout    *flayout;

QDialog        *filtersettings_dialog;

QComboBox      *filterbox,
               *stepsizebox;

QSpinBox       *orderbox;

QDoubleSpinBox *freq1box,
               *freq2box;

QPushButton    *CloseButton,
               *RemoveButton;

void update_filter(void);
void loadFilterSettings(void);


private slots:

void freqbox1valuechanged(double);
void freqbox2valuechanged(double);
void orderboxvaluechanged(int);
void stepsizeboxchanged(int);
void filterboxchanged(int);
void removeButtonClicked();

};



#endif // ADJUSTFILTERSETTINGS_H


