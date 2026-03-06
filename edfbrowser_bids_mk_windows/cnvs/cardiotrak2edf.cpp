/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2023 - 2024 Teunis van Beelen
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



#include "cardiotrak2edf.h"


#define MAX_SIGNALS    (32)


UI_CardiotrakEDFwindow::UI_CardiotrakEDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  ct2edf_dialog = new QDialog;

  ct2edf_dialog->setMinimumSize(600 * mainwindow->w_scaling, 480 * mainwindow->h_scaling);
  ct2edf_dialog->setWindowTitle("CardioTrak EDF to EDF+ converter");
  ct2edf_dialog->setModal(true);
  ct2edf_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  logger = new QPlainTextEdit;
  logger->setReadOnly(true);
  logger->setLineWrapMode(QPlainTextEdit::NoWrap);
  logger_append_txt("CardioTrak EDF to EDF+ converter\n");

  select_file_button = new QPushButton;
  select_file_button->setText("Select File");

  close_button = new QPushButton;
  close_button->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(select_file_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(close_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(logger, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  ct2edf_dialog->setLayout(vlayout1);

  QObject::connect(select_file_button, SIGNAL(clicked()), this,          SLOT(select_file_button_pressed()));
  QObject::connect(close_button,       SIGNAL(clicked()), ct2edf_dialog, SLOT(close()));

  ct2edf_dialog->exec();
}


void UI_CardiotrakEDFwindow::logger_append_txt(const char *txt)
{
  logger->moveCursor(QTextCursor::End);
  logger->insertPlainText(txt);
  logger->ensureCursorVisible();
}


void UI_CardiotrakEDFwindow::select_file_button_pressed()
{
  int i, j, r,
      chns_in=0,
      chns_out=0,
      chns_map[MAX_SIGNALS],
      smpls_in[MAX_SIGNALS],
      smpls_out[MAX_SIGNALS],
      datrecs_in_file,
      hdl_in=-1,
      hdl_out=-1,
      *buf_in[MAX_SIGNALS],
      *buf_out[MAX_SIGNALS],
      pace_chn_idx,
      mrk_chn_idx;

  char path_in[MAX_PATH_LENGTH]="",
       path_out[MAX_PATH_LENGTH]="",
       str1_4096[4096]={""};

  edflib_hdr_t hdr;

  for(i=0; i<MAX_SIGNALS; i++)
  {
    buf_in[i] = NULL;
    buf_out[i] = NULL;
  }

  QProgressDialog progress("Converting...", "Abort", 0, 0, ct2edf_dialog);
                  progress.reset();

  strlcpy(path_in, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  if(!strcmp(path_in, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path_in, MAX_PATH_LENGTH);

  snprintf(str1_4096, 4096, "Converting file %s... ", path_in);
  logger_append_txt(str1_4096);

  if(edfopen_file_readonly(path_in, &hdr, EDFLIB_DO_NOT_READ_ANNOTATIONS))
  {
    switch(hdr.filetype)
    {
      case EDFLIB_MALLOC_ERROR                : logger_append_txt("error: EDFlib: malloc error\n\n");
                                                break;
      case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : logger_append_txt("error: EDFlib: cannot open file, no such file or directory\n\n");
                                                break;
      case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS : logger_append_txt("error: EDFlib: the file is not EDF(+) or BDF(+) compliant\n"
                                                                "(it contains format errors)\n\n");
                                                break;
      case EDFLIB_MAXFILES_REACHED            : logger_append_txt("error: EDFlib: too many files opened\n\n");
                                                break;
      case EDFLIB_FILE_READ_ERROR             : logger_append_txt("error: EDFlib: a read error occurred\n\n");
                                                break;
      case EDFLIB_FILE_ALREADY_OPENED         : logger_append_txt("error: EDFlib: file has already been opened\n\n");
                                                break;
      default                                 : logger_append_txt("error: EDFlib: unknown error\n\n");
                                                break;
    }

    goto OUT_ERROR_1;
  }

  hdl_in = hdr.handle;

  if(hdr.filetype == EDFLIB_FILETYPE_EDFPLUS)
  {
    logger_append_txt("error: file is already an EDFplus file\n\n");
    goto OUT_ERROR_1;
  }

  if(hdr.filetype == EDFLIB_FILETYPE_BDF)
  {
    logger_append_txt("error: file is a BDF file\n\n");
    goto OUT_ERROR_1;
  }

  chns_in = hdr.edfsignals;
  if(chns_in < 3)
  {
    logger_append_txt("error: file has less than 3 signals\n\n");
    goto OUT_ERROR_1;
  }

  datrecs_in_file = hdr.datarecords_in_file;
  if(datrecs_in_file < 1)
  {
    logger_append_txt("error: file does not contain any datarecords\n\n");
    goto OUT_ERROR_1;
  }

  for(i=0, pace_chn_idx=-1, mrk_chn_idx=-1; i<chns_in; i++)
  {
    if(!strcmp(hdr.signalparam[i].label, "Marker          "))
    {
      if(mrk_chn_idx != -1)
      {
        logger_append_txt("error: file has multiple signals with label \"Marker\"\n\n");
        goto OUT_ERROR_1;
      }

      mrk_chn_idx = i;
    }

    if(!strcmp(hdr.signalparam[i].label, "Pacemaker       "))
    {
      if(pace_chn_idx != -1)
      {
        logger_append_txt("error: file has multiple signals with label \"Pacemaker\"\n\n");
        goto OUT_ERROR_1;
      }

      pace_chn_idx = i;
    }
  }

  if(pace_chn_idx == -1)
  {
    logger_append_txt("error: file has no signal with label \"Pacemaker\"\n\n");
    goto OUT_ERROR_1;
  }

  if(mrk_chn_idx == -1)
  {
    logger_append_txt("error: file has no signal with label \"Marker\"\n\n");
    goto OUT_ERROR_1;
  }

  for(i=0; i<chns_in; i++)
  {
    smpls_in[i] = hdr.signalparam[i].smp_in_datarecord;
  }

  for(i=0, chns_out=0; i<chns_in; i++)
  {
    if(strcmp(hdr.signalparam[i].label, "Pacemaker       ") &&
       strcmp(hdr.signalparam[i].label, "Marker          "))
    {
      chns_map[chns_out++] = i;
    }
  }

  for(i=0; i<chns_out; i++)
  {
    smpls_out[i] = smpls_in[chns_map[i]];
  }

  strlcpy(path_out, path_in, MAX_PATH_LENGTH);
  remove_extension_from_filename(path_out);
  strlcat(path_out, "_plus.edf", MAX_PATH_LENGTH);

  strlcpy(path_out, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(path_out), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  if(!strcmp(path_out, ""))
  {
    goto OUT_ERROR_1;
  }

  get_directory_from_path(recent_savedir, path_out, MAX_PATH_LENGTH);

  if(strlen(path_out) < 5)
  {
    strlcat(path_out, ".edf", MAX_PATH_LENGTH);
  }
  else if(strcmp(path_out + strlen(path_out) - 4, ".edf"))
    {
      remove_extension_from_filename(path_out);
      strlcat(path_out, ".edf", MAX_PATH_LENGTH);
    }

  if(!strcmp(path_in, path_out))
  {
    logger_append_txt("error: output filename equals input filename\n\n");
    goto OUT_ERROR_1;
  }

  hdl_out = edfopen_file_writeonly(path_out, EDFLIB_FILETYPE_EDFPLUS, chns_out);
  if(hdl_out < 0)
  {
    logger_append_txt("error: cannot create output EDF file\n\n");
    goto OUT_ERROR_1;
  }

  if(edf_set_datarecord_duration(hdl_out, hdr.datarecord_duration / 100))
  {
    logger_append_txt("error: edf_set_datarecord_duration()\n\n");
    goto OUT_ERROR_1;
  }

  if(edf_set_number_of_annotation_signals(hdl_out, 2))
  {
    logger_append_txt("error: edf_set_number_of_annotation_signals()\n\n");
    goto OUT_ERROR_1;
  }

  if(edf_set_patient_additional(hdl_out, hdr.patient))
  {
    logger_append_txt("error: edf_set_patient_additional()\n\n");
    goto OUT_ERROR_1;
  }

  if(edf_set_recording_additional(hdl_out, hdr.recording))
  {
    logger_append_txt("error: edf_set_recording_additional()\n\n");
    goto OUT_ERROR_1;
  }

  if(edf_set_startdatetime(hdl_out, hdr.startdate_year, hdr.startdate_month, hdr.startdate_day, hdr.starttime_hour, hdr.starttime_minute, hdr.starttime_second))
  {
    logger_append_txt("error: edf_set_startdatetime()\n\n");
    goto OUT_ERROR_1;
  }

  for(i=0; i<chns_out; i++)
  {
    if(edf_set_samplefrequency(hdl_out, i, smpls_out[i]))
    {
      logger_append_txt("error: edf_set_samplefrequency()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_physical_maximum(hdl_out, i, hdr.signalparam[chns_map[i]].phys_max))
    {
      logger_append_txt("error: edf_set_physical_maximum()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_physical_minimum(hdl_out, i, hdr.signalparam[chns_map[i]].phys_min))
    {
      logger_append_txt("error: edf_set_physical_minimum()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_digital_maximum(hdl_out, i, hdr.signalparam[chns_map[i]].dig_max))
    {
      logger_append_txt("error: edf_set_digital_maximum()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_digital_minimum(hdl_out, i, hdr.signalparam[chns_map[i]].dig_min))
    {
      logger_append_txt("error: edf_set_digital_minimum()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_label(hdl_out, i, hdr.signalparam[chns_map[i]].label))
    {
      logger_append_txt("error: edf_set_label()\n\n");
      goto OUT_ERROR_1;
    }

    if(edf_set_physical_dimension(hdl_out, i, hdr.signalparam[chns_map[i]].physdimension))
    {
      logger_append_txt("error: edf_set_physical_dimension()\n\n");
      goto OUT_ERROR_1;
    }
  }

  for(i=0; i<chns_in; i++)
  {
    buf_in[i] = (int *)calloc(1, smpls_in[i] * sizeof(int));
    if(buf_in[i] == NULL)
    {
      logger_append_txt("error: calloc()\n\n");
      goto OUT_ERROR_1;
    }
  }

  for(i=0; i<chns_out; i++)
  {
    buf_out[i] = (int *)calloc(1, smpls_out[i] * sizeof(int));
    if(buf_out[i] == NULL)
    {
      logger_append_txt("error: calloc()\n\n");
      goto OUT_ERROR_1;
    }
  }

/*************************************************************************************/

  progress.setMaximum(datrecs_in_file);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  for(i=0; i<datrecs_in_file; i++)
  {
    if(!(i % 10))
    {
      progress.setValue(i);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        logger_append_txt("Conversion aborted by user.\n\n");
        goto OUT_ERROR_1;
      }
    }

    for(j=0; j<chns_in; j++)
    {
      if(edfread_digital_samples(hdl_in, j, smpls_in[j], buf_in[j]) != smpls_in[j])
      {
        logger_append_txt("error: edfread_digital_samples()\n\n");
        goto OUT_ERROR_1;
      }
    }

    for(j=0; j<chns_out; j++)
    {
      for(r=0; r<smpls_out[j]; r++)
      {
        buf_out[j][r] = buf_in[chns_map[j]][r];
      }

      if(edfwrite_digital_samples(hdl_out, buf_out[j]))
      {
        logger_append_txt("error: edfwrite_digital_samples()\n\n");
        goto OUT_ERROR_1;
      }
    }

    for(r=0; r<smpls_in[mrk_chn_idx]; r++)
    {
      if(buf_in[mrk_chn_idx][r] & (1 << 0))
      {
        if(edfwrite_annotation_utf8_hr(hdl_out, ((i * hdr.datarecord_duration) + (r * (hdr.datarecord_duration / smpls_in[mrk_chn_idx]))) / 10LL, -1, "User marker"))
        {
          logger_append_txt("error: edfwrite_annotation_utf8()\n\n");
          goto OUT_ERROR_1;
        }
      }
    }

    for(r=0; r<smpls_in[pace_chn_idx]; r++)
    {
      if(buf_in[pace_chn_idx][r] & (1 << 15))
      {
        if(edfwrite_annotation_utf8_hr(hdl_out, ((i * hdr.datarecord_duration) / 10LL) + ((buf_in[pace_chn_idx][r] & 0x3ff) * 1000LL), -1, "Pace"))
        {
          logger_append_txt("error: edfwrite_annotation_utf8()\n\n");
          goto OUT_ERROR_1;
        }
      }
      else
      {
        break;
      }
    }
  }

  logger_append_txt("done\n");

/*************************************************************************************/

OUT_ERROR_1:

  progress.reset();

  edfclose_file(hdl_in);
  edfclose_file(hdl_out);

  for(i=0; i<MAX_SIGNALS; i++)
  {
    free(buf_in[i]);
    free(buf_out[i]);
  }
}

























