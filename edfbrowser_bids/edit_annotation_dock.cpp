/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2024 Teunis van Beelen
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



UI_AnnotationEditwindow::UI_AnnotationEditwindow(edfhdrblck_t *e_hdr, QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  edf_hdr = e_hdr;

  is_deleted = 0;

  dockedit = new QToolBar("Annotation editor", w_parent);
  dockedit->setOrientation(Qt::Horizontal);
  dockedit->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  dockedit->setAttribute(Qt::WA_DeleteOnClose, true);

  descriptionLabel = new QLabel;
  descriptionLabel->setText("Description");

  annot_descript_lineEdit = new QLineEdit;

  completer = new QCompleter(this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  annot_descript_lineEdit->setCompleter(completer);

  onsetLabel = new QLabel;
  onsetLabel->setText("Onset");

  posNegTimebox = new QComboBox;
  posNegTimebox->setEditable(false);
  posNegTimebox->addItem("+");
  posNegTimebox->addItem("-");

  onset_daySpinbox = new QSpinBox;
  onset_daySpinbox->setRange(0, 99);
  onset_daySpinbox->setSingleStep(1);
  onset_daySpinbox->setValue(0);
  onset_daySpinbox->setToolTip("24-hour units relative to starttime");

  onset_timeEdit = new QTimeEdit;
  onset_timeEdit->setDisplayFormat("hh:mm:ss.zzz");
  onset_timeEdit->setMinimumTime(QTime(-1, 0, 0, 0));
  onset_timeEdit->setToolTip("Onset time of the event (hh:mm:ss:mmm) relative to starttime");

  onset_us_spinbox = new QSpinBox;
  onset_us_spinbox->setRange(0, 999);
  onset_us_spinbox->setSingleStep(1);
  onset_us_spinbox->setValue(0);
  onset_us_spinbox->setAlignment(Qt::AlignRight);
  onset_us_spinbox->setSuffix(" uSec.");
  onset_us_spinbox->setToolTip("microseconds relative to starttime");
  onset_us_spinbox->setMinimumWidth(75 * mainwindow->w_scaling);
  if(mainwindow->annot_editor_highres)
  {
    onset_us_spinbox->setEnabled(true);
  }
  else
  {
    onset_us_spinbox->setEnabled(false);
  }

  durationLabel = new QLabel;
  durationLabel->setText("Duration");

  duration_spinbox = new QDoubleSpinBox;
  duration_spinbox->setRange(-1.0, 10000000.0);
  duration_spinbox->setSingleStep(1.0);
  if(mainwindow->annot_editor_highres)
  {
    duration_spinbox->setDecimals(6);
  }
  else
  {
    duration_spinbox->setDecimals(3);
  }
  duration_spinbox->setSuffix(" sec");
  duration_spinbox->setValue(-1.0);
  duration_spinbox->setToolTip("Duration of the event in seconds, -1 if not applicable)");

  modifybutton = new QPushButton;
  modifybutton->setText("Modify");
  modifybutton->setEnabled(false);

  deletebutton = new QPushButton;
  deletebutton->setText("Delete");
  deletebutton->setShortcut(QKeySequence::Delete);
  deletebutton->setEnabled(false);

  createbutton = new QPushButton;
  createbutton->setText("Create");

  mainwindow->annot_editor_active = 1;

  mainwindow->show_annot_markers = 1;

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  update_description_completer();

  for(i=0; i<8; i++)
  {
    user_button[i] = new QPushButton;
    user_button[i]->setText(mainwindow->annot_edit_user_button_name[i]);
//    user_button[i]->setShortcut(QKeySequence(Qt::CTRL + (Qt::Key_1 + i)));
    user_button[i]->setShortcut(QKeySequence(Qt::Key_1 + i));
    if(!mainwindow->annot_edit_user_button_enabled[i])
    {
      user_button[i]->setVisible(false);
    }
  }

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(10);
  hlayout1->addWidget(descriptionLabel);
  hlayout1->addWidget(annot_descript_lineEdit);
  hlayout1->addStretch(1);
  hlayout1->addWidget(onsetLabel);
  hlayout1->addWidget(posNegTimebox);
  hlayout1->addWidget(onset_daySpinbox);
  hlayout1->addWidget(onset_timeEdit);
  hlayout1->addWidget(onset_us_spinbox);
  hlayout1->addStretch(1);
  hlayout1->addWidget(durationLabel);
  hlayout1->addWidget(duration_spinbox);
  hlayout1->addStretch(1);
  hlayout1->addWidget(modifybutton);
  hlayout1->addStretch(1);
  hlayout1->addWidget(deletebutton);
  hlayout1->addStretch(1);
  hlayout1->addWidget(createbutton);
  for(i=0; i<8; i++)
  {
    hlayout1->addStretch(1);
    hlayout1->addWidget(user_button[i]);
  }
  hlayout1->addStretch(10);

  annot_edit_frame = new QFrame;
  annot_edit_frame->setFrameShape(QFrame::NoFrame);

  annot_edit_frame->setLayout(hlayout1);

  dockedit->addWidget(annot_edit_frame);

  annot_by_rect_draw_menu = new QMenu(annot_edit_frame);
  if(strlen(mainwindow->annot_by_rect_draw_description[0]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[0]), this, SLOT(annot_by_rect_draw_side_menu_0_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[1]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[1]), this, SLOT(annot_by_rect_draw_side_menu_1_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[2]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[2]), this, SLOT(annot_by_rect_draw_side_menu_2_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[3]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[3]), this, SLOT(annot_by_rect_draw_side_menu_3_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[4]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[4]), this, SLOT(annot_by_rect_draw_side_menu_4_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[5]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[5]), this, SLOT(annot_by_rect_draw_side_menu_5_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[6]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[6]), this, SLOT(annot_by_rect_draw_side_menu_6_clicked()));
  }
  if(strlen(mainwindow->annot_by_rect_draw_description[7]))
  {
    annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[7]), this, SLOT(annot_by_rect_draw_side_menu_7_clicked()));
  }

  QObject::connect(modifybutton, SIGNAL(clicked()),            this, SLOT(modifyButtonClicked()));
  QObject::connect(deletebutton, SIGNAL(clicked()),            this, SLOT(deleteButtonClicked()));
  QObject::connect(createbutton, SIGNAL(clicked()),            this, SLOT(createButtonClicked()));
  QObject::connect(dockedit,     SIGNAL(destroyed(QObject *)), this, SLOT(dockedit_destroyed(QObject *)));

  QObject::connect(user_button[0], SIGNAL(clicked()), this, SLOT(user_button_0_clicked()));
  QObject::connect(user_button[1], SIGNAL(clicked()), this, SLOT(user_button_1_clicked()));
  QObject::connect(user_button[2], SIGNAL(clicked()), this, SLOT(user_button_2_clicked()));
  QObject::connect(user_button[3], SIGNAL(clicked()), this, SLOT(user_button_3_clicked()));
  QObject::connect(user_button[4], SIGNAL(clicked()), this, SLOT(user_button_4_clicked()));
  QObject::connect(user_button[5], SIGNAL(clicked()), this, SLOT(user_button_5_clicked()));
  QObject::connect(user_button[6], SIGNAL(clicked()), this, SLOT(user_button_6_clicked()));
  QObject::connect(user_button[7], SIGNAL(clicked()), this, SLOT(user_button_7_clicked()));

  mainwindow->maincurve->arrowkeys_shortcuts_global_set_enabled(false);
}


UI_AnnotationEditwindow::~UI_AnnotationEditwindow()
{
  mainwindow->maincurve->arrowkeys_shortcuts_global_set_enabled(true);

  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(dockedit);

    mainwindow->annot_editor_active = 0;

    mainwindow->annotationEditDock = NULL;
  }
}


void UI_AnnotationEditwindow::dockedit_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->annot_editor_active = 0;

    mainwindow->annotationEditDock = NULL;

    delete this;
  }
}


void UI_AnnotationEditwindow::modifyButtonClicked()
{
  int file_num;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot = edfplus_annotation_get_item(annot_list, annot_num);

  if(annot->edfhdr == NULL)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15884");
    return;
  }

  if(annot->edfhdr != edf_hdr)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15894");
    return;
  }

  annot->onset = annotEditGetOnset();

  if(dblcmp(duration_spinbox->value(), 0) > 0)
  {
    snprintf(annot->duration, 20, "%f", duration_spinbox->value());

    remove_trailing_zeros(annot->duration);

    annot->long_duration = edfplus_annotation_get_long_from_number(annot->duration);
  }
  else
  {
    annot->duration[0] = 0;

    annot->long_duration = 0LL;
  }

  strncpy(annot->description, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annot->description[MAX_ANNOTATION_LEN] = 0;

  annot->modified = 1;

  edfplus_annotation_cancel_all_selected_in_dock(annot_list);

  annot->selected_in_dock = 1;

  mainwindow->annotations_edited = 1;

  file_num = mainwindow->get_filenum((edfhdrblck_t *)(annot->edfhdr));
  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList(1);
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::deleteButtonClicked()
{
  int file_num;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot = edfplus_annotation_get_item(annot_list, annot_num);

  if(annot->edfhdr == NULL)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15885");
    return;
  }

  if(annot->edfhdr != edf_hdr)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15895");
    return;
  }

  file_num = mainwindow->get_filenum((edfhdrblck_t *)(annot->edfhdr));

  edfplus_annotation_remove_item(annot_list, annot_num);

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);

  mainwindow->annotations_edited = 1;

  mainwindow->save_act->setEnabled(true);

  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList(0);
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::createButtonClicked()
{
  int file_num;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t annotation;

  memset(&annotation, 0, sizeof(annotblck_t));

  annotation.onset = annotEditGetOnset();

  annotation.edfhdr = edf_hdr;

  if(dblcmp(duration_spinbox->value(), 0) > 0)
  {
    snprintf(annotation.duration, 20, "%f", duration_spinbox->value());

    remove_trailing_zeros(annotation.duration);

    annotation.long_duration = edfplus_annotation_get_long_from_number(annotation.duration);
  }
  else
  {
    annotation.duration[0] = 0;

    annotation.long_duration = 0LL;
  }

  strncpy(annotation.description, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annotation.description[MAX_ANNOTATION_LEN] = 0;

  annotation.modified = 1;

  edfplus_annotation_cancel_all_selected_in_dock(annot_list);

  annotation.selected_in_dock = 1;

  edfplus_annotation_add_item(annot_list, annotation);

  mainwindow->annotations_edited = 1;

  file_num = mainwindow->get_filenum((edfhdrblck_t *)(annotation.edfhdr));
  if(file_num >= 0)
  {
    mainwindow->annotations_dock[file_num]->updateList(1);
  }

  update_description_completer();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::annotEditSetOnset(long long onset)
{
  QTime ta;

  if(onset < 0LL)
  {
    onset_daySpinbox->setValue((-(onset)) / TIME_FIXP_SCALING / 86400);

    ta.setHMS(((-(onset)) / TIME_FIXP_SCALING / 3600) % 24,
            (((-(onset)) / TIME_FIXP_SCALING) % 3600) / 60,
            ((-(onset)) / TIME_FIXP_SCALING) % 60,
            ((-(onset)) % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(1);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue(((-onset) % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }
  else
  {
    onset_daySpinbox->setValue(onset / TIME_FIXP_SCALING / 86400);

    ta.setHMS((onset / TIME_FIXP_SCALING / 3600) % 24,
            ((onset / TIME_FIXP_SCALING) % 3600) / 60,
            (onset / TIME_FIXP_SCALING) % 60,
            (onset % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(0);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue((onset % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }

  onset_timeEdit->setTime(ta);
}



long long UI_AnnotationEditwindow::annotEditGetOnset(void)
{
  long long tmp;

  tmp = onset_daySpinbox->value() * 86400;
  tmp += onset_timeEdit->time().hour() * 3600;
  tmp += onset_timeEdit->time().minute() * 60;
  tmp += onset_timeEdit->time().second();
  tmp *= TIME_FIXP_SCALING;
  tmp += (onset_timeEdit->time().msec() * (TIME_FIXP_SCALING / 1000));

  if(mainwindow->annot_editor_highres)
  {
    tmp += onset_us_spinbox->value() * (TIME_FIXP_SCALING / 1000000);
  }

  if(posNegTimebox->currentIndex() == 1)
  {
    tmp *= -1LL;
  }

  return tmp;
}


void UI_AnnotationEditwindow::annotEditSetDuration(long long duration)
{
  duration_spinbox->setValue(((double)duration) / TIME_FIXP_SCALING);
}


void UI_AnnotationEditwindow::annotEditSetDescription(char *descr)
{
  annot_descript_lineEdit->setText(descr);
}


void UI_AnnotationEditwindow::set_edf_header(edfhdrblck_t *e_hdr)
{
  edf_hdr = e_hdr;
}


void UI_AnnotationEditwindow::set_selected_annotation(int annot_nr)
{
  long long l_tmp;

  QTime ta;

  annot_num = annot_nr;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  annotblck_t *annot = edfplus_annotation_get_item(annot_list, annot_num);

  if(annot->edfhdr == NULL)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15886");
    return;
  }

  if(annot->edfhdr != edf_hdr)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15896");
    return;
  }

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->description));

  l_tmp = annot->onset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_FIXP_SCALING / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_FIXP_SCALING / 3600) % 24,
            (((-(l_tmp)) / TIME_FIXP_SCALING) % 3600) / 60,
            ((-(l_tmp)) / TIME_FIXP_SCALING) % 60,
            ((-(l_tmp)) % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(1);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue(((-l_tmp) % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_FIXP_SCALING / 86400);

    ta.setHMS((l_tmp / TIME_FIXP_SCALING / 3600) % 24,
            ((l_tmp / TIME_FIXP_SCALING) % 3600) / 60,
            (l_tmp / TIME_FIXP_SCALING) % 60,
            (l_tmp % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(0);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue((l_tmp % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(true);

  deletebutton->setEnabled(true);
}


void UI_AnnotationEditwindow::set_selected_annotation(annotblck_t *annot)
{
  int n;

  long long l_tmp;

  QTime ta;

  annotlist_t *annot_list = &(edf_hdr->annot_list);

  if(annot->edfhdr == NULL)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15887");
    return;
  }

  if(annot->edfhdr != edf_hdr)
  {
    QMessageBox::critical(dockedit, "Error", "An internal error occurred.\nPlease report this as a bug.\n Error code: 15897");
    return;
  }

  n = edfplus_annotation_get_index(annot_list, annot);

  if(n < 0)  return;

  annot_num = n;

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->description));

  l_tmp = annot->onset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_FIXP_SCALING / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_FIXP_SCALING / 3600) % 24,
            (((-(l_tmp)) / TIME_FIXP_SCALING) % 3600) / 60,
            ((-(l_tmp)) / TIME_FIXP_SCALING) % 60,
            ((-(l_tmp)) % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(1);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue(((-l_tmp) % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_FIXP_SCALING / 86400);

    ta.setHMS((l_tmp / TIME_FIXP_SCALING / 3600) % 24,
            ((l_tmp / TIME_FIXP_SCALING) % 3600) / 60,
            (l_tmp / TIME_FIXP_SCALING) % 60,
            (l_tmp % TIME_FIXP_SCALING) / 10000);

    posNegTimebox->setCurrentIndex(0);

    if(mainwindow->annot_editor_highres)
    {
      onset_us_spinbox->setValue((l_tmp % (TIME_FIXP_SCALING / 1000)) / 10);
    }
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);
}


void UI_AnnotationEditwindow::update_description_completer(void)
{
  int i;

  QStringList string_list;

  QStringListModel *model;

  mainwindow->get_unique_annotations(edf_hdr);

  for(i=0; i<MAX_UNIQUE_ANNOTATIONS; i++)
  {
    if(edf_hdr->unique_annotations_list[i][0] == 0)  break;

    string_list << edf_hdr->unique_annotations_list[i];
  }

  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    if(mainwindow->annot_by_rect_draw_description[i][0] == 0)  break;

    string_list << mainwindow->annot_by_rect_draw_description[i];
  }

  model = (QStringListModel *)(completer->model());
  if(model == NULL)
  {
    model = new QStringListModel(this);
  }

  model->setStringList(string_list);

  completer->setModel(model);
}


void UI_AnnotationEditwindow::user_button_clicked(int button)
{
  if((button < 0) || (button > 7))  return;

  if(mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    if(mainwindow->annot_editor_user_button_stay_on_epoch_boundary)
    {
      mainwindow->pagetime = mainwindow->annot_editor_user_button_page_len;

      mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime /= mainwindow->annot_editor_user_button_epoch_len;
      mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime *= mainwindow->annot_editor_user_button_epoch_len;
    }
  }

  if(mainwindow->annot_editor_user_button_update_annot_description)
  {
    annot_descript_lineEdit->setText(QString::fromUtf8(mainwindow->annot_edit_user_button_name[button]));
  }

  if(mainwindow->annot_editor_user_button_update_annot_onset)
  {
    if(mainwindow->annot_editor_user_button_onset_on_page_middle[button])
    {
      annotEditSetOnset(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime + (mainwindow->pagetime / 2));
    }
    else
    {
      annotEditSetOnset(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime);
    }
  }

  if(mainwindow->annot_editor_user_button_update_annot_duration)
  {
    annotEditSetDuration(mainwindow->annot_editor_user_button_epoch_len);
  }

  createButtonClicked();

  if(mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
    {
      mainwindow->next_epoch(-mainwindow->annot_editor_user_button_epoch_len);
    }
    else
    {
      mainwindow->next_epoch(mainwindow->annot_editor_user_button_epoch_len);
    }
  }
}

void UI_AnnotationEditwindow::user_button_0_clicked()
{
  user_button_clicked(0);
}

void UI_AnnotationEditwindow::user_button_1_clicked()
{
  user_button_clicked(1);
}

void UI_AnnotationEditwindow::user_button_2_clicked()
{
  user_button_clicked(2);
}

void UI_AnnotationEditwindow::user_button_3_clicked()
{
  user_button_clicked(3);
}

void UI_AnnotationEditwindow::user_button_4_clicked()
{
  user_button_clicked(4);
}

void UI_AnnotationEditwindow::user_button_5_clicked()
{
  user_button_clicked(5);
}

void UI_AnnotationEditwindow::user_button_6_clicked()
{
  user_button_clicked(6);
}

void UI_AnnotationEditwindow::user_button_7_clicked()
{
  user_button_clicked(7);
}

void UI_AnnotationEditwindow::process_annot_by_rect_draw(void)
{
  annot_by_rect_draw_menu->exec(QCursor::pos());
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_0_clicked()
{
  annot_by_rect_draw_side_menu_create(0);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_1_clicked()
{
  annot_by_rect_draw_side_menu_create(1);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_2_clicked()
{
  annot_by_rect_draw_side_menu_create(2);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_3_clicked()
{
  annot_by_rect_draw_side_menu_create(3);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_4_clicked()
{
  annot_by_rect_draw_side_menu_create(4);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_5_clicked()
{
  annot_by_rect_draw_side_menu_create(5);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_6_clicked()
{
  annot_by_rect_draw_side_menu_create(6);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_7_clicked()
{
  annot_by_rect_draw_side_menu_create(7);
}

void UI_AnnotationEditwindow::annot_by_rect_draw_side_menu_create(int n)
{
  char str1_512[512]="";

  if(!mainwindow->annot_created_by_rect_draw_active)  return;

  annotEditSetOnset(mainwindow->annot_created_by_rect_draw_onset);

  annotEditSetDuration(mainwindow->annot_created_by_rect_draw_duration);

  strlcpy(str1_512, mainwindow->annot_by_rect_draw_description[n], 512);

  if(mainwindow->annot_created_by_rect_draw_numchan >= 0)
  {
    if((QApplication::keyboardModifiers() == Qt::ControlModifier) &&
       (mainwindow->annot_created_by_rect_draw_active == 2))
    {
      snprintf(str1_512 + strlen(str1_512), 512 - strlen(str1_512), " d:%.2f%s",
               mainwindow->annot_created_by_rect_draw_crosshair_delta,
               mainwindow->signalcomp[mainwindow->annot_created_by_rect_draw_numchan]->physdimension);
    }

    strlcat(str1_512, "@@", 512);

    strlcat(str1_512, mainwindow->signalcomp[mainwindow->annot_created_by_rect_draw_numchan]->signallabel, 512);
  }

  mainwindow->annot_created_by_rect_draw_active = 0;

  trim_spaces(str1_512);

  annotEditSetDescription(str1_512);

  createButtonClicked();
}


void UI_AnnotationEditwindow::set_high_resolution(int val)
{
  if(val)
  {
    onset_us_spinbox->setEnabled(true);
    duration_spinbox->setDecimals(6);
  }
  else
  {
    onset_us_spinbox->setEnabled(false);
    onset_us_spinbox->setValue(0);
    duration_spinbox->setDecimals(3);
  }
}















