/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2024 Teunis van Beelen
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


#ifndef UI_CDSADOCKFORM_H
#define UI_CDSADOCKFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "filtered_block_read.h"

#include "filt/fft_wrap.h"



class UI_Mainwindow;
class simple_tracking_indicator;
class simple_ruler_indicator;
class simple_color_index;



struct cdsa_dock_param_struct
{
  sigcompblck_t *signalcomp;
  int sf;
  int min_hz;
  int max_hz;
  int segment_len;
  int segments_in_recording;
  int block_len;
  int overlap;
  int window_func;
  double max_voltage;
  int max_pwr;
  int min_pwr;
  int log;
  int power_voltage;
  QPixmap *pxm;
  int instance_num;
  char unit[32];
  int no_dialog;
};

typedef struct cdsa_dock_param_struct cdsa_dock_param_t;



class UI_cdsa_dock : public QObject
{
  Q_OBJECT

public:
  UI_cdsa_dock(QWidget *, cdsa_dock_param_t);
  ~UI_cdsa_dock();

  UI_Mainwindow  *mainwindow;

  QDockWidget *cdsa_dock;

  double w_scaling,
         h_scaling;

  cdsa_dock_param_t param;

private:

  simple_tracking_indicator *trck_indic;
  simple_ruler_indicator     *srl_indic;
  simple_color_index       *color_indic;

  unsigned long long sigcomp_uid;

  int is_deleted;

private slots:

  void cdsa_dock_destroyed(QObject *);
  void file_pos_changed(long long);
  void contextmenu_requested(QPoint);

};


class simple_tracking_indicator: public QWidget
{
  Q_OBJECT

public:
  double w_scaling,
         h_scaling;

  simple_tracking_indicator(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_position(long long);
  void set_maximum(long long);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  long long pos, max;

  void draw_small_arrow(QPainter *, int, int, int, QColor);
};


class simple_color_index: public QWidget
{
  Q_OBJECT

public:
  simple_color_index(QWidget *parent=0);

  double w_scaling,
         h_scaling;

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_max_volt(double);
  void set_max_pwr(int);
  void set_min_pwr(int);
  void set_log_enabled(int);
  void set_pwr_enabled(int);
  void set_unit(const char *);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  int max_pwr,
      min_pwr,
      log,
      pwr;

  double max_volt;

  char unit[32];
};


class simple_ruler_indicator: public QWidget
{
  Q_OBJECT

public:
  simple_ruler_indicator(QWidget *parent=0);

  double w_scaling,
         h_scaling;

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_minimum(int);
  void set_maximum(int);
  void set_unit(const char *);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  char unit[32];

  int min, max;
};


#endif










