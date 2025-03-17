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



#ifndef SPECTRUMANALYZER_H
#define SPECTRUMANALYZER_H


#include "qt_headers.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "mainwindow.h"
#include "signalcurve.h"
#include "utils.h"
#include "edf_helper.h"
#include "flywheel.h"

#include "filt/fft_wrap.h"
#include "filt/filter.h"
#include "filt/fir_filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/math_func.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;




class UI_FreqSpectrumWindow : public QThread
{
  Q_OBJECT

public:
  UI_FreqSpectrumWindow(sigcompblck_t *, char *, UI_FreqSpectrumWindow **, int, QWidget *);

  ~UI_FreqSpectrumWindow();

  UI_Mainwindow *mainwindow;

  QDialog *SpectrumDialog;

  sigcompblck_t *signalcomp;

private:

  QHBoxLayout *hlayout1,
              *hlayout2,
              *hlayout3,
              *hlayout4;

  QVBoxLayout *vlayout1,
              *vlayout2,
              *vlayout3;

  SignalCurve *curve1;

  QTimer *t1;

  QSlider *amplitudeSlider,
          *spanSlider,
          *centerSlider,
          *log_minslider;

  QLabel *spanLabel,
         *centerLabel,
         *amplitudeLabel;

  QCheckBox *sqrtCheckBox,
            *VlogCheckBox,
            *BWCheckBox;

  QSpinBox *dftsz_spinbox;

  QComboBox *windowBox,
            *dftsz_box,
            *overlap_box;

  UI_Flywheel *flywheel1;

  int samples,
      dftblocksize,
      window_type,
      spectrumdialog_is_destroyed,
      class_is_deleted,
      spectrumdialognumber,
      flywheel_value,
      first_run,
      fft_inputbufsize,
      overlap;


  volatile int busy,
               malloc_err;

  double samplefreq,
         freqstep,
         maxvalue,
         maxvalue_sqrt,
         maxvalue_vlog,
         maxvalue_sqrt_vlog,
         minvalue_vlog,
         minvalue_sqrt_vlog,
         *buf1_fft_input,
         *buf2_psd,
         *buf3_amp,
         *buf4_psd_log,
         *buf5_amp_log;

  char *viewbuf,
       signallabel[1024],
       physdimension[9];

  UI_FreqSpectrumWindow **spectrumdialog;

  fft_wrap_setting_t *fft_data;

  void run();

private slots:

void update_curve();
void sliderMoved(int);
void SpectrumDialogDestroyed(QObject *);
void print_to_txt();
void update_flywheel(int);
void thr_finished_func();
void dftsz_value_changed(int);
void windowBox_changed(int);
void dftsz_box_changed(int);
void overlap_box_changed(int);

};


#endif



















