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




#ifndef ANNOTATIONSFORM1_H
#define ANNOTATIONSFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edit_annotation_dock.h"
#include "viewcurve.h"
#include "utils.h"
#include "averager_dialog.h"
#include "edf_annot_list.h"
#include "annotlist_filter_dialog.h"
#include "statistics_dialog.h"
#include "hrv_dock.h"
#include "rename_annots_dialog.h"


class UI_Mainwindow;




class UI_Annotationswindow : public QObject
{
  Q_OBJECT

public:
  UI_Annotationswindow(edfhdrblck_t *e_hdr, QWidget *parent);

  UI_Mainwindow *mainwindow;

  QDockWidget  *docklist;

  QListWidget  *list;

  void updateList(int);

  int get_last_pressed_row(void);

private:

  int relative,
      selected,
      invert_filter,
      hide_nk_triggers,
      hide_bs_triggers,
      last_pressed_annotation,
      file_position_changed_signal_block;

  edfhdrblck_t *edf_hdr;

  QDialog *dialog1;

  QCheckBox *relative_checkbox,
            *invert_checkbox;

  QLabel *label1;

  QLineEdit *search_line_edit;

  QPushButton *more_button;

  QAction *show_between_act,
          *average_annot_act,
          *hide_annot_act,
          *unhide_annot_act,
          *hide_same_annots_act,
          *unhide_same_annots_act,
          *unhide_all_annots_act,
          *hide_all_NK_triggers_act,
          *hide_all_BS_triggers_act,
          *unhide_all_NK_triggers_act,
          *unhide_all_BS_triggers_act,
          *filt_ival_time_act,
          *show_stats_act,
          *show_heart_rate_act,
          *edit_annotations_act,
          *remove_duplicates_act,
          *rename_all_act,
          *delete_annots_act,
          *delete_all_annots_act,
          *jump_to_previous_annot_act,
          *jump_to_next_annot_act;

  QTimer *delayed_list_filter_update_timer;

private slots:

  void annotation_selected(QListWidgetItem *, int centered=1);
  void annotation_pressed(QListWidgetItem *);
  void hide_editdock(bool);
  void relative_checkbox_clicked(int);
  void invert_checkbox_clicked(int);
  void more_button_clicked(bool);
  void show_between(bool);
  void average_annot(bool);
  void hide_annot(bool);
  void unhide_annot(bool);
  void hide_same_annots(bool);
  void unhide_same_annots(bool);
  void unhide_all_annots(bool);
  void filter_edited(const QString);
  void hide_all_NK_triggers(bool);
  void hide_all_BS_triggers(bool);
  void unhide_all_NK_triggers(bool);
  void unhide_all_BS_triggers(bool);
  void filt_ival_time(bool);
  void show_stats(bool);
  void show_heart_rate(bool);
  void delayed_list_filter_update();
  void rename_all();
  void delete_annots();
  void delete_all_annots();
  void file_pos_changed(long long);
  void jump_to_next_annot(bool);
  void jump_to_previous_annot(bool);
};



#endif // ANNOTATIONSFORM1_H


