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



#include "annotations_dock.h"



static void process_events(void)
{
  qApp->processEvents();
}


UI_Annotationswindow::UI_Annotationswindow(edfhdrblck_t *e_hdr, QWidget *w_parent)
{
  QPalette palette;


  mainwindow = (UI_Mainwindow *)w_parent;

  edf_hdr = e_hdr;

  docklist = new QDockWidget("Annotations", w_parent);
  docklist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  docklist->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  relative = 1;

  mainwindow->annotations_onset_relative = 1;

  selected = -1;

  invert_filter = 0;

  hide_nk_triggers = 0;

  hide_bs_triggers = 0;

  last_pressed_annotation = -1;

  file_position_changed_signal_block = 0;

  dialog1 = new QDialog;

  relative_checkbox = new QCheckBox("Relative ");
  relative_checkbox->setTristate(false);
  relative_checkbox->setCheckState(Qt::Checked);
  relative_checkbox->setToolTip("If enabled, onset time of annotations will be relative to start of recording, otherwise clocktime");

  label1 = new QLabel;
  label1->setText(" Filter:");

  search_line_edit = new QLineEdit;
  search_line_edit->setMaxLength(16);

  invert_checkbox = new QCheckBox("Inv.");
  invert_checkbox->setTristate(false);
  invert_checkbox->setCheckState(Qt::Unchecked);
  invert_checkbox->setToolTip("Invert filter result if enabled");

  more_button = new QPushButton("More");

  list = new QListWidget(dialog1);
  list->setFont(*mainwindow->monofont);
  list->setAutoFillBackground(true);
  list->setPalette(palette);
  list->setUniformItemSizes(true);

  show_between_act = new QAction("Set timescale from here to next annotation", list);
  hide_annot_act = new QAction("Hide", list);
  unhide_annot_act = new QAction("Unhide", list);
  hide_same_annots_act = new QAction("Hide similar", list);
  unhide_same_annots_act = new QAction("Unhide similar", list);
  unhide_all_annots_act = new QAction("Unhide all", list);
  average_annot_act = new QAction("Average", list);
  hide_all_NK_triggers_act = new QAction("Hide all Nihon Kohden triggers", list);
  hide_all_BS_triggers_act = new QAction("Hide all Biosemi triggers", list);
  unhide_all_NK_triggers_act = new QAction("Unhide all Nihon Kohden triggers", list);
  unhide_all_BS_triggers_act = new QAction("Unhide all Biosemi triggers", list);
  filt_ival_time_act = new QAction("Filter Interval Time", list);
  show_stats_act = new QAction("Heart Rate Variability", list);
  show_heart_rate_act = new QAction("Heart Rate", list);
  edit_annotations_act = new QAction("Edit annotations", list);
  remove_duplicates_act = new QAction("Remove duplicates", list);
  rename_all_act = new QAction("Rename", list);
  delete_annots_act = new QAction("Delete", list);
  delete_all_annots_act = new QAction("Delete all", list);
  delete_annots_act = new QAction("Delete", list);

  list->setContextMenuPolicy(Qt::ActionsContextMenu);
  list->insertAction(NULL, show_between_act);
  list->insertAction(NULL, hide_annot_act);
  list->insertAction(NULL, hide_same_annots_act);
  list->insertAction(NULL, unhide_annot_act);
  list->insertAction(NULL, unhide_same_annots_act);
  list->insertAction(NULL, unhide_all_annots_act);
  list->insertAction(NULL, average_annot_act);
  list->insertAction(NULL, hide_all_NK_triggers_act);
  list->insertAction(NULL, unhide_all_NK_triggers_act);
  list->insertAction(NULL, hide_all_BS_triggers_act);
  list->insertAction(NULL, unhide_all_BS_triggers_act);
  list->insertAction(NULL, filt_ival_time_act);
  list->insertAction(NULL, show_stats_act);
  list->insertAction(NULL, show_heart_rate_act);
  list->insertAction(NULL, edit_annotations_act);
  list->insertAction(NULL, remove_duplicates_act);
  list->insertAction(NULL, rename_all_act);
  list->insertAction(NULL, delete_annots_act);
  list->insertAction(NULL, delete_all_annots_act);

  jump_to_next_annot_act = new QAction("Next annotation", list);
  jump_to_previous_annot_act = new QAction("Previous annotation", list);
#if QT_VERSION < 0x060000
  jump_to_next_annot_act->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  jump_to_previous_annot_act->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
#else
  jump_to_next_annot_act->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
  jump_to_previous_annot_act->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
#endif

  dialog1->setContextMenuPolicy(Qt::PreventContextMenu);
  dialog1->addAction(jump_to_next_annot_act);
  dialog1->addAction(jump_to_previous_annot_act);

  QHBoxLayout *h_layout = new QHBoxLayout;
  h_layout->addWidget(relative_checkbox);
  h_layout->addWidget(label1);
  h_layout->addWidget(search_line_edit, 1000);
  h_layout->addWidget(invert_checkbox);
  h_layout->addWidget(more_button);

  QVBoxLayout *v_layout = new QVBoxLayout(dialog1);
  v_layout->addLayout(h_layout);
  v_layout->addWidget(list);

  delayed_list_filter_update_timer = new QTimer(this);
  delayed_list_filter_update_timer->setSingleShot(true);

  docklist->setWidget(dialog1);

  updateList(0);

  QObject::connect(list,                       SIGNAL(itemClicked(QListWidgetItem *)),   this, SLOT(annotation_selected(QListWidgetItem *)));
  QObject::connect(list,                       SIGNAL(itemPressed(QListWidgetItem *)),   this, SLOT(annotation_pressed(QListWidgetItem *)));
  QObject::connect(docklist,                   SIGNAL(visibilityChanged(bool)),          this, SLOT(hide_editdock(bool)));
  QObject::connect(relative_checkbox,          SIGNAL(stateChanged(int)),                this, SLOT(relative_checkbox_clicked(int)));
  QObject::connect(invert_checkbox,            SIGNAL(stateChanged(int)),                this, SLOT(invert_checkbox_clicked(int)));
  QObject::connect(more_button,                SIGNAL(clicked(bool)),                    this, SLOT(more_button_clicked(bool)));
  QObject::connect(hide_annot_act,             SIGNAL(triggered(bool)),                  this, SLOT(hide_annot(bool)));
  QObject::connect(unhide_annot_act,           SIGNAL(triggered(bool)),                  this, SLOT(unhide_annot(bool)));
  QObject::connect(hide_same_annots_act,       SIGNAL(triggered(bool)),                  this, SLOT(hide_same_annots(bool)));
  QObject::connect(unhide_same_annots_act,     SIGNAL(triggered(bool)),                  this, SLOT(unhide_same_annots(bool)));
  QObject::connect(unhide_all_annots_act,      SIGNAL(triggered(bool)),                  this, SLOT(unhide_all_annots(bool)));
  QObject::connect(average_annot_act,          SIGNAL(triggered(bool)),                  this, SLOT(average_annot(bool)));
  QObject::connect(show_between_act,           SIGNAL(triggered(bool)),                  this, SLOT(show_between(bool)));
  QObject::connect(hide_all_NK_triggers_act,   SIGNAL(triggered(bool)),                  this, SLOT(hide_all_NK_triggers(bool)));
  QObject::connect(hide_all_BS_triggers_act,   SIGNAL(triggered(bool)),                  this, SLOT(hide_all_BS_triggers(bool)));
  QObject::connect(unhide_all_NK_triggers_act, SIGNAL(triggered(bool)),                  this, SLOT(unhide_all_NK_triggers(bool)));
  QObject::connect(unhide_all_BS_triggers_act, SIGNAL(triggered(bool)),                  this, SLOT(unhide_all_BS_triggers(bool)));
  QObject::connect(filt_ival_time_act,         SIGNAL(triggered(bool)),                  this, SLOT(filt_ival_time(bool)));
  QObject::connect(show_stats_act,             SIGNAL(triggered(bool)),                  this, SLOT(show_stats(bool)));
  QObject::connect(show_heart_rate_act,        SIGNAL(triggered(bool)),                  this, SLOT(show_heart_rate(bool)));
  QObject::connect(search_line_edit,           SIGNAL(textEdited(const QString)),        this, SLOT(filter_edited(const QString)));

  QObject::connect(edit_annotations_act,       SIGNAL(triggered(bool)),                  mainwindow, SLOT(annotation_editor()));

  QObject::connect(remove_duplicates_act,      SIGNAL(triggered(bool)),                  mainwindow, SLOT(edfplus_remove_duplicate_annotations()));

  QObject::connect(rename_all_act,             SIGNAL(triggered(bool)),                  this, SLOT(rename_all()));

  QObject::connect(delete_annots_act,          SIGNAL(triggered(bool)),                  this, SLOT(delete_annots()));

  QObject::connect(delete_all_annots_act,      SIGNAL(triggered(bool)),                  this, SLOT(delete_all_annots()));

  QObject::connect(delayed_list_filter_update_timer, SIGNAL(timeout()),                  this, SLOT(delayed_list_filter_update()));

  QObject::connect(mainwindow,                 SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));

  QObject::connect(jump_to_next_annot_act,     SIGNAL(triggered(bool)),                  this, SLOT(jump_to_next_annot(bool)));
  QObject::connect(jump_to_previous_annot_act, SIGNAL(triggered(bool)),                  this, SLOT(jump_to_previous_annot(bool)));
}


void UI_Annotationswindow::file_pos_changed(long long viewtime)
{
  if(file_position_changed_signal_block)
  {
    file_position_changed_signal_block = 0;
    return;
  }

  if(!mainwindow->annotlist_scrolltoitem_while_browsing)  return;

  if(mainwindow->video_player->status != VIDEO_STATUS_STOPPED)  return;

  if(mainwindow->live_stream_active       ||
     mainwindow->playback_realtime_active ||
     mainwindow->signal_averaging_active)
  {
    return;
  }

  int i, sz_visible;

  long long onset,
            pagetime = mainwindow->pagetime;

  annotblck_t *annot;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  int sz = edfplus_annotation_size(annot_list);

  if(sz < 4)  return;

  viewtime -= (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - edf_hdr->viewtime);

  for(i=0, sz_visible=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->hidden)
    {
      continue;
    }

    sz_visible++;

    onset = annot->onset;

    if(sz_visible == 1)
    {
      if(onset >= (viewtime + pagetime))
      {
        list->scrollToTop();
        break;
      }
    }

    if((onset >= viewtime) && (onset < (viewtime + pagetime)))
    {
      if(sz_visible < 4)
      {
        list->scrollToTop();
      }
      else
      {
        list->scrollToBottom();
        list->scrollToItem(list->item(sz_visible - 4));
      }

      break;
    }
  }
  if((i == sz) && (i > 0))
  {
    if(annot->onset < viewtime)
    {
      list->scrollToBottom();
    }
  }
}


void UI_Annotationswindow::delete_annots()
{
  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->files_open > 1)
  {
    QMessageBox::critical(mainwindow, "Error", "Requested action is not permitted when multiple files are opened.");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    QMessageBox::critical(mainwindow, "Error", "There are no annotations.");
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  UI_rename_annots_dialog rename_dialog(mainwindow, 1);
}


void UI_Annotationswindow::delete_all_annots()
{
  char str1_4096[4096]="";

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->files_open > 1)
  {
    QMessageBox::critical(mainwindow, "Error", "Requested action is not permitted when multiple files are opened.");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    QMessageBox::critical(mainwindow, "Error", "There are no annotations.");
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  QMessageBox msgBox;
  msgBox.setText("You are about to delete all annotations.");
  msgBox.setInformativeText("Do you want to continue?");
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() == QMessageBox::No)
  {
    return;
  }

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&edf_hdr->annot_list);
  }

  edfplus_annotation_empty_list(&edf_hdr->annot_list);

  mainwindow->annotations_edited = 1;

  mainwindow->annotations_dock[0]->updateList(0);

  mainwindow->save_act->setEnabled(true);
}


void UI_Annotationswindow::rename_all()
{
  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->files_open > 1)
  {
    QMessageBox::critical(mainwindow, "Error", "Requested action is not permitted when multiple files are opened.");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    QMessageBox::critical(mainwindow, "Error", "There are no annotations.");
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  UI_rename_annots_dialog rename_dialog(mainwindow, 0);
}


void UI_Annotationswindow::more_button_clicked(bool)
{
  QMessageBox::information(mainwindow,  "Info", "Right-click on an annotation for more options.");
}


void UI_Annotationswindow::jump_to_next_annot(bool)
{
  int count, row, centered=1;

  if(mainwindow->live_stream_active       ||
     mainwindow->playback_realtime_active ||
     mainwindow->signal_averaging_active)
  {
    return;
  }

  count = list->count();
  if(count < 2)  return;

  row = list->currentRow();
  if(row >= (count - 1))  return;

  row++;

  list->setCurrentRow(row);

  if(mainwindow->annot_onset_at_start_of_page_on_jump)
  {
    centered = 0;
  }

  annotation_selected(list->item(row), centered);
}


void UI_Annotationswindow::jump_to_previous_annot(bool)
{
  int count, row, centered=1;

  if(mainwindow->live_stream_active       ||
     mainwindow->playback_realtime_active ||
     mainwindow->signal_averaging_active)
  {
    return;
  }

  count = list->count();
  if(count < 2)  return;

  row = list->currentRow();
  if(row < 1)  return;

  row--;

  list->setCurrentRow(row);

  if(mainwindow->annot_onset_at_start_of_page_on_jump)
  {
    centered = 0;
  }

  annotation_selected(list->item(row), centered);
}


void UI_Annotationswindow::show_heart_rate(bool)
{
  int instance_num, row;

  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(list->count() < 1)
  {
    QMessageBox::critical(mainwindow, "Error", "There are no annotations.");
    return;
  }

  row = list->currentRow();
  if(row >= list->count())
  {
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, row);
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  hrv_dock_param_t dock_param;

  memset(&dock_param, 0, sizeof(hrv_dock_param_t));

  for(instance_num=0; instance_num<MAXHRVDOCKS; instance_num++)
  {
    if(mainwindow->hrv_dock[instance_num] == NULL)
    {
      break;
    }
  }

  if(instance_num == MAXHRVDOCKS)
  {
    return;
  }

  dock_param.instance_num = instance_num;

  dock_param.edfhdr = edf_hdr;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.annot_name, annot->description, MAX_ANNOTATION_LEN);
  trim_spaces(dock_param.annot_name);

  mainwindow->hrv_dock[instance_num] = new UI_hrv_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hrv_dock[instance_num]->hrv_dock);

  mainwindow->insertToolBarBreak(mainwindow->hrv_dock[instance_num]->hrv_dock);

  edf_hdr->hrv_dock[instance_num] = instance_num + 1;

  QObject::connect(mainwindow, SIGNAL(annot_docklist_changed()), mainwindow->hrv_dock[instance_num], SLOT(update_curve()));

  mainwindow->enable_hrv_stats_toolbar(annot->description, annot_list, NULL);
}


void UI_Annotationswindow::show_stats(bool)
{
  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  UI_StatisticWindow stats_wndw(NULL, 0LL, mainwindow, annot_list, annot);
}


void UI_Annotationswindow::filt_ival_time(bool)
{
  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  UI_AnnotFilterWindow filter_wndw(mainwindow, annot, mainwindow->annot_filter, edf_hdr);
}


void UI_Annotationswindow::hide_all_NK_triggers(bool)
{
  int i, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  hide_nk_triggers = 1;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hidden = 1;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_all_BS_triggers(bool)
{
  int i, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  hide_bs_triggers = 1;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hidden = 1;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_NK_triggers(bool)
{
  int i, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  hide_nk_triggers = 0;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_NK_TRIGGER))
    {
      annot->hidden = 0;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_BS_triggers(bool)
{
  int i, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  hide_bs_triggers = 0;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(annot->ident & (1 << ANNOT_ID_BS_TRIGGER))
    {
      annot->hidden = 0;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::filter_edited(const QString)
{
  delayed_list_filter_update_timer->start(500);
}


void UI_Annotationswindow::delayed_list_filter_update()
{
  int i, sz;

  char filter_str_32[32];

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  QString text = search_line_edit->text();

  sz = edfplus_annotation_size(annot_list);

  if(sz < 1)  return;

  if(text.length() < 1)
  {
    for(i=0; i<sz; i++)
    {
      annot = edfplus_annotation_get_item(annot_list, i);

      if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) ||
           ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
      {
        annot->hidden = 0;
      }
    }

    updateList(1);

    mainwindow->maincurve->update();

    return;
  }

  strlcpy(filter_str_32, search_line_edit->text().toUtf8().data(), 32);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    if(!(((annot->ident & (1 << ANNOT_ID_NK_TRIGGER)) && hide_nk_triggers) ||
         ((annot->ident & (1 << ANNOT_ID_BS_TRIGGER)) && hide_bs_triggers)))
    {
      if(invert_filter)
      {
        annot->hidden = 0;
      }
      else
      {
        annot->hidden = 1;
      }

      if(strstr(annot->description, filter_str_32))
      {
        if(invert_filter)
        {
          annot->hidden = 1;
        }
        else
        {
          annot->hidden = 0;
        }
      }
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::invert_checkbox_clicked(int state)
{
  int changed=0, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  sz = edfplus_annotation_size(annot_list);

  if(sz < 1)  return;

  if(state==Qt::Checked)
  {
    if(invert_filter == 0)  changed = 1;

    invert_filter = 1;
  }

  if(state==Qt::Unchecked)
  {
    if(invert_filter == 1)  changed = 1;

    invert_filter = 0;
  }

  if(changed == 0)  return;

//  filter_edited(search_line_edit->text());
  delayed_list_filter_update();
}


void UI_Annotationswindow::show_between(bool)
{
  int n, sz;

  long long displaytime;

  if(list->count() < 2)
  {
    return;
  }

  n = list->currentRow();

  if(n >= (list->count() - 1))
  {
    return;
  }

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= (sz - 1))  return;

  annot = edfplus_annotation_get_item(annot_list, n);

  displaytime = annot->onset;

  annot = edfplus_annotation_get_item(annot_list, n + 1);

  displaytime -= annot->onset;

  if(displaytime < 0)
  {
    displaytime *= -1;
  }

  if(displaytime < 1000)
  {
    return;
  }

  mainwindow->pagetime = displaytime;

  annotation_selected(list->currentItem(), 0);
}


void UI_Annotationswindow::hide_annot(bool)
{
  int n, sz;

  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);

  annot->hidden = 1;

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_annot(bool)
{
  int n, sz;

  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item(annot_list, n);

  annot->hidden = 0;

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_same_annots(bool)
{
  int i, n, sz;

  char str1[MAX_ANNOTATION_LEN]={""},
       str2[MAX_ANNOTATION_LEN]={""};


  if(list->count() < 1)
  {
    return;
  }

  n = list->currentRow();

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);
  if(annot == NULL)  return;

  strlcpy(str1, annot->description, MAX_ANNOTATION_LEN);

  trim_spaces(str1);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str2, annot->description, MAX_ANNOTATION_LEN);

    trim_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hidden = 1;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_same_annots(bool)
{
  int i, n, sz;

  char str1[MAX_ANNOTATION_LEN],
       str2[MAX_ANNOTATION_LEN];

  if(list->count() < 1)  return;

  n = list->currentRow();

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  annot = edfplus_annotation_get_item_visible_only(annot_list, n);

  strlcpy(str1, annot->description, MAX_ANNOTATION_LEN);

  trim_spaces(str1);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str2, annot->description, MAX_ANNOTATION_LEN);

    trim_spaces(str2);

    if(!strcmp(str1, str2))
    {
      annot->hidden = 0;
    }
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::unhide_all_annots(bool)
{
  int i, sz;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    annot->hidden = 0;
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::average_annot(bool)
{
  char str1_4096[4096]="";

  annotlist_t *annot_list;

  annotblck_t *annot;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(!mainwindow->signalcomps)
  {
    QMessageBox::critical(mainwindow, "Error", "First add a signal to the screen.");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(mainwindow, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(list->count() < 1)
  {
    return;
  }

  if(edf_hdr == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  annot_list = &(edf_hdr->annot_list);

  annot = edfplus_annotation_get_item_visible_only(annot_list, list->currentRow());
  if(annot == NULL)
  {
    snprintf(str1_4096, 4096, "Nullpointer returned: file: %s line %i", __FILE__, __LINE__);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_4096);
    messagewindow.exec();
    return;
  }

  UI_AveragerWindow average_wndw(mainwindow, annot);
}


void UI_Annotationswindow::relative_checkbox_clicked(int state)
{
  if(state==Qt::Checked)
  {
    relative = 1;

    mainwindow->annotations_onset_relative = 1;
  }

  if(state==Qt::Unchecked)
  {
    relative = 0;

    mainwindow->annotations_onset_relative = 0;
  }

  updateList(1);

  mainwindow->maincurve->update();
}


void UI_Annotationswindow::hide_editdock(bool visible)
{
  if(visible==false)
  {
    delete mainwindow->annotationEditDock;

    mainwindow->annotationEditDock = NULL;
  }
}



void UI_Annotationswindow::updateList(int scroll_to_item_requested)
{
  int j=0,
      sz=0,
      modified=0,
      scroll_val=0,
      selected_in_dock_idx=-1,
      len=0,
      visible_item_idx=0;

  char str1[MAX_ANNOTATION_LEN + 32]="",
       str2_1024[1024]="";

  QListWidgetItem *listitem;

  QString string;

  annotblck_t *annot;

  annotlist_t *annot_list;

  selected = -1;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  list->verticalScrollBar()->setRange(0, 1000000);

  scroll_val = list->verticalScrollBar()->value();

  list->clear();

  annot_list = &(edf_hdr->annot_list);

  sz = edfplus_annotation_size(annot_list);

  QMessageBox messagewindow(QMessageBox::NoIcon, "Busy", "Sorting annotations...");
  messagewindow.setStandardButtons(QMessageBox::NoButton);
  if(sz > 2000)
  {
    messagewindow.show();
  }

  edfplus_annotation_sort(annot_list, &process_events);

  messagewindow.setText("Populating list...");

  for(j=0; j<sz; j++)
  {
    if(!(j % 1000))
    {
      qApp->processEvents();
    }

    annot = edfplus_annotation_get_item(annot_list, j);

    if(annot->hidden)
    {
      continue;
    }

    if(annot->selected_in_dock)
    {
      selected_in_dock_idx = visible_item_idx;
    }

    if(mainwindow->annot_editor_highres)
    {
      if(relative)
      {
        if(annot->onset < 0LL)
        {
          snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "  -%2i:%02i:%02i.%06i",
                  (int)((-annot->onset / TIME_FIXP_SCALING)/ 3600),
                  (int)(((-annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
                  (int)((-annot->onset / TIME_FIXP_SCALING) % 60),
                  (int)((-annot->onset % TIME_FIXP_SCALING) / 10LL));
        }
        else
        {
          snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "  %3i:%02i:%02i.%06i",
                  (int)((annot->onset / TIME_FIXP_SCALING)/ 3600),
                  (int)(((annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
                  (int)((annot->onset / TIME_FIXP_SCALING) % 60),
                  (int)((annot->onset % TIME_FIXP_SCALING) / 10LL));
        }
      }
      else
      {
        snprintf(str1, MAX_ANNOTATION_LEN + 32, "  %3i:%02i:%02i.%06i",
                (int)((((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING)/ 3600) % 24),
                (int)((((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING) % 3600) / 60),
                (int)(((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING) % 60),
                (int)(((annot->onset + edf_hdr->starttime_hr) % TIME_FIXP_SCALING) / 10LL));
      }
    }
    else
    {
      if(relative)
      {
        if(annot->onset < 0LL)
        {
          snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "  -%2i:%02i:%02i.%03i",
                  (int)((-annot->onset / TIME_FIXP_SCALING)/ 3600),
                  (int)(((-annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
                  (int)((-annot->onset / TIME_FIXP_SCALING) % 60),
                  (int)((-annot->onset % TIME_FIXP_SCALING) / 10000LL));
        }
        else
        {
          snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "  %3i:%02i:%02i.%03i",
                  (int)((annot->onset / TIME_FIXP_SCALING)/ 3600),
                  (int)(((annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
                  (int)((annot->onset / TIME_FIXP_SCALING) % 60),
                  (int)((annot->onset % TIME_FIXP_SCALING) / 10000LL));
        }
      }
      else
      {
        snprintf(str1, MAX_ANNOTATION_LEN + 32, "  %3i:%02i:%02i.%03i",
                (int)((((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING)/ 3600) % 24),
                (int)((((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING) % 3600) / 60),
                (int)(((annot->onset + edf_hdr->starttime_hr) / TIME_FIXP_SCALING) % 60),
                (int)(((annot->onset + edf_hdr->starttime_hr) % TIME_FIXP_SCALING) / 10000LL));
      }
    }

    str1[MAX_ANNOTATION_LEN + 31] = 0;

    remove_trailing_zeros(str1);

    string = QString::fromUtf8(annot->description);

    if(string.size() < 20)
    {
      string = string.leftJustified(20, ' ');
    }

    string.append(QString::fromLatin1(str1));

    listitem = new QListWidgetItem(string, list);

    listitem->setData(Qt::UserRole, QVariant(j));

    listitem->setBackground(mainwindow->maincurve->backgroundcolor);

    if(annot->modified==1)
    {
      listitem->setForeground(mainwindow->annot_list_edited_txt_color);

      modified = 1;
    }
    else
    {
      listitem->setForeground(mainwindow->maincurve->text_color);
    }

    if(annot->onset < 0LL)
    {
      len = snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "onset: -%i:%02i:%02i.%06i",
              (int)((-annot->onset / TIME_FIXP_SCALING)/ 3600),
              (int)(((-annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
              (int)((-annot->onset / TIME_FIXP_SCALING) % 60),
              (int)((-annot->onset % TIME_FIXP_SCALING) / 10LL));
    }
    else
    {
      len = snprintf(str1, (MAX_ANNOTATION_LEN + 32) / 2, "onset: %2i:%02i:%02i.%06i",
              (int)((annot->onset / TIME_FIXP_SCALING)/ 3600),
              (int)(((annot->onset / TIME_FIXP_SCALING) % 3600) / 60),
              (int)((annot->onset / TIME_FIXP_SCALING) % 60),
              (int)((annot->onset % TIME_FIXP_SCALING) / 10LL));
    }

    if(annot->duration[0]!=0)
    {
      len += snprintf(str1 + len, MAX_ANNOTATION_LEN + 32 - len, "\nduration: %s",annot->duration);
    }

    remove_trailing_zeros(str1);

    strlcat(str1, "\n", MAX_ANNOTATION_LEN + 32);

    strlcat(str1, annot->description, MAX_ANNOTATION_LEN + 32);

    if(mainwindow->edf_debug && (!mainwindow->annot_editor_active) && (!mainwindow->annotations_edited))
    {
      len = strlen(str1);
#ifdef Q_OS_WIN32
      __mingw_snprintf(str1 + len, MAX_ANNOTATION_LEN + 32 - len, "\ndatarecord: %i\nfile offset: 0x%02llx", annot->datrec, annot->file_offset);
#else
      snprintf(str1 + len, MAX_ANNOTATION_LEN + 32 - len, "\ndatarecord: %i\nfile offset: 0x%02llx", annot->datrec, annot->file_offset);
#endif
    }

    listitem->setToolTip(QString::fromUtf8(str1));

    if(annot->grabbed)
    {
      selected = j;

      annot->grabbed = 0;
    }

    visible_item_idx++;
  }

  if(mainwindow->annot_editor_active)
  {
    if(selected>=0)
    {
      list->setCurrentRow(selected, QItemSelectionModel::ClearAndSelect);

      if(mainwindow->annotationEditDock != NULL)
      {
        mainwindow->annotationEditDock->set_selected_annotation(selected);
      }
    }

    if(modified)
    {
      mainwindow->annotations_edited = 1;

      mainwindow->save_act->setEnabled(true);
    }
  }

  if((scroll_to_item_requested) && (selected_in_dock_idx>=0))
  {
    list->setCurrentItem(list->item(selected_in_dock_idx));
    list->scrollToItem(list->item(selected_in_dock_idx), QAbstractItemView::PositionAtCenter);
  }
  else
  {
    list->verticalScrollBar()->setRange(0, 1000000);

    list->verticalScrollBar()->setValue(scroll_val);
  }

  messagewindow.hide();

  QApplication::restoreOverrideCursor();

  mainwindow->annot_dock_updated();

  snprintf(str2_1024, 1024, "Annotations      (%i)", list->count());

  docklist->setWindowTitle(str2_1024);
}


int UI_Annotationswindow::get_last_pressed_row(void)
{
  return last_pressed_annotation;
}


void UI_Annotationswindow::annotation_pressed(QListWidgetItem *item)
{
  int j, n, sz;

  annotblck_t *annot;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  n = item->data(Qt::UserRole).toInt();

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  last_pressed_annotation = n;

  for(j=0; j<mainwindow->files_open; j++)
  {
    if(mainwindow->annotations_dock[j] != NULL)
    {
      edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[j]->annot_list);
    }
  }

  annot = edfplus_annotation_get_item(annot_list, n);

  annot->selected_in_dock = 1;
}


void UI_Annotationswindow::annotation_selected(QListWidgetItem * item, int centered)
{
  int j, n, sz;

  long long l_tmp;

  annotblck_t *annot;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  n = item->data(Qt::UserRole).toInt();

  sz = edfplus_annotation_size(annot_list);

  if(n >= sz)  return;

  if(mainwindow->annot_onset_at_start_of_page_on_jump)
  {
    centered = 0;
  }

  last_pressed_annotation = n;

  for(j=0; j<mainwindow->files_open; j++)
  {
    if(mainwindow->annotations_dock[j] != NULL)
    {
      edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[j]->annot_list);
    }
  }

  annot = edfplus_annotation_get_item(annot_list, n);

  annot->selected_in_dock = 1;

  if(mainwindow->annot_editor_active)
  {
    if(mainwindow->annotationEditDock != NULL)
    {
      mainwindow->annotationEditDock->set_selected_annotation(n);
    }
  }

  if(mainwindow->video_player->status == VIDEO_STATUS_PLAYING)
  {
    mainwindow->video_player_seek((int)(annot->onset / TIME_FIXP_SCALING));

    return;
  }

  if(mainwindow->video_player->status == VIDEO_STATUS_PAUSED)
  {
    mainwindow->video_player_seek((int)(annot->onset / TIME_FIXP_SCALING));
  }

  if(mainwindow->annot_editor_active)
  {
    if(mainwindow->annotationEditDock != NULL)
    {
      mainwindow->maincurve->setCrosshair_1_center();
    }
  }

  file_position_changed_signal_block = 1;

  l_tmp = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - edf_hdr->viewtime;

  if(centered)
  {
    mainwindow->set_viewtime(annot->onset - (mainwindow->pagetime / 2) + l_tmp);
  }
  else
  {
    mainwindow->set_viewtime(annot->onset + l_tmp);
  }
}






















