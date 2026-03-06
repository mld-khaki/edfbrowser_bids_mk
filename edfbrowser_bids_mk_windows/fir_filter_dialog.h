/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 - 2024 Teunis van Beelen
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



#ifndef ADD_FIR_FILTERFORM1_H
#define ADD_FIR_FILTERFORM1_H



#include "qt_headers.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"

#include "filt/fir_filter.h"
#include "filt/remez.h"
#include "filt/wind_sinc.h"
#include "filtercurve.h"



class UI_Mainwindow;


class UI_FIRFilterRZ_thread : public QThread
{
public:

struct rz_params_struct
{
  int band_num;
  double sf;
  double start_hz[32];
  double stop_hz[32];
  double gain[32];
  double ripple[32];
  double *f_taps;
  int tap_num;
  int abort;
}rz_params;

UI_FIRFilterRZ_thread();
~UI_FIRFilterRZ_thread();

private:

void run();

};


class UI_FIRFilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_FIRFilterDialog(char *recent_dir=NULL, char *save_dir=NULL, QWidget *parent=0);
  ~UI_FIRFilterDialog();

  UI_Mainwindow *mainwindow;


private:

QDialog        *firfilterdialog;

QTabWidget     *tabholder;

QWidget        *sinc_tab,
               *pm_tab;

QPushButton    *CancelButton,
               *ApplyButton,
               *helpButton,
               *fileButton,
               *pm_add_passband_button,
               *pm_add_stopband_button,
               *pm_design_button,
               *sinc_design_button;

QComboBox      *pm_predefined_cbox,
               *sinc_type_cbox;

QListWidget    *signals_list;

QPlainTextEdit *textEdit;

QLabel         *listlabel,
               *varsLabel,
               *freqresplabel,
               *filt_descr_label;

QSpinBox       *db_min_spinbox,
               *db_max_spinbox;

QDoubleSpinBox *pm_sf_spinbox,
               *sinc_sf_spinbox,
               *sinc_cutoff1_spinbox,
               *sinc_cutoff2_spinbox,
               *sinc_trans_bw_spinbox;

QCheckBox      *plotphasecheckbox;

QTableWidget *pm_param_table;

FilterCurve    *curve1;

UI_FIRFilterRZ_thread *thrd;

int n_taps,
    db_max_i,
    db_min_i,
    plot_phase,
    fir_model;

double *taps,
       *z,
       *deg;

char *textbuf,
     *recent_opendir,
     *recent_savedir,
     filter_description_txt_1[512],
     filter_description_txt_2[512];

int pm_add_table_row(void);

private slots:

void ApplyButtonClicked();
void check_text();
void helpbuttonpressed();
void filebuttonpressed();
void db_min_spinbox_changed(int);
void db_max_spinbox_changed(int);
void plotphasecheckbox_changed(int);
void selected_signals_changed();
void pm_add_passband_button_pressed();
void pm_add_stopband_button_pressed();
void pm_design_button_pressed();
void pm_tabel_cell_pressed(int, int);
void pm_predefined_cbox_changed(int);
void pm_paramtable_rowcount_changed();
void current_tab_changed(int);
void sinc_design_button_pressed();
void sinc_type_cbox_changed(int);
void gain_spinbox_changed(double);

};



#endif // ADD_FIR_FILTERFORM1_H








