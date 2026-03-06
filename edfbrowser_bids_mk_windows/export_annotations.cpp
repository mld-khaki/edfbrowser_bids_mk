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



#include "export_annotations.h"


#define ANNOT_EXPORT_FMT_EDF                       (0)
#define ANNOT_EXPORT_FMT_CSV_SEC_REL               (1)
#define ANNOT_EXPORT_FMT_CSV_ISO_TIME              (2)
#define ANNOT_EXPORT_FMT_CSV_ISO_DATE_TIME         (3)
#define ANNOT_EXPORT_FMT_CSV_ISO_TIME_SUBSEC       (4)
#define ANNOT_EXPORT_FMT_CSV_ISO_DATE_TIME_SUBSEC  (5)
#define ANNOT_EXPORT_FMT_XML                       (8)



UI_ExportAnnotationswindow::UI_ExportAnnotationswindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  ExportAnnotsDialog = new QDialog;

  ExportAnnotsDialog->setMinimumSize(600 * mainwindow->w_scaling, 550 * mainwindow->h_scaling);
  ExportAnnotsDialog->setWindowTitle("Export annotations");
  ExportAnnotsDialog->setModal(true);
  ExportAnnotsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  ExportAnnotsDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  formatGroupBox = new QGroupBox("output format");

  CSVRadioButton = new QRadioButton("ASCII/CSV");

  EDFplusRadioButton = new QRadioButton("EDFplus");

  XMLRadioButton = new QRadioButton("XML");

  QVBoxLayout *formatVBoxLayout = new QVBoxLayout;
  formatVBoxLayout->addWidget(CSVRadioButton);
  formatVBoxLayout->addWidget(EDFplusRadioButton);
  formatVBoxLayout->addWidget(XMLRadioButton);
  formatGroupBox->setLayout(formatVBoxLayout);

  fileGroupBox = new QGroupBox("export annotations");

  selectRadioButton = new QRadioButton("from selected file");
  selectRadioButton->setChecked(true);

  mergeRadioButton = new QRadioButton("from all files (merge)");

  QVBoxLayout *fileVBoxLayout = new QVBoxLayout;
  fileVBoxLayout->addWidget(selectRadioButton);
  fileVBoxLayout->addWidget(mergeRadioButton);
  fileGroupBox->setLayout(fileVBoxLayout);

  asciiSettingsGroupBox = new QGroupBox("ASCII / CSV settings");

  separatorBox = new QComboBox;
  separatorBox->addItem("comma");
  separatorBox->addItem("tab");
  separatorBox->addItem("semicolon");
  separatorBox->addItem("pipe (vertical bar)");

  asciiSecondsRadioButton = new QRadioButton("Seconds relative to start of file");
  asciiSecondsRadioButton->setChecked(true);

  asciiISOtimeRadioButton = new QRadioButton("ISO timestamp hh:mm:ss");

  asciiISOtimedateRadioButton = new QRadioButton("ISO datetimestamp yyyy-mm-ddThh:mm:ss");

  asciiISOtimeFractionRadioButton = new QRadioButton("ISO timestamp hh:mm:ss.xxx");

  asciiISOtimedateFractionRadioButton = new QRadioButton("ISO datetimestamp yyyy-mm-ddThh:mm:ss.xxx");

  durationCheckBox = new QCheckBox("Include event duration");
  durationCheckBox->setTristate(false);
  durationCheckBox->setCheckState(Qt::Unchecked);

  text_encoding_combobox = new QComboBox;
  text_encoding_combobox->addItem("UTF-8");
  text_encoding_combobox->addItem("ISO-8859-1 (Latin-1)");

  QFormLayout *asciiSettingsflayout = new QFormLayout;
  asciiSettingsflayout->addRow("Separator:", separatorBox);
  asciiSettingsflayout->addRow("Text encoding", text_encoding_combobox);

  QVBoxLayout *asciiSettingsVBoxLayout = new QVBoxLayout;
  asciiSettingsVBoxLayout->addLayout(asciiSettingsflayout);
  asciiSettingsVBoxLayout->addWidget(asciiSecondsRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(durationCheckBox);
  asciiSettingsGroupBox->setLayout(asciiSettingsVBoxLayout);

  ExportButton = new QPushButton;
  ExportButton->setText("Export");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  separatorBox->setCurrentIndex(mainwindow->export_annotations_var->separator);

  text_encoding_combobox->setCurrentIndex(mainwindow->export_annotations_var->txt_encoding);

  switch(mainwindow->export_annotations_var->format)
  {
    case 0 : EDFplusRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 8 : XMLRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 1 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiSecondsRadioButton->setChecked(true);
             break;

    case 2 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeRadioButton->setChecked(true);
             break;

    case 3 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateRadioButton->setChecked(true);
             break;

    case 4 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeFractionRadioButton->setChecked(true);
             break;

    case 5 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateFractionRadioButton->setChecked(true);
             break;
  }

  if(mainwindow->export_annotations_var->duration)
  {
    durationCheckBox->setChecked(true);
  }
  else
  {
    durationCheckBox->setChecked(false);
  }

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(ExportButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(fileGroupBox);
  vlayout3->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(formatGroupBox);
  hlayout2->addStretch(300);
  hlayout2->addLayout(vlayout3);
  hlayout2->addStretch(1000);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addStretch(1000);
  vlayout2->addLayout(hlayout1);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(asciiSettingsGroupBox);
  hlayout3->addLayout(vlayout2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout2);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout3);

  ExportAnnotsDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,    SIGNAL(clicked()),     ExportAnnotsDialog, SLOT(close()));
  QObject::connect(ExportButton,   SIGNAL(clicked()),     this,               SLOT(ExportButtonClicked()));
  QObject::connect(CSVRadioButton, SIGNAL(toggled(bool)), this,               SLOT(outputformatRadioButtonClicked(bool)));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  ExportAnnotsDialog->exec();
}


void UI_ExportAnnotationswindow::outputformatRadioButtonClicked(bool checked)
{
  if(checked == true)
  {
    asciiSettingsGroupBox->setEnabled(true);
  }
  else
  {
    asciiSettingsGroupBox->setEnabled(false);
  }
}



void UI_ExportAnnotationswindow::ExportButtonClicked()
{
  int i, j, n,
      len,
      csv_format=0,
      hdl=-1,
      annot_cnt,
      temp,
      include_duration,
      txt_encoding=0;

  char path[MAX_PATH_LENGTH]={""},
       str1_1024[1024]={""},
       separator=';';

  FILE *annotationfile=NULL;

  annotlist_t *annot_list;

  annotblck_t *annot;

  edfhdrblck_t *hdr;

  date_time_t tm;


  if(CSVRadioButton->isChecked() == true)
  {
    if(asciiSecondsRadioButton->isChecked() == true)
    {
      csv_format = 1;
    }

    if(asciiISOtimeRadioButton->isChecked() == true)
    {
      csv_format = 2;
    }

    if(asciiISOtimedateRadioButton->isChecked() == true)
    {
      csv_format = 3;
    }

    if(asciiISOtimeFractionRadioButton->isChecked() == true)
    {
      csv_format = 4;
    }

    if(asciiISOtimedateFractionRadioButton->isChecked() == true)
    {
      csv_format = 5;
    }
  }

  if(EDFplusRadioButton->isChecked() == true)
  {
    csv_format = 0;
  }

  if(XMLRadioButton->isChecked() == true)
  {
    csv_format = 8;
  }

  mainwindow->export_annotations_var->format = csv_format;

  mainwindow->export_annotations_var->separator = separatorBox->currentIndex();

  mainwindow->export_annotations_var->txt_encoding = text_encoding_combobox->currentIndex();

  if(mainwindow->export_annotations_var->separator == 0)
  {
    separator = ',';
  }
  else if(mainwindow->export_annotations_var->separator == 1)
  {
    separator = '\t';
  }
  else if(mainwindow->export_annotations_var->separator == 2)
  {
    separator = ';';
  }
  else
  {
    separator = '|';
  }

  txt_encoding = text_encoding_combobox->currentIndex();

  if(durationCheckBox->checkState() == Qt::Checked)
  {
    include_duration = 1;
    mainwindow->export_annotations_var->duration = 1;
  }
  else
  {
    include_duration = 0;
    mainwindow->export_annotations_var->duration = 0;
  }

  if(!mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  if(filelist->count() < 1)
  {
    ExportAnnotsDialog->close();
    return;
  }

  ExportButton->setEnabled(false);
  CloseButton->setEnabled(false);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  if(i==mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  n = i;

  if(mergeRadioButton->isChecked() == true)
  {
    n = mainwindow->sel_viewtime;
  }

  hdr = mainwindow->edfheaderlist[n];

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[n]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  if((csv_format > 0) && (csv_format < 6))
  {
    strlcat(path, "_annotations.txt", MAX_PATH_LENGTH);

    strlcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT *.csv *.CSV)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(csv_format == 8)
  {
    strlcat(path, "_annotations.xml", MAX_PATH_LENGTH);

    strlcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "XML files (*.xml *.XML)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(csv_format == 0)
  {
    strlcat(path, "_annotations.edf", MAX_PATH_LENGTH);

    strlcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(path, ""))
  {
    ExportAnnotsDialog->close();
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox::critical(ExportAnnotsDialog, "Error", "Selected file is in use", QMessageBox::Close);
    return;
  }

  annot_list = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[n]->annot_list);

  if(mergeRadioButton->isChecked() == true)
  {
    for(i=0; i < mainwindow->files_open; i++)
    {
      if(i != mainwindow->sel_viewtime)
      {
        annot_cnt = edfplus_annotation_size(&mainwindow->edfheaderlist[i]->annot_list);

        for(j=0; j < annot_cnt; j++)
        {
          edfplus_annotation_add_item(annot_list, *(edfplus_annotation_get_item(&mainwindow->edfheaderlist[i]->annot_list, j)));

          annot = edfplus_annotation_get_item(annot_list, edfplus_annotation_size(annot_list) - 1);

          annot->onset -= (mainwindow->edfheaderlist[i]->viewtime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime);
        }
      }
    }

    edfplus_annotation_sort(annot_list, NULL);
  }

  annot_cnt = edfplus_annotation_size(annot_list);

///////////////////////////// CSV (text) export //////////////////////////////////////

  if((csv_format > ANNOT_EXPORT_FMT_EDF) && (csv_format < ANNOT_EXPORT_FMT_XML))
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox::critical(ExportAnnotsDialog, "Error", "Cannot open annotation file for writing", QMessageBox::Close);
      ExportAnnotsDialog->close();
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      return;
    }

    if(include_duration)
    {
      fprintf(annotationfile, "Onset%cDuration%cAnnotation\n", separator, separator);
    }
    else
    {
      fprintf(annotationfile, "Onset%cAnnotation\n", separator);
    }

    for(j=0; j<annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }
      strlcpy(str1_1024, annot->description, 1024);
      if(txt_encoding)
      {
        utf8_to_latin1(str1_1024);
      }
      str_replace_ctrl_chars(str1_1024, '.');

      len = strlen(str1_1024);
      for(i=0; i<len; i++)
      {
        if(str1_1024[i] == separator)
        {
          str1_1024[i] = '.';
        }
      }

      if(csv_format == ANNOT_EXPORT_FMT_CSV_SEC_REL)
      {
        if(include_duration)
        {
          fprintf(annotationfile, "%+.7f%c%s%c%s\n", ((double)annot->onset) / TIME_FIXP_SCALING, separator, annot->duration, separator, str1_1024);
        }
        else
        {
          fprintf(annotationfile, "%+.7f%c%s\n", ((double)annot->onset) / TIME_FIXP_SCALING, separator, str1_1024);
        }
      }

      if(csv_format == ANNOT_EXPORT_FMT_CSV_ISO_TIME)
      {
        temp = annot->onset % TIME_FIXP_SCALING;

        if(temp < 0)
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING - 1, &tm);
        }
        else
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING, &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%02i:%02i:%02i%c%s%c%s\n", tm.hour, tm.minute, tm.second, separator, annot->duration, separator, str1_1024);
        }
        else
        {
          fprintf(annotationfile, "%02i:%02i:%02i%c%s\n", tm.hour, tm.minute, tm.second, separator, str1_1024);
        }
      }

      if(csv_format == ANNOT_EXPORT_FMT_CSV_ISO_DATE_TIME)
      {
        temp = annot->onset % TIME_FIXP_SCALING;

        if(temp < 0)
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING - 1, &tm);
        }
        else
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING, &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i%c%s%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, separator, annot->duration, separator, str1_1024);
        }
        else
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, separator, str1_1024);
        }
      }

      if(csv_format == ANNOT_EXPORT_FMT_CSV_ISO_TIME_SUBSEC)
      {
        temp = (hdr->utc_starttime_hr + annot->onset) % TIME_FIXP_SCALING;

        if(temp < 0)
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING - 1, &tm);

          temp += TIME_FIXP_SCALING;
        }
        else
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING, &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%02i:%02i:%02i.%07i%c%s%c%s\n", tm.hour, tm.minute, tm.second, temp, separator, annot->duration, separator, str1_1024);
        }
        else
        {
          fprintf(annotationfile, "%02i:%02i:%02i.%07i%c%s\n", tm.hour, tm.minute, tm.second, temp, separator, str1_1024);
        }
      }

      if(csv_format == ANNOT_EXPORT_FMT_CSV_ISO_DATE_TIME_SUBSEC)
      {
        temp = (hdr->utc_starttime_hr + annot->onset) % TIME_FIXP_SCALING;

        if(temp < 0)
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING - 1, &tm);

          temp += TIME_FIXP_SCALING;
        }
        else
        {
          utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING, &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i%c%s%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, separator, annot->duration, separator, str1_1024);
        }
        else
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, separator, str1_1024);
        }
      }
    }

    fclose(annotationfile);

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// XML export //////////////////////////////////////

  if(csv_format == ANNOT_EXPORT_FMT_XML)
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox::critical(ExportAnnotsDialog, "Error", "Cannot open annotation file for writing", QMessageBox::Close);
      ExportAnnotsDialog->close();
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      return;
    }

    fprintf(annotationfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    fprintf(annotationfile, "<!-- Generated by " PROGRAM_NAME " " PROGRAM_VERSION " -->\n");

    fprintf(annotationfile, "<annotationlist>\n");

    utc_to_date_time(hdr->utc_starttime, &tm);

    fprintf(annotationfile, " <recording_start_time>%04i-%02i-%02iT%02i:%02i:%02i.%07i</recording_start_time>\n",
                            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, (int)hdr->starttime_subsec);

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }
      temp = (hdr->utc_starttime_hr + annot->onset) % TIME_FIXP_SCALING;

      if(temp < 0)
      {
        utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING - 1, &tm);

        temp += TIME_FIXP_SCALING;
      }
      else
      {
        utc_to_date_time((hdr->utc_starttime_hr + annot->onset) / TIME_FIXP_SCALING, &tm);
      }

      fprintf(annotationfile, " <annotation>\n"
                              "  <onset>%04i-%02i-%02iT%02i:%02i:%02i.%07i</onset>\n"
                              "  <duration>%s</duration>\n"
                              "  <description>",
                              tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, annot->duration);

      xml_fwrite_encode_entity(annotationfile, annot->description);

      fprintf(annotationfile,                     "</description>\n"
                              " </annotation>\n");
    }

    fprintf(annotationfile, "</annotationlist>\n");

    fclose(annotationfile);

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// EDFplus export //////////////////////////////////////

  if(csv_format == ANNOT_EXPORT_FMT_EDF)
  {
    hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, 0);
    if(hdl < 0)
    {
      switch(hdl)
      {
        case EDFLIB_MALLOC_ERROR : strlcpy(str1_1024, "EDFlib: malloc error", 1024);
                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY : strlcpy(str1_1024, "EDFlib: no such file or directory", 1024);
                                  break;
        case EDFLIB_MAXFILES_REACHED : strlcpy(str1_1024, "EDFlib: maximum files reached", 1024);
                                  break;
        case EDFLIB_FILE_ALREADY_OPENED : strlcpy(str1_1024, "EDFlib: file already opened", 1024);
                                  break;
        case EDFLIB_NUMBER_OF_SIGNALS_INVALID : strlcpy(str1_1024, "EDFlib: number of signals is invalid", 1024);
                                  break;
        default : strlcpy(str1_1024, "EDFlib: unknown error", 1024);
                                  break;
      }

      QMessageBox::critical(ExportAnnotsDialog, "Error", str1_1024, QMessageBox::Close);
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      ExportAnnotsDialog->close();
      return;
    }

    utc_to_date_time(hdr->utc_starttime, &tm);
    edf_set_startdatetime(hdl, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    if((hdr->edfplus) || (hdr->bdfplus))
    {
      edf_set_patientname(hdl, hdr->plus_patient_name);
      edf_set_patientcode(hdl, hdr->plus_patientcode);
      if(!(strcmp(hdr->plus_sex, "Male")))
      {
        edf_set_sex(hdl, 1);
      }
      if(!(strcmp(hdr->plus_sex, "Female")))
      {
        edf_set_sex(hdl, 0);
      }
      if(hdr->plus_birthdate[0] != 0)
      {
        if(!(strncmp(hdr->plus_birthdate + 3, "jan", 3)))  n = 1;
        if(!(strncmp(hdr->plus_birthdate + 3, "feb", 3)))  n = 2;
        if(!(strncmp(hdr->plus_birthdate + 3, "mar", 3)))  n = 3;
        if(!(strncmp(hdr->plus_birthdate + 3, "apr", 3)))  n = 4;
        if(!(strncmp(hdr->plus_birthdate + 3, "may", 3)))  n = 5;
        if(!(strncmp(hdr->plus_birthdate + 3, "jun", 3)))  n = 6;
        if(!(strncmp(hdr->plus_birthdate + 3, "jul", 3)))  n = 7;
        if(!(strncmp(hdr->plus_birthdate + 3, "aug", 3)))  n = 8;
        if(!(strncmp(hdr->plus_birthdate + 3, "sep", 3)))  n = 9;
        if(!(strncmp(hdr->plus_birthdate + 3, "oct", 3)))  n = 10;
        if(!(strncmp(hdr->plus_birthdate + 3, "nov", 3)))  n = 11;
        if(!(strncmp(hdr->plus_birthdate + 3, "dec", 3)))  n = 12;
        edf_set_birthdate(hdl, atoi(hdr->plus_birthdate + 7), n, atoi(hdr->plus_birthdate));
      }
      edf_set_patient_additional(hdl, hdr->plus_patient_additional);
      edf_set_admincode(hdl, hdr->plus_admincode);
      edf_set_technician(hdl, hdr->plus_technician);
      edf_set_equipment(hdl, hdr->plus_equipment);
      edf_set_recording_additional(hdl, hdr->plus_recording_additional);
    }
    else
    {
      edf_set_patientname(hdl, hdr->patient);
      edf_set_recording_additional(hdl, hdr->recording);
    }

    for(j=0; j<annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      if(annot->long_duration <= 0LL)
      {
        edfwrite_annotation_utf8_hr(hdl, annot->onset / 10LL, -1LL, annot->description);
      }
      else
      {
        edfwrite_annotation_utf8_hr(hdl, annot->onset / 10LL, annot->long_duration / 10LL, annot->description);
      }
    }

    if(edfclose_file(hdl) != 0)
    {
      QMessageBox::critical(ExportAnnotsDialog, "Error", "An error occurred: edfclose_file()", QMessageBox::Close);
    }

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();
  }
}















