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



#include "signals_dialog.h"

#define DEFAULT_COLOR_LIST_SZ  (6)


static const char derivation_err_str[]={
  "It is only possible to make derivations from signals which:\n"
  " - are from the same file\n"
  " - have the same samplerate\n"
  " - have the same physical dimension (e.g. uV)\n"
  " - have the same resolution (e.g. uV/bit)"};



UI_Signalswindow::UI_Signalswindow(QWidget *w_parent)
{
  int i, tmp;

  mainwindow = (UI_Mainwindow *)w_parent;

  smp_per_record = 0;

  color_selected = 0;

  default_color_idx = 0;

  default_color_list[0] = Qt::yellow;
  default_color_list[1] = Qt::green;
  default_color_list[2] = Qt::red;
  default_color_list[3] = Qt::cyan;
  default_color_list[4] = Qt::magenta;
  default_color_list[5] = Qt::blue;

  if(mainwindow->signalcomps > 0)
  {
    tmp = mainwindow->signalcomp[mainwindow->signalcomps - 1]->color;

    for(i=0; i<DEFAULT_COLOR_LIST_SZ; i++)
    {
      if(default_color_list[i] == tmp)  break;
    }

    if(i < DEFAULT_COLOR_LIST_SZ)
    {
      default_color_idx = (i + 1) % DEFAULT_COLOR_LIST_SZ;
    }
  }

  SignalsDialog = new QDialog;

  SignalsDialog->setMinimumSize(850 * mainwindow->w_scaling, 500 * mainwindow->h_scaling);
  SignalsDialog->setWindowTitle("Add signals");
  SignalsDialog->setModal(true);
  SignalsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  SignalsDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setMaximumHeight(75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  label1 = new QLabel;

  label2 = new QLabel;

  label3 = new QLabel;

  label4 = new QLabel;

  label5 = new QLabel;
  label5->setText("Signals in file");

  label6 = new QLabel;
  label6->setText("Signal Composition (derivation)");

  label7 = new QLabel;

  signallist = new QListWidget;
  signallist->setFont(*mainwindow->monofont);
  signallist->setSelectionBehavior(QAbstractItemView::SelectRows);
  signallist->setSelectionMode(QAbstractItemView::ExtendedSelection);
  signallist->setUniformItemSizes(true);

  CloseButton = new QPushButton;
  CloseButton->setText("&Close");

  SelectAllButton = new QPushButton;
  SelectAllButton->setText("&Select All");

  HelpButton = new QPushButton;
  HelpButton->setText("&Help");

  DisplayButton = new QPushButton;
  DisplayButton->setText("&Add signal(s)");
  DisplayButton->setToolTip("Add the above selected signals to the screen (unipolar)");

  DisplayCompButton = new QPushButton;
  DisplayCompButton->setText("&Make derivation");
  DisplayCompButton->setToolTip("Make a derivation of the above selected signals");

  AddButton = new QPushButton;
  AddButton->setText("Add->");

  SubtractButton = new QPushButton;
  SubtractButton->setText("Subtract->");

  RemoveButton = new QPushButton;
  RemoveButton->setText("Remove<-");

  ColorButton = new SpecialButton;
  ColorButton->setMinimumHeight(25);
  if(mainwindow->use_diverse_signal_colors)
  {
    ColorButton->setColor(127);
  }
  else
  {
    ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);
  }
  ColorButton->setToolTip("Click to select the trace color");
  ColorButton->setText("Trace color");

  compositionlist = new QTableWidget;
  compositionlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  compositionlist->setSelectionMode(QAbstractItemView::ExtendedSelection);
  compositionlist->setColumnCount(4);
  compositionlist->setColumnWidth(0, 30);
  compositionlist->setColumnWidth(1, 150);
  compositionlist->setColumnWidth(2, 100);
  compositionlist->setColumnWidth(3, 100);

  QStringList horizontallabels;
  horizontallabels += "Signal";
  horizontallabels += "Label";
  horizontallabels += "Factor";
  horizontallabels += "Samplerate";
  compositionlist->setHorizontalHeaderLabels(horizontallabels);
  compositionlist->resizeColumnsToContents();

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(label1);
  hlayout1->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(label2);
  hlayout2->addStretch(1000);
  hlayout2->addWidget(HelpButton);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(label3);
  hlayout3->addStretch(100);
  hlayout3->addWidget(label7);
  hlayout3->addStretch(100);
  hlayout3->addWidget(label4);
  hlayout3->addStretch(900);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(SelectAllButton);
  hlayout5->addStretch(1000);
  hlayout5->addWidget(DisplayButton);

  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addWidget(DisplayCompButton);
  hlayout6->addStretch(1000);
  hlayout6->addWidget(CloseButton);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(label5);
  vlayout2->addWidget(signallist, 1000);
  vlayout2->addSpacing(35);
  vlayout2->addLayout(hlayout5);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addStretch(1000);
  vlayout3->addWidget(AddButton);
  vlayout3->addWidget(SubtractButton);
  vlayout3->addWidget(RemoveButton);
  vlayout3->addSpacing(35);
  vlayout3->addWidget(ColorButton);
  vlayout3->addStretch(1000);

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addWidget(label6);
  vlayout4->addWidget(compositionlist, 1000);
  vlayout4->addSpacing(35);
  vlayout4->addLayout(hlayout6);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addLayout(vlayout2);
  hlayout4->addLayout(vlayout3);
  hlayout4->addLayout(vlayout4);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist);
  vlayout1->addLayout(hlayout1);
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(hlayout3);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout4, 1000);

  SignalsDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,       SIGNAL(clicked()),                SignalsDialog, SLOT(close()));
  QObject::connect(SelectAllButton,   SIGNAL(clicked()),                this,          SLOT(SelectAllButtonClicked()));
  QObject::connect(HelpButton,        SIGNAL(clicked()),                this,          SLOT(HelpButtonClicked()));
  QObject::connect(DisplayButton,     SIGNAL(clicked()),                this,          SLOT(DisplayButtonClicked()));
  QObject::connect(DisplayCompButton, SIGNAL(clicked()),                this,          SLOT(DisplayCompButtonClicked()));
  QObject::connect(AddButton,         SIGNAL(clicked()),                this,          SLOT(AddButtonClicked()));
  QObject::connect(SubtractButton,    SIGNAL(clicked()),                this,          SLOT(SubtractButtonClicked()));
  QObject::connect(RemoveButton,      SIGNAL(clicked()),                this,          SLOT(RemoveButtonClicked()));
  QObject::connect(ColorButton,       SIGNAL(clicked(SpecialButton *)), this,          SLOT(ColorButtonClicked(SpecialButton *)));
  QObject::connect(filelist,          SIGNAL(currentRowChanged(int)),   this,          SLOT(show_signals(int)));
  QObject::connect(signallist,        SIGNAL(itemSelectionChanged()),   this,          SLOT(selected_signals_changed()));

  curve_color = mainwindow->maincurve->signal_color;

  filelist->setCurrentRow(mainwindow->files_open - 1);

  signallist->setFocus();

  SignalsDialog->exec();
}


void UI_Signalswindow::ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow, 1);

  if(color < 0)  return;

  if(color == 127)
  {
    color_selected = 0;

    mainwindow->use_diverse_signal_colors = 1;

    ColorButton->setColor(127);

    return;
  }

  mainwindow->use_diverse_signal_colors = 0;

  ColorButton->setColor((Qt::GlobalColor)color);

  curve_color = color;

  color_selected = 1;
}


void UI_Signalswindow::DisplayCompButtonClicked()
{
  int i, j, n, edfsignal=0;

  char str1_512[512]="",
       str2_128[128]="";

  sigcompblck_t *newsignalcomp=NULL;


  n = compositionlist->rowCount();

  if(!n)
  {
    return;
  }

  if(mainwindow->signalcomps >= MAXSIGNALS)
  {
    snprintf(str1_512, 512, "Cannot display more than %i traces.", MAXSIGNALS);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_512);
    messagewindow.exec();
    return;
  }

  for(i=0; i<n; i++)
  {
    if((((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value() < 0.001) &&
       (((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value() > -0.001))
    {
      strlcpy(str2_128, ((QLabel *)(compositionlist->cellWidget(i, 0)))->text().toLatin1().data(), 128);

      snprintf(str1_512, 512, "Signal %i in the Composition list has a Factor too close to zero.", atoi(str2_128));
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_512);
      messagewindow.exec();
      return;
    }
  }

  newsignalcomp = (sigcompblck_t *)calloc(1, sizeof(sigcompblck_t));
  if(newsignalcomp==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
    messagewindow.exec();
    SignalsDialog->close();
    return;
  }

  newsignalcomp->uid = mainwindow->uid_seq++;
  newsignalcomp->num_of_signals = n;
  newsignalcomp->edfhdr = mainwindow->edfheaderlist[filelist->currentRow()];
  newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
  if(mainwindow->use_diverse_signal_colors && (!color_selected))
  {
    newsignalcomp->color = default_color_list[default_color_idx++];
    default_color_idx %= DEFAULT_COLOR_LIST_SZ;
  }
  else
  {
    newsignalcomp->color = curve_color;
  }
  newsignalcomp->hasruler = 0;
  newsignalcomp->polarity = 1;

  for(i=0; i<n; i++)
  {
    strlcpy(str1_512, ((QLabel *)(compositionlist->cellWidget(i, 0)))->text().toLatin1().data(), 512);

    edfsignal = atoi(str1_512) - 1;

    strlcpy(str1_512, ((QLabel *)(compositionlist->cellWidget(i, 1)))->text().toLatin1().data(), 512);

    for(j=0; j<newsignalcomp->edfhdr->edfsignals; j++)
    {
      if(!strcmp(newsignalcomp->edfhdr->edfparam[j].label, str1_512))
      {
        if(j != edfsignal)  continue;

        newsignalcomp->edfsignal[i] = j;
        newsignalcomp->factor[i] = ((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value();
        if(newsignalcomp->factor[i] < 0.0001)
        {
          strlcat(newsignalcomp->signallabel, "- ", 512);
        }
        else
        {
          if(i)
          {
            strlcat(newsignalcomp->signallabel, "+ ", 512);
          }
        }
        strlcpy(str2_128, newsignalcomp->edfhdr->edfparam[j].label, 128);
        if(mainwindow->strip_label_types)
        {
          newsignalcomp->signallabel_type_stripped = strip_types_from_label(str2_128);
        }
        strlcat(newsignalcomp->signallabel, str2_128, 512);
        trim_spaces(newsignalcomp->signallabel);
        strlcat(newsignalcomp->signallabel, " ", 512);

        if(mainwindow->default_amplitude_use_physmax_div)
        {
          newsignalcomp->voltpercm = (newsignalcomp->edfhdr->edfparam[j].phys_max - newsignalcomp->edfhdr->edfparam[j].phys_min)
                                       / (mainwindow->default_amplitude_physmax_div * 2);
        }
        else
        {
          newsignalcomp->voltpercm = mainwindow->default_amplitude;
        }
        if(newsignalcomp->voltpercm < 0.0)
        {
          newsignalcomp->voltpercm = -newsignalcomp->voltpercm;
        }

        if(newsignalcomp->edfhdr->edfparam[j].bitvalue < 0.0)
        {
          newsignalcomp->voltpercm *= -1;
        }
        newsignalcomp->sensitivity = newsignalcomp->edfhdr->edfparam[j].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->y_pixelsizefactor);
      }
    }
  }

  newsignalcomp->edfparam_0 = &newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]];

  if(mainwindow->default_invert_eeg_signals)
  {
    if(strlen(newsignalcomp->signallabel) > 4)
    {
      if(!strncmp(newsignalcomp->signallabel, "EEG ", 4))
      {
        if(newsignalcomp->edfparam_0->bitvalue >= 0)
        {
          newsignalcomp->polarity *= -1;

          newsignalcomp->screen_offset_pix *= -1;

          newsignalcomp->screen_offset_unit *= -1;
        }
      }
    }
  }

  remove_trailing_spaces(newsignalcomp->signallabel);
  newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

  strlcpy(newsignalcomp->physdimension, newsignalcomp->edfparam_0->physdimension, 9);
  remove_trailing_spaces(newsignalcomp->physdimension);

  mainwindow->signalcomp[mainwindow->signalcomps] = newsignalcomp;
  mainwindow->signalcomps++;

  while(compositionlist->rowCount())
  {
    compositionlist->removeRow(0);
  }

  mainwindow->setup_viewbuf();

  smp_per_record = 0;
}


void UI_Signalswindow::DisplayButtonClicked()
{
  int i, n, s, old_scomps;

  char str1_512[512]={""};

  sigcompblck_t *newsignalcomp=NULL;

  QListWidgetItem *item=NULL;

  QList<QListWidgetItem *> selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)
  {
    SignalsDialog->close();
    return;
  }

  if(mainwindow->signalcomps + n > MAXSIGNALS)
  {
    snprintf(str1_512, 512, "Cannot display more than %i traces.", MAXSIGNALS);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_512);
    messagewindow.exec();
    return;
  }

  old_scomps = mainwindow->signalcomps;

  for(i=0; i<n; i++)
  {
    newsignalcomp = (sigcompblck_t *)calloc(1, sizeof(sigcompblck_t));
    if(newsignalcomp==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
      messagewindow.exec();
      SignalsDialog->close();
      return;
    }

    newsignalcomp->uid = mainwindow->uid_seq++;
    newsignalcomp->num_of_signals = 1;
    newsignalcomp->edfhdr = mainwindow->edfheaderlist[filelist->currentRow()];
    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    newsignalcomp->edfsignal[0] = s;
    newsignalcomp->factor[0] = 1;
    if(mainwindow->ecg_view_mode)
    {
      strlcpy(str1_512, newsignalcomp->edfhdr->edfparam[s].physdimension, 32);
      trim_spaces(str1_512);
      if(!strcmp(str1_512, "uV"))
      {
        newsignalcomp->voltpercm = 500.0;
      }
      else if(!strcmp(str1_512, "mV"))
        {
          newsignalcomp->voltpercm = 0.5;
        }
        else if(!strcmp(str1_512, "V"))
          {
            newsignalcomp->voltpercm = 0.0005;
          }
          else
          {
            newsignalcomp->voltpercm = mainwindow->default_amplitude;
          }
    }
    else if(mainwindow->default_amplitude_use_physmax_div)
      {
        newsignalcomp->voltpercm = (newsignalcomp->edfhdr->edfparam[s].phys_max - newsignalcomp->edfhdr->edfparam[s].phys_min)
                                     / (mainwindow->default_amplitude_physmax_div * 2);
      }
      else
      {
        newsignalcomp->voltpercm = mainwindow->default_amplitude;
      }

    if(newsignalcomp->voltpercm < 0.0)
    {
      newsignalcomp->voltpercm = -newsignalcomp->voltpercm;
    }

    if(mainwindow->use_diverse_signal_colors && (!color_selected))
    {
      newsignalcomp->color = default_color_list[default_color_idx++];
      default_color_idx %= DEFAULT_COLOR_LIST_SZ;
    }
    else
    {
      newsignalcomp->color = curve_color;
    }
    newsignalcomp->hasruler = 0;
    newsignalcomp->polarity = 1;

    if(newsignalcomp->edfhdr->edfparam[s].bitvalue < 0)
    {
      newsignalcomp->voltpercm *= -1;
    }
    newsignalcomp->sensitivity = newsignalcomp->edfhdr->edfparam[s].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->y_pixelsizefactor);

    if(mainwindow->default_invert_eeg_signals)
    {
      if(strlen(newsignalcomp->edfhdr->edfparam[s].label) > 4)
      {
        if(!strncmp(newsignalcomp->edfhdr->edfparam[s].label, "EEG ", 4))
        {
          if(newsignalcomp->edfhdr->edfparam[s].bitvalue >= 0)
          {
            newsignalcomp->polarity *= -1;

            newsignalcomp->screen_offset_pix *= -1;

            newsignalcomp->screen_offset_unit *= -1;
          }
        }
      }
    }

    strlcpy(newsignalcomp->signallabel, newsignalcomp->edfhdr->edfparam[s].label, 256);
    if(mainwindow->strip_label_types)
    {
      newsignalcomp->signallabel_type_stripped = strip_types_from_label(newsignalcomp->signallabel);
    }
    trim_spaces(newsignalcomp->signallabel);

    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

    newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

    strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[s].physdimension, 9);
    remove_trailing_spaces(newsignalcomp->physdimension);

    newsignalcomp->edfparam_0 = &newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]];

    mainwindow->signalcomp[mainwindow->signalcomps] = newsignalcomp;
    mainwindow->signalcomps++;
  }

  if((i) && (mainwindow->files_open == 1) && (old_scomps == 0))
  {
    if((mainwindow->signalcomp[0]->file_duration / TIME_FIXP_SCALING) < 5)
    {
      mainwindow->pagetime = mainwindow->signalcomp[0]->file_duration;
    }
  }

  SignalsDialog->close();

  mainwindow->setup_viewbuf();

  if(mainwindow->ecg_view_mode)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      if(strcmp(mainwindow->signalcomp[i]->physdimension, "uV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "mV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "V"))
      {
        QMessageBox::warning(SignalsDialog, "Warning",
                             "The physical dimension (unit) of one or more signals on the screen does not equal to uV, mV or V.\n"
                             "As a result, the grid's dimensions for these signals cannot be calculated correctly.");
        break;
      }
    }
  }
}


void UI_Signalswindow::RemoveButtonClicked()
{
  compositionlist->removeRow(compositionlist->currentRow());
}


void UI_Signalswindow::AddButtonClicked()
{
  AddSubtractButtonsClicked(0);
}


void UI_Signalswindow::SubtractButtonClicked()
{
  AddSubtractButtonsClicked(1);
}


void UI_Signalswindow::AddSubtractButtonsClicked(int subtract)
{
  int i, j, k, n, s, row, duplicate;

  char str1_256[256]={""},
       str2_4096[4096]={""};

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)  return;

  if(!compositionlist->rowCount())
  {
    smp_per_record = 0;
    physdimension[0] = 0;
    bitvalue = 0.0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    row = filelist->currentRow();

    if(smp_per_record)
    {
      if(smp_per_record!=mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record)
      {
        tmessagebox(QMessageBox::Warning, "Warning", "Cannot make derivation", derivation_err_str, SignalsDialog);
        continue;
      }
    }
    else
    {
      smp_per_record = mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record;
    }

    if(physdimension[0])
    {
      if(strcmp(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension))
      {
        tmessagebox(QMessageBox::Warning, "Warning", "Cannot make derivation", derivation_err_str, SignalsDialog);
        continue;
      }
    }
    else
    {
      strlcpy(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension, 64);
    }

    if(bitvalue!=0.0)
    {
      if(dblcmp(bitvalue, mainwindow->edfheaderlist[row]->edfparam[s].bitvalue))
      {
        strlcpy(str2_4096, derivation_err_str, 4096);
        strlcat(str2_4096, "\n\nIn this case the problem is that the amplitude resolution of the signals selected for the derivation,\n"
                      "have different values. You can correct this with the tool \"Unify resolution\" in the tools menu.\n"
                      "Have a look at the manual for the details.", 4096);
        tmessagebox(QMessageBox::Warning, "Warning", "Cannot make derivation", str2_4096, SignalsDialog);
        continue;
      }
    }
    else
    {
      bitvalue = mainwindow->edfheaderlist[row]->edfparam[s].bitvalue;
    }

    duplicate = 0;

    k = compositionlist->rowCount();
    for(j=0; j<k; j++)
    {
      strlcpy(str1_256, ((QLabel *)(compositionlist->cellWidget(j, 0)))->text().toLatin1().data(), 256);

      if((s + 1) == atoi(str1_256))
      {
        duplicate = 1;
        break;
      }
    }
    if(duplicate)
    {
      if(subtract)
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->setValue(((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->value() - 1);
      }
      else
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->setValue(((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->value() + 1);
      }
    }
    else
    {
      compositionlist->insertRow(k);
      snprintf(str1_256, 200, "%i", s + 1);
      compositionlist->setCellWidget(k, 0, new QLabel(str1_256));
      ((QLabel *)(compositionlist->cellWidget(k, 0)))->setAlignment(Qt::AlignCenter);
      compositionlist->setCellWidget(k, 1, new QLabel(mainwindow->edfheaderlist[row]->edfparam[s].label));
      ((QLabel *)(compositionlist->cellWidget(k, 1)))->setAlignment(Qt::AlignCenter);

      compositionlist->setCellWidget(k, 2, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setAlignment(Qt::AlignCenter);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setPrefix("x");
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setDecimals(3);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setRange(-128.0, 128.0);
      if(subtract)
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setValue(-1);
      }
      else
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setValue(1);
      }

      convert_to_metric_suffix(str1_256, mainwindow->edfheaderlist[row]->edfparam[s].sf_f, 3, 256);
      remove_trailing_zeros(str1_256);
      strlcat(str1_256, "Hz", 256);
      compositionlist->setCellWidget(k, 3, new QLabel(str1_256));
      ((QLabel *)(compositionlist->cellWidget(k, 3)))->setAlignment(Qt::AlignCenter);
    }
  }

  compositionlist->resizeColumnsToContents();
}


void UI_Signalswindow::show_signals(int row)
{
  int i, j, idx, len, skip, signal_cnt=0;

  char str1_256[256]={""};

  long long file_duration=0LL;

  QListWidgetItem *item=NULL;

  date_time_t date_time;

  if(row<0)  return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  signallist->clear();

  compositionlist->setRowCount(0);

  if((mainwindow->edfheaderlist[row]->edfplus)||(mainwindow->edfheaderlist[row]->bdfplus))
  {
    strlcpy(str1_256, "Subject:  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->plus_patient_name, 256);
    strlcat(str1_256, "  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->plus_birthdate, 256);
    strlcat(str1_256, "  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->plus_patientcode, 256);
    label1->setText(str1_256);
    strlcpy(str1_256, "Recording:  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->plus_admincode, 256);
    label2->setText(str1_256);
  }
  else
  {
    strlcpy(str1_256, "Subject:  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->patient, 256);
    len = strlen(str1_256);
    for(j=0; j<len; j++)
    {
      if(str1_256[j]=='_')
      {
        str1_256[j] = ' ';
      }
    }
    label1->setText(str1_256);
    strlcpy(str1_256, "Recording:  ", 256);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->recording, 256);
    len = strlen(str1_256);
    for(j=0; j<len; j++)
    {
      if(str1_256[j]=='_')
      {
        str1_256[j] = ' ';
      }
    }
    label2->setText(str1_256);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str1_256, 256, "Start:  %i %s %i   %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  if(mainwindow->edfheaderlist[row]->starttime_subsec != 0LL)
  {
#ifdef Q_OS_WIN32
    __mingw_snprintf(str1_256 + strlen(str1_256), 256 - strlen(str1_256), ".%07lli", mainwindow->edfheaderlist[row]->starttime_subsec);
#else
    snprintf(str1_256 + strlen(str1_256), 256 - strlen(str1_256), ".%07lli", mainwindow->edfheaderlist[row]->starttime_subsec);
#endif

    remove_trailing_zeros(str1_256);
  }

  label3->setText(str1_256);

  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;
  snprintf(str1_256, 256,
          "Duration:  %2i:%02i:%02i.%07i",
          (int)((file_duration / TIME_FIXP_SCALING) / 3600LL),
          (int)(((file_duration / TIME_FIXP_SCALING) % 3600LL) / 60LL),
          (int)((file_duration / TIME_FIXP_SCALING) % 60LL),
          (int)(file_duration % TIME_FIXP_SCALING));
  remove_trailing_zeros(str1_256);
  label4->setText(str1_256);

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime +
   ((file_duration + mainwindow->edfheaderlist[row]->starttime_subsec) / TIME_FIXP_SCALING), &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str1_256, 256, "End:  %i %s %i   %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);
#ifdef Q_OS_WIN32
  __mingw_snprintf(str1_256 + strlen(str1_256), 256 - strlen(str1_256), ".%07lli", (mainwindow->edfheaderlist[row]->starttime_subsec + file_duration) % TIME_FIXP_SCALING);
#else
  snprintf(str1_256 + strlen(str1_256), 256 - strlen(str1_256), ".%07lli", (mainwindow->edfheaderlist[row]->starttime_subsec + file_duration) % TIME_FIXP_SCALING);
#endif
  remove_trailing_zeros(str1_256);

  label7->setText(str1_256);

  skip = 0;

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  for(i=0, idx=1; i<signal_cnt; i++)
  {
    if(!(i % 100))
    {
      qApp->processEvents();
    }

    if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
    {
      for(j=0; j<mainwindow->edfheaderlist[row]->nr_annot_chns; j++)
      {
        if(i==mainwindow->edfheaderlist[row]->annot_ch[j])
        {
          skip = 1;
          break;
        }
      }
      if(skip)
      {
        skip = 0;
        continue;
      }
    }

    snprintf(str1_256, 256, "%-3i ", idx++);
    strlcat(str1_256, mainwindow->edfheaderlist[row]->edfparam[i].label, 256);
    strlcat(str1_256, "   ", 256);
    if(mainwindow->edfheaderlist[row]->edfparam[i].sf_int)
    {
      convert_to_metric_suffix(str1_256 + strlen(str1_256), mainwindow->edfheaderlist[row]->edfparam[i].sf_int, 6, 256 - strlen(str1_256));
    }
    else
    {
      convert_to_metric_suffix(str1_256 + strlen(str1_256), mainwindow->edfheaderlist[row]->edfparam[i].sf_f, 6, 256 - strlen(str1_256));
    }
    remove_trailing_zeros(str1_256);
    strlcat(str1_256, "Hz", 256);
    item = new QListWidgetItem;
    item->setText(str1_256);
    item->setData(Qt::UserRole, QVariant(i));
    signallist->addItem(item);
  }

  SelectAllButtonClicked();

  QApplication::restoreOverrideCursor();
}


void UI_Signalswindow::SelectAllButtonClicked()
{
  int row, i;

  QListWidgetItem *item;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  row = signallist->count();

  for(i=0; i<row; i++)
  {
    if(!(i % 100))
    {
      qApp->processEvents();
    }

    item = signallist->item(i);
    item->setSelected(true);
  }

  QApplication::restoreOverrideCursor();
}


void UI_Signalswindow::HelpButtonClicked()
{
  QMessageBox messagewindow(QMessageBox::NoIcon, "Help",
  "On top you will see a list of opened files.\n"
  "Select (highlight) the file from which you want to add signals.\n"
  "At the left part of the dialog you see a list of all the signals which are in the selected file.\n"
  "Select one or more signals and click on the \"Add signals\" button.\n"
  "Now you will see the selected signals on the screen.\n"

  "\nWhen you want to make a combination (derivation) of two or more signals, do as follows.\n"
  "Select the file from which you want to add signals.\n"
  "For example, we choose an EEG file which contains the signals \"P3\" and \"C3\"\n"
  "and we want to subtract \"C3\" from \"P3\".\n"
  "- Select (highlight) the signal \"P3\".\n"
  "- Click on the \"Add\" button.\n"
  "- Select (highlight) the signal \"C3\".\n"
  "- Click on the \"Subtract\" button.\n"
  "- Click on the \"Make derivation\" button.\n"
  "- The result of \"P3\" minus \"C3\" will appear on the screen.",
  QMessageBox::Close, SignalsDialog);

  char str[1024]={""};
  snprintf(str, 1024, "QLabel{min-width:%i px;}", (int)(500 * mainwindow->w_scaling));
  messagewindow.setStyleSheet(str);

  messagewindow.exec();
}


void UI_Signalswindow::selected_signals_changed()
{
  int n;

  char str1_128[128]={""};

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  snprintf(str1_128, 128, "Signals in file:   (%i selected)", n);

  label5->setText(str1_128);
}












