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



#include "bdf_triggers.h"



int BDF_triggers::get_triggers(edfhdrblck_t *hdr)
{
  int i, j,
      sf,
      status_signal=0,
      edfsignals,
      bufsize,
      jump_file,
      trigger_cnt,
      status[24];

  char *buf=NULL;

  long long datarecords,
            long_data_record_duration,
            records_read,
            progress_steps;

  double status_sample_duration;

  FILE *inputfile=NULL;

  annotlist_t *annot_list = &hdr->annot_list;

  annotblck_t annotation;

  memset(&annotation, 0, sizeof(annotblck_t));

  inputfile = hdr->file_hdl;
  edfsignals = hdr->edfsignals;
  datarecords = hdr->datarecords;
  long_data_record_duration = hdr->long_data_record_duration;

  if(edfsignals < 1)
  {
    return 0;
  }

  if(long_data_record_duration != TIME_FIXP_SCALING)
  {
    return 0;
  }

  if(strcmp(hdr->reserved, "24BIT                                       "))
  {
    return 0;
  }

  sf = hdr->edfparam[0].smp_per_record;

  for(i=1; i<edfsignals; i++)
  {
    if(hdr->edfparam[i].smp_per_record != sf)
    {
      return 0;
    }
  }

  for(i=0; i<edfsignals; i++)
  {
    if(hdr->edfparam[i].dig_max != 8388607)
    {
      return 0;
    }

    if(hdr->edfparam[i].dig_min != -8388608)
    {
      return 0;
    }
  }

  for(i=32; ; i*=2)
  {
    if(i > 262144)
    {
      return 0;
    }

    if(i == sf)
    {
      break;
    }
  }

  status_signal = edfsignals - 1;

  if(strcmp(hdr->edfparam[status_signal].label, "Status          ") ||
     strcmp(hdr->edfparam[status_signal].physdimension, "Boolean ") ||
     strcmp(hdr->edfparam[status_signal].prefilter, "No filtering                                                                    ") ||
     strcmp(hdr->edfparam[status_signal].transducer, "Triggers and Status                                                             ") ||
     (hdr->edfparam[status_signal].dig_max != 8388607) ||
     (hdr->edfparam[status_signal].dig_min != -8388608) ||
     (dblcmp(hdr->edfparam[status_signal].phys_max, 8388607)) ||
     (dblcmp(hdr->edfparam[status_signal].phys_min, -8388608)))
  {
    return 0;
  }

  bufsize = sf * 3;

  buf = (char *)calloc(1, bufsize);
  if(buf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read triggers.\n(buf)");
    messagewindow.exec();
    return 1;
  }

  if(fseeko(inputfile, ((long long)edfsignals + 1LL) * 256LL, SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile triggers.");
    messagewindow.exec();
    free(buf);
    return 2;
  }

  jump_file = status_signal * sf * 3;

  status_sample_duration = (double)TIME_FIXP_SCALING / (double)sf;

  trigger_cnt = 0;

  for(i=0; i<24; i++)
  {
    status[i] = 1;
  }

  QProgressDialog progress("Scanning file for triggers...", "Abort", 0, datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100LL;
  if(progress_steps < 1LL)
  {
    progress_steps = 1LL;
  }

  for(records_read=0LL; records_read<datarecords; records_read++)
  {
    if(!(records_read%progress_steps))
    {
      progress.setValue((int)records_read);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    if(trigger_cnt >= 100000)
    {
      break;
    }

    if(fseeko(inputfile, (long long)jump_file, SEEK_CUR))
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile triggers.");
      messagewindow.exec();
      free(buf);
      return 2;
    }

    if(fread(buf, bufsize, 1, inputfile)!=1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile triggers.");
      messagewindow.exec();
      free(buf);
      return 2;
    }

    for(i=0; i<bufsize; i+=3)
    {
      if(buf[i + 2] & 1)
      {
        if(!status[16])  // rising edge detected
        {
          annotation.edfhdr = hdr;
          annotation.onset = (records_read * TIME_FIXP_SCALING) + ((double)(i / 3) * status_sample_duration) + 0.5;
          strlcpy(annotation.description, "new epoch", MAX_ANNOTATION_LEN_II + 1);
          annotation.ident = (1 << ANNOT_ID_BS_TRIGGER);
          if(edfplus_annotation_add_item(annot_list, annotation))
          {
            progress.reset();
            QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (annotation).");
            messagewindow.exec();
            free(buf);
            return 1;
          }

          trigger_cnt++;

          status[16] = 1;
        }
      }
      else
      {
        if(status[16])  // falling edge detected
        {
          status[16] = 0;
        }
      }

      for(j=0; j<16; j++)
      {
        if(*((unsigned short *)(buf + i)) & (1 << j))
        {
          if(!status[j])  // rising edge detected
          {
            annotation.edfhdr = hdr;
            annotation.onset = (records_read * TIME_FIXP_SCALING) + ((double)(i / 3) * status_sample_duration) + 0.5;
            snprintf(annotation.description, MAX_ANNOTATION_LEN_II + 1, "Trigger Input %i", j + 1);
            annotation.ident = (1 << ANNOT_ID_BS_TRIGGER);
            if(edfplus_annotation_add_item(annot_list, annotation))
            {
              progress.reset();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (annotation).");
              messagewindow.exec();
              free(buf);
              return 1;
            }

            trigger_cnt++;

            status[j] = 1;
          }
        }
        else
        {
          if(status[j])  // falling edge detected
          {
            status[j] = 0;
          }
        }
      }
    }
  }

  progress.reset();

  free(buf);

  hdr->genuine_biosemi = 1;

  return 0;
}















