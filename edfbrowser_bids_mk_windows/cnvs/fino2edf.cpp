/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008 - 2024 Teunis van Beelen
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



#include "fino2edf.h"

#define F_MAX_SIGNALS   (32)



UI_FINO2EDFwindow::UI_FINO2EDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 240 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Finometer to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  PatientnameLineEdit = new QLineEdit;
  PatientnameLineEdit->setMaxLength(80);

  RecordingLineEdit = new QLineEdit;
  RecordingLineEdit->setMaxLength(80);
  RecordingLineEdit->setText("Finometer");

  StartDatetimeedit = new QDateTimeEdit;
  StartDatetimeedit->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
  StartDatetimeedit->setDateTime(QDateTime::currentDateTime());
  StartDatetimeedit->setMaximumDateTime(QDateTime::fromString("20841231235959", "yyyyMMddhhmmss"));
  StartDatetimeedit->setMinimumDateTime(QDateTime::fromString("19850101000000", "yyyyMMddhhmmss"));

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  QFormLayout *flayout =  new QFormLayout;
  flayout->addRow("Subject name", PatientnameLineEdit);
  flayout->addRow("Recording", RecordingLineEdit);
  flayout->addRow("Startdate and time", StartDatetimeedit);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_FINO2EDFwindow::SelectFileButton()
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
      datarecords,
      str_start,
      column,
      line_nr;

  char str1_2048[2048]={""},
       path[MAX_PATH_LENGTH]={""},
       outputfilename[MAX_PATH_LENGTH]={""},
       line_2048[2048]={""},
       scratchpad[128]={""},
       labels[F_MAX_SIGNALS][17],
       phys_dim[F_MAX_SIGNALS][9],
       phys_min[F_MAX_SIGNALS][9],
       phys_max[F_MAX_SIGNALS][9],
       patientname[81]={""},
       recording[81]={""},
       datetime[128]={""};


  double sensitivity[F_MAX_SIGNALS],
         new_value[F_MAX_SIGNALS],
         old_value[F_MAX_SIGNALS];

  union{
         short two;
         char one[2];
       } var;



  for(j=0; j<F_MAX_SIGNALS; j++)
  {
    old_value[j] = 0.0;
  }

  enable_widgets(false);

  if(!(strlen(PatientnameLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a subjectname.");
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

  strlcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "Text files (*.txt *.TXT);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

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
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

/***************** check if the txtfile is valid ******************************/

  rewind(inputfile);

  if(fread(scratchpad, 5, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading the inputfile.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(strncmp(scratchpad, "\"T\";", 4))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown data in file (1).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (2).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\n')  break;
  }

  if(fread(scratchpad, 2, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile (3).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(strncmp(scratchpad, "\"s\";", 4))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown data in file (4).");
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

    if((temp==separator)||(temp=='\n'))
    {
      if(edfsignals>=F_MAX_SIGNALS)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many labels/signals (7).");
        messagewindow.exec();
        fclose(inputfile);
        enable_widgets(true);
        return;
      }

      line_2048[i] = 0;

      ok = 0;

      if(!strcmp(line_2048, "\" reSYS\""))
      {
        strlcpy(labels[edfsignals], "reSYS           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" fiSYS\""))
      {
        strlcpy(labels[edfsignals], "fiSYS           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" reDIA\""))
      {
        strlcpy(labels[edfsignals], "reDIA           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" fiDIA\""))
      {
        strlcpy(labels[edfsignals], "fiDIA           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" reMAP\""))
      {
        strlcpy(labels[edfsignals], "reMAP           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" fiMAP\""))
      {
        strlcpy(labels[edfsignals], "fiMAP           ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"  HR\""))
      {
        strlcpy(labels[edfsignals], "HR              ", 17);
        strlcpy(phys_dim[edfsignals], "bpm     ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"   IBI\""))
      {
        strlcpy(labels[edfsignals], "IBI             ", 17);
        strlcpy(phys_dim[edfsignals], "s       ", 9);
        strlcpy(phys_min[edfsignals], "-31.744 ", 9);
        strlcpy(phys_max[edfsignals], "31.744  ", 9);
        sensitivity[edfsignals] = 1000.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"  SV\""))
      {
        strlcpy(labels[edfsignals], "SV              ", 17);
        strlcpy(phys_dim[edfsignals], "ml      ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"    CO\""))
      {
        strlcpy(labels[edfsignals], "CO              ", 17);
        strlcpy(phys_dim[edfsignals], "lpm     ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"   EJT\""))
      {
        strlcpy(labels[edfsignals], "EJT             ", 17);
        strlcpy(phys_dim[edfsignals], "s       ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"    TPR\""))
      {
        strlcpy(labels[edfsignals], "TPR             ", 17);
        strlcpy(phys_dim[edfsignals], "MU      ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" Artifact\""))
      {
        strlcpy(labels[edfsignals], "Artifact        ", 17);
        strlcpy(phys_dim[edfsignals], "_TPSROD2", 9);
        strlcpy(phys_min[edfsignals], "0       ", 9);
        strlcpy(phys_max[edfsignals], "10000000", 9);
        sensitivity[edfsignals] = 0.0063488;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" Zao\""))
      {
        strlcpy(labels[edfsignals], "Zao             ", 17);
        strlcpy(phys_dim[edfsignals], "mMU     ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\"   Cwk\""))
      {
        strlcpy(labels[edfsignals], "Cwk             ", 17);
        strlcpy(phys_dim[edfsignals], "MU      ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line_2048, "\" Height\""))
      {
        strlcpy(labels[edfsignals], "Height          ", 17);
        strlcpy(phys_dim[edfsignals], "mmHg    ", 9);
        strlcpy(phys_min[edfsignals], "-1024   ", 9);
        strlcpy(phys_max[edfsignals], "1024    ", 9);
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!ok)
      {
        snprintf(str1_2048, 2048, "Found unknown label/signal: %.2000s", line_2048);
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

    line_2048[i++] = temp;
  }

  if(!edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There are no labels/signals.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
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

  snprintf(datetime, 128,
          "%02i.%02i.%02i%02i.%02i.%02i",
          StartDatetimeedit->date().day(),
          StartDatetimeedit->date().month(),
          StartDatetimeedit->date().year() % 100,
          StartDatetimeedit->time().hour(),
          StartDatetimeedit->time().minute(),
          StartDatetimeedit->time().second());
  datetime[16] = 0;


/***************** write header *****************************************/

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);
  remove_extension_from_filename(outputfilename);
  strlcat(outputfilename, "_finometer.edf", MAX_PATH_LENGTH);

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
  fprintf(outputfile, "%s", datetime);
  fprintf(outputfile, "%-8i", 256 * edfsignals + 256);
  fprintf(outputfile, "                                            ");
  fprintf(outputfile, "-1      ");
  fprintf(outputfile, "0.01    ");
  fprintf(outputfile, "%-4i", edfsignals);

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", labels[i]);
  }

  for(i=0; i<(80*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_dim[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_min[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_max[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "-31744  ");
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "31744   ");
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

    if(i>1)  break;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(k=0; k<10; k++)  qApp->processEvents();

  i = 0;

  column = 0;

  datarecords = 0;

  str_start = 0;

  line_nr = 2;

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

    line_2048[i] = temp;

  /**************************************/

    if(line_2048[i]==',')
    {
      line_2048[i] = '.';
    }

    if((line_2048[i]==separator)||(line_2048[i]=='\n'))
    {
      if(column)
      {
        new_value[column-1] = atof(line_2048 + str_start);
      }
      else
      {
        new_smpl_time = (int)(atof(line_2048 + str_start) * 100.0);
      }

      if(line_2048[i]=='\n')
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

  QApplication::restoreOverrideCursor();

  fseeko(outputfile, 236LL, SEEK_SET);

  fprintf(outputfile, "%-8i", datarecords);

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
  QMessageBox messagewindow(QMessageBox::Information, "Ready", str1_2048);
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();

  enable_widgets(true);
}


void UI_FINO2EDFwindow::enable_widgets(bool toggle)
{
  StartDatetimeedit->setEnabled(toggle);
  PatientnameLineEdit->setEnabled(toggle);
  RecordingLineEdit->setEnabled(toggle);
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}



