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



#include "show_edf_hdr.h"



UI_EDFhdrwindow::UI_EDFhdrwindow(QWidget *w_parent)
{
  int i;

  signalslist_page = 0;

  mainwindow = (UI_Mainwindow *)w_parent;

  EDFhdrDialog = new QDialog;
  EDFhdrDialog->setMinimumSize(600 * mainwindow->w_scaling, 680 * mainwindow->h_scaling);
  EDFhdrDialog->setWindowTitle("EDF header info");
  EDFhdrDialog->setModal(true);
  EDFhdrDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  EDFhdrDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setMinimumSize(600, 75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  signallist = new QTableWidget;
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);

  closeButton = new QPushButton;
  closeButton->setText("Close");

  line_edit1 = new QLineEdit;
  line_edit1->setReadOnly(true);
  line_edit1a = new QLineEdit;
  line_edit1a->setReadOnly(true);
  line_edit2 = new QLineEdit;
  line_edit2->setReadOnly(true);
  line_edit2a = new QLineEdit;
  line_edit2a->setReadOnly(true);
  line_edit3 = new QLineEdit;
  line_edit3->setReadOnly(true);
  line_edit3a = new QLineEdit;
  line_edit3a->setReadOnly(true);
  line_edit4 = new QLineEdit;
  line_edit4->setReadOnly(true);
  line_edit4a = new QLineEdit;
  line_edit4a->setReadOnly(true);
  line_edit5 = new QLineEdit;
  line_edit5->setReadOnly(true);
  line_edit5a = new QLineEdit;
  line_edit5a->setReadOnly(true);
  line_edit6 = new QLineEdit;
  line_edit6->setReadOnly(true);
  line_edit7 = new QLineEdit;
  line_edit7->setReadOnly(true);
  line_edit8 = new QLineEdit;
  line_edit8->setReadOnly(true);
  line_edit9 = new QLineEdit;
  line_edit9->setReadOnly(true);
  line_edit10 = new QLineEdit;
  line_edit10->setReadOnly(true);
  line_edit11 = new QLineEdit;
  line_edit11->setReadOnly(true);
  line_edit12 = new QLineEdit;
  line_edit12->setReadOnly(true);
  line_edit13 = new QLineEdit;
  line_edit13->setReadOnly(true);
  line_edit20 = new QLineEdit;
  line_edit20->setReadOnly(true);
  line_edit20a = new QLineEdit;
  line_edit20a->setReadOnly(true);
  line_edit21 = new QLineEdit;
  line_edit21->setReadOnly(true);
  line_edit21a = new QLineEdit;
  line_edit21a->setReadOnly(true);
  line_edit22 = new QLineEdit;
  line_edit22->setReadOnly(true);
  line_edit22a = new QLineEdit;
  line_edit22a->setReadOnly(true);
  line_edit23 = new QLineEdit;
  line_edit23->setReadOnly(true);
  line_edit23a = new QLineEdit;
  line_edit23a->setReadOnly(true);

  QFormLayout *flayout1 = new QFormLayout;
  flayout1->addRow("Subject code", line_edit1);
  flayout1->addRow("Sex", line_edit2);
  flayout1->addRow("Start", line_edit3);
  flayout1->addRow("End", line_edit23);
  flayout1->addRow("Duration", line_edit4);
  flayout1->addRow("Reserved", line_edit5);
  flayout1->addRow("Birthdate", line_edit6);
  flayout1->addRow("Subject name", line_edit7);
  flayout1->addRow("Additional info", line_edit8);
  flayout1->addRow("Startdate", line_edit9);
  flayout1->addRow("Administr. code", line_edit10);
  flayout1->addRow("Technician", line_edit11);
  flayout1->addRow("Device", line_edit12);
  flayout1->addRow("Additional info", line_edit13);
  flayout1->addRow("Datarecord duration", line_edit20);
  flayout1->addRow("Datarecords", line_edit22);
  flayout1->addRow("Version", line_edit21);

  QFormLayout *flayout2 = new QFormLayout;
  flayout2->addRow("Subject", line_edit1a);
  flayout2->addRow("Recording", line_edit2a);
  flayout2->addRow("Start", line_edit3a);
  flayout2->addRow("End", line_edit23a);
  flayout2->addRow("Duration", line_edit4a);
  flayout2->addRow("Reserved", line_edit5a);
  flayout2->addRow("Datarecord duration", line_edit20a);
  flayout2->addRow("Datarecords", line_edit22a);
  flayout2->addRow("Version", line_edit21a);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addLayout(flayout1);
  vlayout2->addStretch(1000);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addLayout(flayout2);
  vlayout3->addStretch(1000);

  tab1 = new QWidget;
  tab1->setLayout(vlayout2);

  tab1a = new QWidget;
  tab1a->setLayout(vlayout3);

  stackwidget = new QStackedWidget;
  stackwidget->addWidget(tab1);
  stackwidget->addWidget(tab1a);

  QTabWidget *tabholder = new QTabWidget;

  prevPageButton = new QPushButton;
  prevPageButton->setText("Previous");
  prevPageButton->setEnabled(false);
  nextPageButton = new QPushButton;
  nextPageButton->setText("Next");
  nextPageButton->setEnabled(false);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addStretch(1000);
  hlayout2->addWidget(prevPageButton);
  hlayout2->addWidget(nextPageButton);
  hlayout2->addStretch(1000);

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addWidget(signallist, 100);
  vlayout4->addLayout(hlayout2);

  QFrame *frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);
  frame->setLayout(vlayout4);

  tabholder->addTab(stackwidget, "General");
  tabholder->addTab(frame, "Signals");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(closeButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist);
  vlayout1->addSpacing(20);
  vlayout1->addWidget(tabholder, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  EDFhdrDialog->setLayout(vlayout1);

  QObject::connect(closeButton,    SIGNAL(clicked()),              EDFhdrDialog, SLOT(close()));
  QObject::connect(filelist,       SIGNAL(currentRowChanged(int)), this,         SLOT(show_params(int)));
  QObject::connect(prevPageButton, SIGNAL(clicked()),              this,         SLOT(prevPageButtonClicked()));
  QObject::connect(nextPageButton, SIGNAL(clicked()),              this,         SLOT(nextPageButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  EDFhdrDialog->exec();
}


void UI_EDFhdrwindow::show_params(int row)
{
  int signal_cnt=0;

  char str1_512[512]={""};

  long long file_duration;

  date_time_t date_time;

  if(row<0)  return;

  signalslist_page = 0;

  file_row_selected = row;

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  signalslist_num_pages = signal_cnt / 512;
  if(signal_cnt % 512)  signalslist_num_pages++;

  prevPageButton->setEnabled(false);
  nextPageButton->setEnabled(false);

  if(signal_cnt <= 512)
  {
    prevPageButton->setVisible(false);
    nextPageButton->setVisible(false);
  }
  else
  {
    prevPageButton->setVisible(true);
    nextPageButton->setVisible(true);
  }

  if(signalslist_page < signalslist_num_pages - 1)  nextPageButton->setEnabled(true);

  QApplication::setOverrideCursor(Qt::WaitCursor);


  if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
  {
    stackwidget->setCurrentIndex(0);

    line_edit1->setText(mainwindow->edfheaderlist[row]->plus_patientcode);
    if(mainwindow->edf_debug)  line_edit1->setToolTip("0x08");
    line_edit2->setText(mainwindow->edfheaderlist[row]->plus_sex);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x08 + mainwindow->edfheaderlist[row]->plus_sex_offset);
      line_edit2->setToolTip(str1_512);
    }
    line_edit6->setText(mainwindow->edfheaderlist[row]->plus_birthdate);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x08 + mainwindow->edfheaderlist[row]->plus_birthdate_offset);
      line_edit6->setToolTip(str1_512);
    }
    line_edit7->setText(mainwindow->edfheaderlist[row]->plus_patient_name);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x08 + mainwindow->edfheaderlist[row]->plus_patient_name_offset);
      line_edit7->setToolTip(str1_512);
    }
    line_edit8->setText(mainwindow->edfheaderlist[row]->plus_patient_additional);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x08 + mainwindow->edfheaderlist[row]->plus_patient_additional_offset);
      line_edit8->setToolTip(str1_512);
    }
    line_edit9->setText(mainwindow->edfheaderlist[row]->plus_startdate);
    if(mainwindow->edf_debug)  line_edit9->setToolTip("0x62");
    line_edit10->setText(mainwindow->edfheaderlist[row]->plus_admincode);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x58 + mainwindow->edfheaderlist[row]->plus_admincode_offset);
      line_edit10->setToolTip(str1_512);
    }
    line_edit11->setText(mainwindow->edfheaderlist[row]->plus_technician);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x58 + mainwindow->edfheaderlist[row]->plus_technician_offset);
      line_edit11->setToolTip(str1_512);
    }
    line_edit12->setText(mainwindow->edfheaderlist[row]->plus_equipment);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x58 + mainwindow->edfheaderlist[row]->plus_equipment_offset);
      line_edit12->setToolTip(str1_512);
    }
    line_edit13->setText(mainwindow->edfheaderlist[row]->plus_recording_additional);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 0x58 + mainwindow->edfheaderlist[row]->plus_recording_additional_offset);
      line_edit13->setToolTip(str1_512);
    }
  }
  else // old EDF
  {
    stackwidget->setCurrentIndex(1);

    line_edit1a->setText(mainwindow->edfheaderlist[row]->patient);
    if(mainwindow->edf_debug)  line_edit1a->setToolTip("0x08");
    line_edit2a->setText(mainwindow->edfheaderlist[row]->recording);
    if(mainwindow->edf_debug)  line_edit2a->setToolTip("0x58");
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str1_512, 400, "%i %s %i  %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  if(mainwindow->edfheaderlist[row]->starttime_subsec != 0LL)
  {
#ifdef Q_OS_WIN32
    __mingw_snprintf(str1_512 + strlen(str1_512), 512 - strlen(str1_512), ".%07lli", mainwindow->edfheaderlist[row]->starttime_subsec);
#else
    snprintf(str1_512 + strlen(str1_512), 512 - strlen(str1_512), ".%07lli", mainwindow->edfheaderlist[row]->starttime_subsec);
#endif

    remove_trailing_zeros(str1_512);
  }

  line_edit3->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit3->setToolTip("0xa8");
  line_edit3a->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit3a->setToolTip("0xa8");

  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;

  snprintf(str1_512, 512,
          "%2i:%02i:%02i.%07i",
          (int)((file_duration / TIME_FIXP_SCALING) / 3600LL),
          (int)(((file_duration / TIME_FIXP_SCALING) % 3600LL) / 60LL),
          (int)((file_duration / TIME_FIXP_SCALING) % 60LL),
          (int)(file_duration % TIME_FIXP_SCALING));

  remove_trailing_zeros(str1_512);

  line_edit4->setText(str1_512);
  line_edit4a->setText(str1_512);

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime +
   ((file_duration + mainwindow->edfheaderlist[row]->starttime_subsec) / TIME_FIXP_SCALING), &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str1_512, 400, "%i %s %i  %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);
#ifdef Q_OS_WIN32
  __mingw_snprintf(str1_512 + strlen(str1_512), 512 - strlen(str1_512), ".%07lli", (mainwindow->edfheaderlist[row]->starttime_subsec + file_duration) % TIME_FIXP_SCALING);
#else
  snprintf(str1_512 + strlen(str1_512), 512 - strlen(str1_512), ".%07lli", (mainwindow->edfheaderlist[row]->starttime_subsec + file_duration) % TIME_FIXP_SCALING);
#endif
  remove_trailing_zeros(str1_512);

  line_edit23->setText(str1_512);
  line_edit23a->setText(str1_512);

  line_edit5->setText(mainwindow->edfheaderlist[row]->reserved);
  if(mainwindow->edf_debug)  line_edit5->setToolTip("0xc0");
  line_edit5a->setText(mainwindow->edfheaderlist[row]->reserved);
  if(mainwindow->edf_debug)  line_edit5a->setToolTip("0xc0");

  snprintf(str1_512,  512, "%.12f", mainwindow->edfheaderlist[row]->data_record_duration);

  remove_trailing_zeros(str1_512);

  line_edit20->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit20->setToolTip("0xf4");
  line_edit20a->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit20a->setToolTip("0xf4");

#ifdef Q_OS_WIN32
  __mingw_snprintf(str1_512,  512, "%lli", mainwindow->edfheaderlist[row]->datarecords);
#else
  snprintf(str1_512,  512, "%lli", mainwindow->edfheaderlist[row]->datarecords);
#endif

  line_edit22->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit22->setToolTip("0xec");
  line_edit22a->setText(str1_512);
  if(mainwindow->edf_debug)  line_edit22a->setToolTip("0xec");

  line_edit21->setText(mainwindow->edfheaderlist[row]->version);
  if(mainwindow->edf_debug)  line_edit21->setToolTip("0x00");
  line_edit21a->setText(mainwindow->edfheaderlist[row]->version);
  if(mainwindow->edf_debug)  line_edit21a->setToolTip("0x00");

  load_signal_params(row, signalslist_page);

  QApplication::restoreOverrideCursor();
}


void UI_EDFhdrwindow::load_signal_params(int row, int page)
{
  int i,
      sig_idx=1,
      signal_cnt=0,
      signal_start=0,
      signal_end=0;

  char str1_512[512]={""};

  QLineEdit *ql=NULL;

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  signalslist_num_pages = signal_cnt / 512;
  if(signal_cnt % 512)  signalslist_num_pages++;

  signal_start = page * 512;
  if(signal_start >= signal_cnt)  return;
  signal_end = signal_start + 512;
  if(signal_end > signal_cnt)  signal_end = signal_cnt;

  sig_idx = signal_start + 1;

  for(i=0; i<mainwindow->edfheaderlist[row]->nr_annot_chns; i++)
  {
    if(mainwindow->edfheaderlist[row]->annot_ch[i] < signal_start)
    {
      sig_idx--;
    }
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  signallist->clear();
  signallist->setColumnCount(13);
  signallist->setRowCount(signal_end - signal_start);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->verticalHeader()->setVisible(false);
  signallist->scrollToTop();
  QStringList horizontallabels;
  horizontallabels += "Index";
  horizontallabels += "Label";
  horizontallabels += "Samplefrequency";
  horizontallabels += "Physical maximum";
  horizontallabels += "Physical minimum";
  horizontallabels += "Digital maximum";
  horizontallabels += "Digital minimum";
  horizontallabels += "Resolution";
  horizontallabels += "Physical dimension";
  horizontallabels += "Samples per record";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  horizontallabels += "Reserved";
  signallist->setHorizontalHeaderLabels(horizontallabels);

  for(i=signal_start; i<signal_end; i++)
  {
    if(!(i % 100))
    {
      qApp->processEvents();
    }

    if(mainwindow->edfheaderlist[row]->edfparam[i].annotation)
    {
      ql = new QLineEdit(" *");
    }
    else
    {
      snprintf(str1_512, 512, "%i", sig_idx++);
      ql = new QLineEdit(str1_512);
    }
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i%512, 0, ql);

    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].label);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (i * 16));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 1, ql);
    convert_to_metric_suffix(str1_512, mainwindow->edfheaderlist[row]->edfparam[i].sf_f, 3, 512);
    strlcat(str1_512, "Hz", 512);
    remove_trailing_zeros(str1_512);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i%512, 2, ql);
    snprintf(str1_512, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_max);
    remove_trailing_zeros(str1_512);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 112) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 3, ql);
    snprintf(str1_512, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_min);
    remove_trailing_zeros(str1_512);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 104) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 4, ql);
    snprintf(str1_512, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_max);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 128) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 5, ql);
    snprintf(str1_512, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_min);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 120) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 6, ql);
    if(mainwindow->edfheaderlist[row]->edfparam[i].annotation)
    {
      ql = new QLineEdit("n.a.");
    }
    else
    {
      snprintf(str1_512, 512, "%e", mainwindow->edfheaderlist[row]->edfparam[i].bitvalue);
      remove_trailing_zeros(str1_512);
      ql = new QLineEdit(str1_512);
      ql->setToolTip("Computed as: (physical maximum - physical minimum) / (digital maximum - digital minimum)");
    }
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i%512, 7, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].physdimension);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 96) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 8, ql);
    snprintf(str1_512, 512, "%i", mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record);
    ql = new QLineEdit(str1_512);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 216) + (i * 8));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 9, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].prefilter);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 136) + (i * 80));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 10, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].transducer);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 16) + (i * 80));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 11, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].reserved);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    if(mainwindow->edf_debug)
    {
      snprintf(str1_512, 512, "0x%02x", 256 + (signal_cnt * 224) + (i * 32));
      ql->setToolTip(str1_512);
    }
    signallist->setCellWidget(i%512, 12, ql);
  }

  signallist->resizeColumnsToContents();

  signallist->setColumnWidth(0, 40 * mainwindow->w_scaling);

  QApplication::restoreOverrideCursor();
}


void UI_EDFhdrwindow::prevPageButtonClicked()
{
  if(signalslist_page <= 0)  return;

  signalslist_page--;

  if(signalslist_page == 0)  prevPageButton->setEnabled(false);

  if(signalslist_page < signalslist_num_pages - 1)  nextPageButton->setEnabled(true);

  load_signal_params(file_row_selected, signalslist_page);
}


void UI_EDFhdrwindow::nextPageButtonClicked()
{
  if(signalslist_page >= signalslist_num_pages - 1)  return;

  signalslist_page++;

  if(signalslist_page == signalslist_num_pages - 1)  nextPageButton->setEnabled(false);

  if(signalslist_page)  prevPageButton->setEnabled(true);

  load_signal_params(file_row_selected, signalslist_page);
}













