/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 - 2024 Teunis van Beelen
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



#include "spike_filter_dialog.h"



UI_SpikeFilterDialog::UI_SpikeFilterDialog(QWidget *w_parent)
{
  int i, n, s, sf=0;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  mainwindow = (UI_Mainwindow *)w_parent;

  spikefilterdialog = new QDialog;

  spikefilterdialog->setMinimumSize(400 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  spikefilterdialog->setWindowTitle("Add a spike filter");
  spikefilterdialog->setModal(true);
  spikefilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);
  spikefilterdialog->setSizeGripEnabled(true);

  velocitySpinBox = new QDoubleSpinBox;
  velocitySpinBox->setDecimals(6);
  velocitySpinBox->setSuffix(" units/(0.25 mSec)");
  velocitySpinBox->setMinimum(0.000001);
  velocitySpinBox->setMaximum(1E10);
  velocitySpinBox->setValue(1000.0);

  holdOffSpinBox = new QSpinBox;
  holdOffSpinBox->setSuffix(" milliSec");
  holdOffSpinBox->setMinimum(10);
  holdOffSpinBox->setMaximum(1000);
  holdOffSpinBox->setValue(100);

  QLabel *listlabel = new QLabel("Select signals:");

  list = new QListWidget;
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  list->setToolTip("Only signals with a samplerate of >= 4KHz will be displayed here");

  CancelButton = new QPushButton;
  CancelButton->setText("&Close");

  ApplyButton = new QPushButton;
  ApplyButton->setText("&Apply");
  ApplyButton->setVisible(false);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    sf = ((long long)mainwindow->signalcomp[i]->edfparam_0->smp_per_record * TIME_FIXP_SCALING) /
         mainwindow->signalcomp[i]->edfhdr->long_data_record_duration;

    if(sf < 4000)
    {
      continue;
    }

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
    s = item->data(Qt::UserRole).toInt();

    if(mainwindow->signalcomp[s]->spike_filter != NULL)
    {
      velocitySpinBox->setValue(mainwindow->signalcomp[s]->spike_filter_velocity);

      holdOffSpinBox->setValue(mainwindow->signalcomp[s]->spike_filter_holdoff);

      item->setSelected(true);
    }
    else
    {
      item->setSelected(false);
    }
  }

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Velocity", velocitySpinBox);
  flayout->addRow("  ", (QWidget *)NULL);
  flayout->addRow("Hold-off", holdOffSpinBox);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addSpacing(30);
  hlayout3->addLayout(flayout);
  hlayout3->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addStretch(1000);
  hlayout2->addWidget(CancelButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addStretch(300);
  vlayout1->addLayout(hlayout3);
  vlayout1->addStretch(1000);
  vlayout1->addLayout(hlayout2);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(listlabel);
  vlayout2->addWidget(list, 1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addLayout(vlayout1);
  hlayout1->addLayout(vlayout2);

  spikefilterdialog->setLayout(hlayout1);

  QObject::connect(ApplyButton,     SIGNAL(clicked()),              this,              SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton,    SIGNAL(clicked()),              spikefilterdialog, SLOT(close()));
  QObject::connect(list,            SIGNAL(itemSelectionChanged()), this,              SLOT(ApplyButtonClicked()));
  QObject::connect(velocitySpinBox, SIGNAL(valueChanged(double)),   this,              SLOT(ApplyButtonClicked()));
  QObject::connect(holdOffSpinBox,  SIGNAL(valueChanged(int)),      this,              SLOT(ApplyButtonClicked()));

  spikefilterdialog->exec();
}


void UI_SpikeFilterDialog::ApplyButtonClicked()
{
  int i, s, n, sf, holdoff;

  double velocity;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->spike_filter)
    {
      free_spike_filter(mainwindow->signalcomp[i]->spike_filter);

      mainwindow->signalcomp[i]->spike_filter = NULL;
    }
  }

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    velocity = velocitySpinBox->value() /
               mainwindow->signalcomp[s]->edfparam_0->bitvalue;

    if(velocity < 0) velocity *= -1;

    holdoff = holdOffSpinBox->value();

    sf = ((long long)mainwindow->signalcomp[s]->edfparam_0->smp_per_record * TIME_FIXP_SCALING) /
         mainwindow->signalcomp[s]->edfhdr->long_data_record_duration;

    mainwindow->signalcomp[s]->spike_filter = create_spike_filter(sf, velocity, holdoff, NULL);
    if(mainwindow->signalcomp[s]->spike_filter == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a spike filter.");
      messagewindow.exec();
    }

    mainwindow->signalcomp[s]->spike_filter_velocity = velocitySpinBox->value();

    mainwindow->signalcomp[s]->spike_filter_holdoff = holdOffSpinBox->value();
  }

  mainwindow->setup_viewbuf();
}










