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


#ifndef UI_HYPNOGRAMFORM_H
#define UI_HYPNOGRAMFORM_H

#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "hypnogram_dock.h"
#include "special_button.h"



class UI_Mainwindow;


class UI_hypnogram_window : public QObject
{
  Q_OBJECT

public:
  UI_hypnogram_window(QWidget *, edfhdrblck_t *, int);

  UI_Mainwindow  *mainwindow;

private:

  int instance_num;

  edfhdrblck_t *edfhdr;

  QDialog       *myobjectDialog;

  QLabel        *stage_label,
                *annot_label;

  QLineEdit     *stage1_edit,
                *stage2_edit,
                *stage3_edit,
                *stage4_edit,
                *stage5_edit,
                *stage6_edit,
                *annot1_edit,
                *annot2_edit,
                *annot3_edit,
                *annot4_edit,
                *annot5_edit,
                *annot6_edit,
                *annot1_ov_edit,
                *annot2_ov_edit,
                *annot3_ov_edit,
                *annot4_ov_edit,
                *annot5_ov_edit,
                *annot6_ov_edit;

  QPushButton   *close_button,
                *start_button,
                *default_button;

  QCheckBox     *use_epoch_len_checkbox,
                *use_overlays_checkbox;

  SpecialButton *ov_color_button1,
                *ov_color_button2,
                *ov_color_button3,
                *ov_color_button4,
                *ov_color_button5,
                *ov_color_button6;

  QWidget       *flayout2widget;

private slots:

  void start_button_clicked();
  void default_button_clicked();
  void use_epoch_len_checkbox_changed(int);
  void use_overlays_checkbox_changed(int);
  void ov_color_button1_clicked(SpecialButton *);
  void ov_color_button2_clicked(SpecialButton *);
  void ov_color_button3_clicked(SpecialButton *);
  void ov_color_button4_clicked(SpecialButton *);
  void ov_color_button5_clicked(SpecialButton *);
  void ov_color_button6_clicked(SpecialButton *);

};

#endif










