/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2024 Teunis van Beelen
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



#include "ecg_export.h"



UI_ECGExport::UI_ECGExport(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  myobjectDialog = new QDialog(w_parent);

  myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 445 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Export RR-intervals");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  list = new QListWidget;
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::SingleSelection);

  groupBox1 = new QGroupBox;
  groupBox1->setTitle("Output format");

  radioButton1 = new QRadioButton("RR interval");
  radioButton2 = new QRadioButton("R Onset + RR interval");
  radioButton3 = new QRadioButton("R Onset");
  radioButton2->setChecked(true);

  checkBox1 = new QCheckBox("Whole recording", myobjectDialog);
  checkBox1->setTristate(false);

  checkBox2 = new QCheckBox("Don't write to file,\n"
                            "import as annotations instead",
                            myobjectDialog);
  checkBox2->setTristate(false);

  vbox1 = new QVBoxLayout;
  vbox1->addWidget(radioButton1);
  vbox1->addWidget(radioButton2);
  vbox1->addWidget(radioButton3);

  groupBox1->setLayout(vbox1);

  startButton = new QPushButton;
  startButton->setText("Import/Export");

  cancelButton = new QPushButton;
  cancelButton->setText("Cancel");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  load_signalcomps();

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addStretch(1000);
  hlayout3->addWidget(helpButton);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(groupBox1);
  vlayout2->addSpacing(20);
  vlayout2->addWidget(checkBox1);
  vlayout2->addWidget(checkBox2);
  vlayout2->addStretch(1000);
  vlayout2->addLayout(hlayout3);
  vlayout2->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(list, 1000);
  hlayout2->addLayout(vlayout2);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(startButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(cancelButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(cancelButton, SIGNAL(clicked()), myobjectDialog, SLOT(close()));
  QObject::connect(startButton,  SIGNAL(clicked()), this,           SLOT(Export_RR_intervals()));
  QObject::connect(helpButton,   SIGNAL(clicked()), this,           SLOT(helpbuttonpressed()));

  myobjectDialog->exec();
}



void UI_ECGExport::Export_RR_intervals()
{
  int i,
      len,
      signal_nr,
      type=-1,
      beat_cnt,
      samples_cnt,
      progress_steps,
      datarecords,
      whole_recording=0,
      import_as_annots=0,
      filenum=0;

  char path[MAX_PATH_LENGTH]={""},
       str1_2048[2048]={""};

  double *beat_interval_list=NULL,
         *buf=NULL;

  long long *beat_onset_list=NULL,
            datrecs,
            smpls_left,
            l_time=0LL;

  sigcompblck_t *signalcomp=NULL;


  FILE *outputfile=NULL;

  QList<QListWidgetItem *> selectedlist;

  annotblck_t annotation;

  flt_blck_rd_t *blockrd=NULL;


  selectedlist = list->selectedItems();

  if(selectedlist.size() < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Select a signal first.");
    messagewindow.exec();
    return;
  }

  signal_nr = selectedlist.at(0)->data(Qt::UserRole).toInt();

  if((signal_nr < 0) || (signal_nr >= mainwindow->signalcomps))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Invalid signalcomp number");
    messagewindow.exec();
    return;
  }

  signalcomp = mainwindow->signalcomp[signal_nr];

  if(signalcomp->ecg_filter == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Heart Rate detection is not activated for the selected signal.");
    messagewindow.exec();
    return;
  }

  if(checkBox2->checkState() == Qt::Checked)
  {
    import_as_annots = 1;
  }

  if(checkBox1->checkState() == Qt::Checked)
  {
    whole_recording = 1;

    blockrd = create_flt_blck_rd(signalcomp, 1, 0, 0, NULL, &buf);
    if(blockrd == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "create_flt_blck_rd() returned an error.");
      messagewindow.exec();
      return;
    }

    samples_cnt = get_samples_flt_blck_rd(blockrd);
    if(samples_cnt < 1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, samples_cnt is < 1.");
      messagewindow.exec();
      return;
    }

    filenum = mainwindow->get_filenum(signalcomp->edfhdr);
    if(filenum < 0)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: filenum < 0");
      messagewindow.exec();
      return;
    }

    reset_ecg_filter(signalcomp->ecg_filter);

    datarecords = signalcomp->edfhdr->datarecords;

    QProgressDialog progress("Processing file...", "Abort", 0, datarecords);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(200);

    progress_steps = datarecords / 100;
    if(progress_steps < 1)
    {
      progress_steps = 1;
    }

    for(i=0; i<signalcomp->edfhdr->datarecords; i++)
    {
      if(!(i%progress_steps))
      {
        progress.setValue(i);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          free_flt_blck_rd(blockrd);
          free(buf);
          return;
        }
      }

      if(run_flt_blck_rd(blockrd, i))
      {
        progress.reset();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error while reading file.");
        messagewindow.exec();
        free_flt_blck_rd(blockrd);
        free(buf);
        return;
      }
    }

    free_flt_blck_rd(blockrd);
    blockrd = NULL;
    free(buf);
    buf = NULL;

    progress.reset();
  }

  beat_cnt = ecg_filter_get_beat_cnt(signalcomp->ecg_filter);

  beat_onset_list = ecg_filter_get_onset_beatlist(signalcomp->ecg_filter);

  beat_interval_list = ecg_filter_get_interval_beatlist(signalcomp->ecg_filter);

  if(beat_cnt < 4)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, not enough beats.");
    messagewindow.exec();
    free_flt_blck_rd(blockrd);
    free(buf);
    return;
  }

  if(import_as_annots)
  {
    if(mainwindow->annotationlist_backup==NULL)
    {
      mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[filenum]->annot_list);
    }

    memset(&annotation, 0, sizeof(annotblck_t));
    strncpy(annotation.description, mainwindow->ecg_qrs_rpeak_descr, MAX_ANNOTATION_LEN);
    annotation.description[MAX_ANNOTATION_LEN] = 0;
    annotation.edfhdr = mainwindow->edfheaderlist[filenum];

    for(i=0; i<beat_cnt; i++)
    {
      if(whole_recording)
      {
        l_time = 0LL;
      }
      else
      {
        l_time = signalcomp->edfhdr->viewtime;
      }

      if(l_time < 0LL)
      {
        l_time = 0LL;
      }

      datrecs = beat_onset_list[i] / signalcomp->edfparam_0->smp_per_record;

      smpls_left = beat_onset_list[i] % signalcomp->edfparam_0->smp_per_record;

      l_time += (datrecs * signalcomp->edfhdr->long_data_record_duration);

      l_time += ((smpls_left * signalcomp->edfhdr->long_data_record_duration) / signalcomp->edfparam_0->smp_per_record);

      if(!whole_recording)
      {
        l_time += (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - signalcomp->edfhdr->viewtime);
      }

      annotation.onset = l_time;
      edfplus_annotation_add_item(&mainwindow->edfheaderlist[filenum]->annot_list, annotation);
    }

    if(mainwindow->annotations_dock[filenum] == NULL)
    {
      mainwindow->annotations_dock[filenum] = new UI_Annotationswindow(signalcomp->edfhdr, mainwindow);

      mainwindow->addDockWidget(Qt::RightDockWidgetArea, mainwindow->annotations_dock[filenum]->docklist, Qt::Vertical);

      if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) < 1)
      {
        mainwindow->annotations_dock[filenum]->docklist->hide();
      }
    }

    if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) > 0)
    {
      mainwindow->annotations_dock[filenum]->docklist->show();

      mainwindow->annotations_edited = 1;

      mainwindow->annotations_dock[filenum]->updateList(0);

      mainwindow->save_act->setEnabled(true);
    }
  }
  else
  {
    path[0] = 0;
    if(mainwindow->recent_savedir[0]!=0)
    {
      strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
      strlcat(path, "/", MAX_PATH_LENGTH);
    }
    len = strlen(path);
    get_filename_from_path(path + len, signalcomp->edfhdr->filename, MAX_PATH_LENGTH - len);
    remove_extension_from_filename(path);
    strlcat(path, "_RR_interval.txt", MAX_PATH_LENGTH);

    strlcpy(path, QFileDialog::getSaveFileName(0, "Export RR-interval to ASCII", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT)").toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(path, ""))
    {
      free_flt_blck_rd(blockrd);
      free(buf);
      return;
    }

    get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

    outputfile = fopeno(path, "wb");
    if(outputfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, cannot open outputfile for writing.");
      messagewindow.exec();
      free_flt_blck_rd(blockrd);
      free(buf);
      return;
    }

    if(radioButton1->isChecked() == true)
    {
      type = 1;
    }

    if(radioButton2->isChecked() == true)
    {
      type = 2;
    }

    if(radioButton3->isChecked() == true)
    {
      type = 3;
    }

    if(type == 1)
    {
      for(i=0; i<beat_cnt; i++)
      {
        fprintf(outputfile, "%.4f\n", beat_interval_list[i]);
      }
    }

    if((type == 2) || (type == 3))
    {
      for(i=0; i<beat_cnt; i++)
      {
        if(whole_recording)
        {
          l_time = 0LL;
        }
        else
        {
          l_time = signalcomp->edfhdr->viewtime;
        }

        if(l_time < 0LL)
        {
          l_time = 0LL;
        }

        datrecs = beat_onset_list[i] / signalcomp->edfparam_0->smp_per_record;

        smpls_left = beat_onset_list[i] % signalcomp->edfparam_0->smp_per_record;

        l_time += (datrecs * signalcomp->edfhdr->long_data_record_duration);

        l_time += ((smpls_left * signalcomp->edfhdr->long_data_record_duration) / signalcomp->edfparam_0->smp_per_record);

        if(!whole_recording)
        {
          l_time += (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - signalcomp->edfhdr->viewtime);
        }

        if(type == 2)
        {
          fprintf(outputfile, "%.4f\t%.4f\n", ((double)l_time) / TIME_FIXP_SCALING, beat_interval_list[i]);
        }

        if(type == 3)
        {
          fprintf(outputfile, "%.4f\n", ((double)l_time) / TIME_FIXP_SCALING);
        }
      }
    }

    fclose(outputfile);
  }

  myobjectDialog->close();

  if(!import_as_annots)
  {
    snprintf(str1_2048, 2048, "Done. The %s's and/or RR-intervals are exported to:\n\n%s", mainwindow->ecg_qrs_rpeak_descr, path);
    QMessageBox messagewindow(QMessageBox::Information, "Ready", QString::fromLocal8Bit(str1_2048));
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();
  }

  reset_ecg_filter(signalcomp->ecg_filter);

  mainwindow->setup_viewbuf();
}


void UI_ECGExport::load_signalcomps(void)
{
  int i;

  QListWidgetItem *item;

  list->clear();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    item->setText(mainwindow->signalcomp[i]->signallabel);
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }
}


void UI_ECGExport::helpbuttonpressed()
{
  mainwindow->open_manual("#ECG_detection");
}
























