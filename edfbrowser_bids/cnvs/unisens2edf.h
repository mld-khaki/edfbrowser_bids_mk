/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013 - 2024 Teunis van Beelen
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


#ifndef UI_UNISENS2EDFFORM_H
#define UI_UNISENS2EDFFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"
#include "edflib.h"
#include "utils.h"


class UI_Mainwindow;


class UI_UNISENS2EDFwindow : public QObject
{
  Q_OBJECT

public:

  UI_UNISENS2EDFwindow(QWidget *parent, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow *mainwindow;

private:

QTextEdit    *textEdit1;

QPushButton  *pushButton1,
             *pushButton2;

QDialog      *myobjectDialog;

char  *recent_opendir,
      *recent_savedir,
     binfilename[MAXFILES][MAX_PATH_LENGTH],
     evtfilename[MAXFILES][MAX_PATH_LENGTH],
     physdim[MAXFILES][9],
     signallabel[MAXSIGNALS][17],
     str_timestampStart[32],
     str_measurementId[128],
     csv_sep[MAXFILES],
     csv_dec_sep[MAXFILES];

int straightbinary[MAXFILES],
    big_endian[MAXFILES],
    samplesize[MAXFILES],
    datatype[MAXFILES],
    sf[MAXFILES],
    csv_enc[MAXFILES],
    nedval_enc[MAXFILES],
    nedval_smpl[MAXFILES],
    nedval_value[MAXFILES][MAXSIGNALS],
    nedval_value2[MAXFILES][MAXSIGNALS],
    sf_less_1,
    sf_inv[MAXFILES],
    sf_divider,
    digmax[MAXFILES],
    digmin[MAXFILES],
    adcres[MAXFILES],
    datablocks[MAXFILES],
    edf_signals[MAXFILES],
    total_edf_signals,
    edf_signal_start[MAXFILES],
    max_datablocks,
    file_cnt,
    trig_file_cnt,
    buf1_offset[MAXFILES],
    buf2_offset[MAXFILES],
    buf1_freadsize[MAXFILES],
    char_encoding,
    bdf,
    evt_sf[MAXFILES],
    total_annotations,
    starttime_fraction;

long long adczero[MAXFILES],
          baseline[MAXFILES];


double physmax[MAXFILES],
       physmin[MAXFILES],
       lsbval[MAXFILES];

FILE *binfile[MAXFILES];


int get_signalparameters_from_BIN_attributes(xml_hdl_t *, int);
int get_signalparameters_from_EVT_attributes(xml_hdl_t *, int);
int get_events_from_csv_files(int, int, const char *);
int count_events_from_csv_files(int, const char *, int *);

private slots:

void SelectFileButton();

};




#endif


