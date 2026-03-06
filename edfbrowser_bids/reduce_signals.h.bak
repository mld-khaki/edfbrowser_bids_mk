/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2024 Teunis van Beelen
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


#ifndef UI_REDUCESIGNALSFORM_H
#define UI_REDUCESIGNALSFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "check_edf_file.h"
#include "utils.h"
#include "utc_date_time.h"
#include "active_file_chooser.h"
#include "edf_annot_list.h"
#include "edf_helper.h"

#include "filt/ravg_filter.h"


#define REDUCER_MAX_AA_FILTERS   (4)


class UI_Mainwindow;



class UI_ReduceSignalsWindow : public QObject
{
  Q_OBJECT

public:

  UI_ReduceSignalsWindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

QLabel       *label1,
             *label2,
             *label3,
             *label4,
             *label5,
             *label6;

QPushButton  *pushButton1,
             *pushButton2,
             *pushButton3,
             *pushButton4,
             *pushButton5,
             *pushButton6,
             *pushButton7,
             *helpButton;

QSpinBox     *spinBox1,
             *spinBox2,
             *spinBox3,
             *spinBox4;

QRadioButton *radioButton1,
             *radioButton2;

QTableWidget *SignalsTablewidget;

QDialog      *myobjectDialog;

int  signalslist[MAXSIGNALS],
     dividerlist[MAXSIGNALS],
     file_num,
     aa_filter_order;

char  inputpath[MAX_PATH_LENGTH],
      outputpath[MAX_PATH_LENGTH],
      *recent_savedir;

FILE *inputfile,
     *outputfile;

edfhdrblck_t *edfhdr;

ravgfiltset_t *filterlist[MAXSIGNALS][REDUCER_MAX_AA_FILTERS];

private slots:

void SelectFileButton();
void StartConversion();
void Select_all_signals();
void Deselect_all_signals();
void Set_SRdivider_all_signals();
void spinBox1changed(int);
void spinBox2changed(int);
void radioButton1Toggled(bool);
void radioButton2Toggled(bool);
void helpbuttonpressed();
void LoadFromJson();


};




#endif


