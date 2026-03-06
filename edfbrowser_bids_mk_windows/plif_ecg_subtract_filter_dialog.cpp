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



#include "plif_ecg_subtract_filter_dialog.h"



UI_PLIF_ECG_filter_dialog::UI_PLIF_ECG_filter_dialog(QWidget *w_parent)
{
  int i, n, s, sf=0;

  char str1_256[256]={""};

  double dthreshold=0;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  mainwindow = (UI_Mainwindow *)w_parent;

  plifecgfilterdialog = new QDialog;
  plifecgfilterdialog->setMinimumSize(400 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  plifecgfilterdialog->setWindowTitle("Add a powerline interference filter");
  plifecgfilterdialog->setModal(true);
  plifecgfilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);
  plifecgfilterdialog->setSizeGripEnabled(true);

  plfBox = new QComboBox;
  plfBox->addItem(" 50 Hz");
  plfBox->addItem(" 60 Hz");

  QLabel *listlabel = new QLabel("Select signals:");

  list = new QListWidget;
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  list->setToolTip("Only signals with a physical dimension V, mV or uV and\n"
                   "a samplerate of >= 240Hz and\n"
                   "an integer ratio to 50 or 60 Hz and\n"
                   "a resolution better than or equal to 10 uV/bit will be listed here.");

  CancelButton = new QPushButton;
  CancelButton->setText("&Close");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    sf = mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].sf_int;

    if(sf < 240)  /* don't list signals that have low samplerate */
    {
      continue;
    }

    if((sf % 50) && (sf % 60))  /* don't list signals that cannot be filtered */
    {
      continue;
    }

    strlcpy(str1_256, mainwindow->signalcomp[i]->edfparam_0->physdimension, 256);

    remove_trailing_spaces(str1_256);

    if(strcmp(str1_256, "uV") && strcmp(str1_256, "mV") && strcmp(str1_256, "V"))
    {
      continue;
    }

    if(!strcmp(str1_256, "uV"))
    {
      dthreshold = 5.0 / mainwindow->signalcomp[i]->edfparam_0->bitvalue;
    }
    else if(!strcmp(str1_256, "mV"))
      {
        dthreshold = 5e-3 / mainwindow->signalcomp[i]->edfparam_0->bitvalue;
      }
      else if(!strcmp(str1_256, "V"))
        {
          dthreshold = 5e-6 / mainwindow->signalcomp[i]->edfparam_0->bitvalue;
        }

    if(dthreshold < 0.499)  /* resolution must be better than or equal to 10 uV/bit */
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

    if(mainwindow->signalcomp[s]->plif_ecg_filter != NULL)
    {
      plfBox->setCurrentIndex(mainwindow->signalcomp[s]->plif_ecg_subtract_filter_plf);

      item->setSelected(true);
    }
    else
    {
      item->setSelected(false);
    }
  }

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Powerline frequency", plfBox);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addSpacing(30);
  hlayout3->addLayout(flayout);
  hlayout3->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(helpButton);
  hlayout2->addStretch(1000);
  hlayout2->addSpacing(30);
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

  plifecgfilterdialog->setLayout(hlayout1);

  QObject::connect(CancelButton, SIGNAL(clicked()),                plifecgfilterdialog, SLOT(close()));
  QObject::connect(list,         SIGNAL(itemSelectionChanged()),   this,                SLOT(ApplyButtonClicked()));
  QObject::connect(plfBox,       SIGNAL(currentIndexChanged(int)), this,                SLOT(ApplyButtonClicked()));
  QObject::connect(helpButton,   SIGNAL(clicked()),                this,                SLOT(helpbuttonpressed()));

  plifecgfilterdialog->exec();
}


void UI_PLIF_ECG_filter_dialog::ApplyButtonClicked()
{
  int i, s, n, sf, plf, err=0;

  char str1_256[256];

  double dthreshold=0;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->plif_ecg_filter)
    {
      plif_ecg_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter);

      mainwindow->signalcomp[i]->plif_ecg_filter = NULL;
    }

    if(mainwindow->signalcomp[i]->plif_ecg_filter_sav)
    {
      plif_ecg_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter_sav);

      mainwindow->signalcomp[i]->plif_ecg_filter_sav = NULL;
    }
  }

  if(plfBox->currentIndex() == 0)
  {
    plf = 50;
  }
  else
  {
    plf = 60;
  }

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    sf = mainwindow->signalcomp[s]->edfparam_0->sf_int;

    if((sf % plf) || (!sf))
    {
      err = 1;
      snprintf(str1_256, 256, "Samplefrequency (%iHz) is not an integer multiple of the powerline frequency (%iHz)", sf, plf);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_256);
      messagewindow.exec();
      break;
    }

    strlcpy(str1_256, mainwindow->signalcomp[s]->edfparam_0->physdimension, 256);

    remove_trailing_spaces(str1_256);

    if(!strcmp(str1_256, "uV"))
    {
      dthreshold = 5.0 / mainwindow->signalcomp[s]->edfparam_0->bitvalue;
    }
    else if(!strcmp(str1_256, "mV"))
      {
        dthreshold = 5e-3 / mainwindow->signalcomp[s]->edfparam_0->bitvalue;
      }
      else if(!strcmp(str1_256, "V"))
        {
          dthreshold = 5e-6 / mainwindow->signalcomp[s]->edfparam_0->bitvalue;
        }

    mainwindow->signalcomp[s]->plif_ecg_filter = plif_ecg_create_subtract_filter(sf, plf, dthreshold);
    if(mainwindow->signalcomp[s]->plif_ecg_filter == NULL)
    {
      err = 1;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a powerline interference filter.");
      messagewindow.exec();
      break;
    }

    mainwindow->signalcomp[s]->plif_ecg_filter_sav = plif_ecg_create_subtract_filter(sf, plf, dthreshold);
    if(mainwindow->signalcomp[s]->plif_ecg_filter_sav == NULL)
    {
      err = 1;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a powerline interference filter.");
      messagewindow.exec();
      break;
    }

    mainwindow->signalcomp[s]->plif_ecg_subtract_filter_plf = plfBox->currentIndex();
  }

  if(err)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      if(mainwindow->signalcomp[i]->plif_ecg_filter)
      {
        plif_ecg_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter);

        mainwindow->signalcomp[i]->plif_ecg_filter = NULL;
      }

      if(mainwindow->signalcomp[i]->plif_ecg_filter_sav)
      {
        plif_ecg_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter_sav);

        mainwindow->signalcomp[i]->plif_ecg_filter_sav = NULL;
      }
    }
  }

  mainwindow->setup_viewbuf();
}


void UI_PLIF_ECG_filter_dialog::helpbuttonpressed()
{
  mainwindow->open_manual("#ECG_PLIF_filter");
}











