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



#include "edf_annotations.h"



static void process_events(void)
{
  qApp->processEvents();
}


int EDF_annotations::get_annotations(edfhdrblck_t *edf_hdr, int read_nk_trigger_signal)
{
  int i, j, k, p, r=0, n,
      edfsignals,
      datarecords,
      recordsize,
      discontinuous,
      *annot_ch,
      nr_annot_chns,
      max,
      onset,
      duration,
      duration_start,
      zero,
      max_tal_ln,
      error,
      annots_in_record,
      annots_in_tal,
      samplesize=2,
      nk_triggers_smpls=0,
      nk_triggers_bufoffset=0,
      nk_triggers_enabled=0,
      nk_triggers_channel=0,
      nk_triggers_cnt=0,
      sf,
      progress_steps,
      annot_offset=0;

  unsigned short nk_triggerfields=0,
                 nk_old_triggerfields=0;

  char *scratchpad=NULL,
       *cnv_buf=NULL,
       *time_in_txt=NULL,
       *duration_in_txt=NULL,
       nk_triggerlabel[16][32]={0,};

  long long data_record_duration,
            elapsedtime,
            time_tmp=0LL;

  double nk_trigger_sample_duration=1;

  FILE *inputfile=NULL;

  edfpar_t *edfparam=NULL;

  annotblck_t annotblock;

  inputfile = edf_hdr->file_hdl;
  edfsignals = edf_hdr->edfsignals;
  recordsize = edf_hdr->recordsize;
  edfparam = edf_hdr->edfparam;
  nr_annot_chns = edf_hdr->nr_annot_chns;
  datarecords = edf_hdr->datarecords;
  data_record_duration = edf_hdr->long_data_record_duration;
  discontinuous = edf_hdr->discontinuous;
  annot_ch = edf_hdr->annot_ch;

  if(edf_hdr->edfplus)
  {
    samplesize = 2;
  }
  if(edf_hdr->bdfplus)
  {
    samplesize = 3;
  }

  if((edf_hdr->edfplus) && (read_nk_trigger_signal))
  {
    if(data_record_duration == 1000000LL)
    {
      if(check_device(edf_hdr->plus_equipment) == 0)
      {
        for(i=0; i<edfsignals; i++)
        {
          if(!strcmp(edfparam[i].label, "Events/Markers  "))
          {
            sf = edf_hdr->edfparam[i].smp_per_record;

            error = 1;

            switch(sf)
            {
              case  10 : error = 0;
                         break;
              case  20 : error = 0;
                         break;
              case  50 : error = 0;
                         break;
              case 100 : error = 0;
                         break;
            }

            for(j=0; j<edfsignals; j++)
            {
              if(edf_hdr->edfparam[j].smp_per_record != sf)
              {
                if(!edf_hdr->edfparam[j].annotation)
                {
                  error = 1;
                }
              }
            }

            if(edf_hdr->nr_annot_chns != 1)  error = 1;

            if(!error)
            {
              nk_triggers_channel = i;

              nk_triggers_bufoffset = edfparam[nk_triggers_channel].datrec_offset;

              nk_triggers_smpls = edfparam[nk_triggers_channel].smp_per_record;

              nk_trigger_sample_duration = data_record_duration / (double)nk_triggers_smpls;

              strlcpy(nk_triggerlabel[0], "CAL mode", 32);
              strlcpy(nk_triggerlabel[1], "RESET condition", 32);
              strlcpy(nk_triggerlabel[2], "External mark", 32);
              strlcpy(nk_triggerlabel[3], "Photo/HV mark", 32);
              strlcpy(nk_triggerlabel[4], "Remote mark", 32);
              strlcpy(nk_triggerlabel[5], "HV mark", 32);
              strlcpy(nk_triggerlabel[6], "DC trigger 9", 32);
              strlcpy(nk_triggerlabel[7], "DC trigger 10", 32);
              strlcpy(nk_triggerlabel[8], "DC trigger 11", 32);
              strlcpy(nk_triggerlabel[9], "DC trigger 12", 32);
              strlcpy(nk_triggerlabel[10], "DC trigger 13", 32);
              strlcpy(nk_triggerlabel[11], "DC trigger 14", 32);
              strlcpy(nk_triggerlabel[12], "DC trigger 15", 32);
              strlcpy(nk_triggerlabel[13], "DC trigger 16", 32);
              strlcpy(nk_triggerlabel[14], "", 32);
              strlcpy(nk_triggerlabel[15], "", 32);

              nk_triggers_enabled = 1;

              edf_hdr->genuine_nk = 1;

              break;
            }
          }
        }
      }
    }
  }

  cnv_buf = (char *)calloc(1, recordsize);
  if(cnv_buf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(cnv_buf)");
    messagewindow.exec();
    return 1;
  }

  max_tal_ln = 0;

  for(i=0; i<nr_annot_chns; i++)
  {
    if(max_tal_ln<edfparam[annot_ch[i]].smp_per_record * samplesize)  max_tal_ln = edfparam[annot_ch[i]].smp_per_record * samplesize;
  }

  if(max_tal_ln<128)  max_tal_ln = 128;

  scratchpad = (char *)calloc(1, max_tal_ln + 3);
  if(scratchpad==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(scratchpad)");
    messagewindow.exec();
    free(cnv_buf);
    return 1;
  }

  time_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(time_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(time_in_txt)");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    return 1;
  }

  duration_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(duration_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(duration_in_txt)");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    free(time_in_txt);
    return 1;
  }

  if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile annotations. (fseek())");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    free(time_in_txt);
    free(duration_in_txt);
    return 2;
  }

  QProgressDialog progress("Scanning file for annotations...", "Abort", 0, datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  elapsedtime = 0;

  for(i=0; i<datarecords; i++)
  {
    if(!(i%progress_steps))
    {
      progress.setValue(i);

      qApp->processEvents();

      if((progress.wasCanceled() == true) && i)  /* read at least the first datarecord so that we have the subsecond starttime */
      {
        edf_hdr->annots_not_read = 1;

        free(cnv_buf);
        free(scratchpad);
        free(time_in_txt);
        free(duration_in_txt);
        return 11;
      }
    }

    if(fread(cnv_buf, recordsize, 1, inputfile)!=1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile annotations. (fread())");
      messagewindow.exec();
      free(cnv_buf);
      free(scratchpad);
      free(time_in_txt);
      free(duration_in_txt);
      return 2;
    }


/************** process annotationsignals (if any) **************/

    error = 0;

    for(r=0; r<nr_annot_chns; r++)
    {
      n = 0;
      zero = 0;
      onset = 0;
      duration = 0;
      duration_start = 0;
      scratchpad[0] = 0;
      annots_in_tal = 0;
      annots_in_record = 0;
      annot_offset = 0;

      p = edfparam[annot_ch[r]].datrec_offset;
      max = edfparam[annot_ch[r]].smp_per_record * samplesize;

/************** process one annotation signal ****************/

      if(cnv_buf[p + max - 1]!=0)
      {
        error = 5;
        goto END;
      }

      if(!r)  /* if it's the first annotation signal, then check */
      {       /* the timekeeping annotation */
        error = 1;

        for(k=0; k<(max-2); k++)
        {
          scratchpad[k] = cnv_buf[p + k];

          if(scratchpad[k]==20)
          {
            if(cnv_buf[p + k + 1]!=20)
            {
              error = 6;
              goto END;
            }
            scratchpad[k] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            else
            {
              time_tmp = get_long_time(scratchpad);
              if(i)
              {
                if(discontinuous)
                {
                  if((time_tmp-elapsedtime)<data_record_duration)
                  {
                    error = 4;
                    goto END;
                  }
                }
                else
                {
                  if((time_tmp-elapsedtime)!=data_record_duration)
                  {
                    error = 3;
                    goto END;
                  }
                }
              }
              else  /* first datarecord, first annotation channel */
              {
                if(time_tmp>=TIME_FIXP_SCALING)
                {
                  error = 2;
                  goto END;
                }
                else
                {
                  edf_hdr->starttime_subsec = time_tmp;
                }
              }
              elapsedtime = time_tmp;
              error = 0;
              break;
            }
          }
        }
      }

      for(k=0; k<max; k++)
      {
        scratchpad[n] = cnv_buf[p + k];

        if(!scratchpad[n])
        {
          if(!zero)
          {
            if(k)
            {
              if(cnv_buf[p + k - 1]!=20)
              {
                error = 33;
                goto END;
              }
            }
            n = 0;
            onset = 0;
            duration = 0;
            duration_start = 0;
            scratchpad[0] = 0;
            annots_in_tal = 0;
            annot_offset = k + 1;
          }
          zero++;
          continue;
        }
        if(zero>1)
        {
          error = 34;
          goto END;
        }
        zero = 0;

        if((scratchpad[n]==20)||(scratchpad[n]==21))
        {
          if(scratchpad[n]==21)
          {
            if(duration||duration_start||onset||annots_in_tal)
            {               /* it's not allowed to have multiple duration fields */
              error = 35;   /* in one TAL or to have a duration field which is   */
              goto END;     /* not immediately behind the onsetfield             */
            }
            duration_start = 1;
          }

          if((scratchpad[n]==20)&&onset&&(!duration_start))
          {
            if(r||annots_in_record)
            {
              if(n >= 0)
              {
                memset(&annotblock, 0, sizeof(annotblck_t));
                annotblock.edfhdr = edf_hdr;
                annotblock.onset = get_long_time(time_in_txt);
                annotblock.onset -= edf_hdr->starttime_subsec;

                for(j=0; j<n; j++)
                {
                  if(j==MAX_ANNOTATION_LEN)  break;
                  annotblock.description[j] = scratchpad[j];
                  if((((unsigned char *)annotblock.description)[j] < 32) || (((unsigned char *)annotblock.description)[j] == 127))
                  {
                    if((annotblock.description[j] != '\t') &&
                       (annotblock.description[j] != '\n') &&
                       (annotblock.description[j] != '\r'))
                    {
                      error = 38;  /* non-printable characters in description string */
                      goto END;
                    }
                  }
                }
                annotblock.description[j] = 0;

                if(duration)
                {
                  strlcpy(annotblock.duration, duration_in_txt, 20);

                  annotblock.long_duration = get_long_time(duration_in_txt);
                }

                annotblock.file_offset = (long long)edf_hdr->hdrsize + ((long long)i * (long long)recordsize) + (long long)p + (long long)annot_offset;

                annotblock.datrec = i;

                if(edfplus_annotation_add_item(&edf_hdr->annot_list, annotblock))
                {
                  progress.reset();
                  QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred while reading annotations.");
                  messagewindow.exec();
                  free(cnv_buf);
                  free(scratchpad);
                  free(time_in_txt);
                  free(duration_in_txt);
                  return 1;
                }
              }
            }

            annots_in_tal++;
            annots_in_record++;
            n = 0;
            continue;
          }

          if(!onset)
          {
            scratchpad[n] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            onset = 1;
            n = 0;
            strlcpy(time_in_txt, scratchpad, max_tal_ln + 3);
            continue;
          }

          if(duration_start)
          {
            scratchpad[n] = 0;
            if(is_duration_number(scratchpad))
            {
              error = 37;
              goto END;
            }

            for(j=0; j<n; j++)
            {
              if(j==19)  break;
              duration_in_txt[j] = scratchpad[j];
              if((duration_in_txt[j]<32)||(duration_in_txt[j]>126))
              {
                duration_in_txt[j] = '.';
              }
            }
            duration_in_txt[j] = 0;

            duration = 1;
            duration_start = 0;
            n = 0;
            continue;
          }
        }

        n++;
      }

 END:

/****************** end ************************/

      if(error)
      {
        progress.reset();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot read annotations because there is an EDF or BDF incompatibility in this file.\n"
                                                                  "For more information, run the EDF/BDF compatibility checker in the Tools menu.");
        messagewindow.exec();
        free(cnv_buf);
        free(scratchpad);
        free(time_in_txt);
        free(duration_in_txt);
        return 9;
      }
    }

/************** process NK triggers ****************/

    if(nk_triggers_enabled)
    {
      if(nk_triggers_cnt < 100000)
      {
        for(k=0; k<nk_triggers_smpls; k++)
        {
          nk_triggerfields = *((unsigned char *)cnv_buf + nk_triggers_bufoffset + (k * 2) + 1);

          nk_triggerfields <<= 8;

          nk_triggerfields += *((unsigned char *)cnv_buf + nk_triggers_bufoffset + (k * 2));

          for(j=0; j<14; j++)
          {
            if((nk_triggerfields & (1 << j)) && (!(nk_old_triggerfields & (1 << j))))
            {
              nk_triggers_cnt++;

              memset(&annotblock, 0, sizeof(annotblck_t));
              annotblock.edfhdr = edf_hdr;
              annotblock.onset = ((long long)i * data_record_duration) + (k * nk_trigger_sample_duration) + 0.5;
              strlcpy(annotblock.description, nk_triggerlabel[j], MAX_ANNOTATION_LEN_II + 1);
              annotblock.ident = (1 << ANNOT_ID_NK_TRIGGER);

              if(edfplus_annotation_add_item(&edf_hdr->annot_list, annotblock))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred while reading annotations.");
                messagewindow.exec();
                free(cnv_buf);
                free(scratchpad);
                free(time_in_txt);
                free(duration_in_txt);
                return 1;
              }
            }
          }

          nk_old_triggerfields = nk_triggerfields;
        }
      }

      edf_hdr->nk_triggers_read = 1;
    }
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  edfplus_annotation_sort(&edf_hdr->annot_list, &process_events);

  QApplication::restoreOverrideCursor();

  progress.reset();

  free(cnv_buf);
  free(scratchpad);
  free(time_in_txt);
  free(duration_in_txt);

  return 0;
}


int EDF_annotations::is_duration_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0] == '.')||(str[l-1] == '.'))  return 1;

  for(i=0; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



int EDF_annotations::is_onset_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(l<2)  return 1;

  if((str[0]!='+')&&(str[0]!='-'))  return 1;

  if((str[1] == '.')||(str[l-1] == '.'))  return 1;

  for(i=1; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



long long EDF_annotations::get_long_time(char *str)
{
  int i, len, hasdot=0, dotposition=0, neg=0;

  long long value=0, radix;

  if(str[0] == '+')
  {
    str++;
  }
  else if(str[0] == '-')
    {
      neg = 1;
      str++;
    }

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      hasdot = 1;
      dotposition = i;
      break;
    }
  }

  if(hasdot)
  {
    radix = TIME_FIXP_SCALING;

    for(i=dotposition-1; i>=0; i--)
    {
      value += ((long long)(str[i] - 48)) * radix;
      radix *= 10;
    }

    radix = TIME_FIXP_SCALING / 10;

    for(i=dotposition+1; i<len; i++)
    {
      value += ((long long)(str[i] - 48)) * radix;
      radix /= 10;
    }
  }
  else
  {
    radix = TIME_FIXP_SCALING;

    for(i=len-1; i>=0; i--)
    {
      value += ((long long)(str[i] - 48)) * radix;
      radix *= 10;
    }
  }

  if(neg)  value = -value;

  return value;
}


int EDF_annotations::check_device(char *str)
{
  int error = 1;

  if(strlen(str) < 29)  return 1;

  if(!strncmp(str, "Nihon Kohden EEG-1100A V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-1100B V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-1100C V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden QI-403A   V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden QI-403A   V02.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-2100  V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-2100  V02.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden DAE-2100D V01.30", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden DAE-2100D V02.00", 29))  error = 0;
//  if(!strncmp(str, "Nihon Kohden EEG-1200A V01.00", 29))  error = 0;

  return error;
}





