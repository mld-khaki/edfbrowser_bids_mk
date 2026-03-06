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




#include "active_file_chooser.h"



UI_activeFileChooserWindow::UI_activeFileChooserWindow(int *file_nr, QWidget *mainwndw)
{
  int i;

  mainwindow = (UI_Mainwindow *)mainwndw;

  file_num = file_nr;

  *file_num = -1;

  chooserDialog = new QDialog;

  chooserDialog->setMinimumSize(600, 150);
  chooserDialog->setWindowTitle("Choose file");
  chooserDialog->setModal(true);
  chooserDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  chooserDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  okButton = new QPushButton;
  okButton->setText("OK");

  cancelButton = new QPushButton;
  cancelButton->setText("Cancel");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(okButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(cancelButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist, 1000);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout1);

  chooserDialog->setLayout(vlayout1);

  QObject::connect(cancelButton, SIGNAL(clicked()), chooserDialog, SLOT(close()));
  QObject::connect(okButton,     SIGNAL(clicked()), this,          SLOT(okButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  chooserDialog->exec();
}


void UI_activeFileChooserWindow::okButtonClicked()
{
  if(!mainwindow->files_open)
  {
    *file_num = -1;

    chooserDialog->close();

    return;
  }

  *file_num = filelist->currentRow();

  chooserDialog->close();
}














