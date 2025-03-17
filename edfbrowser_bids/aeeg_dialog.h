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


#ifndef UI_AEEGFORM_H
#define UI_AEEGFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "aeeg_dock.h"
#include "filtered_block_read.h"
#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;

typedef struct aeeg_dock_param_struct aeeg_dock_param_t;


class UI_aeeg_window : public QObject
{
  Q_OBJECT

public:
  UI_aeeg_window(QWidget *, sigcompblck_t *, int, aeeg_dock_param_t *p_par=NULL);

  UI_Mainwindow  *mainwindow;

private:

  int sf, aeeg_instance_nr;

  sigcompblck_t *signalcomp;

  aeeg_dock_param_t *no_dialog_params;

  QDialog       *myobjectDialog;

  QFormLayout   *flayout;

  QSpinBox      *segmentlen_spinbox;

  QDoubleSpinBox *bp_min_hz_spinbox,
                 *bp_max_hz_spinbox,
                 *lp_hz_spinbox,
                 *scale_max_amp_spinbox;

  QPushButton   *close_button,
                *start_button,
                *default_button;

  QCheckBox     *plot_margins_checkbox;

  static int dbl_cmp(const void *, const void *);

private slots:

  void start_button_clicked();
  void default_button_clicked();

};

#endif










