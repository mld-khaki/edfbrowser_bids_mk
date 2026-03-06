/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2025 Teunis van Beelen
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





/* =========================
   Command-line (headless) reduction
   ========================= */

typedef struct
{
  /* 1-based inclusive datarecord range.
     from_datarecord <= 0 means 1.
     to_datarecord   <= 0 means end-of-file. */
  int from_datarecord;
  int to_datarecord;

  /* Optional time-based clipping. If from_timestamp/to_timestamp are provided,
     they override from_datarecord/to_datarecord. Accepted formats include:
       "YYYY-MM-DD HH:MM:SS" , "YYYY-MM-DDTHH:MM:SS" , "HH:MM:SS"
     A few common separator typos are tolerated (e.g., "HH-MM-SS"). */
  char from_timestamp[64];
  char to_timestamp[64];

  /* Optional offsets (minutes) applied like the Python helper:
       adjusted_start = from - pre_offset_minutes
       adjusted_end   = to   + post_offset_minutes
     Default 0. */
  int pre_offset_minutes;
  int post_offset_minutes;

  /* Anti-aliasing filter order (number of cascaded running-average filters).
     Valid range: 0..REDUCER_MAX_AA_FILTERS */
  int aa_filter_order;

  /* If non-zero, attempt to read EDF+/BDF+ annotations and carry them over. */
  int read_annotations;

  /* If non-zero, overwrite output file if it already exists. */
  int overwrite_existing;

  /* Global samplerate divider (applied when a per-signal divider is not provided).
     Must be a divisor of smp_per_record for each selected signal. */
  int global_samplerate_divider;

  /* Optional CSV selection:
       "all"                           -> select all non-annotation signals
       "Fp1,Fp2,ECG"                   -> select by label (trimmed)
       "0,1,2" or "#0,#1,#2"            -> select by EDF signal index (0-based)
       "Fp1:2,Fp2:4,#12:8"             -> per-signal samplerate divider
  */
  char signals_csv[2048];

  /* Optional wildcard mask to pick signals by label.
     Multiple patterns can be comma-separated.
     Wildcards:
       *  matches any sequence
       ?  matches any single character
       #  matches a digit (0-9)
     Example: "c###" matches c001, c123, etc.
     If provided, it is applied as a filter on top of --signals/JSON selection.
     If --signals is omitted, the default selection becomes all signals matching the mask. */
  char mask_signals[2048];

  /* Optional JSON config path. If provided, it overrides/augments signals_csv.
     Expected format:
       {
         "selected_signals": [
           {"label":"Fp1", "samplerate_divider":2},
           ...
         ],
         "from_datarecord": 1,
         "to_datarecord": 123,
         "aa_filter_order": 3
       }
  */
  char json_config_path[MAX_PATH_LENGTH];

} reduce_signals_cli_options_t;


/* Returns 0 on success, non-zero on failure.
   If output_path is NULL or empty, a default "<input>_reduced.(edf|bdf)" is used.
   err_out may be NULL. */
int reduce_signals_cli(const char *input_path,
                       const char *output_path,
                       const reduce_signals_cli_options_t *opt,
                       char *err_out,
                       int err_out_sz);

#endif
