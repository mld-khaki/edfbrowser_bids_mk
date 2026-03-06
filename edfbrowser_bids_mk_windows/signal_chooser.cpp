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




#include "signal_chooser.h"



UI_SignalChooser::UI_SignalChooser(QWidget *w_parent, int job, int *sgnl_nr)
{
  task = job;

  signal_nr = sgnl_nr;

  mainwindow = (UI_Mainwindow *)w_parent;

  signalchooser_dialog = new QDialog(w_parent);
  if(task == 3)
  {
    signalchooser_dialog->setMinimumSize(435 * mainwindow->w_scaling, 420 * mainwindow->h_scaling);
    signalchooser_dialog->setWindowTitle("Organize signals");
  }
  else
  {
    signalchooser_dialog->setMinimumSize(200 * mainwindow->w_scaling, 420 * mainwindow->h_scaling);
    signalchooser_dialog->setWindowTitle("Signals");
  }
  signalchooser_dialog->setModal(true);
  signalchooser_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  signalchooser_dialog->setSizeGripEnabled(true);

  list = new QListWidget;
  if(task == 3)
  {
    list->setSelectionBehavior(QAbstractItemView::SelectRows);
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list->setToolTip("Double-click on an item to edit properties");
  }
  else
  {
    list->setSelectionBehavior(QAbstractItemView::SelectRows);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
  }

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  if(task == 3)
  {
    EditButton = new QPushButton;
    EditButton->setText("Edit");

    UpButton = new QPushButton;
    UpButton->setText("Up");

    DownButton = new QPushButton;
    DownButton->setText("Down");

    InvertButton = new QPushButton;
    InvertButton->setText("Invert");

    DeleteButton = new QPushButton;
    DeleteButton->setText("Remove");
  }

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  if(task == 3)
  {
    vlayout1->addStretch(1000);
    vlayout1->addWidget(EditButton);
    vlayout1->addWidget(UpButton);
    vlayout1->addWidget(DownButton);
    vlayout1->addWidget(InvertButton);
    vlayout1->addWidget(DeleteButton);
  }
  vlayout1->addStretch(1000);
  vlayout1->addWidget(CloseButton);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(list);
  hlayout1->addLayout(vlayout1);

  signalchooser_dialog->setLayout(hlayout1);

  load_signalcomps();

  QObject::connect(CloseButton, SIGNAL(clicked()), signalchooser_dialog, SLOT(close()));

  if(task == 3)
  {
    if(list->count() > 0)
    {
      list->setCurrentRow(0);

      QObject::connect(EditButton,   SIGNAL(clicked()), this, SLOT(signalEdit()));
      QObject::connect(UpButton,     SIGNAL(clicked()), this, SLOT(signalUp()));
      QObject::connect(DownButton,   SIGNAL(clicked()), this, SLOT(signalDown()));
      QObject::connect(InvertButton, SIGNAL(clicked()), this, SLOT(signalInvert()));
      QObject::connect(DeleteButton, SIGNAL(clicked()), this, SLOT(signalDelete()));

      QObject::connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(item_activated(QListWidgetItem *)));
    }
  }
  else
  {
    QObject::connect(list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(call_sidemenu(QListWidgetItem *)));
  }

  signalchooser_dialog->exec();
}



void UI_SignalChooser::load_signalcomps(void)
{
  int i;

  QListWidgetItem *item;

  list->clear();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    if(task == 3)
    {
      item->setText(QString(mainwindow->signalcomp[i]->signallabel) + QString("  Alias: ") + QString(mainwindow->signalcomp[i]->alias));
    }
    else
    {
      item->setText(mainwindow->signalcomp[i]->signallabel);
    }
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }
}


void UI_SignalChooser::item_activated(QListWidgetItem *item)
{
  mainwindow->maincurve->exec_sidemenu(item->data(Qt::UserRole).toInt());

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::call_sidemenu(QListWidgetItem *)
{
  int i, signal_nr2;

  char str1_32[32]={""};

  if(task == 3) return;

  signalchooser_dialog->hide();

  if(task == 0)
  {
    mainwindow->maincurve->exec_sidemenu(list->currentRow());
  }

  if(task == 1)
  {
    for(i=0; i<MAXSPECTRUMDOCKS; i++)
    {
      if(mainwindow->spectrumdock[i]->dock->isHidden())  break;
    }

    if(i<MAXSPECTRUMDOCKS)
    {
      mainwindow->spectrumdock[i]->init(list->currentRow());
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The maximum number of docked Power Spectrum windows has been reached.\n"
                                                                "Close one first.");
      messagewindow.exec();
    }
  }

  if(task == 5)
  {
    signal_nr2 = list->currentRow();

    if(mainwindow->signalcomp[signal_nr2]->ecg_filter != NULL)
    {
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfparam_0->sf_int < 30)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency must be at least 30Hz and must be an integer value.");
      messagewindow.exec();
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Recording length must be at least 30 seconds.");
      messagewindow.exec();
      signalchooser_dialog->close();
      return;
    }

    for(i=0; i<MAXCDSADOCKS; i++)
    {
      if(mainwindow->cdsa_dock[i] == NULL)
      {
        UI_cdsa_window wndw(mainwindow, mainwindow->signalcomp[signal_nr2], i);

        break;
      }
    }
  }

  if(task == 6)
  {
    signal_nr2 = list->currentRow();

    if(mainwindow->signalcomp[signal_nr2]->ecg_filter != NULL)
    {
      signalchooser_dialog->close();
      return;
    }

    strlcpy(str1_32, mainwindow->signalcomp[signal_nr2]->edfparam_0->physdimension, 32);
    trim_spaces(str1_32);
    if((strcmp(str1_32, "uV")) && (strcmp(str1_32, "mV")) && (strcmp(str1_32, "V")))
    {
      QMessageBox::critical(signalchooser_dialog, "Error", "Unknown physical dimension (unit), expected uV or mV or V");
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfparam_0->sf_int < 100)
    {
      QMessageBox::critical(signalchooser_dialog, "Error", "Samplefrequency must be at least 100Hz and must be an integer value.");
      signalchooser_dialog->close();
      return;
    }

    if(mainwindow->signalcomp[signal_nr2]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox::critical(signalchooser_dialog, "Error", "Recording length must be at least 30 seconds.");
      signalchooser_dialog->close();
      return;
    }

    for(i=0; i<MAXAEEGDOCKS; i++)
    {
      if(mainwindow->aeeg_dock[i] == NULL)
      {
        UI_aeeg_window wndw(mainwindow, mainwindow->signalcomp[signal_nr2], i);

        break;
      }
    }
  }

  if(task == 2)
  {
    AdjustFilterSettings filtersettings(mainwindow->signalcomp[list->currentRow()], mainwindow->maincurve);
  }

  if(task == 4)
  {
    if(signal_nr != NULL)
    {
      *signal_nr = list->currentRow();
    }
  }

  signalchooser_dialog->close();
}


void UI_SignalChooser::signalEdit()
{
  int n,
      selected_signals[MAXSIGNALS];

  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  mainwindow->maincurve->exec_sidemenu(selected_signals[0]);

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalUp()
{
  int i, j, n,
      size,
      sigcomp_nr,
      selected_signals[MAXSIGNALS];

  sigcompblck_t *signalcomp;


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  size = list->count();

  if(size < (n + 1))
  {
    return;
  }

  if(selected_signals[0] < 1)
  {
    return;
  }

  for(i=0; i<n; i++)
  {
    sigcomp_nr = selected_signals[i];

    signalcomp = mainwindow->signalcomp[sigcomp_nr];

    mainwindow->signalcomp[sigcomp_nr] = mainwindow->signalcomp[sigcomp_nr - 1];

    mainwindow->signalcomp[sigcomp_nr - 1] = signalcomp;

    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->spectr_dialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->spectr_dialog[j]--;
      }
    }

    for(j=0; j<MAXCDSADOCKS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->cdsa_idx[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->cdsa_idx[j]--;
      }
    }

    for(j=0; j<MAXAEEGDOCKS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->aeeg_idx[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->aeeg_idx[j]--;
      }
    }

    for(j=0; j<MAXAVERAGECURVEDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->avg_dialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->avg_dialog[j]--;
      }
    }

    for(j=0; j<MAXZSCOREDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->zscoredialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->zscoredialog[j]--;
      }
    }
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i] - 1)->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalDown()
{
  int i, j, n,
      size,
      sigcomp_nr,
      selected_signals[MAXSIGNALS];

  sigcompblck_t *signalcomp;


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  size = list->count();

  if(size < (n + 1))
  {
    return;
  }

  if(selected_signals[n-1] > (size - 2))
  {
    return;
  }

  for(i=(n-1); i>=0; i--)
  {
    sigcomp_nr = selected_signals[i];

    signalcomp = mainwindow->signalcomp[sigcomp_nr];

    mainwindow->signalcomp[sigcomp_nr] = mainwindow->signalcomp[sigcomp_nr + 1];

    mainwindow->signalcomp[sigcomp_nr + 1] = signalcomp;

    selected_signals[i] = sigcomp_nr;

    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->spectr_dialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->spectr_dialog[j]++;
      }
    }

    for(j=0; j<MAXCDSADOCKS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->cdsa_idx[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->cdsa_idx[j]++;
      }
    }

    for(j=0; j<MAXAEEGDOCKS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->aeeg_idx[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->aeeg_idx[j]++;
      }
    }

    for(j=0; j<MAXAVERAGECURVEDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->avg_dialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->avg_dialog[j]++;
      }
    }

    for(j=0; j<MAXZSCOREDIALOGS; j++)
    {
      if(mainwindow->signalcomp[sigcomp_nr]->zscoredialog[j] == (sigcomp_nr + 1))
      {
        mainwindow->signalcomp[sigcomp_nr]->zscoredialog[j]++;
      }
    }
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i] + 1)->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}


int UI_SignalChooser::get_selectionlist(int *slist)
{
  int i, j, n, tmp;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  selectedlist = list->selectedItems();

  n = selectedlist.size();

  if(n < 1)
  {
    return 0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);

    slist[i] = item->data(Qt::UserRole).toInt();

    for(j=i; j>0; j--)  // Sort the list!!
    {
      if(slist[j] > slist[j-1])
      {
        break;
      }

      tmp = slist[j];

      slist[j] = slist[j-1];

      slist[j-1] = tmp;
    }
  }

  return n;
}


void UI_SignalChooser::signalDelete()
{
  int k, n,
      sigcomp_nr,
      selected_signr[MAXSIGNALS];


  n = get_selectionlist(selected_signr);

  if(n < 1)
  {
    return;
  }

  for(k=0; k<n; k++)
  {
    sigcomp_nr = selected_signr[k];

    sigcomp_nr -= k;

    mainwindow->remove_signalcomp(sigcomp_nr);
  }

  load_signalcomps();

  mainwindow->setup_viewbuf();
}


void UI_SignalChooser::signalInvert()
{
  int i, n,
      selected_signals[MAXSIGNALS];


  n = get_selectionlist(selected_signals);

  if(n < 1)
  {
    return;
  }

  for(i=0; i<n; i++)
  {
    mainwindow->signalcomp[selected_signals[i]]->polarity *= -1;

    mainwindow->signalcomp[selected_signals[i]]->screen_offset_pix *= -1;

    mainwindow->signalcomp[selected_signals[i]]->screen_offset_unit *= -1;
  }

  load_signalcomps();

  for(i=0; i<n; i++)
  {
    list->item(selected_signals[i])->setSelected(true);
  }

  mainwindow->setup_viewbuf();
}








