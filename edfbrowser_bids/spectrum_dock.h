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



#ifndef SPECTRUMANALYZERDOCK_H
#define SPECTRUMANALYZERDOCK_H



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


struct spectrumdocksettings{
  int signalnr;
  int dashboard;
  int span;
  int center;
  int amp;
  int log_min_sl;
  int wheel;
  int log;
  int sqrt;
  int colorbar;
  int blocksize_predefined;
  int dftblocksize;
  int window_type;
  int overlap;
  double maxvalue;
  double maxvalue_sqrt;
  double maxvalue_vlog;
  double maxvalue_sqrt_vlog;
  double minvalue_vlog;
  double minvalue_sqrt_vlog;
};


class UI_SpectrumDockWindow : public QObject
{
  Q_OBJECT

public:
  UI_SpectrumDockWindow(QWidget *parent);

  ~UI_SpectrumDockWindow();

  UI_Mainwindow *mainwindow;

  QDialog *SpectrumDialog;

  QDockWidget *dock;

  sigcompblck_t *signalcomp;

  struct spectrum_markersblock *spectrum_color;

void rescan();
void init(int);
void clear();
void getsettings(struct spectrumdocksettings *);
void setsettings(struct spectrumdocksettings);


private:

  QWidget *parentwindow;

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

  QCheckBox *sqrtButton,
            *vlogButton,
            *colorBarButton;

  QSpinBox *dftsz_spinbox;

  QComboBox *windowBox,
            *overlap_box,
            *dftsz_box;

  UI_Flywheel *flywheel1;

  int dftblocksize,
      window_type,
      steps,
      spectrumdock_sqrt,
      spectrumdock_vlog,
      dashboard,
      flywheel_value,
      init_maxvalue,
      signal_nr,
      set_settings,
      overlap,
      samples,
      buf1_sz;

  volatile int busy;

  double samplefreq,
         freqstep,
         maxvalue,
         maxvalue_sqrt,
         maxvalue_vlog,
         maxvalue_sqrt_vlog,
         minvalue_vlog,
         minvalue_sqrt_vlog,
         *buf1,
         *buf2,
         *buf3,
         *buf4,
         *buf5;

  char *viewbuf,
       signallabel[512],
       physdimension[9];

  fft_wrap_setting_t *fft_data;

  struct spectrumdocksettings settings;



private slots:

void update_curve();
void sliderMoved(int);
void sqrtButtonClicked(bool);
void vlogButtonClicked(bool);
void colorBarButtonClicked(bool);
void print_to_txt();
void setdashboard();
void update_flywheel(int);
void dftsz_value_changed(int);
void windowBox_changed(int);
void overlap_box_changed(int);
void dftsz_box_changed(int);

void open_close_dock(bool);
};


#endif



















