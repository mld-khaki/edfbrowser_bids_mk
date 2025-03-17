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




#ifndef AVERAGER_FORM1_H
#define AVERAGER_FORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "averager_curve_wnd.h"
#include "utils.h"
#include "edf_helper.h"

#include "filt/filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/math_func.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"

#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;



class UI_AveragerWindow : public QObject
{
  Q_OBJECT

public:
  UI_AveragerWindow(QWidget *, annotblck_t *);

  ~UI_AveragerWindow();

  UI_Mainwindow *mainwindow;


private:

QDialog     *averager_dialog;

QLineEdit   *annot_name_line_edit;

QPushButton *CloseButton,
            *StartButton,
            *set_start_button,
            *set_end_button,
            *set_display_range_button;

QListWidget *list;

QComboBox   *ratioBox;

QTimeEdit    *timeEdit1,
             *timeEdit2;

QTime        time1,
             time2;

QSpinBox *avg_periodspinbox,
         *day_spinbox1,
         *day_spinbox2;

char annot_str[MAX_ANNOTATION_LEN + 1];

int signal_nr;

double *avgbuf;

edfhdrblck_t *edf_hdr;


void process_avg(sigcompblck_t *);

private slots:

void startButtonClicked();
void set_start_button_clicked();
void set_end_button_clicked();
void set_display_range_button_clicked();

};



#endif // AVERAGER_FORM1_H


