/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2024 Teunis van Beelen
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



#include "edit_predefined_mtg.h"



UI_edit_predefined_mtg_window::UI_edit_predefined_mtg_window(QWidget *w_parent)
{
  int i;

  char str1_1200[1200]={""};

  mainwindow = (UI_Mainwindow *)w_parent;

  edit_predefined_mtg_Dialog = new QDialog;
  edit_predefined_mtg_Dialog->setMinimumSize(800 * mainwindow->w_scaling, 370 * mainwindow->h_scaling);
  edit_predefined_mtg_Dialog->setWindowTitle("Edit key-bindings for montages");
  edit_predefined_mtg_Dialog->setModal(true);
  edit_predefined_mtg_Dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  mtg_path_list = new QListWidget;
  mtg_path_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  mtg_path_list->setSelectionMode(QAbstractItemView::SingleSelection);
  mtg_path_list->setSpacing(1);
  for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
  {
    snprintf(str1_1200, 1200, "F%i : ", i + 1);

    if(mainwindow->predefined_mtg_path[i][0] != 0)
    {
      strlcat(str1_1200, &(mainwindow->predefined_mtg_path[i][0]), 1200);
    }

    new QListWidgetItem(QString::fromLocal8Bit(str1_1200), mtg_path_list);
  }

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(mtg_path_list, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  edit_predefined_mtg_Dialog->setLayout(vlayout1);

  QObject::connect(CloseButton,   SIGNAL(clicked()),                      edit_predefined_mtg_Dialog,    SLOT(close()));
  QObject::connect(mtg_path_list, SIGNAL(itemClicked(QListWidgetItem *)), this,                          SLOT(rowClicked(QListWidgetItem *)));

  edit_predefined_mtg_Dialog->exec();
}


void UI_edit_predefined_mtg_window::rowClicked(QListWidgetItem *item)
{
  row = mtg_path_list->row(item);

  listItem = item;

  dialog = new QDialog(edit_predefined_mtg_Dialog);
  dialog->setMinimumSize(140 * mainwindow->w_scaling, 135 * mainwindow->h_scaling);
  dialog->setWindowTitle("Entry");
  dialog->setModal(true);
  dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  button1 = new QPushButton(dialog);
  button1->setGeometry(20, 20, 100, 25);
  button1->setText("Edit");

  button2 = new QPushButton(dialog);
  button2->setGeometry(20, 55, 100, 25);
  button2->setText("Remove");

  button3 = new QPushButton(dialog);
  button3->setGeometry(20, 90, 100, 25);
  button3->setText("Cancel");

  QObject::connect(button1, SIGNAL(clicked()), this,   SLOT(adEntry()));
  QObject::connect(button2, SIGNAL(clicked()), this,   SLOT(removeEntry()));
  QObject::connect(button3, SIGNAL(clicked()), dialog, SLOT(close()));

  dialog->exec();
}


void UI_edit_predefined_mtg_window::adEntry()
{
  char str[1200],
       path[MAX_PATH_LENGTH];


  dialog->close();

  strlcpy(path, QFileDialog::getOpenFileName(0, "Select montage", QString::fromLocal8Bit(mainwindow->recent_montagedir), "MTG files (*.mtg *.MTG)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_montagedir, path, MAX_PATH_LENGTH);

  strlcpy(&(mainwindow->predefined_mtg_path[row][0]), path, MAX_PATH_LENGTH);

  mainwindow->load_predefined_mtg_act[row]->setText(QString::fromLocal8Bit(mainwindow->predefined_mtg_path[row]));

  snprintf(str, 1200, "F%i : ", row + 1);
  strlcat(str, path, 1200);

  listItem->setText(QString::fromLocal8Bit(str));
}


void UI_edit_predefined_mtg_window::removeEntry()
{
  char str1_32[32];

  dialog->close();

  mainwindow->predefined_mtg_path[row][0] = 0;
  mainwindow->load_predefined_mtg_act[row]->setText("Empty");

  snprintf(str1_32, 32, "F%i :", row + 1);

  listItem->setText(str1_32);
}

















