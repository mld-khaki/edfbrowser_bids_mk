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


#include "math_f_dialog.h"



UI_MATH_func_dialog::UI_MATH_func_dialog(QWidget *w_parent)
{
  int i, n;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  mainwindow = (UI_Mainwindow *)w_parent;

  mathdialog = new QDialog;
  mathdialog->setMinimumSize(300 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  mathdialog->setWindowTitle("Add a math function");
  mathdialog->setModal(true);
  mathdialog->setAttribute(Qt::WA_DeleteOnClose, true);
  mathdialog->setSizeGripEnabled(true);

  listlabel = new QLabel;
  listlabel->setText("Select signals:");

  list = new QListWidget;
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  func_bgroup = new QGroupBox("Function");
  when_bgroup = new QGroupBox("When");

  square_rbutton = new QRadioButton("Square");
  sqrt_rbutton = new QRadioButton("Square Root");
  abs_rbutton = new QRadioButton("Absolute");
  pk_hold_rbutton = new QRadioButton("Peak Hold");

  before_rbutton = new QRadioButton("Before filtering");
  after_rbutton = new QRadioButton("After filtering");

  pk_hold_spinbox = new QSpinBox;
  pk_hold_spinbox->setRange(1, 100000);
  pk_hold_spinbox->setSuffix(" smpls");
  pk_hold_spinbox->setValue(100);
  pk_hold_spinbox->setEnabled(false);

  close_button = new QPushButton;
  close_button->setText("&Close");

  apply_button = new QPushButton;
  apply_button->setText("&Apply");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      item->setText(mainwindow->signalcomp[i]->alias);
    }
    else
    {
      item->setText(mainwindow->signalcomp[i]->signallabel);
    }
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }

  n = list->count();

  for(i=0; i<n; i++)
  {
    item = list->item(i);

    item->setSelected(true);
  }

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addWidget(square_rbutton);
  vlayout4->addWidget(sqrt_rbutton);
  vlayout4->addWidget(abs_rbutton);
  vlayout4->addWidget(pk_hold_rbutton);
  vlayout4->addWidget(pk_hold_spinbox);
  vlayout4->addStretch(1);

  func_bgroup->setLayout(vlayout4);

  QVBoxLayout *vlayout5 = new QVBoxLayout;
  vlayout5->addWidget(before_rbutton);
  vlayout5->addWidget(after_rbutton);
  vlayout5->addStretch(1);

  when_bgroup->setLayout(vlayout5);

  square_rbutton->setChecked(true);

  after_rbutton->setChecked(true);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addStretch(100);
  vlayout2->addWidget(func_bgroup);
  vlayout2->addStretch(100);
  vlayout2->addWidget(when_bgroup);
  vlayout2->addStretch(100);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(listlabel);
  vlayout3->addWidget(list, 1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(vlayout2);
  hlayout2->addSpacing(40);
  hlayout2->addLayout(vlayout3);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(close_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(apply_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  mathdialog->setLayout(vlayout1);

  QObject::connect(apply_button,    SIGNAL(clicked()),     this,       SLOT(apply_button_clicked()));
  QObject::connect(pk_hold_rbutton, SIGNAL(toggled(bool)), this,       SLOT(pk_hold_rbutton_clicked(bool)));
  QObject::connect(close_button,    SIGNAL(clicked()),     mathdialog, SLOT(close()));

  mathdialog->exec();
}


void UI_MATH_func_dialog::pk_hold_rbutton_clicked(bool c)
{
  pk_hold_spinbox->setEnabled(c);
}


void UI_MATH_func_dialog::apply_button_clicked()
{
  int i, s, n, functype=MATH_FUNC_NONE, before=0;

  char str1_1024[1024]="";

  QListWidgetItem *item=NULL;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  if(sqrt_rbutton->isChecked() == true)
  {
    functype = MATH_FUNC_SQRT;
  }
  else if(square_rbutton->isChecked() == true)
    {
      functype = MATH_FUNC_SQUARE;
    }
    else if(abs_rbutton->isChecked() == true)
      {
        functype = MATH_FUNC_ABS;
      }
      else if(pk_hold_rbutton->isChecked() == true)
        {
          functype = MATH_FUNC_PK_HOLD;
        }
        else
        {
          functype = MATH_FUNC_NONE;
        }

  if(before_rbutton->isChecked() == true)
  {
    before = 1;
  }
  else
  {
    before = 0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    if(((mainwindow->signalcomp[s]->math_func_cnt_before >= MAXMATHFUNCS) && before) ||
       ((mainwindow->signalcomp[s]->math_func_cnt_after >= MAXMATHFUNCS) && (!before)))
    {
      snprintf(str1_1024, 1024, "The maximum number of math functions has been reached for signal %s", mainwindow->signalcomp[s]->signallabel);
      QMessageBox::critical(mathdialog, "Error", str1_1024);
      break;
    }

    if(before)
    {
      mainwindow->signalcomp[s]->math_func_before[mainwindow->signalcomp[s]->math_func_cnt_before] = create_math_func(functype, pk_hold_spinbox->value());
      if(mainwindow->signalcomp[s]->math_func_before[mainwindow->signalcomp[s]->math_func_cnt_before] == NULL)
      {
        QMessageBox::critical(mathdialog, "Error", "An error occurred when creating a new math function");
        break;
      }

      mainwindow->signalcomp[s]->math_func_cnt_before++;
    }
    else
    {
      mainwindow->signalcomp[s]->math_func_after[mainwindow->signalcomp[s]->math_func_cnt_after] = create_math_func(functype, pk_hold_spinbox->value());
      if(mainwindow->signalcomp[s]->math_func_after[mainwindow->signalcomp[s]->math_func_cnt_after] == NULL)
      {
        QMessageBox::critical(mathdialog, "Error", "An error occurred when creating a new math function");
        break;
      }

      mainwindow->signalcomp[s]->math_func_cnt_after++;
    }
  }

  mainwindow->setup_viewbuf();
}















