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



#include "hypnogram_dialog.h"





UI_hypnogram_window::UI_hypnogram_window(QWidget *w_parent, edfhdrblck_t *e_hdr, int i_num)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  instance_num = i_num;

  edfhdr = e_hdr;

  myobjectDialog = new QDialog;

  QHBoxLayout *hlayout_tmp=NULL;

  if(mainwindow->hypnogram_use_overlays)
  {
    myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 700 * mainwindow->h_scaling);
  }
  else
  {
    myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  }
  myobjectDialog->setWindowTitle("Hypnogram");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  stage_label = new QLabel;
  stage_label->setText("Stage");
  stage_label->setToolTip("The labels as how they will be displayed in the hypnogram");

  annot_label = new QLabel;
  annot_label->setText("Annotation");
  annot_label->setToolTip("The annotations that will be mapped to the labels in the left column");

  stage1_edit = new QLineEdit;
  stage1_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage2_edit = new QLineEdit;
  stage2_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage3_edit = new QLineEdit;
  stage3_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage4_edit = new QLineEdit;
  stage4_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage5_edit = new QLineEdit;
  stage5_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage6_edit = new QLineEdit;
  stage6_edit->setToolTip("The label as how it will be displayed in the hypnogram");

  annot1_edit = new QLineEdit;
  annot1_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot2_edit = new QLineEdit;
  annot2_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot3_edit = new QLineEdit;
  annot3_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot4_edit = new QLineEdit;
  annot4_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot5_edit = new QLineEdit;
  annot5_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot6_edit = new QLineEdit;
  annot6_edit->setToolTip("The annotation that will be mapped to the label in the left column");

  annot1_ov_edit = new QLineEdit;
  annot1_ov_edit->setToolTip("The annotation that will be used for the overlay");
  annot2_ov_edit = new QLineEdit;
  annot2_ov_edit->setToolTip("The annotation that will be used for the overlay");
  annot3_ov_edit = new QLineEdit;
  annot3_ov_edit->setToolTip("The annotation that will be used for the overlay");
  annot4_ov_edit = new QLineEdit;
  annot4_ov_edit->setToolTip("The annotation that will be used for the overlay");
  annot5_ov_edit = new QLineEdit;
  annot5_ov_edit->setToolTip("The annotation that will be used for the overlay");
  annot6_ov_edit = new QLineEdit;
  annot6_ov_edit->setToolTip("The annotation that will be used for the overlay");

  ov_color_button1 = new SpecialButton;
  ov_color_button1->setColor(mainwindow->hypnogram_annot_ov_color[0]);
  ov_color_button1->setToolTip("Click to change the overlay color");
  ov_color_button2 = new SpecialButton;
  ov_color_button2->setColor(mainwindow->hypnogram_annot_ov_color[1]);
  ov_color_button2->setToolTip("Click to change the overlay color");
  ov_color_button3 = new SpecialButton;
  ov_color_button3->setColor(mainwindow->hypnogram_annot_ov_color[2]);
  ov_color_button3->setToolTip("Click to change the overlay color");
  ov_color_button4 = new SpecialButton;
  ov_color_button4->setColor(mainwindow->hypnogram_annot_ov_color[3]);
  ov_color_button4->setToolTip("Click to change the overlay color");
  ov_color_button5 = new SpecialButton;
  ov_color_button5->setColor(mainwindow->hypnogram_annot_ov_color[4]);
  ov_color_button5->setToolTip("Click to change the overlay color");
  ov_color_button6 = new SpecialButton;
  ov_color_button6->setColor(mainwindow->hypnogram_annot_ov_color[5]);
  ov_color_button6->setToolTip("Click to change the overlay color");

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  stage1_edit->setText(mainwindow->hypnogram_stage_name[0]);
  stage2_edit->setText(mainwindow->hypnogram_stage_name[1]);
  stage3_edit->setText(mainwindow->hypnogram_stage_name[2]);
  stage4_edit->setText(mainwindow->hypnogram_stage_name[3]);
  stage5_edit->setText(mainwindow->hypnogram_stage_name[4]);
  stage6_edit->setText(mainwindow->hypnogram_stage_name[5]);

  annot1_edit->setText(mainwindow->hypnogram_annot_name[0]);
  annot2_edit->setText(mainwindow->hypnogram_annot_name[1]);
  annot3_edit->setText(mainwindow->hypnogram_annot_name[2]);
  annot4_edit->setText(mainwindow->hypnogram_annot_name[3]);
  annot5_edit->setText(mainwindow->hypnogram_annot_name[4]);
  annot6_edit->setText(mainwindow->hypnogram_annot_name[5]);

  annot1_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[0]);
  annot2_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[1]);
  annot3_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[2]);
  annot4_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[3]);
  annot5_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[4]);
  annot6_ov_edit->setText(mainwindow->hypnogram_annot_ov_name[5]);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(stage_label);
  vlayout2->addWidget(stage1_edit);
  vlayout2->addWidget(stage2_edit);
  vlayout2->addWidget(stage3_edit);
  vlayout2->addWidget(stage4_edit);
  vlayout2->addWidget(stage5_edit);
  vlayout2->addWidget(stage6_edit);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(annot_label);
  vlayout3->addWidget(annot1_edit);
  vlayout3->addWidget(annot2_edit);
  vlayout3->addWidget(annot3_edit);
  vlayout3->addWidget(annot4_edit);
  vlayout3->addWidget(annot5_edit);
  vlayout3->addWidget(annot6_edit);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(vlayout2);
  hlayout2->addStretch(100);
  hlayout2->addLayout(vlayout3);
  hlayout2->addStretch(1000);

  QFormLayout *flayout1 = new QFormLayout;
  flayout1->setSpacing(20);

  flayout1->addRow(" ", (QWidget *)NULL);

  use_epoch_len_checkbox = new QCheckBox;
  use_epoch_len_checkbox->setTristate(false);
  use_epoch_len_checkbox->setToolTip("If enabled, it will draw according to the duration of the annotation.\n"
                                     "If disabled, it will draw from onset to the next onset.");
  if(mainwindow->hypnogram_use_epoch_len)
  {
    use_epoch_len_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    use_epoch_len_checkbox->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(use_epoch_len_checkbox);
  hlayout_tmp->addStretch(1000);
  flayout1->addRow("Use annotations' duration for epoch length", hlayout_tmp);
  flayout1->labelForField(hlayout_tmp)->setToolTip("If enabled, it will draw according to the duration of the annotation.\n"
                                                   "If disabled, it will draw from onset to the next onset.");
  QObject::connect(use_epoch_len_checkbox, SIGNAL(stateChanged(int)), this, SLOT(use_epoch_len_checkbox_changed(int)));

  use_overlays_checkbox = new QCheckBox;
  use_overlays_checkbox->setTristate(false);
  if(mainwindow->hypnogram_use_overlays)
  {
    use_overlays_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    use_overlays_checkbox->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(use_overlays_checkbox);
  hlayout_tmp->addStretch(1000);
  flayout1->addRow("Add overlays", hlayout_tmp);
  QObject::connect(use_overlays_checkbox, SIGNAL(stateChanged(int)), this, SLOT(use_overlays_checkbox_changed(int)));

  QFormLayout *flayout2 = new QFormLayout;
  flayout2->setSpacing(20);

  flayout2->addRow(" ", (QWidget *)NULL);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot1_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button1);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot2_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button2);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot3_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button3);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot4_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button4);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot5_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button5);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annot6_ov_edit);
  hlayout_tmp->addStretch(100);
  hlayout_tmp->addWidget(ov_color_button6);
  hlayout_tmp->addStretch(1000);
  flayout2->addRow("Overlay", hlayout_tmp);

  flayout2widget = new QWidget;
  flayout2widget->setLayout(flayout2);
  if(mainwindow->hypnogram_use_overlays)
  {
    flayout2widget->setVisible(true);
  }
  else
  {
    flayout2widget->setVisible(false);
  }

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(close_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(default_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(start_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(flayout1);
  vlayout1->addWidget(flayout2widget);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(30);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(close_button,     SIGNAL(clicked()),                myobjectDialog, SLOT(close()));
  QObject::connect(default_button,   SIGNAL(clicked()),                this,           SLOT(default_button_clicked()));
  QObject::connect(start_button,     SIGNAL(clicked()),                this,           SLOT(start_button_clicked()));
  QObject::connect(ov_color_button1, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button1_clicked(SpecialButton *)));
  QObject::connect(ov_color_button2, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button2_clicked(SpecialButton *)));
  QObject::connect(ov_color_button3, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button3_clicked(SpecialButton *)));
  QObject::connect(ov_color_button4, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button4_clicked(SpecialButton *)));
  QObject::connect(ov_color_button5, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button5_clicked(SpecialButton *)));
  QObject::connect(ov_color_button6, SIGNAL(clicked(SpecialButton *)), this,           SLOT(ov_color_button6_clicked(SpecialButton *)));

  myobjectDialog->exec();
}


void UI_hypnogram_window::use_overlays_checkbox_changed(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->hypnogram_use_overlays = 1;

    flayout2widget->setVisible(true);

    myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 700 * mainwindow->h_scaling);
  }
  else
  {
    mainwindow->hypnogram_use_overlays = 0;

    flayout2widget->setVisible(false);

    myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  }

  myobjectDialog->adjustSize();
}


void UI_hypnogram_window::use_epoch_len_checkbox_changed(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->hypnogram_use_epoch_len = 1;
  }
  else
  {
    mainwindow->hypnogram_use_epoch_len = 0;
  }
}


void UI_hypnogram_window::default_button_clicked()
{
  stage1_edit->setText("W");
  stage2_edit->setText("R");
  stage3_edit->setText("N1");
  stage4_edit->setText("N2");
  stage5_edit->setText("N3");
  stage6_edit->setText("N4");

  annot1_edit->setText("W");
  annot2_edit->setText("R");
  annot3_edit->setText("N1");
  annot4_edit->setText("N2");
  annot5_edit->setText("N3");
  annot6_edit->setText("N4");

  annot1_ov_edit->clear();
  annot2_ov_edit->clear();
  annot3_ov_edit->clear();
  annot4_ov_edit->clear();
  annot5_ov_edit->clear();
  annot6_ov_edit->clear();

  ov_color_button1->setColor(QColor(0, 128, 0, 32));
  ov_color_button2->setColor(QColor(0, 0, 128, 32));
  ov_color_button3->setColor(QColor(128, 128, 0, 32));
  ov_color_button4->setColor(QColor(128, 0, 128, 32));
  ov_color_button5->setColor(QColor(0, 64, 192, 32));
  ov_color_button6->setColor(QColor(128, 192, 64, 32));

  use_overlays_checkbox->setCheckState(Qt::Unchecked);
}


void UI_hypnogram_window::start_button_clicked()
{
  int i;

  hypnogram_dock_param_t dock_param;

  memset(&dock_param, 0, sizeof(hypnogram_dock_param_t));

  dock_param.instance_num = instance_num;

  dock_param.edfhdr = edfhdr;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.stage_name[0], stage1_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.stage_name[1], stage2_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.stage_name[2], stage3_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.stage_name[3], stage4_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.stage_name[4], stage5_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.stage_name[5], stage6_edit->text().toUtf8().data(), 32);

  for(i=0; i<HYPNOGRAM_STAGENUM; i++)
  {
    trim_spaces(dock_param.stage_name[i]);

    strlcpy(mainwindow->hypnogram_stage_name[i], dock_param.stage_name[i], 32);
  }

  strlcpy(dock_param.annot_name[0], annot1_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_name[1], annot2_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_name[2], annot3_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_name[3], annot4_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_name[4], annot5_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_name[5], annot6_edit->text().toUtf8().data(), 32);

  for(i=0; i<HYPNOGRAM_STAGENUM; i++)
  {
    trim_spaces(dock_param.annot_name[i]);

    strlcpy(mainwindow->hypnogram_annot_name[i], dock_param.annot_name[i], 32);
  }

  strlcpy(dock_param.annot_ov_name[0], annot1_ov_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_ov_name[1], annot2_ov_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_ov_name[2], annot3_ov_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_ov_name[3], annot4_ov_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_ov_name[4], annot5_ov_edit->text().toUtf8().data(), 32);
  strlcpy(dock_param.annot_ov_name[5], annot6_ov_edit->text().toUtf8().data(), 32);

  mainwindow->hypnogram_annot_ov_color[0] = ov_color_button1->color();
  mainwindow->hypnogram_annot_ov_color[1] = ov_color_button2->color();
  mainwindow->hypnogram_annot_ov_color[2] = ov_color_button3->color();
  mainwindow->hypnogram_annot_ov_color[3] = ov_color_button4->color();
  mainwindow->hypnogram_annot_ov_color[4] = ov_color_button5->color();
  mainwindow->hypnogram_annot_ov_color[5] = ov_color_button6->color();

  for(i=0; i<HYPNOGRAM_OVNUM; i++)
  {
    trim_spaces(dock_param.annot_ov_name[i]);

    strlcpy(mainwindow->hypnogram_annot_ov_name[i], dock_param.annot_ov_name[i], 32);

    dock_param.annot_ov_color[i].r = mainwindow->hypnogram_annot_ov_color[i].red();
    dock_param.annot_ov_color[i].g = mainwindow->hypnogram_annot_ov_color[i].green();
    dock_param.annot_ov_color[i].b = mainwindow->hypnogram_annot_ov_color[i].blue();
    dock_param.annot_ov_color[i].a = mainwindow->hypnogram_annot_ov_color[i].alpha();
  }

  dock_param.use_epoch_len = mainwindow->hypnogram_use_epoch_len;

  dock_param.use_overlays = mainwindow->hypnogram_use_overlays;

  mainwindow->hypnogram_dock[instance_num] = new UI_hypnogram_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  mainwindow->insertToolBarBreak(mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  edfhdr->hypnogram_idx[instance_num] = instance_num + 1;

  QObject::connect(mainwindow, SIGNAL(annot_docklist_changed()), mainwindow->hypnogram_dock[instance_num], SLOT(update_curve()));

  myobjectDialog->close();
}


void UI_hypnogram_window::ov_color_button1_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[0], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[0] = temp;

    b->setColor(temp);
  }
}


void UI_hypnogram_window::ov_color_button2_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[1], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[1] = temp;

    b->setColor(temp);
  }
}


void UI_hypnogram_window::ov_color_button3_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[2], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[2] = temp;

    b->setColor(temp);
  }
}


void UI_hypnogram_window::ov_color_button4_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[3], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[3] = temp;

    b->setColor(temp);
  }
}


void UI_hypnogram_window::ov_color_button5_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[4], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[4] = temp;

    b->setColor(temp);
  }
}


void UI_hypnogram_window::ov_color_button6_clicked(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->hypnogram_annot_ov_color[5], myobjectDialog, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->hypnogram_annot_ov_color[5] = temp;

    b->setColor(temp);
  }
}


























