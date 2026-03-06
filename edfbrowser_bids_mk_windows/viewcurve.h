/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2024 Teunis van Beelen
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



#ifndef VIEWCURVE_H
#define VIEWCURVE_H

#include "qt_headers.h"

#if QT_VERSION < 0x050000
#include <QPrinter>
#else
#if QT_VERSION < 0x060000
#include <QtPrintSupport>
#else
#include <QPrintDialog>
#endif
#endif
#if QT_VERSION >= 0x050D00
#include <QPageSize>
#endif

#include <QPrintDialog>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "spectrumanalyzer.h"
#include "utils.h"
#include "adjustfiltersettings.h"
#include "statistics_dialog.h"
#include "z_score_dialog.h"
#include "cdsa_dialog.h"
#include "cdsa_dock.h"
#include "aeeg_dialog.h"
#include "aeeg_dock.h"
#include "run_qrs_detector.h"
#include "averager_dialog.h"
#include "edf_helper.h"

#include "filt/filter.h"
#include "filt/spike_filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"
#include "filt/ravg_filter.h"
#include "filt/ecg_filter.h"
#include "filt/z_ratio_filter.h"
#include "filt/math_func.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;



class drawCurve_stage_1_thread : public QThread
{
public:
  void init_vars(UI_Mainwindow *, sigcompblck_t **, int,
                 int, char *, int, int, int *, int,
                 graphbuf_t *, double,
                 double, crosshair_t *,
                 crosshair_t *, int, int);


private:

  int i, h, w,
      signalcomps,
      *screensamples,
      *screensamples_b,
      printing,
      cpu_cnt,
      linear_interpol;

  char *viewbuf;

  double printsize_x_factor,
         printsize_y_factor;

  UI_Mainwindow *mainwindow;

  sigcompblck_t *signalcomp, **signalcomp_b;

  graphbuf_t *graphicBuf;

  crosshair_t *crosshair_1,
                         *crosshair_2;

void run();

};



class ViewCurve : public QWidget
{
  Q_OBJECT

public:
  ViewCurve(QWidget *parent=0);
  ~ViewCurve();

  UI_Mainwindow *mainwindow;

  int use_move_events,
      panning_moving,
      mouse_x,
      mouse_y,
      mouse_old_x,
      mouse_old_y,
      sidemenu_active,
      signal_nr,
      mouse_press_coordinate_x,
      mouse_press_coordinate_y,
      mouse_release_coordinate_x,
      draw_zoom_rectangle,
      signal_color,
      floating_ruler_color,
      ruler_active,
      ruler_moving,
      ruler_x_position,
      ruler_y_position,
      *screensamples,
      graphicBufWidth,
      blackwhite_printing,
      backup_color_10[MAXSIGNALS],
      backup_color_11,
      backup_color_12,
      backup_color_14,
      annot_marker_moving,
      pressed_on_label,
      label_press_y_position,
      cpu_cnt,
      float_ruler_more,
      floating_ruler_use_var_width,
      linear_interpol,
      active_marker_context_menu_request_idx;

  long long pan_mov_start_viewtime[MAXFILES];

  double original_screen_offset_pix,
         original_screen_offset_unit,
         w_scaling,
         h_scaling;

  graphbuf_t *graphicBuf,
             *printBuf;

  activemarkersblck_t *active_markers;

  crosshair_t crosshair_1,
              crosshair_2;

  QColor backgroundcolor,
         small_ruler_color,
         big_ruler_color,
         mouse_rect_color,
         baseline_color,
         text_color,
         annot_marker_color,
         annot_marker_selected_color,
         annot_duration_color,
         annot_duration_color_selected,
         backup_color_1,
         backup_color_2,
         backup_color_3,
         backup_color_4,
         backup_color_5,
         backup_color_13,
         backup_color_15,
         backup_color_16,
         backup_color_17,
         backup_color_18,
         mc_annot_ov_color_predefined_bu[8];

  QFont *printfont;

  QShortcut *shift_page_left_shortcut,
            *shift_page_right_shortcut,
            *shift_page_up_shortcut,
            *shift_page_down_shortcut;

  void drawCurve_stage_1(QPainter *painter=NULL, int w_width=0, int w_height=0, int print_linewidth=0, int print_format=PRINT_FORMAT_NONE);
  void setCrosshair_1_center(void);
  void arrowkeys_shortcuts_global_set_enabled(bool);
  void set_vertical_scroll_value(int);

public slots:

  void exec_sidemenu(int);
  void print_to_pdf_A3();
  void print_to_pdf_A4();
  void print_to_pdf_Letter();
  void print_to_image(int, int, int);
  void print_to_printer();
  void next_crosshair_act_triggered();

private slots:

  void print_to_pdf(int);

private:

  QDialog     *sidemenu;

  QPushButton *sidemenuButton1,
              *sidemenuButton2,
              *sidemenuButton3,
              *sidemenuButton4,
              *sidemenuButton5,
              *sidemenuButton6,
              *sidemenuButton7,
              *sidemenuButton8,
              *sidemenuButton9,
              *sidemenuButton10,
              *sidemenuButton11,
              *sidemenuButton12,
              *sidemenuButton13,
              *sidemenuButton14,
              *sidemenuButton15,
              *sidemenuButton16;

  QDoubleSpinBox *ScaleBox,
                 *ScaleBox2;

  QLineEdit   *AliasLineEdit;

  QPrinter    *printer;

  QPen        *special_pen,
              *annot_marker_pen,
              *signal_pen,
              *ruler_pen;

  QMenu       *context_menu;

  QAction     *select_annot_act,
              *hide_annot_act,
              *average_annot_act,
              *annot_sidemenu_act;

  int         printing,
              w,
              h,
              scroll_h_offset,
              scrollbar_h_value;

  double      original_sensitivity,
              painter_pixelsizefactor,
              printsize_x_factor,
              printsize_y_factor;

  drawCurve_stage_1_thread *thr[MAXSIGNALS];

  inline void floating_ruler(QPainter *, int, int, sigcompblck_t *, int, double);

  void backup_colors_for_printing(void);
  void restore_colors_after_printing(void);
  void draw_default_horizontal_hr_graticule(QPainter *, long long, int, int);
  void draw_ecg_horizontal_hr_graticule(QPainter *, long long, int, int);
  void draw_default_horizontal_graticule(QPainter *, int);
  void draw_ecg_horizontal_graticule(QPainter *, int);
  void draw_generic_horizontal_graticule(QPainter *painter, int);

protected slots:

  void RulerButton();
  void FittopaneButton();
  void ScaleBoxChanged(double);
  void ScaleBox2Changed(double);
  void RemovefilterButton();
  void RemovesignalButton();
  void ColorButton();
  void CrosshairButton();
  void FreqSpecButton();
  void Z_scoringButton();
  void AdjustFilterButton();
  void StatisticsButton();
  void ECGdetectButton();
  void signalInvert();
  void sidemenu_close();
  void cdsa_button();
  void aeeg_button();
  void QRSdetectButton();

  void select_annot_clicked(bool);
  void hide_annot_clicked(bool);
  void average_annot_clicked(bool);
  void annot_sidemenu_act_by_crosshair(bool);

protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);
  void drawCurve_stage_2(QPainter *painter, int w_width=0, int w_height=0, int print_linewidth=0, int print_format=PRINT_FORMAT_NONE);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void wheelEvent(QWheelEvent *);
  void dragEnterEvent(QDragEnterEvent *);
  void dropEvent(QDropEvent *);

signals:

  void file_dropped(void);
  void annot_created_by_rect_draw(void);
};



#endif


