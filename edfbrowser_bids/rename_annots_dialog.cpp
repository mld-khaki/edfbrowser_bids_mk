/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2021 - 2024 Teunis van Beelen
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



#include "rename_annots_dialog.h"


UI_rename_annots_dialog::UI_rename_annots_dialog(QWidget *w_parent, int delete_ann)
{
  int n;

  mainwindow = (UI_Mainwindow *)w_parent;

  delete_annots = delete_ann;

  replacements_cnt = 0;

  edfhdrblck_t *hdr;

  hdr = mainwindow->edfheaderlist[0];

  annotblck_t *annot=NULL;

  rename_dialog = new QDialog(mainwindow);
  rename_dialog->setMinimumSize(300 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  if(delete_annots)
  {
    rename_dialog->setWindowTitle("Delete annotations");
  }
  else
  {
    rename_dialog->setWindowTitle("Rename annotations");
  }
  rename_dialog->setModal(true);
  rename_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  rename_dialog->setSizeGripEnabled(true);

  line_edit1 = new QLineEdit;
  line_edit1->setMaxLength(MAX_ANNOTATION_LEN);
  if(!delete_annots)
  {
    line_edit2 = new QLineEdit;
    line_edit2->setMaxLength(MAX_ANNOTATION_LEN);
  }

  cancel_button = new QPushButton;
  cancel_button->setText("Close");
  rename_button = new QPushButton;
  if(delete_annots)
  {
    rename_button->setText("Delete");
  }
  else
  {
    rename_button->setText("Rename");
  }

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow(" ", (QWidget *)NULL);
  if(delete_annots)
  {
    flayout->addRow("Delete:", line_edit1);
    flayout->addRow(" ", (QWidget *)NULL);
  }
  else
  {
    flayout->addRow("Replace:", line_edit1);
    flayout->addRow(" ", (QWidget *)NULL);
    flayout->addRow("with:", line_edit2);
    flayout->addRow(" ", (QWidget *)NULL);
  }

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addSpacing(10);
  hlayout1->addWidget(cancel_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(rename_button);
  hlayout1->addSpacing(10);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);
  vlayout1->addSpacing(10);

  if(mainwindow->annotations_dock[0] != NULL)
  {
    n = mainwindow->annotations_dock[0]->get_last_pressed_row();
    if(n >= 0)
    {
      annot = edfplus_annotation_get_item_visible_only(&hdr->annot_list, n);
      if(annot != NULL)
      {
        line_edit1->setText(QString::fromUtf8(annot->description));
      }
    }
  }

  QObject::connect(cancel_button, SIGNAL(clicked()), rename_dialog, SLOT(close()));
  if(delete_annots)
  {
    QObject::connect(rename_button, SIGNAL(clicked()), this, SLOT(delete_all_func()));
  }
  else
  {
    QObject::connect(rename_button, SIGNAL(clicked()), this, SLOT(rename_all_func()));
  }

  rename_dialog->setLayout(vlayout1);
  rename_dialog->exec();
}


void UI_rename_annots_dialog::delete_all_func()
{
  int backup = 0;

  char str1_4096[4096]="";

  edfhdrblck_t *hdr;

  hdr = mainwindow->edfheaderlist[0];

  if(line_edit1->text().size() < 1)
  {
    QMessageBox::critical(rename_dialog, "Error", "Need a description string.");
    return;
  }

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&hdr->annot_list);

    backup = 1;
  }

  replacements_cnt = edfplus_annotation_delete_multiple(&hdr->annot_list, line_edit1->text().toUtf8().data());

  if(replacements_cnt > 0)
  {
    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[0]->updateList(0);

    mainwindow->save_act->setEnabled(true);

    snprintf(str1_4096, 4096, "Deleted %i annotations.", replacements_cnt);

    QMessageBox::information(rename_dialog, "Ready", str1_4096);
  }
  else
  {
    if(backup)
    {
      if(mainwindow->annotationlist_backup != NULL)
      {
        edfplus_annotation_empty_list(&hdr->annot_list);

        hdr->annot_list = *mainwindow->annotationlist_backup;

        free(mainwindow->annotationlist_backup);

        mainwindow->annotationlist_backup = NULL;
      }
    }

    QMessageBox::information(rename_dialog, "Ready", "There are no annotations with the given description.");
  }
}


void UI_rename_annots_dialog::rename_all_func()
{
  int backup = 0;

  char str1_4096[4096]="";

  edfhdrblck_t *hdr;

  hdr = mainwindow->edfheaderlist[0];

  if(line_edit1->text().size() < 1)
  {
    QMessageBox::critical(rename_dialog, "Error", "Need a description string.");
    return;
  }

  if(line_edit2->text().size() < 1)
  {
    QMessageBox::critical(rename_dialog, "Error", "Need a description string.");
    return;
  }

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&hdr->annot_list);

    backup = 1;
  }

  replacements_cnt = edfplus_annotation_rename_multiple(&hdr->annot_list, line_edit1->text().toUtf8().data(), line_edit2->text().toUtf8().data());

  if(replacements_cnt > 0)
  {
    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[0]->updateList(0);

    mainwindow->save_act->setEnabled(true);

    snprintf(str1_4096, 4096, "Renamed %i annotations.", replacements_cnt);

    QMessageBox::information(rename_dialog, "Ready", str1_4096);
  }
  else
  {
    if(backup)
    {
      if(mainwindow->annotationlist_backup != NULL)
      {
        edfplus_annotation_empty_list(&hdr->annot_list);

        hdr->annot_list = *mainwindow->annotationlist_backup;

        free(mainwindow->annotationlist_backup);

        mainwindow->annotationlist_backup = NULL;
      }
    }

    QMessageBox::information(rename_dialog, "Ready", "There are no annotations with the given description.");
  }
}






























