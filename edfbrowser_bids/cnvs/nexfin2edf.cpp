/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2024 Teunis van Beelen
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



#include "nexfin2edf.h"


#define N_MAX_SIGNALS   (32)


UI_NEXFIN2EDFwindow::UI_NEXFIN2EDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 280 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Nexfin to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  PatientnameLineEdit = new QLineEdit;
  PatientnameLineEdit->setMaxLength(80);

  RecordingLineEdit = new QLineEdit;
  RecordingLineEdit->setMaxLength(80);
  RecordingLineEdit->setText("Nexfin");

  StartDatetimeedit = new QDateTimeEdit;
  StartDatetimeedit->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
  StartDatetimeedit->setDateTime(QDateTime::currentDateTime());
  StartDatetimeedit->setMaximumDateTime(QDateTime::fromString("20841231235959", "yyyyMMddhhmmss"));
  StartDatetimeedit->setMinimumDateTime(QDateTime::fromString("19850101000000", "yyyyMMddhhmmss"));

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  radio100button = new QRadioButton;
  radio100button->setText("Beat-to-beat data");
  radio100button->setChecked(true);

  radio103button = new QRadioButton;
  radio103button->setText("Blood pressure waveform");

  radio032button = new QRadioButton;
  radio032button->setText("Analog inputs");

  analoginputscombobox = new QComboBox;
  analoginputscombobox->addItem("1 input");
  analoginputscombobox->addItem("4 inputs");
  analoginputscombobox->setCurrentIndex(1);
  analoginputscombobox->setEnabled(false);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(radio032button);
  hlayout2->addWidget(analoginputscombobox);
  hlayout2->addStretch(1000);

  QFormLayout *flayout =  new QFormLayout;
  flayout->addRow("Subject name", PatientnameLineEdit);
  flayout->addRow("Recording", RecordingLineEdit);
  flayout->addRow("Startdate and time", StartDatetimeedit);
  flayout->addRow(" ", radio100button);
  flayout->addRow(" ", radio103button);
  flayout->addRow(" ", hlayout2);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1,    SIGNAL(clicked()),     this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,    SIGNAL(clicked()),     myobjectDialog, SLOT(close()));
  QObject::connect(radio032button, SIGNAL(toggled(bool)), this,           SLOT(radiobuttonchecked(bool)));

  myobjectDialog->exec();
}


void UI_NEXFIN2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL,
       *outputfile=NULL;

  int i, j, k, p,
      temp,
      separator=';',
      edfsignals=0,
      ok,
      timestep,
      new_smpl_time=0,
      str_start,
      column,
      line_nr;

  long long datarecords=-1LL,
            file_size;

  char str1_2048[2048]={""},
       path[MAX_PATH_LENGTH]={""},
       outputfilename[MAX_PATH_LENGTH]={""},
       line_str_2048[2048]={""},
       scratchpad_128[128]={""},
       labels[N_MAX_SIGNALS][17],
       phys_dim[N_MAX_SIGNALS][9],
       phys_min[N_MAX_SIGNALS][9],
       phys_max[N_MAX_SIGNALS][9],
       patientname[81]={""},
       recording[81]={""},
       datetime_128[128]={""};


  double sensitivity[N_MAX_SIGNALS],
         new_value[N_MAX_SIGNALS],
         old_value[N_MAX_SIGNALS];

  union{
         short two;
         char one[2];
       } var;


  if(radio100button->isChecked()==true)
  {
    file_type = 100;
  }

  if(radio103button->isChecked()==true)
  {
    file_type = 103;
  }

  if(radio032button->isChecked()==true)
  {
    file_type = 32;
  }

  for(j=0; j<N_MAX_SIGNALS; j++)
  {
    old_value[j] = 0.0;
  }

  enable_widgets(false);

  if(!(strlen(PatientnameLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a subject name.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  if(!(strlen(RecordingLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a recordingdescription.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  if(file_type==100)
  {
    strlcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "CSV files (*_100.csv *_100.CSV)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(file_type==103)
  {
    strlcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "BIN files (*_103.bin *_103.BIN)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(file_type==32)
  {
    strlcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "BIN files (*_032.bin *_032.BIN)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(path, ""))
  {
    enable_widgets(true);
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for reading.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str1_2048));
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  if(file_type==100)
  {
/***************** check if the txtfile is valid ******************************/

  rewind(inputfile);

  if(fread(scratchpad_128, 32, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot read from file.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(strncmp(scratchpad_128, "Time;IBI;HR;LVET;HRS;Sys;Dia;MAP", 32))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown data in file (1).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

/***************** collect items *****************************************/

  rewind(inputfile);

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (5).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp==separator)
    {
      break;
    }
  }

  i = 0;

  while(temp!='\n')
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (6).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\r')
    {
      continue;
    }

    if(((temp==separator)||(temp=='\n'))&&(edfsignals<7))
    {
      if(edfsignals>=N_MAX_SIGNALS)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many labels/signals (7).");
        messagewindow.exec();
        fclose(inputfile);
        enable_widgets(true);
        return;
      }

      line_str_2048[i] = 0;

      ok = 0;

      if(!strcmp(line_str_2048, "IBI"))
      {
        strlcpy(labels[edfsignals], "IBI             ", 17);
        strlcpy(phys_dim[edfsignals], "s       ", 9);
        strlcpy(phys_min[edfsignals], "-31.744 ", 9);
        strlcpy(phys_max[edfsignals], "31.744  ", 9);
        sensitivity[edfsignals] = 1000.0;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "HR"))
      {
        strlcpy(labels[edfsignals], "HR              ", 17);
        strlcpy(phys_dim[edfsignals], "bpm     ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "LVET"))
      {
        strlcpy(labels[edfsignals], "LVET            ", 17);
        strlcpy(phys_dim[edfsignals], "s       ", 9);
        strlcpy(phys_min[edfsignals], "-10     ", 9);
        strlcpy(phys_max[edfsignals], "10      ", 9);
        sensitivity[edfsignals] = 3174.4;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "HRS"))
      {
        strlcpy(labels[edfsignals], "HRS             ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "Sys"))
      {
        strlcpy(labels[edfsignals], "Sys             ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "Dia"))
      {
        strlcpy(labels[edfsignals], "Dia             ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_str_2048, "MAP"))
      {
        strlcpy(labels[edfsignals], "MAP             ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!ok)
      {
        snprintf(str1_2048, 2048, "Found unknown label/signal: %.2000s", line_str_2048);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
        messagewindow.exec();
        fclose(inputfile);
        enable_widgets(true);
        return;
      }

      edfsignals++;

      i = 0;
    }

    if(temp==separator)
    {
      continue;
    }

    line_str_2048[i++] = temp;
  }

  if(!edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There are no labels/signals.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  }  /********** end if(file_type==100)  ********************/

  if(file_type==103)
  {
    edfsignals = 1;
  }

  if(file_type==32)
  {
    if(analoginputscombobox->currentIndex()==1)
    {
      edfsignals = 4;
    }
    else
    {
      edfsignals = 1;
    }
  }

  p = snprintf(patientname, 81, "%s", PatientnameLineEdit->text().toLatin1().data());
  latin1_to_ascii(patientname, p);
  for( ; p<80; p++)
  {
    patientname[p] = ' ';
  }
  patientname[80] = 0;

  p = snprintf(recording, 81, "%s", RecordingLineEdit->text().toLatin1().data());
  latin1_to_ascii(recording, p);
  for( ; p<80; p++)
  {
    recording[p] = ' ';
  }
  recording[80] = 0;

  snprintf(datetime_128, 128,
          "%02i.%02i.%02i%02i.%02i.%02i",
          StartDatetimeedit->date().day(),
          StartDatetimeedit->date().month(),
          StartDatetimeedit->date().year() % 100,
          StartDatetimeedit->time().hour(),
          StartDatetimeedit->time().minute(),
          StartDatetimeedit->time().second());
  datetime_128[16] = 0;


/***************** write header *****************************************/

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);
  remove_extension_from_filename(outputfilename);
  strlcat(outputfilename, ".edf", MAX_PATH_LENGTH);

  path[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strlcpy(path, recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  strlcat(path, outputfilename, MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopeno(path, "wb");
  if(outputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for writing.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
    messagewindow.exec();
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  fprintf(outputfile, "0       ");
  fprintf(outputfile, "%s", patientname);
  fprintf(outputfile, "%s", recording);
  fprintf(outputfile, "%s", datetime_128);
  fprintf(outputfile, "%-8i", 256 * edfsignals + 256);
  fprintf(outputfile, "                                            ");
  fprintf(outputfile, "-1      ");
  if(file_type==100)
  {
    fprintf(outputfile, "0.01    ");
  }
  if(file_type==103)
  {
    fprintf(outputfile, "0.005   ");
  }
  if(file_type==32)
  {
    fprintf(outputfile, "0.005   ");
  }
  fprintf(outputfile, "%-4i", edfsignals);

  if(file_type==100)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "%s", labels[i]);
    }
  }

  if(file_type==103)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "Blood pressure  ");
    }
  }

  if(file_type==32)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "AD-%i            ", i + 1);
    }
  }

  for(i=0; i<(80*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

  if(file_type==100)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "%s", phys_dim[i]);
    }
  }

  if(file_type==103)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "mmHg    ");
    }
  }

  if(file_type==32)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "mV      ");
    }
  }

  if(file_type==100)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "%s", phys_min[i]);
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "%s", phys_max[i]);
    }
  }

  if(file_type==103)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "-8192   ");
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "8191.75 ");
    }
  }

  if(file_type==32)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "-81920  ");
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "81917.5 ");
    }
  }

  if(file_type==100)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "-31744  ");
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "31744   ");
    }
  }

  if(file_type==103)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "-32768  ");
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "32767   ");
    }
  }

  if(file_type==32)
  {
    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "-32768  ");
    }

    for(i=0; i<edfsignals; i++)
    {
      fprintf(outputfile, "32767   ");
    }
  }

  for(i=0; i<(80*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "1       ");
  }

  for(i=0; i<(32*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

/***************** start conversion **************************************/

  rewind(inputfile);

  if(file_type==100)   /*** filetype is 100 **********/
  {

  for(i=0; ; )
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (8).");
      messagewindow.exec();
      fclose(inputfile);
      fclose(outputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\n')  i++;

    if(i>0)  break;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(k=0; k<10; k++)  qApp->processEvents();

  i = 0;

  column = 0;

  datarecords = 0LL;

  str_start = 0;

  line_nr = 1;

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      for(k=0; k<edfsignals; k++)
      {
        temp = (int)(new_value[k] * sensitivity[k]);

        if(temp>31744)  temp = 31744;

        if(temp<-31744)  temp = -31744;

        var.two = (short)temp;

        fputc(var.one[0], outputfile);
        fputc(var.one[1], outputfile);
      }

      datarecords++;

      break;
    }

    line_str_2048[i] = temp;

  /**************************************/

    if(line_str_2048[i]==',')
    {
      line_str_2048[i] = '.';
    }

    if((line_str_2048[i]==separator)||(line_str_2048[i]=='\n'))
    {
      if(column)
      {
        new_value[column-1] = atof(line_str_2048 + str_start);
      }
      else
      {
        new_smpl_time = (int)(atof(line_str_2048 + str_start) * 100.0);
      }

      if(line_str_2048[i]=='\n')
      {
        /**************************************/

        line_nr++;

        if(column!=edfsignals)
        {
          QApplication::restoreOverrideCursor();
          snprintf(str1_2048, 2048, "Number of separators in line %i is wrong.", line_nr);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }

        timestep = new_smpl_time - datarecords;

        if(timestep<=0)
        {
          QApplication::restoreOverrideCursor();
          snprintf(str1_2048, 2048, "Timestep <= 0 in line %i.", line_nr);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }

        for(j=0; j<timestep; j++)
        {
          for(k=0; k<edfsignals; k++)
          {
            temp = (int)((old_value[k] + ((new_value[k] - old_value[k]) * ((double)j / (double)timestep))) * sensitivity[k]);

            if(temp>31744)  temp = 31744;

            if(temp<-31744)  temp = -31744;

            var.two = (short)temp;

            fputc(var.one[0], outputfile);
            fputc(var.one[1], outputfile);
          }

          datarecords++;
        }

        for(j=0; j<edfsignals; j++)
        {
          old_value[j] = new_value[j];
        }

        /**************************************/

        str_start = 0;

        i = 0;

        column = 0;

        continue;
      }

      str_start = i + 1;

      column++;
    }

  /**************************************/

    i++;

    if(i>2046)
    {
      QApplication::restoreOverrideCursor();
      snprintf(str1_2048, 2048, "Line %i is too long.", line_nr);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
      messagewindow.exec();
      fclose(inputfile);
      fclose(outputfile);
      enable_widgets(true);
      return;
    }
  }

  }  /********** end if(file_type==100)  ********************/

  if((file_type==103) || (file_type==32))
  {
    fseeko(inputfile, 0LL, SEEK_END);

    file_size = ftello(inputfile);

    file_size /= 2LL;

    file_size /= edfsignals;

    rewind(inputfile);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for(k=0; k<10; k++)  qApp->processEvents();

    if(file_type==103)
    {
      for(datarecords=0LL; datarecords<file_size; datarecords++)
      {
        temp = fgetc(inputfile);
        fputc(temp, outputfile);

        temp = fgetc(inputfile);
        if(temp==EOF)
        {
          QApplication::restoreOverrideCursor();
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (18).");
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }
        fputc(temp, outputfile);
      }
    }

    if(file_type==32)
    {
      k = edfsignals * 2 - 1;

      for(datarecords=0LL; datarecords<file_size; datarecords++)
      {
        for(i=0; i<k; i++)
        {
          temp = fgetc(inputfile);
          fputc(temp, outputfile);
        }

        temp = fgetc(inputfile);
        if(temp==EOF)
        {
          QApplication::restoreOverrideCursor();
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (28).");
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }
        fputc(temp, outputfile);
      }
    }
  }

  QApplication::restoreOverrideCursor();

  fseeko(outputfile, 236LL, SEEK_SET);

#ifdef Q_OS_WIN32
  __mingw_fprintf(outputfile, "%-8lli", datarecords);
#else
  fprintf(outputfile, "%-8lli", datarecords);
#endif

  if(fclose(outputfile))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while closing outputfile.");
    messagewindow.exec();
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  if(fclose(inputfile))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while closing inputfile.");
    messagewindow.exec();
  }

  snprintf(str1_2048, 2048, "Done, EDF file is located at %s", path);
  QMessageBox messagewindow(QMessageBox::Information, "Ready", QString::fromLocal8Bit(str1_2048));
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();

  enable_widgets(true);
}


void UI_NEXFIN2EDFwindow::enable_widgets(bool toggle)
{
  StartDatetimeedit->setEnabled(toggle);
  PatientnameLineEdit->setEnabled(toggle);
  RecordingLineEdit->setEnabled(toggle);
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}



void UI_NEXFIN2EDFwindow::radiobuttonchecked(bool checked)
{
  if(checked==true)
  {
    analoginputscombobox->setEnabled(true);
  }
  else
  {
    analoginputscombobox->setEnabled(false);
  }
}




