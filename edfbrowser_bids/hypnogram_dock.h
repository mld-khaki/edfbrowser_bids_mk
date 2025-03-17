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


#ifndef UI_HYPNOGRAMDOCKFORM_H
#define UI_HYPNOGRAMDOCKFORM_H

#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edf_annot_list.h"


#define HYPNOGRAM_STAGENUM   (6)
#define HYPNOGRAM_OVNUM      (6)


class UI_Mainwindow;
class simple_tracking_indicator2;
class simple_ruler_indicator2;
class hypnogram_curve_widget;


typedef struct
{
  int instance_num;
  int use_epoch_len;
  int use_overlays;
  char stage_name[HYPNOGRAM_STAGENUM][32];
  char annot_name[HYPNOGRAM_STAGENUM][32];
  char annot_ov_name[HYPNOGRAM_OVNUM][32];
  rgbac_t annot_ov_color[HYPNOGRAM_OVNUM];
  edfhdrblck_t *edfhdr;
  UI_Mainwindow  *mainwindow;
} hypnogram_dock_param_t;


class UI_hypnogram_dock : public QObject
{
  Q_OBJECT

public:
  UI_hypnogram_dock(QWidget *, hypnogram_dock_param_t);
  ~UI_hypnogram_dock();

  UI_Mainwindow  *mainwindow;

  QToolBar *hypnogram_dock;

  double w_scaling,
         h_scaling;

  hypnogram_dock_param_t param;

public slots:
  void update_curve(void);

private:

  QMenu *context_menu;

  QSpinBox *height_spinbox;

  simple_tracking_indicator2 *trck_indic;

  simple_ruler_indicator2 *srl_indic;

  hypnogram_curve_widget *hypnogram_curve;

  int is_deleted;

private slots:

  void file_pos_changed(long long);
  void hypnogram_dock_destroyed(QObject *);
  void show_settings(bool);
  void close_dock(bool);
  void show_context_menu(QPoint);
  void height_spinbox_changed(int);

};


class simple_tracking_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_tracking_indicator2(QWidget *parent=0);

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

  double w_scaling,
         h_scaling;

  void draw_small_arrow(QPainter *, int, int, int, QColor);
};


class simple_ruler_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_ruler_indicator2(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_params(hypnogram_dock_param_t *);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  double w_scaling,
         h_scaling;

  hypnogram_dock_param_t param;
};


class hypnogram_curve_widget: public QWidget
{
  Q_OBJECT

public:
  hypnogram_curve_widget(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  UI_Mainwindow  *mainwindow;

  void set_params(hypnogram_dock_param_t *);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  hypnogram_dock_param_t param;

  QColor annot_ov_q_color[HYPNOGRAM_OVNUM];
};


#endif










