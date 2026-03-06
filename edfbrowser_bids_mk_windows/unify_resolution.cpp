/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2021 - 2024 Teunis van Beelen
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



#include "unify_resolution.h"


UI_unify_resolution::UI_unify_resolution(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(800 * mainwindow->w_scaling, 600 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Unify resolution");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  file_path_label = new QLabel;

  signals_list = new QTableWidget;
  signals_list->setSelectionMode(QAbstractItemView::NoSelection);
  signals_list->setColumnCount(6);
  signals_list->setColumnWidth(0, 120 * mainwindow->w_scaling);
  signals_list->setColumnWidth(1, 120 * mainwindow->w_scaling);
  signals_list->setColumnWidth(2, 120 * mainwindow->w_scaling);
  signals_list->setColumnWidth(3, 120 * mainwindow->w_scaling);
  signals_list->setColumnWidth(4, 120 * mainwindow->w_scaling);
  signals_list->setColumnWidth(5, 120 * mainwindow->w_scaling);

  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Resolution";
  horizontallabels += "Physical Maximum";
  horizontallabels += "Physical Minimum";
  horizontallabels += "Digital Maximum";
  horizontallabels += "Digital Minimum";
  signals_list->setHorizontalHeaderLabels(horizontallabels);

  QLabel *phys_max_label = new QLabel;
  phys_max_label->setText("New physical maximum:");

  phys_max_spinbox = new QDoubleSpinBox;
  phys_max_spinbox->setDecimals(4);
  phys_max_spinbox->setRange(-9999999, 99999999);
  phys_max_spinbox->setValue(3000);

  select_file_button = new QPushButton;
  select_file_button->setText("Select file");
  if(mainwindow->files_open < 2)
  {
    select_file_button->setEnabled(false);
  }

  select_button = new QPushButton;
  select_button->setText("Select all signals");
  select_button->setEnabled(false);

  deselect_button = new QPushButton;
  deselect_button->setText("Deselect all signals");
  deselect_button->setEnabled(false);

  SaveButton = new QPushButton;
  SaveButton->setText("Save");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addStretch(1000);
  hlayout6->addWidget(helpButton);
  hlayout6->addStretch(1000);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(phys_max_label);
  hlayout5->addStretch(1000);
  hlayout5->addWidget(phys_max_spinbox);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(select_button);
  hlayout4->addStretch(1000);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(deselect_button);
  hlayout3->addStretch(1000);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addStretch(1000);
  vlayout2->addLayout(hlayout6);
  vlayout2->addStretch(1000);
  vlayout2->addLayout(hlayout5);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout4);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(hlayout3);
  vlayout2->addSpacing(20);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(signals_list, 1000);
  hlayout1->addLayout(vlayout2);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addSpacing(20);
  hlayout2->addWidget(select_file_button);
  hlayout2->addStretch(1000);
  hlayout2->addWidget(SaveButton);
  hlayout2->addStretch(1000);
  hlayout2->addWidget(CloseButton);
  hlayout2->addSpacing(20);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(file_path_label);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout1, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout2);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(select_button,      SIGNAL(clicked()),     this,           SLOT(select_button_clicked()));
  QObject::connect(deselect_button,    SIGNAL(clicked()),     this,           SLOT(deselect_button_clicked()));
  QObject::connect(select_file_button, SIGNAL(clicked()),     this,           SLOT(select_file_button_clicked()));
  QObject::connect(CloseButton,        SIGNAL(clicked()),     myobjectDialog, SLOT(close()));
  QObject::connect(SaveButton,         SIGNAL(clicked()),     this,           SLOT(SaveButtonClicked()));
  QObject::connect(helpButton,         SIGNAL(clicked()),     this,           SLOT(helpbuttonpressed()));

  file_num = -1;

  inputpath[0] = 0;

  outputpath[0] = 0;

  edfhdr = NULL;

  if(mainwindow->files_open == 1)
  {
    select_file_button_clicked();
  }

  myobjectDialog->exec();
}


void UI_unify_resolution::SaveButtonClicked()
{
  int i, j, len=0,
      tmp,
      num_checked=0,
      is_checked[MAXSIGNALS];

  short *ptr16=NULL;

  long long datrec;

  char str_128[128]={""},
       phys_dim_16[16]={""},
       str_phys_max_32[32]={""},
       str_phys_min_32[32]={""},
       *buf_in=NULL,
       *ptr8=NULL;

  double cnv_factor[MAXSIGNALS],
         new_bitval=1,
         d_offset;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FILE *outputfile=NULL;

  QProgressDialog progress("Processing file...", "Abort", 0, 1);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  if((phys_max_spinbox->value() < 1.0) && (phys_max_spinbox->value() > -1.0))
  {
    QMessageBox::critical(myobjectDialog, "Error", "Please set the new physical maximum value higher than 1.0 or lower than -1.0.", QMessageBox::Close);
    return;
  }

  if(edfhdr->edf)
  {
    new_bitval = (phys_max_spinbox->value() * 2) / 0x10000;
  }
  else
  {
    new_bitval = (phys_max_spinbox->value() * 2) / 0x1000000;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(((QCheckBox *)(signals_list->cellWidget(i, 0)))->checkState() == Qt::Checked)
    {
      if(!num_checked)
      {
        strlcpy(phys_dim_16, edfhdr->edfparam[i].physdimension, 16);
        trim_spaces(phys_dim_16);
      }
      else
      {
        strlcpy(str_128, edfhdr->edfparam[i].physdimension, 16);
        trim_spaces(str_128);
        if(strcmp(phys_dim_16, str_128))
        {
          QMessageBox::critical(myobjectDialog, "Error", "Selected signals must have equal physical dimension (unit).");
          return;
        }
      }

      cnv_factor[i] = edfhdr->edfparam[i].bitvalue / new_bitval;

      if(cnv_factor[i] < 0)
      {
        cnv_factor[i] *= -1;
      }

      num_checked++;

      is_checked[i] = 1;
    }
    else
    {
      is_checked[i] = 0;
    }
  }

  if(num_checked < 1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "At least one signal needs to be selected.");
    return;
  }

  snprintf(str_phys_max_32, 32, "%.8f", phys_max_spinbox->value());
  str_phys_max_32[8] = 0;
  if(str_phys_max_32[7] == '.')
  {
    str_phys_max_32[7] = ' ';
  }

  snprintf(str_phys_min_32, 32, "%.8f", -phys_max_spinbox->value());
  str_phys_min_32[8] = 0;
  if(str_phys_min_32[7] == '.')
  {
    str_phys_min_32[7] = ' ';
  }

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
    strlcat(outputpath, "_unified.edf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }
  else
  {
    strlcat(outputpath, "_unified.bdf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "BDF files (*.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(outputpath, ""))
  {
    return;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    QMessageBox::critical(myobjectDialog, "Error", "Selected file is in use.");
    return;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Cannot open outputfile for writing.");
    return;
  }

  buf_in = (char *)malloc(edfhdr->hdrsize);
  if(buf_in == NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Malloc error.");
    goto OUT_ERROR;
  }

  fseek(edfhdr->file_hdl, 0, SEEK_SET);

  if(fread(buf_in, edfhdr->hdrsize, 1, edfhdr->file_hdl) != 1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Cannot read from inputfile.");
    goto OUT_ERROR;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(is_checked[i])
    {
      memcpy(buf_in + 256 + (104 * edfhdr->edfsignals) + (8 * i), str_phys_min_32, 8);
      memcpy(buf_in + 256 + (112 * edfhdr->edfsignals) + (8 * i), str_phys_max_32, 8);
      if(edfhdr->edf)
      {
        memcpy(buf_in + 256 + (120 * edfhdr->edfsignals) + (8 * i), "-32768  ", 8);
        memcpy(buf_in + 256 + (128 * edfhdr->edfsignals) + (8 * i), "32767   ", 8);
      }
      else
      {
        memcpy(buf_in + 256 + (120 * edfhdr->edfsignals) + (8 * i), "-8388608", 8);
        memcpy(buf_in + 256 + (128 * edfhdr->edfsignals) + (8 * i), "8388607 ", 8);
      }
    }
  }

  if(fwrite(buf_in, edfhdr->hdrsize, 1, outputfile) != 1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Cannot write to outputfile.");
    goto OUT_ERROR;
  }

  free(buf_in);
  buf_in = (char *)malloc(edfhdr->recordsize);
  if(buf_in == NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Malloc error.");
    goto OUT_ERROR;
  }

  progress.setRange(0, edfhdr->datarecords / 100);
  progress.setValue(0);

  for(datrec=0; datrec<edfhdr->datarecords; datrec++)
  {
    if(!(datrec % 100))
    {
      progress.setValue(datrec / 100);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto OUT_ERROR;
      }
    }

    if(fread(buf_in, edfhdr->recordsize, 1, edfhdr->file_hdl) != 1)
    {
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", "Cannot read from inputfile.");
      goto OUT_ERROR;
    }

    if(edfhdr->edf)
    {
      for(i=0; i<edfhdr->edfsignals; i++)
      {
        if(!is_checked[i])  continue;

        d_offset = edfhdr->edfparam[i].offset;

        ptr16 = (short *)(buf_in + edfhdr->edfparam[i].datrec_offset);

        for(j=0; j<edfhdr->edfparam[i].smp_per_record; j++)
        {
          tmp = ((*ptr16 + d_offset) * cnv_factor[i]) + 0.5;

          if(tmp > 32767)
          {
            tmp = 32767;
          }

          if(tmp < -32768)
          {
            tmp = -32768;
          }

          *ptr16 = tmp;

          ptr16++;
        }
      }
    }
    else
    {
      for(i=0; i<edfhdr->edfsignals; i++)
      {
        if(!is_checked[i])  continue;

        d_offset = edfhdr->edfparam[i].offset;

        ptr8 = buf_in + edfhdr->edfparam[i].datrec_offset;

        for(j=0; j<edfhdr->edfparam[i].smp_per_record; j++)
        {
          var.two[0] = *((unsigned short *)ptr8);

          var.four[2] = *((unsigned char *)(ptr8 + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          var.one_signed = ((var.one_signed + d_offset) * cnv_factor[i]) + 0.5;

          if(var.one_signed > 8388607)
          {
            var.one_signed = 8388607;
          }

          if(var.one_signed < -8388608)
          {
            var.one_signed = -8388608;
          }

          *((unsigned short *)ptr8) = var.two[0];

          *((unsigned char *)(ptr8 + 2)) = var.four[2];

          ptr8 += 3;
        }
      }
    }

    if(fwrite(buf_in, edfhdr->recordsize, 1, outputfile) != 1)
    {
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", "Cannot write to outputfile.");
      goto OUT_ERROR;
    }
  }

  progress.reset();
  QMessageBox::information(myobjectDialog, "Ready", "Done.");

OUT_ERROR:

  if(outputfile != NULL)
  {
    fclose(outputfile);
  }

  free(buf_in);
}


void UI_unify_resolution::select_button_clicked()
{
  int i, num_rows;

  num_rows = signals_list->rowCount();

  for(i=0; i<num_rows; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }
  }
}


void UI_unify_resolution::deselect_button_clicked()
{
  int i, num_rows;

  num_rows = signals_list->rowCount();

  for(i=0; i<num_rows; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
  }
}


void UI_unify_resolution::select_file_button_clicked()
{
  int i;

  file_num = -1;

  inputpath[0] = 0;

  file_path_label->clear();

  signals_list->setRowCount(0);

  edfhdr = NULL;

  select_button->setEnabled(false);

  deselect_button->setEnabled(false);

  if(mainwindow->files_open < 1)  return;

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

  file_path_label->setText(inputpath);

  signals_list->setRowCount(edfhdr->edfsignals);

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    signals_list->setCellWidget(i, 0, new QCheckBox(edfhdr->edfparam[i].label));
    ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setTristate(false);
    if(edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
      ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setEnabled(false);
    }
    else
    {
      ((QCheckBox *)(signals_list->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }

    signals_list->setCellWidget(i, 1, new QDoubleSpinBox);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 1)))->setRange(-1e9, 1e9);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 1)))->setSuffix(edfhdr->edfparam[i].physdimension);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 1)))->setDecimals(8);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 1)))->setEnabled(false);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 1)))->setValue(edfhdr->edfparam[i].bitvalue);

    signals_list->setCellWidget(i, 2, new QDoubleSpinBox);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 2)))->setRange(-1e9, 1e9);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 2)))->setDecimals(6);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 2)))->setEnabled(false);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 2)))->setValue(edfhdr->edfparam[i].phys_max);

    signals_list->setCellWidget(i, 3, new QDoubleSpinBox);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 3)))->setRange(-1e9, 1e9);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 3)))->setDecimals(6);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 3)))->setEnabled(false);
    ((QDoubleSpinBox *)(signals_list->cellWidget(i, 3)))->setValue(edfhdr->edfparam[i].phys_min);

    signals_list->setCellWidget(i, 4, new QSpinBox);
    ((QSpinBox *)(signals_list->cellWidget(i, 4)))->setRange(-9999999, 99999999);
    ((QSpinBox *)(signals_list->cellWidget(i, 4)))->setEnabled(false);
    ((QSpinBox *)(signals_list->cellWidget(i, 4)))->setValue(edfhdr->edfparam[i].dig_max);

    signals_list->setCellWidget(i, 5, new QSpinBox);
    ((QSpinBox *)(signals_list->cellWidget(i, 5)))->setRange(-9999999, 99999999);
    ((QSpinBox *)(signals_list->cellWidget(i, 5)))->setEnabled(false);
    ((QSpinBox *)(signals_list->cellWidget(i, 5)))->setValue(edfhdr->edfparam[i].dig_min);
  }

  select_button->setEnabled(true);

  deselect_button->setEnabled(true);
}


void UI_unify_resolution::helpbuttonpressed()
{
  mainwindow->open_manual("#Unify_resolution");
}













