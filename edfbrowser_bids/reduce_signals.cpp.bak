/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2024 Teunis van Beelen
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



#include "reduce_signals.h"



UI_ReduceSignalsWindow::UI_ReduceSignalsWindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(675 * mainwindow->w_scaling, 500 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Reduce signals and/or duration");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  label1 = new QLabel;

  label2 = new QLabel;
  label2->setText("from datarecord");
  label2->setEnabled(false);

  label3 = new QLabel;
  label3->setText("to datarecord");
  label3->setEnabled(false);

  label4 = new QLabel;
  label4->setEnabled(false);

  label5 = new QLabel;
  label5->setEnabled(false);

  label6 = new QLabel;
  label6->setText("Anti-aliasing filter order");

  radioButton1 = new QRadioButton("whole duration");
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);

  radioButton2 = new QRadioButton("selection");
  radioButton2->setEnabled(false);

  spinBox1 = new QSpinBox;
  spinBox1->setRange(1, 2147483647);
  spinBox1->setValue(1);
  spinBox1->setEnabled(false);

  spinBox2 = new QSpinBox;
  spinBox2->setRange(1, 2147483647);
  spinBox2->setValue(2147483647);
  spinBox2->setEnabled(false);

  spinBox3 = new QSpinBox;
  spinBox3->setRange(1, 100000);
  spinBox3->setValue(1);
  spinBox3->setEnabled(false);

  spinBox4 = new QSpinBox;
  spinBox4->setRange(1, REDUCER_MAX_AA_FILTERS + 1);
  spinBox4->setValue(REDUCER_MAX_AA_FILTERS);
  spinBox4->setEnabled(false);

  SignalsTablewidget = new QTableWidget;
  SignalsTablewidget->setSelectionMode(QAbstractItemView::NoSelection);
  SignalsTablewidget->setColumnCount(2);
  SignalsTablewidget->setColumnWidth(0, 150 * mainwindow->w_scaling);
  SignalsTablewidget->setColumnWidth(1, 150 * mainwindow->w_scaling);

  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Samplerate divider";
  SignalsTablewidget->setHorizontalHeaderLabels(horizontallabels);

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");
  if(mainwindow->files_open < 2)
  {
    pushButton1->setEnabled(false);
  }

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  pushButton3 = new QPushButton;
  pushButton3->setText("Reduce");
  pushButton3->setEnabled(false);

  pushButton4 = new QPushButton;
  pushButton4->setText("Select all signals");
  pushButton4->setEnabled(false);

  pushButton5 = new QPushButton;
  pushButton5->setText("Deselect all signals");
  pushButton5->setEnabled(false);

  pushButton6 = new QPushButton;
  pushButton6->setText("Set samplerate divider:");
  pushButton6->setEnabled(false);

  pushButton7 = new QPushButton;
  pushButton7->setText("Load from JSON (MK LHSC):");
  pushButton7->setEnabled(false);
  pushButton7->setStyleSheet("font-weight: bold; color: darkMagenta;");


  helpButton = new QPushButton;
  helpButton->setText("Help");

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(pushButton4);
  hlayout3->addStretch(1000);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(pushButton5);
  hlayout4->addStretch(1000);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(pushButton6);
  hlayout5->addWidget(spinBox3);
  hlayout5->addStretch(1000);

  QHBoxLayout *hlayout13 = new QHBoxLayout;
  hlayout13->addWidget(pushButton7);
  hlayout13->addStretch(1000);


  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addWidget(label6);
  hlayout6->addWidget(spinBox4);
  hlayout6->addStretch(1000);

  QHBoxLayout *hlayout7 = new QHBoxLayout;
  hlayout7->addWidget(radioButton1);
  hlayout7->addStretch(1000);

  QHBoxLayout *hlayout8 = new QHBoxLayout;
  hlayout8->addWidget(radioButton2);
  hlayout8->addStretch(1000);

  QHBoxLayout *hlayout9 = new QHBoxLayout;
  hlayout9->addWidget(label2);
  hlayout9->addStretch(1000);

  QHBoxLayout *hlayout10 = new QHBoxLayout;
  hlayout10->addWidget(spinBox1);
  hlayout10->addWidget(label4);
  hlayout10->addStretch(1000);

  QHBoxLayout *hlayout11 = new QHBoxLayout;
  hlayout11->addWidget(label3);
  hlayout11->addStretch(1000);

  QHBoxLayout *hlayout12 = new QHBoxLayout;
  hlayout12->addWidget(spinBox2);
  hlayout12->addWidget(label5);
  hlayout12->addStretch(1000);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout3);
  vlayout2->addLayout(hlayout4);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout5);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout6);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout13);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout7);
  vlayout2->addLayout(hlayout8);
  vlayout2->addLayout(hlayout9);
  vlayout2->addLayout(hlayout10);
  vlayout2->addLayout(hlayout11);
  vlayout2->addLayout(hlayout12);
  vlayout2->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(SignalsTablewidget, 1000);
  hlayout2->addSpacing(20);
  hlayout2->addLayout(vlayout2);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton3);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(helpButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label1);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout2, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1,    SIGNAL(clicked()),         this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,    SIGNAL(clicked()),         myobjectDialog, SLOT(close()));
  QObject::connect(pushButton3,    SIGNAL(clicked()),         this,           SLOT(StartConversion()));
  QObject::connect(pushButton4,    SIGNAL(clicked()),         this,           SLOT(Select_all_signals()));
  QObject::connect(pushButton5,    SIGNAL(clicked()),         this,           SLOT(Deselect_all_signals()));
  QObject::connect(pushButton6,    SIGNAL(clicked()),         this,           SLOT(Set_SRdivider_all_signals()));
  QObject::connect(pushButton7,    SIGNAL(clicked()),         this,           SLOT(LoadFromJson()));
  QObject::connect(spinBox1,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox1changed(int)));
  QObject::connect(spinBox2,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox2changed(int)));
  QObject::connect(radioButton1,   SIGNAL(toggled(bool)),     this,           SLOT(radioButton1Toggled(bool)));
  QObject::connect(radioButton2,   SIGNAL(toggled(bool)),     this,           SLOT(radioButton2Toggled(bool)));
  QObject::connect(helpButton,     SIGNAL(clicked()),         this,           SLOT(helpbuttonpressed()));

  edfhdr = NULL;
  inputfile = NULL;
  outputfile = NULL;
  file_num = -1;

  inputpath[0] = 0;

  if(mainwindow->files_open == 1)
  {
    SelectFileButton();
  }

  myobjectDialog->exec();
}


void UI_ReduceSignalsWindow::spinBox1changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad_256[256]={""};

  QObject::disconnect(spinBox2,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox2changed(int)));
  spinBox2->setMinimum(value);
  QObject::connect(spinBox2,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox2changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)((((value - 1) * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING) / 86400LL);
  seconds = ((value - 1) * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING;
  if(seconds < 0)
  {
    seconds = 0;
  }
  seconds %= 86400LL;
  milliSec = ((value - 1) * edfhdr->long_data_record_duration) % TIME_FIXP_SCALING;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(scratchpad_256, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(scratchpad_256, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label4->setText(scratchpad_256);
}


void UI_ReduceSignalsWindow::spinBox2changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad_256[256]={""};

  QObject::disconnect(spinBox1,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox1changed(int)));
  spinBox1->setMaximum(value);
  QObject::connect(spinBox1,       SIGNAL(valueChanged(int)), this,           SLOT(spinBox1changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)(((value * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING) / 86400LL);
  seconds = (value * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING;
  seconds %= 86400LL;
  milliSec = (value * edfhdr->long_data_record_duration) % TIME_FIXP_SCALING;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(scratchpad_256, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(scratchpad_256, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(scratchpad_256);
}


void UI_ReduceSignalsWindow::radioButton1Toggled(bool checked)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad_256[256]={""};

  if(checked == true)
  {
    spinBox1->setEnabled(false);
    spinBox2->setEnabled(false);
    label2->setEnabled(false);
    label3->setEnabled(false);
    label4->setEnabled(false);
    label5->setEnabled(false);

    if(edfhdr == NULL)
    {
      return;
    }
    spinBox1->setValue(1);
    spinBox2->setMaximum(edfhdr->datarecords);
    spinBox2->setValue(edfhdr->datarecords);
    spinBox1->setMaximum(edfhdr->datarecords);

    days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING) / 86400LL);
    seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING;
    seconds %= 86400LL;
    milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_FIXP_SCALING;
    milliSec /= 10000LL;

    if(days > 0)
    {
      label4->setText("0d 0:00:00.000");

      snprintf(scratchpad_256, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }
    else
    {
      label4->setText("0:00:00.000");

      snprintf(scratchpad_256, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }

    label5->setText(scratchpad_256);
  }
}


void UI_ReduceSignalsWindow::radioButton2Toggled(bool checked)
{
  if(checked == true)
  {
    spinBox1->setEnabled(true);
    spinBox2->setEnabled(true);
    label2->setEnabled(true);
    label3->setEnabled(true);
    label4->setEnabled(true);
    label5->setEnabled(true);
  }
}


void UI_ReduceSignalsWindow::Select_all_signals()
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }
  }
}



void UI_ReduceSignalsWindow::Deselect_all_signals()
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
  }
}



void UI_ReduceSignalsWindow::Set_SRdivider_all_signals()
{
  int i, j, sr, idx;

  sr = spinBox3->value();

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(edfhdr->edfparam[i].annotation)
    {
      continue;
    }

    idx = 0;

    for(j=1; j<=edfhdr->edfparam[i].smp_per_record; j++)
    {
      if(!(edfhdr->edfparam[i].smp_per_record % j))
      {
        if(j == sr)
        {
          if(idx < ((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->count())
          {
            ((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->setCurrentIndex(idx);
          }

          break;
        }

        idx++;
      }
    }
  }
}



void UI_ReduceSignalsWindow::SelectFileButton()
{
  int i, j, k, days, found;

  long long seconds,
            milliSec;

  char str1_2048[2048]={""},
       str2_256[256]={""};

  label1->clear();
  label4->clear();
  label5->clear();

  SignalsTablewidget->setRowCount(0);

  inputfile = NULL;
  outputfile = NULL;

  inputpath[0] = 0;

  edfhdr = NULL;

  file_num = -1;

  pushButton3->setEnabled(false);
  pushButton4->setEnabled(false);
  pushButton5->setEnabled(false);
  pushButton6->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  spinBox3->setEnabled(false);
  spinBox4->setEnabled(false);
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);
  label4->setEnabled(false);
  label5->setEnabled(false);

  if(mainwindow->files_open > 1)
  {
    UI_activeFileChooserWindow afchooser(&file_num, mainwindow);

    if(file_num < 0)
    {
      return;
    }
  }
  else
  {
    file_num = 0;
  }

  edfhdr = mainwindow->edfheaderlist[file_num];

  strlcpy(inputpath, edfhdr->filename, MAX_PATH_LENGTH);

  inputfile = edfhdr->file_hdl;
  if(inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for reading.", inputpath);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str1_2048));
    messagewindow.exec();

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

  if(edfhdr->datarecords > 2147483647LL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Failure", "This file contains more than 2147483647 datarecords.\n"
                                                                "This tool cannot handle more than 2147483647 datarecords.");
    messagewindow.exec();

    inputfile = NULL;

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

/***************** load signalproperties ******************************/

  label1->setText(inputpath);

  SignalsTablewidget->setRowCount(edfhdr->edfsignals);

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    SignalsTablewidget->setCellWidget(i, 0, new QCheckBox(edfhdr->edfparam[i].label));
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setTristate(false);
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Checked);

    if(edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setEnabled(false);
    }

    if(!(edfhdr->edfparam[i].annotation))
    {
      SignalsTablewidget->setCellWidget(i, 1, new QComboBox);
      ((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->setEditable(false);

      for(j=1; j<=edfhdr->edfparam[i].smp_per_record; j++)
      {
        if(!(edfhdr->edfparam[i].smp_per_record % j))
        {
          snprintf(str2_256, 256, "%i  (", j);
          convert_to_metric_suffix(str2_256 + strlen(str2_256), edfhdr->edfparam[i].sf_f / j, 3, 256 - strlen(str2_256));
          remove_trailing_zeros(str2_256);
          strlcat(str2_256, "Hz)", 256);
          ((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->addItem(str2_256, QVariant(j));
        }
      }
    }
  }

  for(i=0, found=0; i<edfhdr->edfsignals; i++)
  {
    if(edfhdr->edfparam[i].annotation)  continue;

    for(j=0; j<mainwindow->signalcomps; j++)
    {
      for(k=0; k<mainwindow->signalcomp[j]->num_of_signals; k++)
      {
        if(mainwindow->signalcomp[j]->edfsignal[k] == i)
        {
          found = 1;

          break;
        }
      }

      if(found)  break;
    }

    if(found)
    {
      found = 0;
    }
    else
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
  }

  pushButton3->setEnabled(true);
  pushButton4->setEnabled(true);
  pushButton5->setEnabled(true);
  pushButton6->setEnabled(true);
  pushButton7->setEnabled(true);


  spinBox1->setValue(1);
  spinBox2->setMaximum(edfhdr->datarecords);
  spinBox2->setValue(edfhdr->datarecords);
  spinBox1->setMaximum(edfhdr->datarecords);
  spinBox3->setEnabled(true);
  spinBox4->setEnabled(true);

  radioButton1->setEnabled(true);
  radioButton2->setEnabled(true);

  label4->setText("0:00:00.000");
  days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING) / 86400LL);
  seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_FIXP_SCALING;
  seconds %= 86400LL;
  milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_FIXP_SCALING;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(str1_2048, 2048, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(str1_2048, 2048, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(str1_2048);
}



void UI_ReduceSignalsWindow::StartConversion()
{
  int i, j, k, n,
      new_edfsignals,
      datarecords=0,
      annot_smp_per_record,
      annot_recordsize,
      timestamp_digits=0,
      timestamp_decimals=0,
      annot_len,
      annot_cnt=0,
      tallen=0,
      len,
      annots_per_datrec=0,
      smplrt,
      tmp,
      val,
      progress_steps,
      datrecs_processed,
      annot_list_sz=0;

  char *readbuf=NULL,
       scratchpad_256[256]={""};

  long long new_starttime,
            time_diff,
            onset_diff,
            taltime,
            l_temp,
            endtime=0,
            l_tmp;

  date_time_t dts;

  annotlist_t new_annot_list,
                         *annot_list=NULL;

  annotblck_t *annot_ptr=NULL;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  memset(&new_annot_list, 0, sizeof(annotlist_t));

  QProgressDialog progress("Processing file...", "Abort", 0, 1);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  pushButton3->setEnabled(false);
  pushButton4->setEnabled(false);
  pushButton5->setEnabled(false);
  pushButton6->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  spinBox3->setEnabled(false);
  spinBox4->setEnabled(false);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);

  if(edfhdr==NULL)
  {
    return;
  }

  if(file_num < 0)
  {
    return;
  }

  new_edfsignals = 0;

  annot_smp_per_record = 0;

  aa_filter_order = spinBox4->value() - 1;

  time_diff = (long long)(spinBox1->value() - 1) * edfhdr->long_data_record_duration;

  taltime = (time_diff + edfhdr->starttime_subsec) % TIME_FIXP_SCALING;

  endtime = (long long)(spinBox2->value() - (spinBox1->value() - 1)) * edfhdr->long_data_record_duration + taltime;

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      if(((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->checkState()==Qt::Checked)
      {
        signalslist[new_edfsignals] = i;

        dividerlist[new_edfsignals] = ((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->itemData(((QComboBox *)(SignalsTablewidget->cellWidget(i, 1)))->currentIndex()).toInt();

        new_edfsignals++;
      }
    }
  }

  datarecords = spinBox2->value() - spinBox1->value() + 1;

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    timestamp_decimals = edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdr);
    if(timestamp_decimals < 0)
    {
      QMessageBox::critical(myobjectDialog, "Error", "Internal error, get_tal_timestamp_decimal_cnt(");
      goto END_1;
    }

    timestamp_digits = edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdr);
    if(timestamp_digits < 0)
    {
      QMessageBox::critical(myobjectDialog, "Error", "Internal error, get_tal_timestamp_digit_cnt(");
      goto END_1;
    }

    annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

    annot_list_sz = edfplus_annotation_size(annot_list);

    for(i=0; i<annot_list_sz; i++)
    {
      annot_ptr = edfplus_annotation_get_item(annot_list, i);

      l_temp = annot_ptr->onset - time_diff;

      if((l_temp >= 0LL) && (l_temp <= endtime))
      {
        edfplus_annotation_add_item(&new_annot_list, *(edfplus_annotation_get_item(annot_list, i)));
      }
    }

    new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_subsec) / TIME_FIXP_SCALING);

    onset_diff = (new_starttime - edfhdr->utc_starttime) * TIME_FIXP_SCALING;

    annot_list_sz = edfplus_annotation_size(&new_annot_list);

    if(annot_list_sz > 0)
    {
      for(i=0; i<annot_list_sz; i++)
      {
        annot_ptr = edfplus_annotation_get_item(&new_annot_list, i);

        annot_ptr->onset -= onset_diff;
      }

      edfplus_annotation_sort(&new_annot_list, NULL);

      annots_per_datrec = annot_list_sz / datarecords;

      if(annot_list_sz % datarecords)
      {
        annots_per_datrec++;
      }
    }
    else
    {
      annots_per_datrec = 0;
    }

    annot_len = edfplus_annotation_get_max_annotation_strlen(&new_annot_list);

    annot_recordsize = (annot_len * annots_per_datrec) + timestamp_digits + timestamp_decimals + 4;

    if(timestamp_decimals)
    {
      annot_recordsize++;
    }

    if(edfhdr->edf)
    {
      annot_smp_per_record = annot_recordsize / 2;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 2;
      }
    }
    else
    {
      annot_smp_per_record = annot_recordsize / 3;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 3;
      }
    }
  }
  else
  {
    annot_smp_per_record = 0;

    annot_recordsize = 0;
  }

  readbuf = (char *)malloc(edfhdr->recordsize);
  if(readbuf==NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Malloc error, (readbuf).");
    goto END_2;
  }
///////////////////////////////////////////////////////////////////

  for(i=0; i<new_edfsignals; i++)
  {
    if(dividerlist[i] > 1)
    {
      for(j=0; j<aa_filter_order; j++)
      {
        filterlist[i][j] = create_ravg_filter(1, dividerlist[i]);

        if(filterlist[i][j] == NULL)
        {
          QMessageBox::critical(myobjectDialog, "Error", "Malloc error, (create_ravg_filter()).");

          goto END_3;
        }
      }
    }
  }
///////////////////////////////////////////////////////////////////

  outputpath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strlcpy(outputpath, recent_savedir, MAX_PATH_LENGTH);
    strlcat(outputpath, "/", MAX_PATH_LENGTH);
  }
  len = strlen(outputpath);
  get_filename_from_path(outputpath + len, inputpath, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(outputpath);
  if(edfhdr->edf)
  {
    strlcat(outputpath, "_reduced.edf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }
  else
  {
    strlcat(outputpath, "_reduced.bdf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "BDF files (*.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(outputpath, ""))
  {
    goto END_3;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    QMessageBox::critical(myobjectDialog, "Error", "Error, selected file is in use.");
    goto END_3;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Cannot open outputfile for writing.");
    goto END_3;
  }

  new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_subsec) / TIME_FIXP_SCALING);

  utc_to_date_time(new_starttime, &dts);

  rewind(inputfile);
  if(fread(scratchpad_256, 168, 1, inputfile)!=1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Read error (1).");
    goto END_4;
  }

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    if(scratchpad_256[98] != 'X')
    {
      snprintf(scratchpad_256 + 98, 256 - 98, "%02i-%s-%04i", dts.day, dts.month_str, dts.year);

      scratchpad_256[109] = ' ';
    }
  }

  if(fwrite(scratchpad_256, 168, 1, outputfile)!=1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Write error (1).");
    goto END_4;
  }

  fprintf(outputfile, "%02i.%02i.%02i%02i.%02i.%02i",
          dts.day,
          dts.month,
          dts.year % 100,
          dts.hour,
          dts.minute,
          dts.second);

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 512);
  }
  else
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 256);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if((!edfhdr->edfplus) && (!edfhdr->bdfplus))
  {
    for(i=0; i<44; i++)
    {
      fputc(' ', outputfile);
    }
  }
  fprintf(outputfile, "%-8i", datarecords);
  snprintf(scratchpad_256, 256, "%f", edfhdr->data_record_duration);
  convert_trailing_zeros_to_spaces(scratchpad_256);
  if(scratchpad_256[7]=='.')
  {
    scratchpad_256[7] = ' ';
  }
  scratchpad_256[8] = 0;

  fprintf(outputfile, "%s", scratchpad_256);
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-4i", new_edfsignals + 1);
  }
  else
  {
    fprintf(outputfile, "%-4i", new_edfsignals);
  }

  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].label);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF Annotations ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF Annotations ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].transducer);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].physdimension);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<8; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad_256, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_min);
    convert_trailing_zeros_to_spaces(scratchpad_256);
    if(scratchpad_256[7]=='.')
    {
      scratchpad_256[7] = ' ';
    }
    scratchpad_256[8] = 0;
    fprintf(outputfile, "%s", scratchpad_256);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "-1      ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad_256, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_max);
    convert_trailing_zeros_to_spaces(scratchpad_256);
    if(scratchpad_256[7]=='.')
    {
      scratchpad_256[7] = ' ';
    }
    scratchpad_256[8] = 0;
    fprintf(outputfile, "%s", scratchpad_256);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "1       ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_min);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "-32768  ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "-8388608");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_max);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "32767   ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "8388607 ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].prefilter);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].smp_per_record / dividerlist[i]);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", annot_smp_per_record);
  }
  for(i=0; i<(new_edfsignals * 32); i++)
  {
   fputc(' ', outputfile);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<32; i++)
    {
      fputc(' ', outputfile);
    }
  }
///////////////////////////////////////////////////////////////////

  progress.setRange(0, datarecords);
  progress.setValue(0);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  fseeko(inputfile, (long long)edfhdr->hdrsize + ((long long)(spinBox1->value() - 1) * (long long)edfhdr->recordsize), SEEK_SET);

  for(datrecs_processed=0; datrecs_processed<datarecords; datrecs_processed++)
  {
    if(!(datrecs_processed % progress_steps))
    {
      progress.setValue(datrecs_processed);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto END_4;
      }
    }

    if(fread(readbuf, edfhdr->recordsize, 1, inputfile) != 1)
    {
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", "Read error (2).");
      goto END_4;
    }

    if(edfhdr->edf)
    {
      for(i=0; i<new_edfsignals; i++)
      {
        if(dividerlist[i] == 1)
        {
          smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record;

          for(j=0; j<smplrt; j++)
          {
            val = *(((signed short *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset)) + j);

            if(val > edfhdr->edfparam[signalslist[i]].dig_max)
            {
              val = edfhdr->edfparam[signalslist[i]].dig_max;
            }
            else if(val < edfhdr->edfparam[signalslist[i]].dig_min)
              {
                val = edfhdr->edfparam[signalslist[i]].dig_min;
              }

            fputc(val & 0xff, outputfile);
            if(fputc(val >> 8, outputfile)==EOF)
            {
              progress.reset();
              QMessageBox::critical(myobjectDialog, "Error", "Write error (4).");
              goto END_4;
            }
          }
        }
        else
        {
          smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record / dividerlist[i];

          for(j=0; j<smplrt; j++)
          {
            tmp = 0;

            for(k=0; k<dividerlist[i]; k++)
            {
              val = *(((signed short *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset)) + (dividerlist[i] * j) + k);

              if(val > edfhdr->edfparam[signalslist[i]].dig_max)
              {
                val = edfhdr->edfparam[signalslist[i]].dig_max;
              }
              else if(val < edfhdr->edfparam[signalslist[i]].dig_min)
                {
                  val = edfhdr->edfparam[signalslist[i]].dig_min;
                }

              for(n=0; n<aa_filter_order; n++)
              {
                val = run_ravg_filter(val, filterlist[i][n]);
              }

              tmp += val;
            }

            tmp /= dividerlist[i];

            fputc(tmp & 0xff, outputfile);
            if(fputc((tmp >> 8) & 0xff, outputfile)==EOF)
            {
              progress.reset();
              QMessageBox::critical(myobjectDialog, "Error", "Write error (4).");
              goto END_4;
            }
          }
        }
      }
    }
    else
    {
      for(i=0; i<new_edfsignals; i++)
      {
        if(dividerlist[i] == 1)
        {
          smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record;

          for(j=0; j<smplrt; j++)
          {
            var.two[0] = *((unsigned short *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset + (j * 3)));
            var.four[2] = *((unsigned char *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset + (j * 3) + 2));

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            if(var.one_signed > edfhdr->edfparam[signalslist[i]].dig_max)
            {
              var.one_signed = edfhdr->edfparam[signalslist[i]].dig_max;
            }
            else if(var.one_signed < edfhdr->edfparam[signalslist[i]].dig_min)
              {
                var.one_signed = edfhdr->edfparam[signalslist[i]].dig_min;
              }

            fputc(var.four[0], outputfile);
            fputc(var.four[1], outputfile);
            if(fputc(var.four[2], outputfile)==EOF)
            {
              progress.reset();
              QMessageBox::critical(myobjectDialog, "Error", "Write error (4).");
              goto END_4;
            }
          }
        }
        else
        {
          smplrt = edfhdr->edfparam[signalslist[i]].smp_per_record / dividerlist[i];

          for(j=0; j<smplrt; j++)
          {
            l_tmp = 0LL;

            for(k=0; k<dividerlist[i]; k++)
            {
              var.two[0] = *((unsigned short *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset + (dividerlist[i] * j * 3) + (k * 3)));
              var.four[2] = *((unsigned char *)(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset + (dividerlist[i] * j * 3) + (k * 3) + 2));

              if(var.four[2]&0x80)
              {
                var.four[3] = 0xff;
              }
              else
              {
                var.four[3] = 0x00;
              }

              if(var.one_signed > edfhdr->edfparam[signalslist[i]].dig_max)
              {
                var.one_signed = edfhdr->edfparam[signalslist[i]].dig_max;
              }
              else if(var.one_signed < edfhdr->edfparam[signalslist[i]].dig_min)
                {
                  var.one_signed = edfhdr->edfparam[signalslist[i]].dig_min;
                }

              for(n=0; n<aa_filter_order; n++)
              {
                var.one_signed = run_ravg_filter(var.one_signed, filterlist[i][n]);
              }

              l_tmp += var.one_signed;
            }

            l_tmp /= dividerlist[i];

            fputc(l_tmp & 0xff, outputfile);
            fputc((l_tmp >> 8) & 0xff, outputfile);
            if(fputc((l_tmp >> 16) & 0xff, outputfile)==EOF)
            {
              progress.reset();
              QMessageBox::critical(myobjectDialog, "Error", "Write error (4).");
              goto END_4;
            }
          }
        }
      }
    }

    if(edfhdr->edfplus || edfhdr->bdfplus)
    {
      switch(timestamp_decimals)
      {
        case 0 : tallen = fprintf(outputfile, "+%i", (int)(taltime / TIME_FIXP_SCALING));
                  break;
        case 1 : tallen = fprintf(outputfile, "+%i.%01i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 1000000LL));
                  break;
        case 2 : tallen = fprintf(outputfile, "+%i.%02i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 100000LL));
                  break;
        case 3 : tallen = fprintf(outputfile, "+%i.%03i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 10000LL));
                  break;
        case 4 : tallen = fprintf(outputfile, "+%i.%04i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 1000LL));
                  break;
        case 5 : tallen = fprintf(outputfile, "+%i.%05i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 100LL));
                  break;
        case 6 : tallen = fprintf(outputfile, "+%i.%06i", (int)(taltime / TIME_FIXP_SCALING), (int)((taltime % TIME_FIXP_SCALING) / 10LL));
                  break;
        case 7 : tallen = fprintf(outputfile, "+%i.%07i", (int)(taltime / TIME_FIXP_SCALING), (int)(taltime % TIME_FIXP_SCALING));
                  break;
      }

      fputc(20, outputfile);
      fputc(20, outputfile);
      fputc(0, outputfile);

      tallen += 3;

      if(annot_cnt < annot_list_sz)
      {
        for(i=0; i<annots_per_datrec; i++)
        {
          if(annot_cnt >= annot_list_sz)  break;

          annot_ptr = edfplus_annotation_get_item(&new_annot_list, annot_cnt++);

          len = snprintf(scratchpad_256, 256, "%+i.%07i",
          (int)(annot_ptr->onset / TIME_FIXP_SCALING),
          (int)(annot_ptr->onset % TIME_FIXP_SCALING));

          for(j=0; j<7; j++)
          {
            if(scratchpad_256[len - j - 1] != '0')
            {
              break;
            }
          }

          if(j)
          {
            len -= j;

            if(j == 7)
            {
              len--;
            }
          }

          if(fwrite(scratchpad_256, len, 1, outputfile) != 1)
          {
            progress.reset();
            QMessageBox::critical(myobjectDialog, "Error", "Write error (5).");
            goto END_4;
          }

          tallen += len;

          if(annot_ptr->duration[0]!=0)
          {
            fputc(21, outputfile);
            tallen++;

            tallen += fprintf(outputfile, "%s", annot_ptr->duration);
          }

          fputc(20, outputfile);
          tallen++;

          tallen += fprintf(outputfile, "%s", annot_ptr->description);

          fputc(20, outputfile);
          fputc(0, outputfile);
          tallen += 2;
        }
      }

      for(k=tallen; k<annot_recordsize; k++)
      {
        fputc(0, outputfile);
      }

      taltime += edfhdr->long_data_record_duration;
    }
  }

  progress.reset();
  QMessageBox::information(myobjectDialog, "Ready", "Done.");

END_4:

  fclose(outputfile);
  outputfile = NULL;

END_3:

  for(i=0; i<new_edfsignals; i++)
  {
    if(dividerlist[i] > 1)
    {
      for(j=0; j<aa_filter_order; j++)
      {
        free_ravg_filter(filterlist[i][j]);
      }
    }
  }

END_2:

  if(readbuf != NULL)
  {
    free(readbuf);
    readbuf = NULL;
  }

END_1:

  inputfile = NULL;

  inputpath[0] = 0;

  label1->clear();
  label4->clear();
  label5->clear();

  file_num = -1;

  edfhdr = NULL;

  edfplus_annotation_empty_list(&new_annot_list);

  SignalsTablewidget->setRowCount(0);
}


void UI_ReduceSignalsWindow::helpbuttonpressed()
{
  mainwindow->open_manual("#Reduce_signals");
}




#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void UI_ReduceSignalsWindow::LoadFromJson()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Select JSON File", "", "JSON Files (*.json)");

    if (filePath.isEmpty()) {
        return;  // User canceled
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(myobjectDialog, "Error", "Cannot open JSON file.");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        QMessageBox::critical(myobjectDialog, "Error", "Invalid JSON format.");
        return;
    }

    QJsonObject rootObj = doc.object();

    if (!rootObj.contains("selected_signals") || !rootObj["selected_signals"].isArray()) {
        QMessageBox::critical(myobjectDialog, "Error", "Invalid JSON structure. Missing 'selected_signals' array.");
        return;
    }

    QJsonArray selectedSignalsArray = rootObj["selected_signals"].toArray();
    QMap<QString, int> selectedSignals;  // Maps {label -> samplerate_divider}

    // Extract selected signals from JSON
    for (const QJsonValue &value : selectedSignalsArray) {
        if (value.isObject()) {
            QJsonObject signalObj = value.toObject();
            if (signalObj.contains("label") && signalObj.contains("samplerate_divider")) {
                QString label = signalObj["label"].toString().trimmed(); // Normalize label
                int samplerateDivider = signalObj["samplerate_divider"].toInt();
                selectedSignals[label] = samplerateDivider;
            }
        }
    }

    // Iterate over the existing signals in the checklist and update their selection state
    for (int i = 0; i < SignalsTablewidget->rowCount(); ++i) {
        QWidget *widget = SignalsTablewidget->cellWidget(i, 0);  // Get cell widget in column 0 (checkbox)
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);  // Cast to checkbox
        QComboBox *comboBox = qobject_cast<QComboBox *>(SignalsTablewidget->cellWidget(i, 1)); // Get combobox in column 1

        if (checkBox) {
            QString signalLabel = checkBox->text().trimmed();  // Normalize text

            if (selectedSignals.contains(signalLabel)) {
                checkBox->setCheckState(Qt::Checked);  // Select the signal

                if (comboBox) {
                    int samplerateDivider = selectedSignals[signalLabel];
                    int index = comboBox->findData(samplerateDivider);
                    if (index != -1) {
                        comboBox->setCurrentIndex(index);  // Set correct sample rate
                    }
                }
            } else {
                checkBox->setCheckState(Qt::Unchecked);  // Uncheck if not in JSON
            }
        }
    }

    QMessageBox::information(myobjectDialog, "Success", "Selected signals updated based on JSON.");
}
