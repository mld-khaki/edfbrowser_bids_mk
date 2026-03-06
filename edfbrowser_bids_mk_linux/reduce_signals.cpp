/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2025 Teunis van Beelen
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


/* =========================
   Command-line (headless) reduction
   ========================= */

#include <stdarg.h>
#include <math.h>

#include <QMap>
#include <QMapIterator>
#include <QStringList>
#include <QByteArray>
#include <QIODevice>

#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "edf_annotations.h"

static void rs_set_err(char *err_out, int err_out_sz, const char *fmt, ...)
{
  if((err_out == NULL) || (err_out_sz < 2))  return;

  va_list args;
  va_start(args, fmt);
  vsnprintf(err_out, err_out_sz, fmt, args);
  err_out[err_out_sz - 1] = 0;
  va_end(args);
}

static QString rs_norm_label(const char *lbl)
{
  if(lbl == NULL)  return QString();
  return QString::fromLocal8Bit(lbl).trimmed();
}

static int rs_find_signal_index(edfhdrblck_t *edfhdr, const QString &needle)
{
  if(edfhdr == NULL)  return -1;

  const QString n = needle.trimmed();

  for(int i=0; i<edfhdr->edfsignals; i++)
  {
    if(edfhdr->edfparam[i].annotation)  continue;

    if(rs_norm_label(edfhdr->edfparam[i].label).compare(n, Qt::CaseInsensitive) == 0)
    {
      return i;
    }
  }

  return -1;
}

/* Wildcard matching:
     * matches any sequence
     ? matches one char
     # matches one digit
*/
static bool rs_match_pattern_impl(const QString &s, int si, const QString &p, int pi)
{
  const int slen = s.size();
  const int plen = p.size();

  int sidx = si;
  int pidx = pi;

  while(true)
  {
    if(pidx >= plen)
    {
      return (sidx >= slen);
    }

    const QChar pc = p.at(pidx);

    if(pc == QChar('*'))
    {
      while((pidx + 1 < plen) && (p.at(pidx + 1) == QChar('*')))
      {
        pidx++;
      }

      if(pidx + 1 >= plen)
      {
        return true;
      }

      for(int k = sidx; k <= slen; k++)
      {
        if(rs_match_pattern_impl(s, k, p, pidx + 1))
        {
          return true;
        }
      }

      return false;
    }

    if(sidx >= slen)
    {
      return false;
    }

    if(pc == QChar('?'))
    {
      sidx++;
      pidx++;
      continue;
    }

    if(pc == QChar('#'))
    {
      if(!s.at(sidx).isDigit())
      {
        return false;
      }
      sidx++;
      pidx++;
      continue;
    }

    if(s.at(sidx).toCaseFolded() != pc.toCaseFolded())
    {
      return false;
    }

    sidx++;
    pidx++;
  }
}

static bool rs_match_pattern(const QString &label, const QString &pattern)
{
  const QString s = label.trimmed();
  const QString p = pattern.trimmed();

  if(p.isEmpty())
  {
    return false;
  }

  return rs_match_pattern_impl(s, 0, p, 0);
}

static bool rs_match_any_mask(const QString &label, const QString &mask_csv)
{
  const QString m = mask_csv.trimmed();
  if(m.isEmpty())
  {
    return true;
  }

  const QStringList patterns = m.split(',', Qt::SkipEmptyParts);
  for(const QString &pat : patterns)
  {
    if(rs_match_pattern(label, pat))
    {
      return true;
    }
  }

  return false;
}

static bool rs_load_json_config(const QString &json_path,
                                QMap<QString, int> &label_to_divider,
                                bool &has_range,
                                int &from_rec,
                                int &to_rec,
                                bool &has_aa,
                                int &aa_order,
                                QString &err)
{
  label_to_divider.clear();
  has_range = false;
  has_aa = false;
  err.clear();

  QFile f(json_path);
  if(!f.open(QIODevice::ReadOnly))
  {
    err = "Cannot open JSON file: " + json_path;
    return false;
  }

  const QByteArray jsonData = f.readAll();
  f.close();

  QJsonParseError jerr;
  QJsonDocument doc = QJsonDocument::fromJson(jsonData, &jerr);
  if(jerr.error != QJsonParseError::NoError)
  {
    err = "Invalid JSON: " + jerr.errorString();
    return false;
  }

  if(!doc.isObject())
  {
    err = "Invalid JSON: root is not an object";
    return false;
  }

  const QJsonObject rootObj = doc.object();

  if(rootObj.contains("selected_signals") && rootObj["selected_signals"].isArray())
  {
    const QJsonArray selectedSignalsArray = rootObj["selected_signals"].toArray();

    for(const QJsonValue &value : selectedSignalsArray)
    {
      if(!value.isObject())  continue;

      const QJsonObject signalObj = value.toObject();

      if(!signalObj.contains("label"))  continue;

      const QString label = signalObj["label"].toString().trimmed();
      if(label.isEmpty())  continue;

      int divider = 1;
      if(signalObj.contains("samplerate_divider"))
      {
        divider = signalObj["samplerate_divider"].toInt(1);
      }

      label_to_divider[label] = divider;
    }
  }

  if(rootObj.contains("from_datarecord") || rootObj.contains("to_datarecord"))
  {
    from_rec = rootObj.value("from_datarecord").toInt(from_rec);
    to_rec   = rootObj.value("to_datarecord").toInt(to_rec);
    has_range = true;
  }

  if(rootObj.contains("aa_filter_order"))
  {
    aa_order = rootObj.value("aa_filter_order").toInt(aa_order);
    has_aa = true;
  }

  return true;
}

static void rs_make_default_outpath(const char *input_path, int is_edf, char *output_path, int out_sz)
{
  if((output_path == NULL) || (out_sz < 4))  return;

  output_path[0] = 0;

  if((input_path == NULL) || (input_path[0] == 0))  return;

  strlcpy(output_path, input_path, out_sz);
  remove_extension_from_filename(output_path);

  if(is_edf)
  {
    strlcat(output_path, "_reduced.edf", out_sz);
  }
  else
  {
    strlcat(output_path, "_reduced.bdf", out_sz);
  }
}

static bool rs_parse_date(const QString &date_part, QDate &out_date)
{
  QString d = date_part.trimmed();
  if(d.isEmpty())  return false;

  /* Accept:
       YYYY-MM-DD
       DD.MM.YYYY
       DD.MM.YY
  */
  if(d.contains('-'))
  {
    const QStringList parts = d.split('-', Qt::SkipEmptyParts);
    if(parts.size() == 3)
    {
      bool okY=false, okM=false, okD=false;
      int y = parts[0].toInt(&okY);
      int m = parts[1].toInt(&okM);
      int day = parts[2].toInt(&okD);
      if(okY && okM && okD)
      {
        QDate dt(y,m,day);
        if(dt.isValid())
        {
          out_date = dt;
          return true;
        }
      }
    }
  }

  if(d.contains('.'))
  {
    const QStringList parts = d.split('.', Qt::SkipEmptyParts);
    if(parts.size() == 3)
    {
      bool okD=false, okM=false, okY=false;
      int day = parts[0].toInt(&okD);
      int m = parts[1].toInt(&okM);
      int y = parts[2].toInt(&okY);
      if(okD && okM && okY)
      {
        if(y < 100) y += 2000;
        QDate dt(y,m,day);
        if(dt.isValid())
        {
          out_date = dt;
          return true;
        }
      }
    }
  }

  return false;
}

static bool rs_parse_time(const QString &time_part, QTime &out_time)
{
  QString t = time_part.trimmed();
  if(t.isEmpty())  return false;

  /* normalize common typos: HH-MM-SS -> HH:MM:SS */
  t.replace('-', ':');

  QString frac;
  QString main = t;

  if(t.contains('.'))
  {
    const int dot = t.indexOf('.');
    main = t.left(dot);
    frac = t.mid(dot + 1);
  }
  else
  {
    const QStringList parts0 = t.split(':', Qt::KeepEmptyParts);
    if(parts0.size() == 4)
    {
      main = parts0[0] + ":" + parts0[1] + ":" + parts0[2];
      frac = parts0[3];
    }
  }

  const QStringList parts = main.split(':', Qt::SkipEmptyParts);
  if(parts.size() < 2 || parts.size() > 3)  return false;

  bool okH=false, okM=false, okS=true;
  int h = parts[0].toInt(&okH);
  int m = parts[1].toInt(&okM);
  int s = 0;
  if(parts.size() == 3)
  {
    s = parts[2].toInt(&okS);
  }

  if(!(okH && okM && okS))  return false;

  int ms = 0;
  if(!frac.isEmpty())
  {
    QString digits;
    for(const QChar c : frac)
    {
      if(c.isDigit()) digits.append(c);
      else break;
    }
    if(!digits.isEmpty())
    {
      if(digits.size() > 3) digits = digits.left(3);
      while(digits.size() < 3) digits.append('0');
      bool ok=false;
      ms = digits.toInt(&ok);
      if(!ok) ms = 0;
    }
  }

  QTime qt(h,m,s,ms);
  if(!qt.isValid())  return false;

  out_time = qt;
  return true;
}

static bool rs_parse_datetime_flexible(const QString &input,
                                       const QDate &fallback_date,
                                       QDateTime &out_dt,
                                       bool &had_explicit_date,
                                       QString &err)
{
  err.clear();
  had_explicit_date = false;

  QString s = input.trimmed();
  if(s.isEmpty())
  {
    err = "Empty datetime string.";
    return false;
  }

  s.replace('T', ' ');

  QString datePart;
  QString timePart;

  const int sp = s.indexOf(' ');
  if(sp >= 0)
  {
    datePart = s.left(sp).trimmed();
    timePart = s.mid(sp + 1).trimmed();
    had_explicit_date = true;
  }
  else
  {
    timePart = s;
  }

  QDate dt = fallback_date;
  if(had_explicit_date)
  {
    if(!rs_parse_date(datePart, dt))
    {
      err = "Invalid date: " + datePart;
      return false;
    }
  }

  QTime tm;
  if(!rs_parse_time(timePart, tm))
  {
    err = "Invalid time: " + timePart;
    return false;
  }

  /* Interpret as UTC to align with edfhdr->utc_starttime usage. */
  out_dt = QDateTime(dt, tm, Qt::UTC);
  if(!out_dt.isValid())
  {
    err = "Invalid datetime: " + s;
    return false;
  }

  return true;
}

/* Convert time window to datarecord range using the user's Python-style rounding.
   - If input timestamps contain an explicit date, we use absolute time (UTC).
   - If only time-of-day is supplied, we emulate the Python helper behavior including
     a midnight wrap relative to the recording start time-of-day. */
static bool rs_compute_record_range_from_timestamps(edfhdrblck_t *edfhdr,
                                                    const reduce_signals_cli_options_t &opt,
                                                    int &out_from_rec,
                                                    int &out_to_rec,
                                                    QString &err)
{
  err.clear();

  if(edfhdr == NULL)
  {
    err = "Internal error: edfhdr is NULL.";
    return false;
  }

  if(edfhdr->datarecords <= 0)
  {
    err = "File has no datarecords.";
    return false;
  }

  const long long total_recs = edfhdr->datarecords;

  if(total_recs > 2147483647LL)
  {
    err = "This tool cannot handle more than 2147483647 datarecords.";
    return false;
  }

  QDateTime rec_start_dt = QDateTime::fromSecsSinceEpoch(edfhdr->utc_starttime, Qt::UTC);
  QDate fallback_date = rec_start_dt.date();

  /* Defaults */
  QString from_str = QString::fromLocal8Bit(opt.from_timestamp).trimmed();
  QString to_str   = QString::fromLocal8Bit(opt.to_timestamp).trimmed();

  if(from_str.isEmpty() && to_str.isEmpty())
  {
    out_from_rec = opt.from_datarecord > 0 ? opt.from_datarecord : 1;
    out_to_rec   = opt.to_datarecord > 0 ? opt.to_datarecord : (int)total_recs;
    return true;
  }

  bool from_has_date = false;
  bool to_has_date = false;
  QDateTime from_dt, to_dt;

  if(!from_str.isEmpty())
  {
    if(!rs_parse_datetime_flexible(from_str, fallback_date, from_dt, from_has_date, err))
    {
      err = "Invalid --from: " + err;
      return false;
    }
  }

  QDate to_fallback = fallback_date;
  if(from_has_date) to_fallback = from_dt.date();

  if(!to_str.isEmpty())
  {
    if(!rs_parse_datetime_flexible(to_str, to_fallback, to_dt, to_has_date, err))
    {
      err = "Invalid --to: " + err;
      return false;
    }
  }

  /* Apply offsets in minutes (like the Python helper) */
  const int pre_off_s  = opt.pre_offset_minutes  * 60;
  const int post_off_s = opt.post_offset_minutes * 60;

  const double recdur_sec = edfhdr->data_record_duration > 0.0 ? edfhdr->data_record_duration : 1.0;

  /* Path A: both timestamps are time-of-day only -> Python helper semantics */
  if((!from_str.isEmpty() && !from_has_date) && (!to_str.isEmpty() && !to_has_date))
  {
    const QTime rec_tm = rec_start_dt.time();
    const int rec_start_sod = rec_tm.hour() * 3600 + rec_tm.minute() * 60 + rec_tm.second();

    const QTime from_tm = from_dt.time();
    const QTime to_tm2 = to_dt.time();

    int target_from_sod = from_tm.hour() * 3600 + from_tm.minute() * 60 + from_tm.second();
    int target_to_sod   = to_tm2.hour() * 3600 + to_tm2.minute() * 60 + to_tm2.second();

    int adjusted_start = target_from_sod - pre_off_s;
    int adjusted_end   = target_to_sod + post_off_s;

    if(adjusted_start < rec_start_sod) adjusted_start += 24 * 3600;
    if(adjusted_end   < rec_start_sod) adjusted_end   += 24 * 3600;

    double sec_from_start = (double)(adjusted_start - rec_start_sod);
    double sec_to_end     = (double)(adjusted_end   - rec_start_sod);

    const double total_duration_sec = (double)edfhdr->datarecords * recdur_sec;

    if(sec_from_start < 0.0) sec_from_start = 0.0;
    if(sec_to_end < 0.0) sec_to_end = 0.0;
    if(sec_from_start > total_duration_sec) sec_from_start = total_duration_sec;
    if(sec_to_end > total_duration_sec) sec_to_end = total_duration_sec;

    long long from0 = llround(sec_from_start / recdur_sec);
    long long to0_excl = llround(sec_to_end / recdur_sec);

    long long from_rec = from0 + 1;
    long long to_rec = to0_excl;

    if(from_rec < 1) from_rec = 1;
    if(to_rec < 1) to_rec = 1;
    if(from_rec > total_recs) from_rec = total_recs;
    if(to_rec > total_recs) to_rec = total_recs;

    if(to_rec < from_rec)
    {
      err = QString("Invalid time window after conversion: from_rec=%1 > to_rec=%2").arg(from_rec).arg(to_rec);
      return false;
    }

    out_from_rec = (int)from_rec;
    out_to_rec = (int)to_rec;
    return true;
  }

  /* Path B: any explicit date present -> absolute UTC timestamps */
  auto dt_to_fixp = [](const QDateTime &dt) -> long long
  {
    const qint64 ms = dt.toMSecsSinceEpoch();
    const qint64 sec = ms / 1000;
    const qint64 rem_ms = ms % 1000;
    return (long long)sec * TIME_FIXP_SCALING + (long long)rem_ms * 10000LL;
  };

  long long start_fixp = (long long)edfhdr->utc_starttime * TIME_FIXP_SCALING + (long long)edfhdr->starttime_subsec;

  /* If only one side supplied, default the other to file bounds */
  QDateTime adj_from = from_dt;
  QDateTime adj_to = to_dt;

  if(from_str.isEmpty())
  {
    adj_from = rec_start_dt;
  }
  if(to_str.isEmpty())
  {
    /* end time: recording start + total duration */
    const long long total_fixp = (long long)edfhdr->datarecords * edfhdr->long_data_record_duration;
    long long end_fixp = start_fixp + total_fixp;
    const qint64 end_sec = end_fixp / TIME_FIXP_SCALING;
    const qint64 end_ms  = (end_fixp % TIME_FIXP_SCALING) / 10000LL;
    adj_to = QDateTime::fromSecsSinceEpoch(end_sec, Qt::UTC).addMSecs(end_ms);
  }

  if(pre_off_s)  adj_from = adj_from.addSecs(-pre_off_s);
  if(post_off_s) adj_to   = adj_to.addSecs(+post_off_s);

  long long from_fixp = dt_to_fixp(adj_from);
  long long to_fixp   = dt_to_fixp(adj_to);

  long long delta_from = from_fixp - start_fixp;
  long long delta_to   = to_fixp   - start_fixp;

  if(delta_from < 0LL) delta_from = 0LL;
  if(delta_to < 0LL) delta_to = 0LL;

  const long long total_fixp = (long long)edfhdr->datarecords * edfhdr->long_data_record_duration;
  if(delta_from > total_fixp) delta_from = total_fixp;
  if(delta_to > total_fixp) delta_to = total_fixp;

  long long from0 = llround((double)delta_from / (double)edfhdr->long_data_record_duration);
  long long to0_excl = llround((double)delta_to / (double)edfhdr->long_data_record_duration);

  long long from_rec = from0 + 1;
  long long to_rec = to0_excl;

  if(from_rec < 1) from_rec = 1;
  if(to_rec < 1) to_rec = 1;
  if(from_rec > total_recs) from_rec = total_recs;
  if(to_rec > total_recs) to_rec = total_recs;

  if(to_rec < from_rec)
  {
    err = QString("Invalid time window after conversion: from_rec=%1 > to_rec=%2").arg(from_rec).arg(to_rec);
    return false;
  }

  out_from_rec = (int)from_rec;
  out_to_rec = (int)to_rec;
  return true;
}

class rs_reduce_cli_runner
{
public:
  rs_reduce_cli_runner(const char *in_path,
                       const char *out_path,
                       const reduce_signals_cli_options_t &opt,
                       char *err_out,
                       int err_out_sz)
  : input_path_(in_path ? in_path : ""),
    output_path_in_(out_path ? out_path : ""),
    opt_(opt),
    err_out_(err_out),
    err_out_sz_(err_out_sz)
  {
    inputfile_ = NULL;
    outputfile_ = NULL;
    edfhdr_ = NULL;
    readbuf_ = NULL;
    memset(filterlist_, 0, sizeof(filterlist_));
    memset(&new_annot_list_, 0, sizeof(annotlist_t));
    out_path_[0] = 0;
    memset(signalslist_, 0, sizeof(signalslist_));
    memset(dividerlist_, 0, sizeof(dividerlist_));
    new_edfsignals_ = 0;
    annot_smp_per_record_ = 0;
    annot_recordsize_ = 0;
    timestamp_digits_ = 0;
    timestamp_decimals_ = 0;
    annot_len_ = 0;
    annot_list_sz_ = 0;
    annots_per_datrec_ = 0;
    time_diff_ = 0LL;
    taltime_ = 0LL;
    endtime_ = 0LL;
  }

  ~rs_reduce_cli_runner()
  {
    if(outputfile_ != NULL)
    {
      fclose(outputfile_);
      outputfile_ = NULL;
    }

    for(int i=0; i<MAXSIGNALS; i++)
    {
      for(int j=0; j<REDUCER_MAX_AA_FILTERS; j++)
      {
        if(filterlist_[i][j] != NULL)
        {
          free_ravg_filter(filterlist_[i][j]);
          filterlist_[i][j] = NULL;
        }
      }
    }

    if(readbuf_ != NULL)
    {
      free(readbuf_);
      readbuf_ = NULL;
    }

    edfplus_annotation_empty_list(&new_annot_list_);

    if(edfhdr_ != NULL)
    {
      edfplus_annotation_empty_list(&edfhdr_->annot_list);

      if(edfhdr_->edfparam != NULL)
      {
        free(edfhdr_->edfparam);
        edfhdr_->edfparam = NULL;
      }

      free(edfhdr_);
      edfhdr_ = NULL;
    }

    if(inputfile_ != NULL)
    {
      fclose(inputfile_);
      inputfile_ = NULL;
    }
  }

  int run()
  {
    if(input_path_.isEmpty())
    {
      rs_set_err(err_out_, err_out_sz_, "No input path provided.");
      return 1;
    }

    if(opt_.from_datarecord <= 0) opt_.from_datarecord = 1;
    if(opt_.aa_filter_order < 0) opt_.aa_filter_order = 0;
    if(opt_.aa_filter_order > REDUCER_MAX_AA_FILTERS) opt_.aa_filter_order = REDUCER_MAX_AA_FILTERS;
    if(opt_.global_samplerate_divider <= 0) opt_.global_samplerate_divider = 1;

    if(!open_input_())
    {
      return 2;
    }

    if(edfhdr_->discontinuous)
    {
      rs_set_err(err_out_, err_out_sz_, "EDFbrowser cannot process EDF+D/BDF+D (discontinuous) files for reduction.");
      return 3;
    }

    if(!resolve_output_path_())
    {
      return 4;
    }

    if(!apply_json_and_time_range_())
    {
      return 5;
    }

    if(!validate_range_())
    {
      return 6;
    }

    if(!build_signal_selection_())
    {
      return 7;
    }

    if(!prepare_annotations_())
    {
      return 8;
    }

    if(!allocate_buffers_and_filters_())
    {
      return 9;
    }

    if(!open_output_())
    {
      return 10;
    }

    if(!write_output_header_())
    {
      return 11;
    }

    if(!process_records_())
    {
      return 12;
    }

    rs_set_err(err_out_, err_out_sz_, "");
    fprintf(stdout, "[reduce-signals] Done.\n");
    return 0;
  }

private:
  QString input_path_;
  QString output_path_in_;
  reduce_signals_cli_options_t opt_;
  char *err_out_;
  int err_out_sz_;

  FILE *inputfile_;
  FILE *outputfile_;
  edfhdrblck_t *edfhdr_;
  char *readbuf_;

  ravgfiltset_t *filterlist_[MAXSIGNALS][REDUCER_MAX_AA_FILTERS];

  /* selection */
  int signalslist_[MAXSIGNALS];
  int dividerlist_[MAXSIGNALS];
  int new_edfsignals_;

  /* output path */
  char out_path_[MAX_PATH_LENGTH];

  /* annotations for output */
  annotlist_t new_annot_list_;
  int annot_smp_per_record_;
  int annot_recordsize_;
  int timestamp_digits_;
  int timestamp_decimals_;
  int annot_len_;
  int annot_list_sz_;
  int annots_per_datrec_;
  long long time_diff_;
  long long taltime_;
  long long endtime_;

  bool open_input_()
  {
    char errbuf_local[4096] = {0};

    inputfile_ = fopeno(input_path_.toLocal8Bit().constData(), "rb");
    if(inputfile_ == NULL)
    {
      rs_set_err(err_out_, err_out_sz_, "Cannot open input file for reading: %s", input_path_.toLocal8Bit().constData());
      return false;
    }

    edfhdr_ = check_edf_file(inputfile_, errbuf_local, sizeof(errbuf_local), 0, 0);
    if(edfhdr_ == NULL)
    {
      rs_set_err(err_out_, err_out_sz_, "Not a valid EDF/BDF file: %s\n%s", input_path_.toLocal8Bit().constData(), errbuf_local);
      return false;
    }

    strlcpy(edfhdr_->filename, input_path_.toLocal8Bit().constData(), MAX_PATH_LENGTH);
    edfhdr_->file_hdl = inputfile_;

    memset(&edfhdr_->annot_list, 0, sizeof(annotlist_t));

    if((opt_.read_annotations != 0) && (edfhdr_->edfplus || edfhdr_->bdfplus))
    {
      EDF_annotations annotations;
      annotations.get_annotations(edfhdr_, 0);

      if(edfhdr_->annots_not_read)
      {
        edfplus_annotation_empty_list(&edfhdr_->annot_list);
        fprintf(stderr, "[reduce-signals] Warning: annotations were not read; output will keep an empty annotation channel.\n");
      }
    }

    return true;
  }

  bool resolve_output_path_()
  {
    /* Determine output path */
    if(!output_path_in_.isEmpty())
    {
      strlcpy(out_path_, output_path_in_.toLocal8Bit().constData(), MAX_PATH_LENGTH);
    }
    else
    {
      rs_make_default_outpath(input_path_.toLocal8Bit().constData(), edfhdr_->edf, out_path_, MAX_PATH_LENGTH);
    }

    if(out_path_[0] == 0)
    {
      rs_set_err(err_out_, err_out_sz_, "No output path provided and default could not be formed.");
      return false;
    }

    QFileInfo outInfo(QString::fromLocal8Bit(out_path_));
    if(outInfo.exists() && (opt_.overwrite_existing == 0))
    {
      rs_set_err(err_out_, err_out_sz_, "Output file already exists (use --overwrite to allow): %s", out_path_);
      return false;
    }

    return true;
  }

  bool apply_json_and_time_range_()
  {
    /* Apply JSON config if provided */
    QMap<QString, int> json_label_to_divider;
    bool json_has_range = false;
    bool json_has_aa = false;
    int json_from = opt_.from_datarecord;
    int json_to = opt_.to_datarecord;
    int json_aa = opt_.aa_filter_order;

    if(opt_.json_config_path[0] != 0)
    {
      QString jerr;
      if(!rs_load_json_config(QString::fromLocal8Bit(opt_.json_config_path),
                              json_label_to_divider,
                              json_has_range,
                              json_from,
                              json_to,
                              json_has_aa,
                              json_aa,
                              jerr))
      {
        rs_set_err(err_out_, err_out_sz_, "%s", jerr.toLocal8Bit().constData());
        return false;
      }

      if(json_has_range)
      {
        opt_.from_datarecord = json_from;
        opt_.to_datarecord = json_to;
      }

      if(json_has_aa)
      {
        opt_.aa_filter_order = json_aa;
        if(opt_.aa_filter_order < 0) opt_.aa_filter_order = 0;
        if(opt_.aa_filter_order > REDUCER_MAX_AA_FILTERS) opt_.aa_filter_order = REDUCER_MAX_AA_FILTERS;
      }
    }

    /* If user provided --from/--to timestamps, compute the datarecord range now (overrides JSON/record range). */
    if((opt_.from_timestamp[0] != 0) || (opt_.to_timestamp[0] != 0))
    {
      QString terr;
      int t_from = opt_.from_datarecord;
      int t_to = opt_.to_datarecord;

      if(!rs_compute_record_range_from_timestamps(edfhdr_, opt_, t_from, t_to, terr))
      {
        rs_set_err(err_out_, err_out_sz_, "%s", terr.toLocal8Bit().constData());
        return false;
      }

      opt_.from_datarecord = t_from;
      opt_.to_datarecord = t_to;

      fprintf(stderr, "[reduce-signals] Converted time window to datarecords: %d..%d\n", t_from, t_to);
    }

    /* If to_datarecord omitted, default to end-of-file */
    if(opt_.to_datarecord <= 0)
    {
      if(edfhdr_->datarecords > 2147483647LL)
      {
        rs_set_err(err_out_, err_out_sz_, "This tool cannot handle more than 2147483647 datarecords.");
        return false;
      }
      opt_.to_datarecord = (int)edfhdr_->datarecords;
    }

    if(opt_.from_datarecord <= 0) opt_.from_datarecord = 1;

    return true;
  }

  bool validate_range_()
  {
    if(opt_.from_datarecord < 1) opt_.from_datarecord = 1;

    if(opt_.from_datarecord > opt_.to_datarecord)
    {
      rs_set_err(err_out_, err_out_sz_, "Invalid range: from_datarecord (%d) > to_datarecord (%d).",
                 opt_.from_datarecord, opt_.to_datarecord);
      return false;
    }

    if(opt_.to_datarecord > (int)edfhdr_->datarecords)
    {
      rs_set_err(err_out_, err_out_sz_, "Invalid range: to_datarecord (%d) exceeds file datarecords (%lli).",
                 opt_.to_datarecord, edfhdr_->datarecords);
      return false;
    }

    return true;
  }

  bool build_signal_selection_()
  {
    /* Re-load JSON (if any) just for selection map; this keeps run() logic clean */
    QMap<QString, int> json_label_to_divider;
    bool dummy_range=false, dummy_aa=false;
    int dummy_from=opt_.from_datarecord, dummy_to=opt_.to_datarecord, dummy_aa_order=opt_.aa_filter_order;
    QString dummy_err;

    if(opt_.json_config_path[0] != 0)
    {
      rs_load_json_config(QString::fromLocal8Bit(opt_.json_config_path),
                          json_label_to_divider,
                          dummy_range, dummy_from, dummy_to,
                          dummy_aa, dummy_aa_order,
                          dummy_err);
    }

    int selected[MAXSIGNALS];
    int divider_by_idx[MAXSIGNALS];

    for(int i=0; i<edfhdr_->edfsignals; i++)
    {
      selected[i] = 0;
      divider_by_idx[i] = 1;
    }

    const QString csv = QString::fromLocal8Bit(opt_.signals_csv).trimmed();

    if(!json_label_to_divider.isEmpty())
    {
      QMapIterator<QString, int> it(json_label_to_divider);
      while(it.hasNext())
      {
        it.next();

        const QString label = it.key().trimmed();
        const int div = it.value() > 0 ? it.value() : 1;

        const int idx = rs_find_signal_index(edfhdr_, label);
        if(idx < 0)
        {
          fprintf(stderr, "[reduce-signals] Warning: signal label not found, skipping: %s\n",
                  label.toLocal8Bit().constData());
          continue;
        }

        selected[idx] = 1;
        divider_by_idx[idx] = div;
      }
    }
    else if(csv.isEmpty() || (csv.compare("all", Qt::CaseInsensitive) == 0))
    {
      for(int i=0; i<edfhdr_->edfsignals; i++)
      {
        if(edfhdr_->edfparam[i].annotation)  continue;
        selected[i] = 1;
        divider_by_idx[i] = opt_.global_samplerate_divider;
      }
    }
    else
    {
      const QStringList tokens = csv.split(',', Qt::SkipEmptyParts);

      for(const QString &rawTok : tokens)
      {
        const QString tok = rawTok.trimmed();
        if(tok.isEmpty())  continue;

        QString left = tok;
        QString right;

        const int colonPos = tok.indexOf(':');
        if(colonPos >= 0)
        {
          left = tok.left(colonPos).trimmed();
          right = tok.mid(colonPos + 1).trimmed();
        }

        int divider = opt_.global_samplerate_divider;
        if(!right.isEmpty())
        {
          bool ok = false;
          divider = right.toInt(&ok);
          if(!ok || (divider <= 0))
          {
            rs_set_err(err_out_, err_out_sz_, "Invalid samplerate divider in --signals token: %s",
                       tok.toLocal8Bit().constData());
            return false;
          }
        }

        int idx = -1;

        if(left.startsWith('#'))
        {
          bool ok = false;
          idx = left.mid(1).toInt(&ok);
          if(!ok) idx = -1;
        }
        else
        {
          idx = rs_find_signal_index(edfhdr_, left);

          if(idx < 0)
          {
            bool ok = false;
            idx = left.toInt(&ok);
            if(!ok) idx = -1;
          }
        }

        if((idx < 0) || (idx >= edfhdr_->edfsignals) || edfhdr_->edfparam[idx].annotation)
        {
          rs_set_err(err_out_, err_out_sz_, "Signal not found / invalid in --signals token: %s",
                     tok.toLocal8Bit().constData());
          return false;
        }

        selected[idx] = 1;
        divider_by_idx[idx] = divider;
      }
    }

    /* Apply mask filtering (if any) */
    const QString mask_csv = QString::fromLocal8Bit(opt_.mask_signals).trimmed();
    if(!mask_csv.isEmpty())
    {
      for(int i=0; i<edfhdr_->edfsignals; i++)
      {
        if(edfhdr_->edfparam[i].annotation)
        {
          selected[i] = 0;
          continue;
        }

        if(!selected[i])  continue;

        const QString lbl = rs_norm_label(edfhdr_->edfparam[i].label);
        if(!rs_match_any_mask(lbl, mask_csv))
        {
          selected[i] = 0;
        }
      }
    }

    /* Validate and compact */
    new_edfsignals_ = 0;

    for(int i=0; i<edfhdr_->edfsignals; i++)
    {
      if(edfhdr_->edfparam[i].annotation)  continue;
      if(!selected[i])  continue;

      int div = divider_by_idx[i];
      if(div <= 0) div = 1;

      if(edfhdr_->edfparam[i].smp_per_record < div)
      {
        rs_set_err(err_out_, err_out_sz_,
                   "Invalid divider for signal '%s': divider=%d exceeds smp/record=%d",
                   rs_norm_label(edfhdr_->edfparam[i].label).toLocal8Bit().constData(),
                   div, edfhdr_->edfparam[i].smp_per_record);
        return false;
      }

      if((edfhdr_->edfparam[i].smp_per_record % div) != 0)
      {
        rs_set_err(err_out_, err_out_sz_,
                   "Invalid divider for signal '%s': divider=%d must evenly divide smp/record=%d",
                   rs_norm_label(edfhdr_->edfparam[i].label).toLocal8Bit().constData(),
                   div, edfhdr_->edfparam[i].smp_per_record);
        return false;
      }

      signalslist_[new_edfsignals_] = i;
      dividerlist_[new_edfsignals_] = div;
      new_edfsignals_++;
    }

    if(new_edfsignals_ < 1)
    {
      rs_set_err(err_out_, err_out_sz_, "No signals selected.");
      return false;
    }

    return true;
  }

  bool prepare_annotations_()
  {
    const int datarecords = opt_.to_datarecord - opt_.from_datarecord + 1;

    time_diff_ = (long long)(opt_.from_datarecord - 1) * edfhdr_->long_data_record_duration;
    taltime_ = (time_diff_ + edfhdr_->starttime_subsec) % TIME_FIXP_SCALING;
    endtime_ = (long long)(datarecords) * edfhdr_->long_data_record_duration + taltime_;

    annot_smp_per_record_ = 0;
    annot_recordsize_ = 0;
    timestamp_digits_ = 0;
    timestamp_decimals_ = 0;
    annot_len_ = 0;
    annot_list_sz_ = 0;
    annots_per_datrec_ = 0;

    if(!(edfhdr_->edfplus || edfhdr_->bdfplus))
    {
      return true;
    }

    timestamp_decimals_ = edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdr_);
    if(timestamp_decimals_ < 0)
    {
      rs_set_err(err_out_, err_out_sz_, "Internal error: get_tal_timestamp_decimal_cnt()");
      return false;
    }

    timestamp_digits_ = edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdr_);
    if(timestamp_digits_ < 0)
    {
      rs_set_err(err_out_, err_out_sz_, "Internal error: get_tal_timestamp_digit_cnt()");
      return false;
    }

    annot_list_sz_ = edfplus_annotation_size(&edfhdr_->annot_list);

    for(int i=0; i<annot_list_sz_; i++)
    {
      annotblck_t *annot_ptr = edfplus_annotation_get_item(&edfhdr_->annot_list, i);

      long long l_temp = annot_ptr->onset - time_diff_;

      if((l_temp >= 0LL) && (l_temp <= endtime_))
      {
        edfplus_annotation_add_item(&new_annot_list_, *annot_ptr);
      }
    }

    const long long new_starttime = edfhdr_->utc_starttime + ((time_diff_ + edfhdr_->starttime_subsec) / TIME_FIXP_SCALING);
    const long long onset_diff = (new_starttime - edfhdr_->utc_starttime) * TIME_FIXP_SCALING;

    annot_list_sz_ = edfplus_annotation_size(&new_annot_list_);

    if(annot_list_sz_ > 0)
    {
      for(int i=0; i<annot_list_sz_; i++)
      {
        annotblck_t *annot_ptr = edfplus_annotation_get_item(&new_annot_list_, i);
        annot_ptr->onset -= onset_diff;
      }

      edfplus_annotation_sort(&new_annot_list_, NULL);

      annots_per_datrec_ = annot_list_sz_ / datarecords;
      if(annot_list_sz_ % datarecords)  annots_per_datrec_++;
    }
    else
    {
      annots_per_datrec_ = 0;
    }

    annot_len_ = edfplus_annotation_get_max_annotation_strlen(&new_annot_list_);

    annot_recordsize_ = (annot_len_ * annots_per_datrec_) + timestamp_digits_ + timestamp_decimals_ + 4;
    if(timestamp_decimals_)  annot_recordsize_++;

    if(edfhdr_->edf)
    {
      annot_smp_per_record_ = annot_recordsize_ / 2;

      if(annot_smp_per_record_ < 1) annot_smp_per_record_ = 1;

      if(annot_recordsize_ % annot_smp_per_record_)
      {
        annot_smp_per_record_++;
        annot_recordsize_ = annot_smp_per_record_ * 2;
      }
    }
    else
    {
      annot_smp_per_record_ = annot_recordsize_ / 3;

      if(annot_smp_per_record_ < 1) annot_smp_per_record_ = 1;

      if(annot_recordsize_ % annot_smp_per_record_)
      {
        annot_smp_per_record_++;
        annot_recordsize_ = annot_smp_per_record_ * 3;
      }
    }

    return true;
  }

  bool allocate_buffers_and_filters_()
  {
    readbuf_ = (char *)malloc(edfhdr_->recordsize);
    if(readbuf_ == NULL)
    {
      rs_set_err(err_out_, err_out_sz_, "Malloc error (readbuf).");
      return false;
    }

    for(int i=0; i<new_edfsignals_; i++)
    {
      if(dividerlist_[i] > 1)
      {
        for(int j=0; j<opt_.aa_filter_order; j++)
        {
          filterlist_[i][j] = create_ravg_filter(1, dividerlist_[i]);
          if(filterlist_[i][j] == NULL)
          {
            rs_set_err(err_out_, err_out_sz_, "Malloc error (create_ravg_filter()).");
            return false;
          }
        }
      }
    }

    return true;
  }

  bool open_output_()
  {
    outputfile_ = fopeno(out_path_, "wb");
    if(outputfile_ == NULL)
    {
      rs_set_err(err_out_, err_out_sz_, "Cannot open output file for writing: %s", out_path_);
      return false;
    }

    return true;
  }

  bool write_output_header_()
  {
    /* This header-writing code mirrors the GUI's StartConversion() logic. */
    int i;
    date_time_t dts;
    char scratchpad_256[256] = {0};

    const int datarecords = opt_.to_datarecord - opt_.from_datarecord + 1;

    const long long new_starttime = edfhdr_->utc_starttime + ((time_diff_ + edfhdr_->starttime_subsec) / TIME_FIXP_SCALING);

    utc_to_date_time(new_starttime, &dts);

    rewind(inputfile_);
    if(fread(scratchpad_256, 168, 1, inputfile_) != 1)
    {
      rs_set_err(err_out_, err_out_sz_, "Read error (header).");
      return false;
    }

    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      if(scratchpad_256[98] != 'X')
      {
        snprintf(scratchpad_256 + 98, 256 - 98, "%02i-%s-%04i", dts.day, dts.month_str, dts.year);
        scratchpad_256[109] = ' ';
      }
    }

    if(fwrite(scratchpad_256, 168, 1, outputfile_) != 1)
    {
      rs_set_err(err_out_, err_out_sz_, "Write error (header 1).");
      return false;
    }

    fprintf(outputfile_, "%02i.%02i.%02i%02i.%02i.%02i",
            dts.day,
            dts.month,
            dts.year % 100,
            dts.hour,
            dts.minute,
            dts.second);

    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      fprintf(outputfile_, "%-8i", new_edfsignals_ * 256 + 512);
    }
    else
    {
      fprintf(outputfile_, "%-8i", new_edfsignals_ * 256 + 256);
    }

    if(edfhdr_->edfplus)
    {
      fprintf(outputfile_, "EDF+C");
      for(i=0; i<39; i++)  fputc(' ', outputfile_);
    }
    if(edfhdr_->bdfplus)
    {
      fprintf(outputfile_, "BDF+C");
      for(i=0; i<39; i++)  fputc(' ', outputfile_);
    }
    if((!edfhdr_->edfplus) && (!edfhdr_->bdfplus))
    {
      for(i=0; i<44; i++)  fputc(' ', outputfile_);
    }

    fprintf(outputfile_, "%-8i", datarecords);

    snprintf(scratchpad_256, 256, "%f", edfhdr_->data_record_duration);
    convert_trailing_zeros_to_spaces(scratchpad_256);
    if(scratchpad_256[7]=='.')
    {
      scratchpad_256[7] = ' ';
    }
    scratchpad_256[8] = 0;
    fprintf(outputfile_, "%s", scratchpad_256);

    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      fprintf(outputfile_, "%-4i", new_edfsignals_ + 1);
    }
    else
    {
      fprintf(outputfile_, "%-4i", new_edfsignals_);
    }

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%s", edfhdr_->edfparam[signalslist_[i]].label);
    }
    if(edfhdr_->edfplus)  fprintf(outputfile_, "EDF Annotations ");
    if(edfhdr_->bdfplus)  fprintf(outputfile_, "BDF Annotations ");

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%s", edfhdr_->edfparam[signalslist_[i]].transducer);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      for(i=0; i<80; i++)  fputc(' ', outputfile_);
    }

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%s", edfhdr_->edfparam[signalslist_[i]].physdimension);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      for(i=0; i<8; i++)  fputc(' ', outputfile_);
    }

    for(i=0; i<new_edfsignals_; i++)
    {
      snprintf(scratchpad_256, 256, "%f", edfhdr_->edfparam[signalslist_[i]].phys_min);
      convert_trailing_zeros_to_spaces(scratchpad_256);
      if(scratchpad_256[7]=='.')  scratchpad_256[7] = ' ';
      scratchpad_256[8] = 0;
      fprintf(outputfile_, "%s", scratchpad_256);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)  fprintf(outputfile_, "-1      ");

    for(i=0; i<new_edfsignals_; i++)
    {
      snprintf(scratchpad_256, 256, "%f", edfhdr_->edfparam[signalslist_[i]].phys_max);
      convert_trailing_zeros_to_spaces(scratchpad_256);
      if(scratchpad_256[7]=='.')  scratchpad_256[7] = ' ';
      scratchpad_256[8] = 0;
      fprintf(outputfile_, "%s", scratchpad_256);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)  fprintf(outputfile_, "1       ");

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%-8i", edfhdr_->edfparam[signalslist_[i]].dig_min);
    }
    if(edfhdr_->edfplus) fprintf(outputfile_, "-32768  ");
    if(edfhdr_->bdfplus) fprintf(outputfile_, "-8388608");

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%-8i", edfhdr_->edfparam[signalslist_[i]].dig_max);
    }
    if(edfhdr_->edfplus) fprintf(outputfile_, "32767   ");
    if(edfhdr_->bdfplus) fprintf(outputfile_, "8388607 ");

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%s", edfhdr_->edfparam[signalslist_[i]].prefilter);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      for(i=0; i<80; i++)  fputc(' ', outputfile_);
    }

    for(i=0; i<new_edfsignals_; i++)
    {
      fprintf(outputfile_, "%-8i", edfhdr_->edfparam[signalslist_[i]].smp_per_record / dividerlist_[i]);
    }
    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      fprintf(outputfile_, "%-8i", annot_smp_per_record_);
    }

    for(i=0; i<(new_edfsignals_ * 32); i++)  fputc(' ', outputfile_);
    if(edfhdr_->edfplus || edfhdr_->bdfplus)
    {
      for(i=0; i<32; i++)  fputc(' ', outputfile_);
    }

    return true;
  }

  bool process_records_()
  {
    const int datarecords = opt_.to_datarecord - opt_.from_datarecord + 1;

    fprintf(stdout, "[reduce-signals] Input:  %s\n", input_path_.toLocal8Bit().constData());
    fprintf(stdout, "[reduce-signals] Output: %s\n", out_path_);
    fprintf(stdout, "[reduce-signals] Records: %d..%d (%d records)\n",
            opt_.from_datarecord, opt_.to_datarecord, datarecords);
    fprintf(stdout, "[reduce-signals] Signals: %d\n", new_edfsignals_);
    fprintf(stdout, "[reduce-signals] AA order: %d\n", opt_.aa_filter_order);

    fseeko(inputfile_, (long long)edfhdr_->hdrsize + ((long long)(opt_.from_datarecord - 1) * (long long)edfhdr_->recordsize), SEEK_SET);

    int annot_cnt = 0;

    union {
            unsigned int one;
            signed int one_signed;
            unsigned short two[2];
            signed short two_signed[2];
            unsigned char four[4];
          } var;

    const int progress_steps = (datarecords / 100) < 1 ? 1 : (datarecords / 100);

    for(int datrecs_processed=0; datrecs_processed<datarecords; datrecs_processed++)
    {
      if(!(datrecs_processed % progress_steps))
      {
        fprintf(stdout, "\r[reduce-signals] %d/%d", datrecs_processed, datarecords);
        fflush(stdout);
      }

      if(fread(readbuf_, edfhdr_->recordsize, 1, inputfile_) != 1)
      {
        rs_set_err(err_out_, err_out_sz_, "Read error (data).");
        return false;
      }

      if(edfhdr_->edf)
      {
        for(int i=0; i<new_edfsignals_; i++)
        {
          int smplrt;
          int tmp;
          int val;

          if(dividerlist_[i] == 1)
          {
            smplrt = edfhdr_->edfparam[signalslist_[i]].smp_per_record;

            for(int j=0; j<smplrt; j++)
            {
              val = *(((signed short *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset)) + j);

              if(val > edfhdr_->edfparam[signalslist_[i]].dig_max)  val = edfhdr_->edfparam[signalslist_[i]].dig_max;
              else if(val < edfhdr_->edfparam[signalslist_[i]].dig_min)  val = edfhdr_->edfparam[signalslist_[i]].dig_min;

              fputc(val & 0xff, outputfile_);
              if(fputc(val >> 8, outputfile_) == EOF)
              {
                rs_set_err(err_out_, err_out_sz_, "Write error (EDF).");
                return false;
              }
            }
          }
          else
          {
            smplrt = edfhdr_->edfparam[signalslist_[i]].smp_per_record / dividerlist_[i];

            for(int j=0; j<smplrt; j++)
            {
              tmp = 0;

              for(int k=0; k<dividerlist_[i]; k++)
              {
                val = *(((signed short *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset)) + (dividerlist_[i] * j) + k);

                if(val > edfhdr_->edfparam[signalslist_[i]].dig_max)  val = edfhdr_->edfparam[signalslist_[i]].dig_max;
                else if(val < edfhdr_->edfparam[signalslist_[i]].dig_min)  val = edfhdr_->edfparam[signalslist_[i]].dig_min;

                for(int n=0; n<opt_.aa_filter_order; n++)
                {
                  val = run_ravg_filter(val, filterlist_[i][n]);
                }

                tmp += val;
              }

              tmp /= dividerlist_[i];

              fputc(tmp & 0xff, outputfile_);
              if(fputc((tmp >> 8) & 0xff, outputfile_) == EOF)
              {
                rs_set_err(err_out_, err_out_sz_, "Write error (EDF).");
                return false;
              }
            }
          }
        }
      }
      else
      {
        for(int i=0; i<new_edfsignals_; i++)
        {
          int smplrt;

          if(dividerlist_[i] == 1)
          {
            smplrt = edfhdr_->edfparam[signalslist_[i]].smp_per_record;

            for(int j=0; j<smplrt; j++)
            {
              var.two[0] = *((unsigned short *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset + (j * 3)));
              var.four[2] = *((unsigned char *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset + (j * 3) + 2));

              if(var.four[2] & 0x80)  var.four[3] = 0xff;
              else                    var.four[3] = 0x00;

              if(var.one_signed > edfhdr_->edfparam[signalslist_[i]].dig_max)  var.one_signed = edfhdr_->edfparam[signalslist_[i]].dig_max;
              else if(var.one_signed < edfhdr_->edfparam[signalslist_[i]].dig_min)  var.one_signed = edfhdr_->edfparam[signalslist_[i]].dig_min;

              fputc(var.four[0], outputfile_);
              fputc(var.four[1], outputfile_);
              if(fputc(var.four[2], outputfile_) == EOF)
              {
                rs_set_err(err_out_, err_out_sz_, "Write error (BDF).");
                return false;
              }
            }
          }
          else
          {
            smplrt = edfhdr_->edfparam[signalslist_[i]].smp_per_record / dividerlist_[i];

            for(int j=0; j<smplrt; j++)
            {
              long long l_tmp = 0LL;

              for(int k=0; k<dividerlist_[i]; k++)
              {
                var.two[0] = *((unsigned short *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset + (dividerlist_[i] * j * 3) + (k * 3)));
                var.four[2] = *((unsigned char *)(readbuf_ + edfhdr_->edfparam[signalslist_[i]].datrec_offset + (dividerlist_[i] * j * 3) + (k * 3) + 2));

                if(var.four[2] & 0x80)  var.four[3] = 0xff;
                else                    var.four[3] = 0x00;

                if(var.one_signed > edfhdr_->edfparam[signalslist_[i]].dig_max)  var.one_signed = edfhdr_->edfparam[signalslist_[i]].dig_max;
                else if(var.one_signed < edfhdr_->edfparam[signalslist_[i]].dig_min)  var.one_signed = edfhdr_->edfparam[signalslist_[i]].dig_min;

                for(int n=0; n<opt_.aa_filter_order; n++)
                {
                  var.one_signed = run_ravg_filter(var.one_signed, filterlist_[i][n]);
                }

                l_tmp += var.one_signed;
              }

              l_tmp /= dividerlist_[i];

              fputc(l_tmp & 0xff, outputfile_);
              fputc((l_tmp >> 8) & 0xff, outputfile_);
              if(fputc((l_tmp >> 16) & 0xff, outputfile_) == EOF)
              {
                rs_set_err(err_out_, err_out_sz_, "Write error (BDF).");
                return false;
              }
            }
          }
        }
      }

      if(edfhdr_->edfplus || edfhdr_->bdfplus)
      {
        int tallen = 0;

        switch(timestamp_decimals_)
        {
          case 0 : tallen = fprintf(outputfile_, "+%i", (int)(taltime_ / TIME_FIXP_SCALING)); break;
          case 1 : tallen = fprintf(outputfile_, "+%i.%01i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 1000000LL)); break;
          case 2 : tallen = fprintf(outputfile_, "+%i.%02i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 100000LL)); break;
          case 3 : tallen = fprintf(outputfile_, "+%i.%03i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 10000LL)); break;
          case 4 : tallen = fprintf(outputfile_, "+%i.%04i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 1000LL)); break;
          case 5 : tallen = fprintf(outputfile_, "+%i.%05i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 100LL)); break;
          case 6 : tallen = fprintf(outputfile_, "+%i.%06i", (int)(taltime_ / TIME_FIXP_SCALING), (int)((taltime_ % TIME_FIXP_SCALING) / 10LL)); break;
          case 7 : tallen = fprintf(outputfile_, "+%i.%07i", (int)(taltime_ / TIME_FIXP_SCALING), (int)(taltime_ % TIME_FIXP_SCALING)); break;
        }

        fputc(20, outputfile_);
        fputc(20, outputfile_);
        fputc(0, outputfile_);
        tallen += 3;

        if(annot_cnt < annot_list_sz_)
        {
          for(int i=0; i<annots_per_datrec_; i++)
          {
            if(annot_cnt >= annot_list_sz_)  break;

            annotblck_t *annot_ptr = edfplus_annotation_get_item(&new_annot_list_, annot_cnt++);

            char scratchpad_256[256];
            int len = snprintf(scratchpad_256, 256, "%+i.%07i",
                               (int)(annot_ptr->onset / TIME_FIXP_SCALING),
                               (int)(annot_ptr->onset % TIME_FIXP_SCALING));

            int j;
            for(j=0; j<7; j++)
            {
              if(scratchpad_256[len - j - 1] != '0')  break;
            }

            if(j)
            {
              len -= j;
              if(j == 7)  len--;
            }

            if(fwrite(scratchpad_256, len, 1, outputfile_) != 1)
            {
              rs_set_err(err_out_, err_out_sz_, "Write error (annotations).");
              return false;
            }

            tallen += len;

            if(annot_ptr->duration[0] != 0)
            {
              fputc(21, outputfile_);
              tallen++;
              tallen += fprintf(outputfile_, "%s", annot_ptr->duration);
            }

            fputc(20, outputfile_);
            tallen++;
            tallen += fprintf(outputfile_, "%s", annot_ptr->description);

            fputc(20, outputfile_);
            fputc(0, outputfile_);
            tallen += 2;
          }
        }

        for(int k=tallen; k<annot_recordsize_; k++)  fputc(0, outputfile_);

        taltime_ += edfhdr_->long_data_record_duration;
      }
    }

    fprintf(stdout, "\r[reduce-signals] %d/%d\n", datarecords, datarecords);
    return true;
  }
};

int reduce_signals_cli(const char *input_path,
                       const char *output_path,
                       const reduce_signals_cli_options_t *opt_in,
                       char *err_out,
                       int err_out_sz)
{
  reduce_signals_cli_options_t opt;
  memset(&opt, 0, sizeof(opt));

  /* Reasonable defaults for CLI */
  opt.from_datarecord = 1;
  opt.to_datarecord = 0;
  opt.pre_offset_minutes = 0;
  opt.post_offset_minutes = 0;
  opt.aa_filter_order = 0;
  opt.read_annotations = 1;
  opt.overwrite_existing = 0;
  opt.global_samplerate_divider = 1;
  opt.signals_csv[0] = 0;
  opt.mask_signals[0] = 0;
  opt.json_config_path[0] = 0;
  opt.from_timestamp[0] = 0;
  opt.to_timestamp[0] = 0;

  if(opt_in != NULL)
  {
    opt = *opt_in;
  }

  rs_reduce_cli_runner runner(input_path, output_path, opt, err_out, err_out_sz);
  return runner.run();
}
