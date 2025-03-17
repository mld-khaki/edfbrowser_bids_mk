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




#ifndef STATISTICS_DIALOGFORM1_H
#define STATISTICS_DIALOGFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "signalcurve.h"
#include "edf_annot_list.h"
#include "ecg_statistics.h"
#include "utils.h"

#include "filt/ecg_filter.h"


class UI_Mainwindow;


class UI_StatisticWindow : public QObject
{
  Q_OBJECT

public:

UI_StatisticWindow(sigcompblck_t *,
                   long long,
                   QWidget *,
                   annotlist_t *annot_list=NULL,
                   annotblck_t *annot=NULL);

UI_Mainwindow *mainwindow;


private:

QDialog     *StatDialog;

QHBoxLayout *hlayout1,
            *hlayout1_1_1;

QVBoxLayout *vlayout1_1,
            *vlayout2_1;

QSlider     *startSlider,
            *stopSlider;

QLabel      *Label1,
            *startLabel,
            *stopLabel;

QPushButton  *pushButton1;

SignalCurve  *curve1;

int beat_cnt,
    bpm_distribution[300],
    max_val,
    start_ruler,
    end_ruler;

ecg_hr_statistics_t hr_stat;


private slots:

void startSliderMoved(int);
void stopSliderMoved(int);

};



#endif // STATISTICS_DIALOGFORM1_H


