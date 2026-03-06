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




#include "jump_dialog.h"



UI_JumpMenuDialog::UI_JumpMenuDialog(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  starttime = 0LL;

  jump_dialog = new QDialog(w_parent);

  jump_dialog->setMinimumSize(435 * mainwindow->w_scaling, 200 * mainwindow->h_scaling);
  jump_dialog->setWindowTitle("Jump to");
  jump_dialog->setModal(true);
  jump_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  daybox1 = new QSpinBox;
  daybox1->setRange(0, 30);
  daybox1->setEnabled(false);

  daybox2 = new QSpinBox;
  daybox2->setRange(0, 30);
  daybox2->setEnabled(false);

  timeEdit1 = new QTimeEdit;
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit1->setEnabled(false);

  timeEdit2 = new QTimeEdit;
  timeEdit2->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit2->setEnabled(false);

  jumpButton = new QPushButton;
  jumpButton->setText("Jump");
  jumpButton->setEnabled(false);

  CloseButton = new QPushButton;
  CloseButton->setText("Cancel");

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(new QLabel("dd"));
  hlayout2->addWidget(new QLabel("hh:mm:ss.ms"));

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(daybox1);
  hlayout3->addWidget(timeEdit1);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(daybox2);
  hlayout4->addWidget(timeEdit2);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow(" ", hlayout2);
  flayout->addRow("Offset from start of recording", hlayout3);
  flayout->addRow("Absolute time", hlayout4);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addLayout(flayout);
  hlayout5->addStretch(1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(jumpButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout5);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  jump_dialog->setLayout(vlayout1);

  if(mainwindow->files_open)
  {
    daybox1->setEnabled(true);
    daybox2->setEnabled(true);
    timeEdit1->setEnabled(true);
    timeEdit2->setEnabled(true);

    starttime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_hr;

    starttime /= 10000;

    time2.setHMS((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL));

    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));

    timeEdit2->setTime(time2);

    jumpButton->setEnabled(true);
  }

  QObject::connect(CloseButton, SIGNAL(clicked()),                  jump_dialog, SLOT(close()));
  QObject::connect(jumpButton,  SIGNAL(clicked()),                  this,        SLOT(jumpbutton_pressed()));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));

  jump_dialog->exec();
}


void UI_JumpMenuDialog::offsetday_changed(int days)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  milliseconds += ((long long)days * 86400000LL);

  milliseconds += starttime;

  time2.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit2->setTime(time2);

  daybox2->setValue((int)(milliseconds / 86400000LL));

  if(daybox2->value() < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::absoluteday_changed(int days)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::disconnect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  if(days < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  milliseconds = (long long)(timeEdit2->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit2->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit2->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit2->time().msec());

  milliseconds += ((long long)days * 86400000LL);

  if(milliseconds<0)  milliseconds = 0;

  milliseconds -= starttime;

  time1.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit1->setTime(time1);

  daybox1->setValue((int)(milliseconds / 86400000LL));

  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::offsettime_changed(const QTime &time_1)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  milliseconds = (long long)(time_1.hour()) * 3600000LL;
  milliseconds += (long long)(time_1.minute()) * 60000LL;
  milliseconds += (long long)(time_1.second()) * 1000LL;
  milliseconds += (long long)(time_1.msec());

  milliseconds += ((long long)daybox1->value() * 86400000LL);

  milliseconds += starttime;

  time2.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit2->setTime(time2);

  daybox2->setValue((int)(milliseconds / 86400000LL));

  if(daybox2->value() < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::absolutetime_changed(const QTime &time_2)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::disconnect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));

  milliseconds = (long long)(time_2.hour()) * 3600000LL;
  milliseconds += (long long)(time_2.minute()) * 60000LL;
  milliseconds += (long long)(time_2.second()) * 1000LL;
  milliseconds += (long long)(time_2.msec());

  milliseconds += ((long long)daybox2->value() * 86400000LL);

  if(milliseconds<0)  milliseconds = 0;

  milliseconds -= starttime;

  time1.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit1->setTime(time1);

  daybox1->setValue((int)(milliseconds / 86400000LL));

  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
}


void UI_JumpMenuDialog::jumpbutton_pressed()
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  milliseconds += ((long long)daybox1->value() * 86400000LL);

  mainwindow->set_viewtime(milliseconds * (TIME_FIXP_SCALING / 1000LL));

  jump_dialog->close();
}





