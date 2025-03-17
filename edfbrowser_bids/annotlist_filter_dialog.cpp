/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 - 2024 Teunis van Beelen
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




#include "annotlist_filter_dialog.h"



UI_AnnotFilterWindow::UI_AnnotFilterWindow(QWidget *w_parent, annotblck_t *annot, annotfilter_t *filter_p, edfhdrblck_t *e_hdr)
{
  int n;

  mainwindow = (UI_Mainwindow *)w_parent;

  annot_filter_dialog = new QDialog(w_parent);

  edf_hdr = e_hdr;

  annot_list = &(edf_hdr->annot_list);

  filter_params = filter_p;

  n = mainwindow->get_filenum(edf_hdr);

  annots_dock = mainwindow->annotations_dock[n];

  annot_filter_dialog->setMinimumSize(350, 250);
  annot_filter_dialog->setWindowTitle("Filter annotations");
  annot_filter_dialog->setModal(true);
  annot_filter_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  annot_filter_dialog->setSizeGripEnabled(true);

  annot_lineedit = new QLineEdit;
  annot_lineedit->setReadOnly(true);
  annot_lineedit->setToolTip("Label of the annotation to be filtered");

  t1_spinbox = new QSpinBox;
  t1_spinbox->setRange(1, 500000);
  t1_spinbox->setSuffix(" mSec");
  t1_spinbox->setValue(filter_params->tmin);
  t1_spinbox->setToolTip("Minimum interval between two consegutive annotations");

  t2_spinbox = new QSpinBox;
  t2_spinbox->setRange(1, 500000);
  t2_spinbox->setSuffix(" mSec");
  t2_spinbox->setValue(filter_params->tmax);
  t2_spinbox->setToolTip("Maximum interval between two consegutive annotations");

  invert_checkbox = new QCheckBox;
  invert_checkbox->setTristate(false);
  invert_checkbox->setToolTip("Invert the filter result");
  if(filter_params->invert)
  {
    invert_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    invert_checkbox->setCheckState(Qt::Unchecked);
  }

  hide_other_checkbox = new QCheckBox;
  hide_other_checkbox->setTristate(false);
  hide_other_checkbox->setToolTip("Hide annotations with a different label");
  if(filter_params->hide_other)
  {
    hide_other_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    hide_other_checkbox->setCheckState(Qt::Unchecked);
  }

  hide_in_list_checkbox = new QCheckBox;
  hide_in_list_checkbox->setTristate(false);
  hide_in_list_checkbox->setToolTip("If enabled, filter affects the annotationlist only, not the annotation markers in the signal window");
  if(filter_params->hide_in_list_only)
  {
    hide_in_list_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    hide_in_list_checkbox->setCheckState(Qt::Unchecked);
  }

  ApplyButton = new QPushButton;
  ApplyButton->setText("Apply");

  UndoButton = new QPushButton;
  UndoButton->setText("Undo");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  sel_annot_ptr = annot;
  strlcpy(sel_annot_str, sel_annot_ptr->description, MAX_ANNOTATION_LEN + 1);
  trim_spaces(sel_annot_str);

  annot_lineedit->setText(sel_annot_str);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Filter annotation:", annot_lineedit);
  flayout->addRow("Minimum interval:", t1_spinbox);
  flayout->addRow("Maximum interval:", t2_spinbox);
  flayout->addRow("Invert", invert_checkbox);
  flayout->addRow("Hide other annotations", hide_other_checkbox);
  flayout->addRow("Filter list only", hide_in_list_checkbox);
  flayout->labelForField(hide_in_list_checkbox)->setToolTip("If enabled, filter affects the annotationlist only, not the annotation markers in the signal window");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(ApplyButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(UndoButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout1);

  annot_filter_dialog->setLayout(vlayout1);

  QObject::connect(CloseButton, SIGNAL(clicked()), annot_filter_dialog, SLOT(close()));
  QObject::connect(ApplyButton, SIGNAL(clicked()), this,                SLOT(apply_filter()));
  QObject::connect(UndoButton,  SIGNAL(clicked()), this,                SLOT(undo_filter()));

  annot_filter_dialog->exec();
}


void UI_AnnotFilterWindow::undo_filter()
{
  int i, sz;

  annotblck_t *annot;

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    annot->hidden = 0;
  }

  annots_dock->updateList(0);

  mainwindow->maincurve->update();
}


void UI_AnnotFilterWindow::apply_filter()
{
  int i,
      sz,
      is_set=0,
      t1_val,
      t2_val;

  long long t1,
            t_diff,
            t_min,
            t_max;

  char annot_str[MAX_ANNOTATION_LEN + 1];

  annotblck_t *annot, *annot_before;

  if(invert_checkbox->checkState() == Qt::Checked)
  {
    filter_params->invert = 1;
  }
  else
  {
    filter_params->invert = 0;
  }

  if(hide_in_list_checkbox->checkState() == Qt::Checked)
  {
    filter_params->hide_in_list_only = 1;
  }
  else
  {
    filter_params->hide_in_list_only = 0;
  }

  if(hide_other_checkbox->checkState() == Qt::Checked)
  {
    filter_params->hide_other = 1;
  }
  else
  {
    filter_params->hide_other = 0;
  }

  t1_val = t1_spinbox->value();

  t2_val = t2_spinbox->value();

  if(t1_val >= t2_val)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Minimum interval must be less than\n"
                                                              "maximum interval");
    messagewindow.exec();
    return;
  }

  filter_params->tmin = t1_val;

  filter_params->tmax = t2_val;

  t_min = filter_params->tmin * (TIME_FIXP_SCALING / 1000LL);

  t_max = filter_params->tmax * (TIME_FIXP_SCALING / 1000LL);

  sz = edfplus_annotation_size(annot_list);

  for(i=0; i<sz; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(annot_str, annot->description, MAX_ANNOTATION_LEN + 1);
    trim_spaces(sel_annot_str);

    if(strcmp(annot_str, sel_annot_str))
    {
      if(filter_params->hide_other)
      {
        annot->hidden = 1;
      }
      else
      {
        annot->hidden = 0;
      }

      continue;
    }

    if(is_set == 0)
    {
      is_set = 1;

      t1 = annot->onset;

      annot->hidden = 1;

      annot_before = annot;

      continue;
    }

    t_diff = annot->onset - t1;

    t1 = annot->onset;

    if(filter_params->invert)
    {
      if((t_diff < t_min) || (t_diff > t_max))
      {
        annot->hidden = 1;
      }
      else
      {
        annot->hidden = 0;

        annot_before->hidden = 0;
      }
    }
    else
    {
      if((t_diff >= t_min) && (t_diff <= t_max))
      {
        annot->hidden = 1;
      }
      else
      {
        annot->hidden = 0;

        annot_before->hidden = 0;
      }
    }

    annot_before = annot;
  }

  annots_dock->updateList(0);

  mainwindow->maincurve->update();
}












