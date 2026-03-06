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



#include "print_to_bdf.h"


void print_screen_to_bdf(UI_Mainwindow *mainwindow)
{
  int i, j, k, p, r, hf, tmp,
      n=0,
      records,
      records_written,
      signalcomps,
      duration_factor[MAXFILES],
      bdfplus=0,
      tallen,
      annotationlist_nr=-1,
      annotations_left=1,
      add_one_sec=0,
      annot_smp_per_record=16,
      annot_list_sz=0,
      annot_cnt=0,
      type,
      len,
      integer_sf=0,
      datrec_multiple_int=0,
      smpl_rate_divider=1;

  const int smpl_div_arr[14]={100,80,64,50,40,32,20,16,10, 8, 5, 4, 2, 1};

  long long taltime=0LL,
            l_temp,
            referencetime=0LL,
            annot_difftime=0LL,
            s2;

  unsigned long long duration,
            smpls_written[MAXSIGNALS],
            preamble=0LL,
            smpls_preamble[MAXSIGNALS];

  char path[MAX_PATH_LENGTH]={""},
       scratchpad_4096[4096]={""},
       datrecduration_32[32]={""},
       str1_1024[1024]={""},
       *viewbuf=NULL;

  double frequency,
         frequency2,
         dig_value,
         f_tmp=0;

  FILE *outputfile;


  sigcompblck_t **signalcomp;

  union {
          signed int one_signed;
          signed short two_signed[2];
          unsigned char four[4];
        } wr_var;

  union {
          signed int one_signed;
          signed short two_signed[2];
          unsigned char four[4];
        } null_bytes[MAXSIGNALS];

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  date_time_t date_time;

  annotlist_t *annot_list=NULL;

  annotblck_t *annot_ptr=NULL;

/////////////////////////////////////////////////////////////////////////

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

  if((!mainwindow->files_open)||(!signalcomps))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Print to BDF", "Put a signal on the screen and try again.");
    messagewindow.exec();
    return;
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->edfheaderlist[i]->discontinuous)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Print to BDF", "Sorry, discontinues BDF/EDF files cannot be printed to BDF.\n");
      messagewindow.exec();
      return;
    }

    if(mainwindow->edfheaderlist[i]->edfplus || mainwindow->edfheaderlist[i]->bdfplus)
    {
      bdfplus = 1;

      for(j=0; j<mainwindow->edfheaderlist[i]->nr_annot_chns; j++)
      {
        if(mainwindow->edfheaderlist[i]->edfparam[mainwindow->edfheaderlist[i]->annot_ch[j]].smp_per_record>annot_smp_per_record)
        {
          annot_smp_per_record = mainwindow->edfheaderlist[i]->edfparam[mainwindow->edfheaderlist[i]->annot_ch[j]].smp_per_record;
        }
      }
    }
  }

  annot_smp_per_record += 11;

  taltime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_subsec;

  duration = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfhdr->long_data_record_duration > duration)
    {
      duration = signalcomp[i]->edfhdr->long_data_record_duration;
      n = i;
    }
  }

  fseeko(signalcomp[n]->edfhdr->file_hdl, 244LL, SEEK_SET);
  if(fread(datrecduration_32, 8, 1, signalcomp[n]->edfhdr->file_hdl)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
    messagewindow.exec();
    return;
  }

  for(hf=0, integer_sf=1, i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].sf_int)
    {
      if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].sf_int >= 10000)
      {
        hf = 1;
      }
    }
    else
    {
      integer_sf = 0;

      hf = 0;

      break;
    }
  }

  if(integer_sf)
  {
    smpl_rate_divider = 1;

    if(hf)
    {
      j = 0;
    }
    else
    {
      j = 8;
    }

    for(; j<14; j++)
    {
      for(i=0; i<signalcomps; i++)
      {
        if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].sf_int % smpl_div_arr[j])
        {
          break;
        }
      }

      if(i == signalcomps)
      {
        smpl_rate_divider = smpl_div_arr[j];

        break;
      }
    }
  }

  for(datrec_multiple_int=1, i=0; i<signalcomps; i++)
  {
    if(duration % signalcomp[i]->edfhdr->long_data_record_duration)
    {
      datrec_multiple_int = 0;

      break;
    }

    duration_factor[mainwindow->get_filenum(signalcomp[i]->edfhdr)] = duration / signalcomp[i]->edfhdr->long_data_record_duration;
  }

  if((!integer_sf) && (!datrec_multiple_int))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "This combination of files/signals cannot be printed to BDF,\n"
                                                              "either the datarecordblock duration of the files must be an integer multiple\n"
                                                              "or the samplerates of the signals must have integer values.");
    messagewindow.exec();
    return;
  }

  if(integer_sf)
  {
    snprintf(datrecduration_32, 32, "%f", 1.0 / smpl_rate_divider);

    remove_trailing_zeros(datrecduration_32);

    strlcat(datrecduration_32, "        ", 32);

    duration = TIME_FIXP_SCALING / smpl_rate_divider;

    datrec_multiple_int = 0;
  }

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strlcat(path, "_screenprint.bdf", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Print to BDF", QString::fromLocal8Bit(path), "BDF files (*.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, selected file is in use.");
    messagewindow.exec();
    return;
  }

  outputfile = fopeno(path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot create a file for writing.");
    messagewindow.exec();
    return;
  }

  referencetime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime;

  if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime<0)
  {
    if((-mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime) % TIME_FIXP_SCALING)
    {
      preamble = TIME_FIXP_SCALING - ((-mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime) % TIME_FIXP_SCALING);

      referencetime--;

      add_one_sec = 1;
    }
  }
  else
  {
    preamble = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime % TIME_FIXP_SCALING;

    if(preamble)  add_one_sec = 1;
  }

  for(i=0; i<signalcomps; i++)
  {
    smpls_preamble[i] = (preamble * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record) / signalcomp[i]->edfhdr->long_data_record_duration;
  }

  referencetime += (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime / TIME_FIXP_SCALING);

  utc_to_date_time(referencetime, &date_time);

/************************* write BDF-header ***************************************/

  fseeko(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl, 8LL, SEEK_SET);

  rewind(outputfile);

  if(bdfplus)
  {
    fprintf(outputfile, "\xff" "BIOSEMI");
    if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus || mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
    {
      if(fread(scratchpad_4096, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad_4096, 80, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
    else
    {
      fprintf(outputfile, "X X X X ");
      if(fread(scratchpad_4096, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad_4096, 72, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    fprintf(outputfile, "Startdate %02i-", date_time.day);

    switch(date_time.month)
    {
      case  1 : fprintf(outputfile, "JAN");
                break;
      case  2 : fprintf(outputfile, "FEB");
                break;
      case  3 : fprintf(outputfile, "MAR");
                break;
      case  4 : fprintf(outputfile, "APR");
                break;
      case  5 : fprintf(outputfile, "MAY");
                break;
      case  6 : fprintf(outputfile, "JUN");
                break;
      case  7 : fprintf(outputfile, "JUL");
                break;
      case  8 : fprintf(outputfile, "AUG");
                break;
      case  9 : fprintf(outputfile, "SEP");
                break;
      case 10 : fprintf(outputfile, "OCT");
                break;
      case 11 : fprintf(outputfile, "NOV");
                break;
      case 12 : fprintf(outputfile, "DEC");
                break;
      default : fprintf(outputfile, "ERR");
                break;
    }

    fprintf(outputfile, "-%04i ", date_time.year);

    if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus || mainwindow->edfheaderlist[mainwindow->sel_viewtime]->bdfplus)
    {
      if(fread(scratchpad_4096, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(scratchpad_4096[10]=='X')
      {
        if(fwrite(scratchpad_4096 + 12, 58, 1, outputfile)!=1)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
          messagewindow.exec();
          fclose(outputfile);
          return;
        }
      }
      else
      {
        if(fwrite(scratchpad_4096 + 22, 58, 1, outputfile)!=1)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
          messagewindow.exec();
          fclose(outputfile);
          return;
        }
      }
    }
    else
    {
      fprintf(outputfile, "X X X ");

      if(fread(scratchpad_4096, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad_4096 + 28, 52, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }
  else
  {
    if(fread(scratchpad_4096, 160, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }

    fprintf(outputfile, "\xff" "BIOSEMI");

    if(fwrite(scratchpad_4096, 160, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  fprintf(outputfile,
          "%02i.%02i.%02i%02i.%02i.%02i",
          date_time.day,
          date_time.month,
          date_time.year % 100,
          date_time.hour,
          date_time.minute,
          date_time.second);

  fprintf(outputfile, "%-8i", (signalcomps + bdfplus) * 256 + 256);

  if(bdfplus)
  {
    fprintf(outputfile, "BDF+C");

    for(i=0; i<39; i++)  fputc(' ', outputfile);
  }
  else
  {
    for(i=0; i<44; i++)  fputc(' ', outputfile);
  }

  records = (int)(mainwindow->pagetime / duration);

  if(mainwindow->pagetime % duration)
  {
    records++;
  }

  if(add_one_sec)
  {
    records += TIME_FIXP_SCALING / duration;
  }

  fprintf(outputfile, "%-8i", records);

  if(fwrite(datrecduration_32, 8, 1, outputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
    messagewindow.exec();
    fclose(outputfile);
    return;
  }

  fprintf(outputfile, "%-4i", signalcomps + bdfplus);

  for(i=0; i<signalcomps; i++)
  {
    strlcpy(scratchpad_4096, signalcomp[i]->signallabel, 4096);
    strlcat(scratchpad_4096, "                ", 4096);
    if(fwrite(scratchpad_4096, 16, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(bdfplus)
  {
    if(fwrite("BDF Annotations ", 16, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(fwrite(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].transducer, 80, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(bdfplus)
  {
    for(i=0; i<80; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    strlcpy(scratchpad_4096, signalcomp[i]->physdimension, 4096);
    strlcat(scratchpad_4096, "        ", 4096);
    if(fwrite(scratchpad_4096, 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(bdfplus)
  {
    for(i=0; i<8; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)((signalcomp[i]->edfsignal[0] * 8) + (104 * signalcomp[i]->edfhdr->edfsignals) + 256), SEEK_SET);
      if(fread(scratchpad_4096, 8, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
      if(signalcomp[i]->polarity == -1)
      {
        if(scratchpad_4096[0] == '+')
        {
          scratchpad_4096[0] = '-';
        }
        else if(scratchpad_4096[0] == '-')
          {
            for(j=0; j<7; j++)
            {
              scratchpad_4096[j] = scratchpad_4096[j+1];
            }
            scratchpad_4096[7] = ' ';
          }
          else
          {
            if(scratchpad_4096[7] != ' ')
            {
              for(j=0; j<8; j++)
              {
                if(scratchpad_4096[j] == '.')  break;
              }
              if(j == 8)
              {
                snprintf(str1_1024, 1024,
                         "signal %i has been set to \"inverted\" but the physical minimum field has no free space left to write the minus sign",
                         i + 1);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_1024);
                messagewindow.exec();
                fclose(outputfile);
                return;
              }
            }
            for(j=7; j>0; j--)
            {
              scratchpad_4096[j] = scratchpad_4096[j-1];
            }
            scratchpad_4096[0] = '-';
          }
      }
      if(fwrite(scratchpad_4096, 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      if(fwrite("0       ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      if(fwrite("-1      ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(bdfplus)
  {
    if(fwrite("-1      ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)((signalcomp[i]->edfsignal[0] * 8) + (112 * signalcomp[i]->edfhdr->edfsignals) + 256), SEEK_SET);
      if(fread(scratchpad_4096, 8, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
      if(signalcomp[i]->polarity == -1)
      {
        if(scratchpad_4096[0] == '+')
        {
          scratchpad_4096[0] = '-';
        }
        else if(scratchpad_4096[0] == '-')
          {
            for(j=0; j<7; j++)
            {
              scratchpad_4096[j] = scratchpad_4096[j+1];
            }
            scratchpad_4096[7] = ' ';
          }
          else
          {
            if(scratchpad_4096[7] != ' ')
            {
              for(j=0; j<8; j++)
              {
                if(scratchpad_4096[j] == '.')  break;
              }
              if(j == 8)
              {
                snprintf(str1_1024, 1024,
                         "signal %i has been set to \"inverted\" but the physical maximum field has no free space left to write the minus sign",
                         i + 1);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_1024);
                messagewindow.exec();
                fclose(outputfile);
                return;
              }
            }
            for(j=7; j>0; j--)
            {
              scratchpad_4096[j] = scratchpad_4096[j-1];
            }
            scratchpad_4096[0] = '-';
          }
      }
      if(fwrite(scratchpad_4096, 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      if(fwrite("1000    ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      if(fwrite("1       ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(bdfplus)
  {
    if(fwrite("1       ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min);

      if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min<0)
      {
        null_bytes[i].one_signed = 0;
      }
      else
      {
        null_bytes[i].one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min;
      }
    }
    else
    {
      if(fwrite("-8388608", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      null_bytes[i].one_signed = -8388608;
    }
  }

  if(bdfplus)
  {
    if(fwrite("-8388608", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max);
    }
    else
    {
      if(fwrite("8388607 ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(bdfplus)
  {
    if(fwrite("8388607 ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    strlcpy(scratchpad_4096, signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].prefilter, 4096);
    strlcat(scratchpad_4096, "                                                                                ", 4096);
    for(p = strlen(scratchpad_4096) - 1; p>=0; p--)
    {
      if(scratchpad_4096[p]!=' ')  break;
    }
    p++;
    if(p) p++;

    if(signalcomp[i]->spike_filter)
    {
      p += snprintf(scratchpad_4096 + p, 4096 - p, "Spike:%f", signalcomp[i]->spike_filter->velocity);

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad_4096[k]!='0')  break;
      }

      if(scratchpad_4096[k]=='.')  scratchpad_4096[k] = 0;
      else  scratchpad_4096[k+1] = 0;

      p = strlen(scratchpad_4096);
    }

    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      if(signalcomp[i]->filter[j]->is_LPF == 1)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "LP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      if(signalcomp[i]->filter[j]->is_LPF == 0)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "HP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad_4096[k]!='0')  break;
      }

      if(scratchpad_4096[k]=='.')  scratchpad_4096[k] = 0;
      else  scratchpad_4096[k+1] = 0;

      strlcat(scratchpad_4096, "Hz ", 4096);

      p = strlen(scratchpad_4096);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      type = signalcomp[i]->fidfilter_type[j];

      frequency = signalcomp[i]->fidfilter_freq[j];

      frequency2 = signalcomp[i]->fidfilter_freq2[j];

      if(type == 0)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "HP:%f", frequency);
      }

      if(type == 1)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "LP:%f", frequency);
      }

      if(type == 2)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "N:%f", frequency);
      }

      if(type == 3)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "BP:%f", frequency);
      }

      if(type == 4)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "BS:%f", frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad_4096[k]!='0')  break;
      }

      if(scratchpad_4096[k]=='.')  scratchpad_4096[k] = 0;
      else  scratchpad_4096[k+1] = 0;

      p = strlen(scratchpad_4096);

      if((type == 3) || (type == 4))
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "-%f", frequency2);

        for(k=(p-1); k>0; k--)
        {
          if(scratchpad_4096[k]!='0')  break;
        }

        if(scratchpad_4096[k]=='.')  scratchpad_4096[k] = 0;
        else  scratchpad_4096[k+1] = 0;
      }

      strlcat(scratchpad_4096, "Hz ", 4096);

      p = strlen(scratchpad_4096);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      if(signalcomp[i]->ravg_filter_type[j] == 0)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "HP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      if(signalcomp[i]->ravg_filter_type[j] == 1)
      {
        p += snprintf(scratchpad_4096 + p, 4096 - p, "LP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      p = strlen(scratchpad_4096);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
    {
      p += snprintf(scratchpad_4096 + p, 4096 - p, "FIR ");
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      p += snprintf(scratchpad_4096 + p, 4096 - p, "ECG:HR ");
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      p += snprintf(scratchpad_4096 + p, 4096 - p, "Z-ratio ");
    }

    for(;p<81; p++)
    {
      scratchpad_4096[p] = ' ';
    }

    if(fwrite(scratchpad_4096, 80, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(bdfplus)
  {
    for(i=0; i<80; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    if(datrec_multiple_int)
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record
       * duration_factor[mainwindow->get_filenum(signalcomp[i]->edfhdr)]);
    }
    else
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].sf_int / smpl_rate_divider);
    }
  }

  if(bdfplus)
  {
    fprintf(outputfile, "%-8i", annot_smp_per_record);
  }

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<32; j++)  fputc(' ', outputfile);
  }

  if(bdfplus)
  {
    for(i=0; i<32; i++)  fputc(' ', outputfile);
  }

/////////////////////////////////////////////////////////////////////

  memset(smpls_written, 0, sizeof(unsigned long long[MAXSIGNALS]));

  mainwindow->print_to_edf_active = 1;

  mainwindow->setup_viewbuf();

  viewbuf = mainwindow->viewbuf;

  if(viewbuf==NULL)
  {
    fclose(outputfile);
    return;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(records_written=0; records_written<records; records_written++)
  {
    qApp->processEvents();

    for(i=0; i<signalcomps; i++)
    {
      if(datrec_multiple_int)
      {
        r = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record * duration_factor[mainwindow->get_filenum(signalcomp[i]->edfhdr)];
      }
      else
      {
        r = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].sf_int / smpl_rate_divider;
      }

      for(k=0; k<r; k++)
      {
        if(smpls_preamble[i])
        {
          smpls_preamble[i]--;

          fputc(null_bytes[i].four[0], outputfile);
          fputc(null_bytes[i].four[1], outputfile);
          if(fputc(null_bytes[i].four[2], outputfile)==EOF)
          {
            QApplication::restoreOverrideCursor();
            QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
            messagewindow.exec();
            fclose(outputfile);
            return;
          }
        }
        else
        {
          dig_value = 0;

          s2 = smpls_written[i] + signalcomp[i]->sample_timeoffset - signalcomp[i]->sample_start;

          for(j=0; j<signalcomp[i]->num_of_signals; j++)
          {
            if((smpls_written[i]<signalcomp[i]->sample_stop)&&(s2>=0))
            {
              if(signalcomp[i]->edfhdr->bdf)
              {
                var.two[0] = *((unsigned short *)(
                  viewbuf
                  + signalcomp[i]->viewbufoffset
                  + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                  + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset
                  + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)));

                var.four[2] = *((unsigned char *)(
                  viewbuf
                  + signalcomp[i]->viewbufoffset
                  + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                  + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset
                  + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)
                  + 2));

                if(var.four[2]&0x80)
                {
                  var.four[3] = 0xff;
                }
                else
                {
                  var.four[3] = 0x00;
                }

                if(var.one_signed > signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max)
                {
                  var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max;
                }
                else if(var.one_signed < signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min)
                  {
                    var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min;
                  }

                f_tmp = var.one_signed;
              }

              if(signalcomp[i]->edfhdr->edf)
              {
                tmp = *(((short *)(
                  viewbuf
                  + signalcomp[i]->viewbufoffset
                  + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                  + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset))
                  + (s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record));

                if(tmp > signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max)
                {
                  tmp = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max;
                }
                else if(tmp < signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min)
                  {
                    tmp = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min;
                  }

                f_tmp = tmp;
              }

              f_tmp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
              f_tmp *= signalcomp[i]->factor[j];

              f_tmp -= signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
            }
            else
            {
              f_tmp = 0;
            }

            dig_value += f_tmp;
          }

          if(signalcomp[i]->spike_filter)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime<=0)
              {
                reset_spike_filter(signalcomp[i]->spike_filter);
              }
              else
              {
                spike_filter_restore_buf(signalcomp[i]->spike_filter);
              }
            }

            dig_value = run_spike_filter(dig_value, signalcomp[i]->spike_filter);
          }

          for(p=0; p<signalcomp[i]->math_func_cnt_before; p++)
          {
            dig_value = run_math_func(dig_value, signalcomp[i]->math_func_before[p]);
          }

          for(p=0; p<signalcomp[i]->filter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime==0)
              {
                reset_filter(dig_value, signalcomp[i]->filter[p]);
              }
              else
              {
                signalcomp[i]->filter[p]->old_input = signalcomp[i]->filterpreset_a[p];
                signalcomp[i]->filter[p]->old_output = signalcomp[i]->filterpreset_b[p];
              }
            }

            dig_value = first_order_filter(dig_value, signalcomp[i]->filter[p]);
          }

          for(p=0; p<signalcomp[i]->ravg_filter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime!=0)
              {
                ravg_filter_restore_buf(signalcomp[i]->ravg_filter[p]);
              }
            }

            dig_value = run_ravg_filter(dig_value, signalcomp[i]->ravg_filter[p]);
          }

          for(p=0; p<signalcomp[i]->fidfilter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime!=0)
              {
                memcpy(signalcomp[i]->fidbuf[p], signalcomp[i]->fidbuf2[p], fid_run_bufsize(signalcomp[i]->fid_run[p]));
              }
            }

            dig_value = signalcomp[i]->fidfuncp[p](signalcomp[i]->fidbuf[p], dig_value);
          }

          for(p=0; p<signalcomp[i]->fir_filter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime!=0)
              {
                fir_filter_restore_buf(signalcomp[i]->fir_filter[p]);
              }
            }

            dig_value = run_fir_filter(dig_value, signalcomp[i]->fir_filter[p]);
          }

          for(p=0; p<signalcomp[i]->math_func_cnt_after; p++)
          {
            dig_value = run_math_func(dig_value, signalcomp[i]->math_func_after[p]);
          }

          if(signalcomp[i]->plif_ecg_filter)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime<=0)
              {
                plif_ecg_reset_subtract_filter(signalcomp[i]->plif_ecg_filter, 0);
              }
              else
              {
                plif_ecg_subtract_filter_state_copy(signalcomp[i]->plif_ecg_filter, signalcomp[i]->plif_ecg_filter_sav);
              }
            }

            dig_value = plif_ecg_run_subtract_filter(dig_value, signalcomp[i]->plif_ecg_filter);
          }

          if(signalcomp[i]->plif_eeg_filter)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime<=0)
              {
                plif_eeg_reset_subtract_filter(signalcomp[i]->plif_eeg_filter);
              }
              else
              {
                plif_eeg_subtract_filter_state_copy(signalcomp[i]->plif_eeg_filter, signalcomp[i]->plif_eeg_filter_sav);
              }
            }

            dig_value = plif_eeg_run_subtract_filter(dig_value, signalcomp[i]->plif_eeg_filter);
          }

          if(signalcomp[i]->ecg_filter != NULL)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime != 0)
              {
                ecg_filter_restore_buf(signalcomp[i]->ecg_filter);
              }
            }

            dig_value = run_ecg_filter(dig_value, signalcomp[i]->ecg_filter);

            dig_value = (dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue * 16777.216) - 8388608.0;
          }

          if(signalcomp[i]->zratio_filter != NULL)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(signalcomp[i]->edfhdr->viewtime != 0)
              {
                zratio_filter_restore_buf(signalcomp[i]->zratio_filter);
              }
            }

            dig_value = run_zratio_filter(dig_value, signalcomp[i]->zratio_filter);

            dig_value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue * 8388608.0;
          }

          dig_value *= signalcomp[i]->polarity;

          if((smpls_written[i]>=signalcomp[i]->sample_start)&&(smpls_written[i]<signalcomp[i]->sample_stop))
          {
            wr_var.one_signed = dig_value;

            if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
            {
              if(wr_var.one_signed>signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max)
              {
                wr_var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max;
              }
              if(wr_var.one_signed<signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min)
              {
                wr_var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min;
              }
            }
            else
            {
              if(wr_var.one_signed > 8388607)
              {
                wr_var.one_signed = 8388607;
              }
              if(wr_var.one_signed < -8388608)
              {
                wr_var.one_signed = -8388608;
              }
            }

            fputc(wr_var.four[0], outputfile);
            fputc(wr_var.four[1], outputfile);
            if(fputc(wr_var.four[2], outputfile)==EOF)
            {
              QApplication::restoreOverrideCursor();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
              messagewindow.exec();
              fclose(outputfile);
              return;
            }
          }
          else
          {
            fputc(null_bytes[i].four[0], outputfile);
            fputc(null_bytes[i].four[1], outputfile);
            if(fputc(null_bytes[i].four[2], outputfile)==EOF)
            {
              QApplication::restoreOverrideCursor();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
              messagewindow.exec();
              fclose(outputfile);
              return;
            }
          }

          smpls_written[i]++;
        }
      }
    }

    if(bdfplus)
    {
      tallen = fprintf(outputfile, "+%i.%07i",
      (int)(taltime / TIME_FIXP_SCALING),
      (int)(taltime % TIME_FIXP_SCALING));

      fputc(20, outputfile);
      fputc(20, outputfile);
      fputc(0, outputfile);

      tallen += 3;

      while(annotations_left)
      {
        while((annot_list_sz < 1) || (annot_cnt >= annot_list_sz))
        {
          annotationlist_nr++;
          if(annotationlist_nr>=mainwindow->files_open)
          {
            annotations_left = 0;
            annot_list = NULL;
            annot_list_sz = 0;
            break;
          }
          annot_list = &mainwindow->edfheaderlist[annotationlist_nr]->annot_list;

          annot_list_sz = edfplus_annotation_size(annot_list);

          annot_cnt = 0;

          annot_difftime = 0LL;

          if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[annotationlist_nr]->utc_starttime) * TIME_FIXP_SCALING;
          }

          if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime) * TIME_FIXP_SCALING;
          }

          if((mainwindow->viewtime_sync==VIEWTIME_UNSYNCED) || (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime) * TIME_FIXP_SCALING;
            annot_difftime += (mainwindow->edfheaderlist[annotationlist_nr]->viewtime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime);
          }

          if(annotationlist_nr != mainwindow->sel_viewtime)
          {
            annot_difftime -= mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_subsec;
            annot_difftime += mainwindow->edfheaderlist[annotationlist_nr]->starttime_subsec;
          }
        }

        if(!annotations_left)  break;

        if((annot_list != NULL) && (annot_cnt < annot_list_sz))
        {
          annot_ptr = edfplus_annotation_get_item(annot_list, annot_cnt);

          l_temp = annot_ptr->onset - annot_difftime;

          if((l_temp >= 0LL) && ((unsigned long long)l_temp < (mainwindow->pagetime + TIME_FIXP_SCALING)))
          {
            tallen += fprintf(outputfile, "%+i.%07i",
            (int)(l_temp / TIME_FIXP_SCALING),
            (int)(l_temp % TIME_FIXP_SCALING));

            if(annot_ptr->duration[0]!=0)
            {
              fputc(21, outputfile);
              tallen++;

              tallen += fprintf(outputfile, "%s", annot_ptr->duration);
            }

            fputc(20, outputfile);
            tallen++;

            tallen += fprintf(outputfile, "%s", annot_ptr->description);

            fputc(20, outputfile);
            fputc(0, outputfile);
            tallen += 2;

            annot_cnt++;

            break;
          }
          else
          {
            annot_cnt++;

            continue;
          }
        }
      }

      for(j=tallen; j<(annot_smp_per_record * 3); j++)
      {
        fputc(0, outputfile);
      }

      taltime += duration;
    }
  }

  QApplication::restoreOverrideCursor();

  fclose(outputfile);
}























