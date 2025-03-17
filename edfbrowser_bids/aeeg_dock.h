/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 - 2024 Teunis van Beelen
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


#ifndef UI_AEEGDOCKFORM_H
#define UI_AEEGDOCKFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"


class UI_Mainwindow;
class log_vruler_indicator;
class aeeg_curve_widget;


struct aeeg_dock_param_struct
{
  sigcompblck_t *signalcomp;
  int sf;
  double bp_min_hz;
  double bp_max_hz;
  double lp_hz;
  int max_nearby_pct;
  int min_nearby_pct;
  int segment_len;
  int segments_in_recording;
  int medians_in_recording;
  int instance_num;
  char unit[32];
  int no_dialog;
  int plot_margins;
  double *max_seg_val;
  double *min_seg_val;
  double *max_median_val;
  double *min_median_val;
  double scale_max_amp;
  UI_Mainwindow *mainwindow;
};

typedef struct aeeg_dock_param_struct aeeg_dock_param_t;



class UI_aeeg_dock : public QObject
{
  Q_OBJECT

public:
  UI_aeeg_dock(QWidget *, aeeg_dock_param_t);
  ~UI_aeeg_dock();

  UI_Mainwindow  *mainwindow;

  QToolBar *aeeg_dock;

  double w_scaling,
         h_scaling;

  aeeg_dock_param_t param;

private:

  unsigned long long sigcomp_uid;

  int is_deleted;

  aeeg_curve_widget *aeeg_curve;

  log_vruler_indicator *srl_indic1,
                       *srl_indic2;

  QMenu *context_menu;

  QSpinBox *height_spinbox;

  QCheckBox *plot_margins_checkbox;

private slots:

  void aeeg_dock_destroyed(QObject *);
  void file_pos_changed(long long);
  void show_settings(bool);
  void close_dock(bool);
  void show_context_menu(QPoint);
  void height_spinbox_changed(int);
  void plot_margins_checkbox_changed(int);

};


class log_vruler_indicator: public QWidget
{
  Q_OBJECT

public:
  log_vruler_indicator(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_params(aeeg_dock_param_t *);

  void set_range(int, int);
  void set_scaling(double, double);
  void set_mirrored(bool);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  aeeg_dock_param_t param;

  log_vruler_indicator *srl_indic;

  aeeg_curve_widget *aeeg_curve;

  int max_val,
      min_val,
      mirrored;

  double w_scaling,
         h_scaling;
};


class aeeg_curve_widget: public QWidget
{
  Q_OBJECT

public:
  aeeg_curve_widget(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  UI_Mainwindow  *mainwindow;

  void set_params(aeeg_dock_param_t *);

  void set_range(int, int);

  void set_trace_color(QColor);

  void make_logarithmic(void);

  void set_marker_position(int);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  int marker_pos;

  aeeg_dock_param_t param;

  double aeeg_min,
         aeeg_max,
         aeeg_range;

  QColor trace_color;
};


#endif










