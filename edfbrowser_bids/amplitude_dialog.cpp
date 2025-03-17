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




#include "pagetime_dialog.h"



UI_Userdefined_amplitude_Dialog::UI_Userdefined_amplitude_Dialog(QWidget *w_parent)
{
  int signal_nr = 0;

  mainwindow = (UI_Mainwindow *)w_parent;

  set_amplitude_dialog = new QDialog(w_parent);
  set_amplitude_dialog->setMinimumSize(250 * mainwindow->w_scaling, 150 * mainwindow->h_scaling);
  set_amplitude_dialog->setWindowTitle("Set amplitude");
  set_amplitude_dialog->setModal(true);
  set_amplitude_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  set_amplitude_dialog->setSizeGripEnabled(true);

  QLabel *label1 = new QLabel;
  label1->setText("Amplitude");

  ScaleBox = new QDoubleSpinBox;
  ScaleBox->setDecimals(8);
  ScaleBox->setMaximum(1000000.0);
  ScaleBox->setMinimum(0.0000001);
  if(mainwindow->signalcomp[signal_nr]->edfparam_0->bitvalue < 0.0)
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm * -1.0);
  }
  else
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm);
  }
  ScaleBox->setSuffix(" /cm");

  OKButton = new QPushButton;
  OKButton->setText("Apply");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(OKButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label1);
  vlayout1->addWidget(ScaleBox);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  set_amplitude_dialog->setLayout(vlayout1);

  QObject::connect(CloseButton, SIGNAL(clicked()), set_amplitude_dialog, SLOT(close()));
  QObject::connect(OKButton,    SIGNAL(clicked()), this,                  SLOT(okbutton_pressed()));

  set_amplitude_dialog->exec();
}


void UI_Userdefined_amplitude_Dialog::okbutton_pressed()
{
  int i;

  double value2, original_value;

  if(!mainwindow->files_open)
  {
    set_amplitude_dialog->close();

    return;
  }

  if(!mainwindow->signalcomps)
  {
    set_amplitude_dialog->close();

    return;
  }

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    value2 = ScaleBox->value();

    if(mainwindow->signalcomp[i]->edfparam_0->bitvalue < 0.0)
    {
      value2 *= -1.0;
    }

    mainwindow->signalcomp[i]->sensitivity = (mainwindow->signalcomp[i]->edfparam_0->bitvalue / value2) / mainwindow->y_pixelsizefactor;

    original_value = mainwindow->signalcomp[i]->voltpercm;

    mainwindow->signalcomp[i]->voltpercm = value2;

    mainwindow->signalcomp[i]->screen_offset_pix *= (original_value / value2);

    mainwindow->signalcomp[i]->screen_offset_unit *= (original_value / value2);
  }

  mainwindow->maincurve->drawCurve_stage_1();
}





