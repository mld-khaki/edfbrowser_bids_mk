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



#include "bdf2edf.h"



UI_BDF2EDFwindow::UI_BDF2EDFwindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = mainwindow->recent_opendir;
  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;

  use_hpf = 1;

  myobjectDialog->setMinimumSize(550 * mainwindow->w_scaling, 450 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("BDF+ to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  label1 = new QLabel;

  SignalsTablewidget = new QTableWidget;
  SignalsTablewidget->setSelectionMode(QAbstractItemView::NoSelection);
  SignalsTablewidget->setColumnCount(3);

  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "HighPassFilter";
  horizontallabels += "Divider";
  SignalsTablewidget->setHorizontalHeaderLabels(horizontallabels);
  SignalsTablewidget->resizeColumnsToContents();

  spinBox1 = new QDoubleSpinBox;
  spinBox1->setDecimals(3);
  spinBox1->setSuffix(" Hz");
  spinBox1->setRange(0.001, 100.0);
  spinBox1->setValue(0.1);

  spinBox2 = new QDoubleSpinBox;
  spinBox2->setDecimals(3);
  spinBox2->setRange(1.0, 256.0);
  spinBox2->setValue(1.0);
  spinBox2->setToolTip("Increasing the divider lowers the amplitude resolution but increases the amplitude range (physical maximum and minimum)");

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  pushButton3 = new QPushButton;
  pushButton3->setText("Convert");
  pushButton3->setEnabled(false);

  pushButton4 = new QPushButton;
  pushButton4->setText("Select all signals");
  pushButton4->setEnabled(false);

  pushButton5 = new QPushButton;
  pushButton5->setText("Deselect all signals");
  pushButton5->setEnabled(false);

  HPFcheckBox = new QCheckBox;
  HPFcheckBox->setTristate(false);
  HPFcheckBox->setChecked(true);
  HPFcheckBox->setToolTip("The highpass filter is usually necessary in order to remove the DC-offset");

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Enable HPF:", HPFcheckBox);
  flayout->labelForField(HPFcheckBox)->setToolTip("The highpass filter is usually necessary in order to remove the DC-offset");
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Highpass filter:", spinBox1);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Divider:", spinBox2);
  flayout->labelForField(spinBox2)->setToolTip("Increasing the divider lowers the amplitude resolution but increases the amplitude range (physical maximum and minimum)");

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(pushButton4);
  hlayout3->addStretch(1000);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(pushButton5);
  hlayout4->addStretch(1000);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addLayout(hlayout3);
  vlayout2->addLayout(hlayout4);
  vlayout2->addStretch(400);
  vlayout2->addLayout(flayout);
  vlayout2->addStretch(1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(SignalsTablewidget, 1000);
  hlayout1->addLayout(vlayout2);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(pushButton1);
  hlayout2->addStretch(400);
  hlayout2->addWidget(pushButton3);
  hlayout2->addStretch(1000);
  hlayout2->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addSpacing(10);
  vlayout1->addWidget(label1);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);
  vlayout1->addSpacing(30);
  vlayout1->addLayout(hlayout2);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1,    SIGNAL(clicked()),            this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,    SIGNAL(clicked()),            myobjectDialog, SLOT(close()));
  QObject::connect(pushButton3,    SIGNAL(clicked()),            this,           SLOT(StartConversion()));
  QObject::connect(pushButton4,    SIGNAL(clicked()),            this,           SLOT(Select_all_signals()));
  QObject::connect(pushButton5,    SIGNAL(clicked()),            this,           SLOT(Deselect_all_signals()));
  QObject::connect(spinBox1,       SIGNAL(valueChanged(double)), this,           SLOT(spinbox1_changed(double)));
  QObject::connect(spinBox2,       SIGNAL(valueChanged(double)), this,           SLOT(spinbox2_changed(double)));
  QObject::connect(HPFcheckBox,    SIGNAL(stateChanged(int)),    this,           SLOT(hpf_checkbox_changed(int)));
  QObject::connect(myobjectDialog, SIGNAL(destroyed()),          this,           SLOT(free_edfheader()));

  edfhdr = NULL;
  inputfile = NULL;
  outputfile = NULL;

  inputpath[0] = 0;

  myobjectDialog->exec();
}


void UI_BDF2EDFwindow::free_edfheader()
{
  if(edfhdr != NULL)
  {
    free(edfhdr->edfparam);
    free(edfhdr);
    edfhdr = NULL;
  }
}


void UI_BDF2EDFwindow::hpf_checkbox_changed(int checked)
{
  int i;

  if(checked)
  {
    use_hpf = 1;

    spinBox1->setEnabled(true);

    if(edfhdr!=NULL)
    {
      for(i=0; i<edfhdr->edfsignals; i++)
      {
        if(!edfhdr->edfparam[i].annotation)
        {
          ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setEnabled(true);
        }
      }
    }
  }
  else
  {
    use_hpf = 0;

    spinBox1->setEnabled(false);

    if(edfhdr!=NULL)
    {
      for(i=0; i<edfhdr->edfsignals; i++)
      {
        if(!edfhdr->edfparam[i].annotation)
        {
          ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setEnabled(false);
        }
      }
    }
  }
}


void UI_BDF2EDFwindow::Select_all_signals()
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



void UI_BDF2EDFwindow::Deselect_all_signals()
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



void UI_BDF2EDFwindow::spinbox1_changed(double value)
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
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setValue(value);
    }
  }
}



void UI_BDF2EDFwindow::spinbox2_changed(double value)
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
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setValue(value);
    }
  }
}



void UI_BDF2EDFwindow::SelectFileButton()
{
  int i;

  char str1_2048[2048];

  if(edfhdr!=NULL)
  {
    label1->setText("");

    SignalsTablewidget->setRowCount(0);

    free_edfheader();

    inputfile = NULL;
    outputfile = NULL;

    inputpath[0] = 0;

    pushButton3->setEnabled(false);
    pushButton4->setEnabled(false);
    pushButton5->setEnabled(false);
  }

  strlcpy(inputpath, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "BDF files (*.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(inputpath, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, inputpath, MAX_PATH_LENGTH);

  inputfile = fopeno(inputpath, "rb");
  if(inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for reading.", inputpath);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str1_2048));
    messagewindow.exec();
    return;
  }

/***************** check if the file is valid ******************************/

  edfhdr = check_edf_file(inputfile, str1_2048, 2048, 0, 0);
  if(edfhdr==NULL)
  {
    fclose(inputfile);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
    messagewindow.exec();
    return;
  }

  if(!edfhdr->bdf)
  {
    fclose(inputfile);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not a valid BDF file.");
    messagewindow.exec();
    free_edfheader();
    return;
  }

/***************** load signalproperties ******************************/

  label1->setText(QString::fromLocal8Bit(inputpath));

  SignalsTablewidget->setRowCount(edfhdr->edfsignals);

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    SignalsTablewidget->setCellWidget(i, 0, new QCheckBox(edfhdr->edfparam[i].label));
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setTristate(false);
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Checked);

    if(!edfhdr->edfparam[i].annotation)
    {
      SignalsTablewidget->setCellWidget(i, 1, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setDecimals(3);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setSuffix(" Hz");
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setRange(0.001, 100.0);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setValue(spinBox1->value());
      if(!use_hpf)
      {
        ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setEnabled(false);
      }

      SignalsTablewidget->setCellWidget(i, 2, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setDecimals(3);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setRange(1.0, 256.0);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setValue(spinBox2->value());
    }
    else
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setEnabled(false);
    }
  }

  pushButton3->setEnabled(true);
  pushButton4->setEnabled(true);
  pushButton5->setEnabled(true);

  SignalsTablewidget->resizeColumnsToContents();
}



void UI_BDF2EDFwindow::StartConversion()
{
  int i, j, k,
      datrecs,
      new_edfsignals,
      datarecords,
      len,
      progress_steps;

  char *readbuf=NULL,
       scratchpad_256[256]={""};

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  union {
          signed short one_short;
          unsigned char two_bytes[2];
        } var2;



  pushButton3->setEnabled(false);
  pushButton4->setEnabled(false);
  pushButton5->setEnabled(false);

  if(edfhdr==NULL)
  {
    return;
  }

  if(edfhdr->edfsignals>MAXSIGNALS)
  {
    return;
  }

  new_edfsignals = 0;

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      if(((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->checkState()==Qt::Checked)
      {
        signalslist[new_edfsignals] = i;

        annotlist[new_edfsignals] = 0;

        filterlist[new_edfsignals] = create_filter(0, ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->value(), edfhdr->edfparam[i].sf_f);

        dividerlist[new_edfsignals] = ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->value();

        new_edfsignals++;
      }
    }
    else
    {
      signalslist[new_edfsignals] = i;

      annotlist[new_edfsignals] = 1;

      filterlist[new_edfsignals] = create_filter(0, 0.01, edfhdr->edfparam[i].sf_f);

      dividerlist[new_edfsignals] = 1.0;

      new_edfsignals++;
    }
  }

  datarecords = edfhdr->datarecords;

  QProgressDialog progress("Converting...", "Abort", 0, datarecords, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  if(!new_edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You must select at least one signal.");
    messagewindow.exec();
    goto END_1;
  }

  readbuf = (char *)malloc(edfhdr->recordsize);
  if(readbuf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error, (readbuf).");
    messagewindow.exec();
    goto END_2;
  }

/////////////////////////// write header ////////////////////////////////////////

  outputpath[0] = 0;

  if(recent_savedir[0]!=0)
  {
    strlcpy(outputpath, recent_savedir, MAX_PATH_LENGTH);
    strlcat(outputpath, "/", MAX_PATH_LENGTH);
  }
  len = strlen(outputpath);
  get_filename_from_path(outputpath + len, inputpath, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(outputpath);

  strlcat(outputpath, ".edf", MAX_PATH_LENGTH);

  strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(outputpath, ""))
  {
    goto END_2;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, selected file is in use.");
    messagewindow.exec();
    goto END_2;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open outputfile for writing.");
    messagewindow.exec();
    goto END_2;
  }

  fprintf(outputfile, "0       ");
  fseeko(inputfile, 8LL, SEEK_SET);
  if(fread(scratchpad_256, 176, 1, inputfile)!=1)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Read error (1).");
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Read error (1).");
    messagewindow.exec();
    goto END_3;
  }
  if(fwrite(scratchpad_256, 176, 1, outputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Write error (1).");
    messagewindow.exec();
    goto END_3;
  }
  fprintf(outputfile, "%-8i", new_edfsignals * 256 + 256);
  if(edfhdr->bdfplus)
  {
    if(edfhdr->discontinuous)
    {
      fprintf(outputfile, "EDF+D");
    }
    else
    {
      fprintf(outputfile, "EDF+C");
    }
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  else
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
  fprintf(outputfile, "%-4i", new_edfsignals);

  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "EDF Annotations ");
    }
    else
    {
      fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].label);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].transducer);
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].physdimension);
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "-1      ");
    }
    else
    {
      snprintf(scratchpad_256, 256, "%f", edfhdr->edfparam[signalslist[i]].bitvalue * -32768.0 * dividerlist[i]);
      convert_trailing_zeros_to_spaces(scratchpad_256);
      if(scratchpad_256[7]=='.')
      {
        scratchpad_256[7] = ' ';
      }
      scratchpad_256[8] = 0;
      fprintf(outputfile, "%s", scratchpad_256);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "1       ");
    }
    else
    {
      snprintf(scratchpad_256, 256, "%f", edfhdr->edfparam[signalslist[i]].bitvalue * 32767.0 * dividerlist[i]);
      convert_trailing_zeros_to_spaces(scratchpad_256);
      if(scratchpad_256[7]=='.')
      {
        scratchpad_256[7] = ' ';
      }
      scratchpad_256[8] = 0;
      fprintf(outputfile, "%s", scratchpad_256);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "-32768  ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "32767   ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      for(j=0; j<80; j++)
      {
        fputc(' ', outputfile);
      }
    }
    else
    {
      snprintf(scratchpad_256, 256, "HP:%f", ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(signalslist[i], 1)))->value());
      remove_trailing_zeros(scratchpad_256);
      strlcat(scratchpad_256, "Hz ", 256);

      strlcat(scratchpad_256, edfhdr->edfparam[signalslist[i]].prefilter, 256);

      for(j=strlen(scratchpad_256); j<200; j++)
      {
        scratchpad_256[j] = ' ';
      }

      scratchpad_256[200] = 0;

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad_256 + j, "No filtering", 12))
        {
          for(k=j; k<(j+12); k++)
          {
            scratchpad_256[k] = ' ';
          }
        }
      }

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad_256 + j, "None", 4))
        {
          for(k=j; k<(j+4); k++)
          {
            scratchpad_256[k] = ' ';
          }
        }
      }

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad_256 + j, "HP: DC;", 7))
        {
          for(k=j; k<(j+7); k++)
          {
            scratchpad_256[k] = ' ';
          }
        }
      }

      scratchpad_256[80] = 0;

      fprintf(outputfile, "%s", scratchpad_256);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      if(edfhdr->edfparam[signalslist[i]].smp_per_record % 2)
      {
        fprintf(outputfile, "%-8i", ((edfhdr->edfparam[signalslist[i]].smp_per_record * 15) / 10) + 1);
      }
      else
      {
        fprintf(outputfile, "%-8i", (edfhdr->edfparam[signalslist[i]].smp_per_record * 15) / 10);
      }
    }
    else
    {
      fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].smp_per_record);
    }
  }
  for(i=0; i<(new_edfsignals * 32); i++)
  {
   fputc(' ', outputfile);
  }

///////////////////////////// start conversion //////////////////////////////////////

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  fseeko(inputfile, (long long)(edfhdr->hdrsize), SEEK_SET);

  for(datrecs=0; datrecs<datarecords; datrecs++)
  {
    if(!(datrecs%progress_steps))
    {
      progress.setValue(datrecs);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto END_3;
      }
    }

    if(fread(readbuf, edfhdr->recordsize, 1, inputfile) != 1)
    {
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", "Read error (2).");
      goto END_3;
    }

    for(i=0; i<new_edfsignals; i++)
    {
      if(annotlist[i])
      {
        if(fwrite(readbuf + edfhdr->edfparam[signalslist[i]].datrec_offset, edfhdr->edfparam[signalslist[i]].smp_per_record * 3, 1, outputfile)!=1)
        {
          progress.reset();
          QMessageBox::critical(myobjectDialog, "Error", "Write error (2).");
          goto END_3;
        }

        if(edfhdr->edfparam[signalslist[i]].smp_per_record % 2)
        {
          if(fputc(0, outputfile)==EOF)
          {
            progress.reset();
            QMessageBox::critical(myobjectDialog, "Error", "Write error (3).");
            goto END_3;
          }
        }
      }
      else
      {
        for(j=0; j<edfhdr->edfparam[signalslist[i]].smp_per_record; j++)
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

          var.one_signed += edfhdr->edfparam[signalslist[i]].offset;

          if(use_hpf)
          {
            var.one_signed = (first_order_filter(var.one_signed, filterlist[i]) / dividerlist[i]) + 0.5;
          }
          else
          {
            var.one_signed /= dividerlist[i];
          }

          if(var.one_signed>32767)  var.one_signed = 32767;

          if(var.one_signed<-32768)  var.one_signed = -32768;

          var2.one_short = var.one_signed;

          fputc(var2.two_bytes[0], outputfile);
          if(fputc(var2.two_bytes[1], outputfile)==EOF)
          {
            progress.reset();
            QMessageBox::critical(myobjectDialog, "Error", "Write error (4).");
            goto END_3;
          }
        }
      }
    }
  }

  progress.reset();
  QMessageBox::information(myobjectDialog, "Ready", "Done.");

END_3:

  fclose(outputfile);
  outputfile = NULL;

END_2:

  free(readbuf);

END_1:

  for(i=0; i<new_edfsignals; i++)
  {
    free(filterlist[i]);
  }

  fclose(inputfile);
  inputfile = NULL;

  inputpath[0] = 0;
  outputpath[0] = 0;

  free_edfheader();

  label1->setText("");

  SignalsTablewidget->setRowCount(0);
}

















