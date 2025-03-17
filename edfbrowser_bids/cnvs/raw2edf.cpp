/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012 - 2024 Teunis van Beelen
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



#include "raw2edf.h"



UI_RAW2EDFapp::UI_RAW2EDFapp(QWidget *w_parent, raw2edf_var_t *raw2edf_var_p, char *recent_dir, char *save_dir)
{
  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  mainwindow = (UI_Mainwindow *)w_parent;

  raw2edf_var = raw2edf_var_p;

  raw2edfDialog = new QDialog;
  raw2edfDialog->setMinimumSize(525 * mainwindow->w_scaling, 375 * mainwindow->h_scaling);
  raw2edfDialog->setWindowTitle("Binary/raw data to EDF converter");
  raw2edfDialog->setModal(true);
  raw2edfDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  raw2edfDialog->setSizeGripEnabled(true);

  SamplefreqSpinbox = new QSpinBox;
  SamplefreqSpinbox->setRange(1,1000000);
  SamplefreqSpinbox->setSuffix(" Hz");
  SamplefreqSpinbox->setValue(raw2edf_var->sf);
  SamplefreqSpinbox->setToolTip("Samplerate");

  SignalsSpinbox = new QSpinBox;
  SignalsSpinbox->setRange(1,256);
  SignalsSpinbox->setValue(raw2edf_var->chns);
  SignalsSpinbox->setToolTip("Number of channels");

  SampleSizeCombobox = new QComboBox;
  SampleSizeCombobox->addItem("12 bits (1.5 byte)");
  SampleSizeCombobox->addItem("8 bits (1 byte)");
  SampleSizeCombobox->addItem("16 bits (2 bytes)");
  SampleSizeCombobox->setToolTip("Bits per sample");
  SampleSizeCombobox->setCurrentIndex(raw2edf_var->samplesize);

  OffsetSpinbox = new QSpinBox;
  OffsetSpinbox->setRange(0,1000000);
  OffsetSpinbox->setValue(raw2edf_var->offset);
  OffsetSpinbox->setToolTip("Bytes to skip from start of file to data startpoint");

  EncodingCombobox = new QComboBox;
  EncodingCombobox->addItem("2's complement");
  EncodingCombobox->addItem("bipolar offset binary");
  EncodingCombobox->setCurrentIndex(raw2edf_var->straightbinary);
  EncodingCombobox->setToolTip("bipolar offset binary or\n"
                               "2's complement");

  EndiannessCombobox = new QComboBox;
  EndiannessCombobox->addItem("little endian");
  EndiannessCombobox->addItem("big endian");
  EndiannessCombobox->setCurrentIndex(raw2edf_var->endianness);
  EndiannessCombobox->setToolTip("little endian: least significant byte comes first\n"
                                 "big endian: most significant byte comes first");

  skipblocksizeSpinbox = new QSpinBox;
  skipblocksizeSpinbox->setRange(0, 1000000);
  skipblocksizeSpinbox->setValue(raw2edf_var->skipblocksize);
  skipblocksizeSpinbox->setToolTip("Skip after every n databytes");

  skipbytesSpinbox = new QSpinBox;
  skipbytesSpinbox->setRange(1, 1000000);
  skipbytesSpinbox->setValue(raw2edf_var->skipbytes);
  skipbytesSpinbox->setToolTip("Number of bytes that must be skipped");

  PhysicalMaximumSpinbox = new QSpinBox;
  PhysicalMaximumSpinbox->setRange(1, 10000000);
  PhysicalMaximumSpinbox->setValue(raw2edf_var->phys_max);
  PhysicalMaximumSpinbox->setToolTip("The maximum physical value that can be measured\n"
                                     "e.g. the maximum value of an inputchannel.");

  PhysicalDimensionLineEdit = new QLineEdit;
  PhysicalDimensionLineEdit->setMaxLength(8);
  PhysicalDimensionLineEdit->setText(raw2edf_var->phys_dim);
  PhysicalDimensionLineEdit->setToolTip("uV, bpm, mL, Ltr, etc.");
  PhysicalDimensionLineEdited(PhysicalDimensionLineEdit->text());

  variableTypeLineEdit = new QLineEdit;
  variableTypeLineEdit->setText("i16");
  variableTypeLineEdit->setReadOnly(true);

  PatientnameLineEdit = new QLineEdit;
  PatientnameLineEdit->setMaxLength(80);

  RecordingLineEdit = new QLineEdit;
  RecordingLineEdit->setMaxLength(80);

  StartDatetimeedit = new QDateTimeEdit;
  StartDatetimeedit->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
  StartDatetimeedit->setDateTime(QDateTime::currentDateTime());
  StartDatetimeedit->setToolTip("dd/MM/yyyy hh:mm:ss");

  GoButton = new QPushButton;
  GoButton->setText("Start");
  GoButton->setToolTip("Start the conversion");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  SaveButton = new QPushButton;
  SaveButton->setText("Save");
  SaveButton->setToolTip("Save the entered parameters to a template for later use");

  LoadButton = new QPushButton;
  LoadButton->setText("Load");
  LoadButton->setToolTip("Load parameters from a template");

  edfsignals = 1;

  QFormLayout *flayout1 = new QFormLayout;
  flayout1->addRow("Samplefrequency", SamplefreqSpinbox);
  flayout1->addRow("Number of signals", SignalsSpinbox);
  flayout1->addRow("Sample size", SampleSizeCombobox);
  flayout1->addRow("Offset", OffsetSpinbox);
  flayout1->addRow("Encoding", EncodingCombobox);
  flayout1->addRow("Endianness", EndiannessCombobox);

  QFormLayout *flayout2 = new QFormLayout;
  flayout2->addRow("Data blocksize", skipblocksizeSpinbox);
  flayout2->addRow("Skip bytes", skipbytesSpinbox);
  flayout2->addRow("Physical maximum", PhysicalMaximumSpinbox);
  flayout2->addRow("Physical dimension", PhysicalDimensionLineEdit);
  flayout2->addRow("sampletype", variableTypeLineEdit);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(flayout1);
  hlayout2->addSpacing(20);
  hlayout2->addLayout(flayout2);
  hlayout2->addStretch(1000);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(StartDatetimeedit);
  hlayout3->addStretch(1000);

  QFormLayout *flayout3 = new QFormLayout;
  flayout3->addRow("Subject name", PatientnameLineEdit);
  flayout3->addRow("Recording", RecordingLineEdit);
  flayout3->addRow("Startdate and time", hlayout3);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(GoButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(helpButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(SaveButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(LoadButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(flayout3);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  raw2edfDialog->setLayout(vlayout1);

  QObject::connect(GoButton,                    SIGNAL(clicked()),                this,            SLOT(gobuttonpressed()));
  QObject::connect(CloseButton,                 SIGNAL(clicked()),                raw2edfDialog,   SLOT(close()));
  QObject::connect(SaveButton,                  SIGNAL(clicked()),                this,            SLOT(savebuttonpressed()));
  QObject::connect(LoadButton,                  SIGNAL(clicked()),                this,            SLOT(loadbuttonpressed()));
  QObject::connect(PhysicalDimensionLineEdit,   SIGNAL(textEdited(QString)),      this,            SLOT(PhysicalDimensionLineEdited(QString)));
  QObject::connect(SampleSizeCombobox,          SIGNAL(currentIndexChanged(int)), this,            SLOT(sampleTypeChanged(int)));
  QObject::connect(EncodingCombobox,            SIGNAL(currentIndexChanged(int)), this,            SLOT(sampleTypeChanged(int)));
  QObject::connect(helpButton,                  SIGNAL(clicked()),                this,            SLOT(helpbuttonpressed()));

  sampleTypeChanged(0);

  raw2edfDialog->exec();
}


void UI_RAW2EDFapp::gobuttonpressed()
{
  int i, j, k, r,
      hdl,
      chns,
      sf,
      *buf,
      datarecords,
      tmp,
      tmp1,
      tmp2,
      straightbinary,
      samplesize,
      skipblocksize,
      skipbytes,
      skipblockcntr,
      bytecntr,
      big_endian,
      odd_even=0;

  char str1_1024[1024]={""},
       path[MAX_PATH_LENGTH]={""};

  double phys_max;

  long long d_offset;

  FILE *inputfile=NULL;


  sf = SamplefreqSpinbox->value();
  raw2edf_var->sf = sf;

  chns = SignalsSpinbox->value();
  raw2edf_var->chns = chns;

  phys_max = PhysicalMaximumSpinbox->value();
  raw2edf_var->phys_max = phys_max;

  straightbinary = EncodingCombobox->currentIndex();
  raw2edf_var->straightbinary = straightbinary;

  big_endian = EndiannessCombobox->currentIndex();
  raw2edf_var->endianness = big_endian;

  samplesize = SampleSizeCombobox->currentIndex();
  raw2edf_var->samplesize = samplesize;

  d_offset = OffsetSpinbox->value();
  raw2edf_var->offset = d_offset;

  skipblocksize = skipblocksizeSpinbox->value();
  raw2edf_var->skipblocksize = skipblocksize;

  skipbytes = skipbytesSpinbox->value();
  raw2edf_var->skipbytes = skipbytes;

  strlcpy(raw2edf_var->phys_dim, PhysicalDimensionLineEdit->text().toLatin1().data(), 16);
  trim_spaces(raw2edf_var->phys_dim);

  if(!(strlen(PatientnameLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a subjectname.");
    messagewindow.exec();
    return;
  }

  if(!(strlen(RecordingLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a recording description.");
    messagewindow.exec();
    return;
  }

  strlcpy(path, QFileDialog::getOpenFileName(0, "Open data file", QString::fromLocal8Bit(recent_opendir), "All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return;
  }

  remove_extension_from_filename(path);
  strlcat(path, ".edf", MAX_PATH_LENGTH);

  hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, chns);

  if(hdl<0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for writing.\nedfopen_file_writeonly()");
    messagewindow.exec();
    fclose(inputfile);
    return;
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_samplefrequency(hdl, i, sf))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_samplefrequency()");
      messagewindow.exec();
      fclose(inputfile);
      return;
    }
  }

  if(samplesize == 2)
  {
    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_maximum(hdl, i, 32767))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_maximum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_minimum(hdl, i, -32768))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_minimum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }
  }

  if(samplesize == 1)
  {
    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_maximum(hdl, i, 255))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_maximum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_minimum(hdl, i, -256))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_minimum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }
  }

  if(samplesize == 0)
  {
    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_maximum(hdl, i, 4095))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_maximum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_minimum(hdl, i, -4096))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_digital_minimum()");
        messagewindow.exec();
        fclose(inputfile);
        return;
      }
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_maximum(hdl, i, phys_max))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_physical_maximum()");
      messagewindow.exec();
      fclose(inputfile);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_minimum(hdl, i, -phys_max))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_physical_minimum()");
      messagewindow.exec();
      fclose(inputfile);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_dimension(hdl, i, raw2edf_var->phys_dim))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_physical_dimension()");
      messagewindow.exec();
      fclose(inputfile);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    snprintf(str1_1024, 1024, "ch. %i", i + 1);

    if(edf_set_label(hdl, i, str1_1024))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_label()");
      messagewindow.exec();
      fclose(inputfile);
      return;
    }
  }

  if(edf_set_startdatetime(hdl, StartDatetimeedit->date().year(), StartDatetimeedit->date().month(), StartDatetimeedit->date().day(),
                                StartDatetimeedit->time().hour(), StartDatetimeedit->time().minute(), StartDatetimeedit->time().second()))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_startdatetime()");
    messagewindow.exec();
    fclose(inputfile);
    edfclose_file(hdl);
    return;
  }

  buf = (int *)malloc(sizeof(int) * sf * chns);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "malloc()");
    messagewindow.exec();
    fclose(inputfile);
    edfclose_file(hdl);
    return;
  }

//   printf("samplefrequency:  %14i Hz\n"
//          "channels:         %14i\n"
//          "physical maximum: %14.6f uV\n"
//          "straightbinary:   %14i\n"
//          "samplesize:       %14i byte(s)\n"
//          "offset:           %14lli bytes\n"
//          "skip blocksize:   %14i bytes\n"
//          "skip bytes:       %14i bytes\n\n",
//          sf,
//          chns,
//          phys_max,
//          straightbinary,
//          samplesize,
//          d_offset,
//          skipblocksize,
//          skipbytes);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  raw2edfDialog->setEnabled(false);

  fseeko(inputfile, d_offset, SEEK_SET);

  datarecords = 0;

  union{
         int one[1];
         signed short two[2];
         char four[4];
       } var;

  skipblockcntr = 0;

  bytecntr = 0;

  if(samplesize > 0)
  {
    while(1)
    {
      qApp->processEvents();

      for(j=0; j<sf; j++)
      {
        for(k=0; k<chns; k++)
        {
          var.one[0] = 0;

  //         tmp = fgetc(inputfile);
  //         if(tmp == EOF)
  //         {
  //           goto END_1;
  //         }
  //
  //         tmp += (fgetc(inputfile) * 256);
  //
  //         buf[j + (k * sf)] = tmp - 32768;

          if(samplesize == 2)
          {
            tmp = fgetc(inputfile);
            if(tmp == EOF)
            {
              goto END_1;
            }
            bytecntr++;

  //           printf("1: skipblockcntr is %i    tmp is %02X   bytecntr is %i\n", skipblockcntr, tmp, bytecntr);

            if(skipblocksize)
            {
              if(++skipblockcntr >= skipblocksize)
              {
                for(r=0; r<skipbytes; r++)
                {
                  tmp1 = fgetc(inputfile);
                  if(tmp1 == EOF)
                  {
                    goto END_1;
                  }
  //                bytecntr++;

  //                printf("2: skipblockcntr is %i    tmp is %02X   bytecntr is %i\n", skipblockcntr, tmp, bytecntr);

                }

                skipblockcntr = 0;
              }
            }

            if(big_endian)
            {
              var.four[1] = tmp;
            }
            else
            {
              var.four[0] = tmp;
            }
          }

          if(samplesize == 1)
          {
            var.four[0] = 0;
          }

          tmp = fgetc(inputfile);
          if(tmp == EOF)
          {
            goto END_1;
          }
          bytecntr++;

  //         printf("3: skipblockcntr is %i    tmp is %02X   bytecntr is %i\n", skipblockcntr, tmp, bytecntr);

          if(skipblocksize)
          {
            if(++skipblockcntr >= skipblocksize)
            {
              for(r=0; r<skipbytes; r++)
              {
                tmp1 = fgetc(inputfile);
                if(tmp1 == EOF)
                {
                  goto END_1;
                }
                bytecntr++;

  //               printf("4: skipblockcntr is %i    tmp is %02X   bytecntr is %i\n", skipblockcntr, tmp, bytecntr);

              }

              skipblockcntr = 0;
            }
          }

          if(big_endian && (samplesize == 2))
          {
            var.four[0] = tmp;
          }
          else
          {
            var.four[1] = tmp;
          }

          if(straightbinary)
          {
            var.two[0] -= 32768;
          }

          if(samplesize == 1)
          {
            var.two[0] >>= 8;
          }

          buf[j + (k * sf)] = var.two[0];

//           if(bytecntr <= 72)
//           {
//             printf("%2i   0x%02X\n", bytecntr, (int)(var.two[0]));
//           }
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        QApplication::restoreOverrideCursor();
        raw2edfDialog->setEnabled(true);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Write error during conversion.\nedf_blockwrite_digital_samples()");
        messagewindow.exec();
        edfclose_file(hdl);
        fclose(inputfile);
        free(buf);
        return;
      }

      datarecords++;
  //    if(datarecords == 1)  break;
    }
  }

  if(samplesize == 0)
  {
    while(1)
    {
      qApp->processEvents();

      for(j=0; j<sf; j++)
      {
        for(k=0; k<chns; k++)
        {
          var.one[0] = 0;

          if(odd_even)
          {
            odd_even = 0;

            tmp1 = fgetc(inputfile);
            if(tmp1 == EOF)
            {
              goto END_1;
            }

            if(skipblocksize)
            {
              if(++skipblockcntr >= skipblocksize)
              {
                for(r=0; r<skipbytes; r++)
                {
                  tmp = fgetc(inputfile);
                  if(tmp == EOF)
                  {
                    goto END_1;
                  }
                }

                skipblockcntr = 0;
              }
            }

            var.one[0] = (tmp2 & 0xf0) << 4;
            var.one[0] += tmp1;
            if(straightbinary)
            {
              if(var.one[0] & 0x800)
              {
                var.one[0] &= 0x7ff;
              }
              else
              {
                var.one[0] |= 0xfffff800;
              }
            }
            else
            {
              if(var.one[0] & 0x800)
              {
                var.one[0] |= 0xfffff000;
              }
            }
          }
          else
          {
            odd_even = 1;

            tmp1 = fgetc(inputfile);
            if(tmp1 == EOF)
            {
              goto END_1;
            }

            if(skipblocksize)
            {
              if(++skipblockcntr >= skipblocksize)
              {
                for(r=0; r<skipbytes; r++)
                {
                  tmp = fgetc(inputfile);
                  if(tmp == EOF)
                  {
                    goto END_1;
                  }
                }

                skipblockcntr = 0;
              }
            }

            tmp2 = fgetc(inputfile);
            if(tmp2 == EOF)
            {
              goto END_1;
            }

            if(skipblocksize)
            {
              if(++skipblockcntr >= skipblocksize)
              {
                for(r=0; r<skipbytes; r++)
                {
                  tmp = fgetc(inputfile);
                  if(tmp == EOF)
                  {
                    goto END_1;
                  }
                }

                skipblockcntr = 0;
              }
            }

            var.one[0] = (tmp2 & 0x0f) << 8;
            var.one[0] += tmp1;
            if(straightbinary)
            {
              if(var.one[0] & 0x800)
              {
                var.one[0] &= 0x7ff;
              }
              else
              {
                var.one[0] |= 0xfffff800;
              }
            }
            else
            {
              if(var.one[0] & 0x800)
              {
                var.one[0] |= 0xfffff000;
              }
            }
          }

          buf[j + (k * sf)] = var.two[0];
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        QApplication::restoreOverrideCursor();
        raw2edfDialog->setEnabled(true);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Write error during conversion.\nedf_blockwrite_digital_samples()");
        messagewindow.exec();
        edfclose_file(hdl);
        fclose(inputfile);
        free(buf);
        return;
      }

      datarecords++;
    }
  }

END_1:

  edfclose_file(hdl);
  fclose(inputfile);
  free(buf);

  QApplication::restoreOverrideCursor();
  raw2edfDialog->setEnabled(true);

  snprintf(str1_1024, 1024, "A new EDF file has been created:\n\n%.980s", path);
  QMessageBox messagewindow(QMessageBox::Information, "Ready", str1_1024);
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();
}



void UI_RAW2EDFapp::savebuttonpressed()
{
  char path[MAX_PATH_LENGTH]={""},
       str1_128[128]={""};

  FILE *outputfile=NULL;


  raw2edf_var->sf = SamplefreqSpinbox->value();

  raw2edf_var->chns = SignalsSpinbox->value();

  raw2edf_var->phys_max = PhysicalMaximumSpinbox->value();

  raw2edf_var->straightbinary = EncodingCombobox->currentIndex();

  raw2edf_var->endianness = EndiannessCombobox->currentIndex();

  raw2edf_var->samplesize = SampleSizeCombobox->currentIndex();

  raw2edf_var->offset = OffsetSpinbox->value();

  raw2edf_var->skipblocksize = skipblocksizeSpinbox->value();

  raw2edf_var->skipbytes = skipbytesSpinbox->value();

  strlcpy(raw2edf_var->phys_dim, PhysicalDimensionLineEdit->text().toLatin1().data(), 16);
  trim_spaces(raw2edf_var->phys_dim);

  path[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strlcpy(path, recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  strlcat(path, "binary_to_edf.template", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Save parameters", QString::fromLocal8Bit(path), "Template files (*.template *.TEMPLATE)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopeno(path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for writing.");
    messagewindow.exec();
    return;
  }

  fprintf(outputfile, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_raw2edf_template>\n");

  fprintf(outputfile, "  <sf>%i</sf>\n", raw2edf_var->sf);

  fprintf(outputfile, "  <chns>%i</chns>\n", raw2edf_var->chns);

  fprintf(outputfile, "  <phys_max>%i</phys_max>\n", raw2edf_var->phys_max);

  fprintf(outputfile, "  <straightbinary>%i</straightbinary>\n", raw2edf_var->straightbinary);

  fprintf(outputfile, "  <endianness>%i</endianness>\n", raw2edf_var->endianness);

  fprintf(outputfile, "  <samplesize>%i</samplesize>\n", raw2edf_var->samplesize);

  fprintf(outputfile, "  <offset>%i</offset>\n", raw2edf_var->offset);

  fprintf(outputfile, "  <skipblocksize>%i</skipblocksize>\n", raw2edf_var->skipblocksize);

  fprintf(outputfile, "  <skipbytes>%i</skipbytes>\n", raw2edf_var->skipbytes);

  xml_strlcpy_encode_entity(str1_128, raw2edf_var->phys_dim, 128);

  fprintf(outputfile, "  <phys_dim>%s</phys_dim>\n", str1_128);

  fprintf(outputfile, "</" PROGRAM_NAME "_raw2edf_template>\n");

  fclose(outputfile);
}


void UI_RAW2EDFapp::loadbuttonpressed()
{

  char path[MAX_PATH_LENGTH]={""},
       result[XML_STRBUFLEN]={""};

  xml_hdl_t *xml_hdl=NULL;

  strlcpy(path, QFileDialog::getOpenFileName(0, "Load parameters", QString::fromLocal8Bit(recent_opendir), "Template files (*.template *.TEMPLATE);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);
  if(xml_hdl==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_raw2edf_template"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this template.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return;
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "sf", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->sf = atoi(result);
    if(raw2edf_var->sf < 1)  raw2edf_var->sf = 1;
    if(raw2edf_var->sf > 1000000)  raw2edf_var->sf = 1000000;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "chns", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->chns = atoi(result);
    if(raw2edf_var->chns < 1)  raw2edf_var->chns = 1;
    if(raw2edf_var->chns > 256)  raw2edf_var->chns = 256;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "phys_max", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->phys_max = atoi(result);
    if(raw2edf_var->phys_max < 1)  raw2edf_var->phys_max = 1;
    if(raw2edf_var->phys_max > 10000000)  raw2edf_var->phys_max = 10000000;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "straightbinary", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->straightbinary = atoi(result);
    if(raw2edf_var->straightbinary < 0)  raw2edf_var->straightbinary = 0;
    if(raw2edf_var->straightbinary > 1)  raw2edf_var->straightbinary = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "endianness", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->endianness = atoi(result);
    if(raw2edf_var->endianness < 0)  raw2edf_var->endianness = 0;
    if(raw2edf_var->endianness > 1)  raw2edf_var->endianness = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "samplesize", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->samplesize = atoi(result);
    if(raw2edf_var->samplesize < 0)  raw2edf_var->samplesize = 0;
    if(raw2edf_var->samplesize > 2)  raw2edf_var->samplesize = 2;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "offset", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->offset = atoi(result);
    if(raw2edf_var->offset < 0)  raw2edf_var->offset = 0;
    if(raw2edf_var->offset > 1000000)  raw2edf_var->offset = 1000000;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "skipblocksize", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->skipblocksize = atoi(result);
    if(raw2edf_var->skipblocksize < 0)  raw2edf_var->skipblocksize = 0;
    if(raw2edf_var->skipblocksize > 1000000)  raw2edf_var->skipblocksize = 1000000;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "skipbytes", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    raw2edf_var->skipbytes = atoi(result);
    if(raw2edf_var->skipbytes < 1)  raw2edf_var->skipbytes = 1;
    if(raw2edf_var->skipbytes > 1000000)  raw2edf_var->skipbytes = 1000000;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "phys_dim", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(raw2edf_var->phys_dim, result, 16);
    latin1_to_ascii(raw2edf_var->phys_dim, strlen(raw2edf_var->phys_dim));
    trim_spaces(raw2edf_var->phys_dim);

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

  SamplefreqSpinbox->setValue(raw2edf_var->sf);

  SignalsSpinbox->setValue(raw2edf_var->chns);

  PhysicalMaximumSpinbox->setValue(raw2edf_var->phys_max);

  EncodingCombobox->setCurrentIndex(raw2edf_var->straightbinary);

  EndiannessCombobox->setCurrentIndex(raw2edf_var->endianness);

  SampleSizeCombobox->setCurrentIndex(raw2edf_var->samplesize);

  OffsetSpinbox->setValue(raw2edf_var->offset);

  skipblocksizeSpinbox->setValue(raw2edf_var->skipblocksize);

  skipbytesSpinbox->setValue(raw2edf_var->skipbytes);
}


void UI_RAW2EDFapp::sampleTypeChanged(int)
{
  if(SampleSizeCombobox->currentIndex() == 0)
  {
    if(EncodingCombobox->currentIndex() == 0)
    {
      variableTypeLineEdit->setText("I12");
    }
    else
    {
      variableTypeLineEdit->setText("U12");
    }

    EndiannessCombobox->setEnabled(false);
  }
  else if(SampleSizeCombobox->currentIndex() == 1)
    {
      if(EncodingCombobox->currentIndex() == 0)
      {
        variableTypeLineEdit->setText("I8");
      }
      else
      {
        variableTypeLineEdit->setText("U8");
      }

      EndiannessCombobox->setEnabled(false);
    }
    else if(SampleSizeCombobox->currentIndex() == 2)
      {
        if(EncodingCombobox->currentIndex() == 0)
        {
          variableTypeLineEdit->setText("I16");
        }
        else
        {
          variableTypeLineEdit->setText("U16");
        }

        EndiannessCombobox->setEnabled(true);
      }
}


void UI_RAW2EDFapp::PhysicalDimensionLineEdited(QString qstr)
{
  qstr.prepend(" ");

  PhysicalMaximumSpinbox->setSuffix(qstr);
}


void UI_RAW2EDFapp::helpbuttonpressed()
{
  mainwindow->open_manual("#Binary_to_EDF");
}











