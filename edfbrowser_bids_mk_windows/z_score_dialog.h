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


#ifndef ZSCORE_FORM1_H
#define ZSCORE_FORM1_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "filtered_block_read.h"
#include "signalcurve.h"
#include "annotations_dock.h"

#include "filt/z_ratio_filter.h"

#include "third_party/kiss_fft/kiss_fftr.h"


class UI_Mainwindow;


class UI_ZScoreWindow : public QObject
{
  Q_OBJECT

public:
  UI_ZScoreWindow(QWidget *, UI_ZScoreWindow **, int, int);
  ~UI_ZScoreWindow();

  UI_Mainwindow *mainwindow;

  QDialog     *zscore_dialog;

private:

QPushButton *CloseButton,
            *StartButton,
            *get_annotButton,
            *defaultButton,
            *jumpButton,
            *addTraceButton;

QSpinBox    *pagelenSpinbox;

QDoubleSpinBox *thresholdSpinbox,
               *crossoverSpinbox,
               *hysteresisSpinbox;

QRadioButton *epochRadioButton,
             *pageRadioButton,
             *wakesleepRadioButton;

SignalCurve *curve1;

UI_ZScoreWindow **zscoredialog;

QAction     *shift_page_right_Act,
            *shift_page_left_Act;

int signalnr,
    zscore_page_len,
    avg_majority,
    epoch_cntr,
    zscore_pages,
    zscore_wake,
    *zscore_sleepstage_buf,
    *zscore_error_buf,
    plot_type,
    zscore_dialog_is_destroyed,
    zscore_dialognumber,
    zscore_error_detection;

double *zscore_epoch_buf,
       *zscore_page_buf,
       crossoverfreq,
       z_threshold,
       z_hysteresis;


private slots:

void startButtonClicked();
void get_annotationsButtonClicked();
void jumpButtonClicked();
void ZscoreDialogDestroyed(QObject *);
void RadioButtonsClicked(bool);
void defaultButtonClicked();
void markersMoved();
void addTraceButtonClicked();
void shift_page_right();
void shift_page_left();

};



#endif // ZSCORE_FORM1_H


