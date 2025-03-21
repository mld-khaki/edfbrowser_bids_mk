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



#include "edfplusd_cnv.h"



UI_EDFDwindow::UI_EDFDwindow(QWidget *w_parent, char *recent_ope_dir, char *recent_sav_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_ope_dir;
  recent_savedir = recent_sav_dir;

  myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 480 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("EDF+D/BDF+D to EDF+C/BDF+C converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  textEdit1 = new QTextEdit;
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  textEdit1->append("EDF+D/BDF+D to EDF+C/BDF+C converter.\n");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(textEdit1, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_EDFDwindow::SelectFileButton()
{
  FILE *inputfile=NULL,
       *outputfile=NULL;

  int i, j,
      file_number,
      offset,
      offset_2,
      datarecords,
      datarecords_read,
      datarecords_written,
      cnt,
      annot_signal_size,
      annot_signal_size_2,
      annot_signal_nr,
      annot_signal_nr_2,
      annots_written,
      len,
      progress_steps,
      annot_list_size;

  long long former_timestamp,
            next_timestamp,
            new_hdr_timestamp,
            chunk_starttime,
            chunk_endtime;

  char str1_2048[2048]={""},
       inputpath[MAX_PATH_LENGTH]={""},
       output_path[MAX_PATH_LENGTH]={""},
       *fileheader=NULL,
       *readbuf=NULL,
       *tal=NULL;

  annotblck_t *annot_ptr=NULL;

  edfhdrblck_t *edfhdr=NULL;


  pushButton1->setEnabled(false);

  strlcpy(inputpath, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF *.rec *.REC)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(inputpath, ""))
  {
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_opendir, inputpath, MAX_PATH_LENGTH);

  inputfile = fopeno(inputpath, "rb");
  if(inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Error, cannot open file %s for reading.\n", inputpath);
    textEdit1->append(QString::fromLocal8Bit(str1_2048));
    pushButton1->setEnabled(true);
    return;
  }

  snprintf(str1_2048, 2048, "Processing file %s", inputpath);
  textEdit1->append(str1_2048);

/***************** check if the file is valid ******************************/

  edfhdr = check_edf_file(inputfile, str1_2048, 2048, 0, 0);
  if(edfhdr==NULL)
  {
    fclose(inputfile);
    textEdit1->append("Error, file is not a valid EDF or BDF file.\n");
    pushButton1->setEnabled(true);
    return;
  }

  if(((!edfhdr->edfplus)&&(!edfhdr->bdfplus))||(!edfhdr->discontinuous))
  {
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    textEdit1->append("Error, file is not an EDF+D or BDF+D file.\n");
    pushButton1->setEnabled(true);
    return;
  }

/****************** get annotations ******************************/

  edfhdr->file_hdl = inputfile;

  EDF_annotations annotations_func;

  if(annotations_func.get_annotations(edfhdr, 0))
  {
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    textEdit1->append("Error, there is an incompatibility with the annotations.\n");
    pushButton1->setEnabled(true);
    return;
  }

  if(edfhdr->annots_not_read)
  {
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    textEdit1->append("Aborted.\n");
    pushButton1->setEnabled(true);
    return;
  }

  annot_list_size = edfplus_annotation_size(&edfhdr->annot_list);

/***************** start conversion ******************************/

  datarecords = edfhdr->datarecords;

  annot_signal_nr = edfhdr->annot_ch[0];

  offset = edfhdr->edfparam[annot_signal_nr].datrec_offset;

  if(edfhdr->edfplus)
  {
    annot_signal_size = edfhdr->edfparam[annot_signal_nr].smp_per_record * 2;
  }
  else
  {
    annot_signal_size = edfhdr->edfparam[annot_signal_nr].smp_per_record * 3;
  }

  fileheader = (char *)calloc(1, edfhdr->hdrsize);
  if(fileheader==NULL)
  {
    textEdit1->append("Malloc error, (fileheader).\n");
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  readbuf = (char *)calloc(1, edfhdr->recordsize);
  if(readbuf==NULL)
  {
    textEdit1->append("Malloc error, (readbuf).\n");
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    free(fileheader);
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  rewind(inputfile);
  if(fread(fileheader, edfhdr->hdrsize, 1, inputfile) != 1)
  {
    textEdit1->append("Read error.\n");
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    free(fileheader);
    free(readbuf);
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

///////////////////////////////////////////////////////////////////

  file_number = 1;

  strlcpy(output_path, inputpath, MAX_PATH_LENGTH);
  remove_extension_from_filename(output_path);
  if(edfhdr->edfplus)
  {
    snprintf(output_path + strlen(output_path), MAX_PATH_LENGTH - strlen(output_path), "_%04i.edf", file_number);
  }
  else
  {
    snprintf(output_path + strlen(output_path), MAX_PATH_LENGTH - strlen(output_path), "_%04i.bdf", file_number);
  }

  strlcpy(str1_2048, "Creating file ", 2048);
  len = strlen(str1_2048);
  get_filename_from_path(str1_2048 + len, output_path, MAX_PATH_LENGTH - len);
  textEdit1->append(QString::fromLocal8Bit(str1_2048));

  outputfile = fopeno(output_path, "w+b");
  if(outputfile==NULL)
  {
    textEdit1->append("Error, cannot open outputfile for writing.\n");
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    free(fileheader);
    free(readbuf);
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(fwrite(fileheader, edfhdr->hdrsize, 1, outputfile) != 1)
  {
    textEdit1->append("Write error.\n");
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    free(fileheader);
    free(readbuf);
    fclose(inputfile);
    fclose(outputfile);
    pushButton1->setEnabled(true);
    return;
  }

  QProgressDialog progress("Processing file...", "Abort", 0, datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  datarecords_written = 0;

  new_hdr_timestamp = 0LL;

  former_timestamp = 0LL;

  next_timestamp = 0LL;

  chunk_starttime = 0LL;

  chunk_endtime = 0LL;

  tal = readbuf + offset;

  fseeko(inputfile, (long long)(edfhdr->hdrsize), SEEK_SET);

  for(datarecords_read=0; datarecords_read<datarecords; datarecords_read++)
  {
    if(!(datarecords_read%progress_steps))
    {
      progress.setValue(datarecords_read);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        textEdit1->append("Aborted.\n");

        break;
      }
    }

    if(fread(readbuf, edfhdr->recordsize, 1, inputfile) != 1)
    {
      progress.reset();
      textEdit1->append("Read error.\n");
      edfplus_annotation_empty_list(&edfhdr->annot_list);
      free(edfhdr->edfparam);
      free(edfhdr);
      free(fileheader);
      free(readbuf);
      fclose(inputfile);
      fclose(outputfile);
      pushButton1->setEnabled(true);
      return;
    }

    if(edfhdr->nr_annot_chns>1)
    {
      for(i=1; i<edfhdr->nr_annot_chns; i++)
      {
        annot_signal_nr_2 = edfhdr->annot_ch[i];

        offset_2 = edfhdr->edfparam[annot_signal_nr_2].datrec_offset;

        if(edfhdr->edfplus)
        {
          annot_signal_size_2 = edfhdr->edfparam[annot_signal_nr_2].smp_per_record * 2;
        }
        else
        {
          annot_signal_size_2 = edfhdr->edfparam[annot_signal_nr_2].smp_per_record * 3;
        }

        memset(readbuf + offset_2, 0, annot_signal_size_2);
      }
    }

    next_timestamp = get_datarecord_timestamp(tal);

    if(!datarecords_read)
    {
      former_timestamp = next_timestamp - edfhdr->long_data_record_duration;
    }

    if(next_timestamp != (long long)(former_timestamp + edfhdr->long_data_record_duration))
    {
      chunk_endtime = next_timestamp;

      write_values_to_hdr(outputfile, new_hdr_timestamp, datarecords_written, edfhdr);

      annots_written = 0;

      for(j=0; j<annot_list_size; j++)
      {
        annot_ptr = edfplus_annotation_get_item(&edfhdr->annot_list, j);

        if(((annot_ptr->onset>=chunk_starttime)||(file_number==1))
           &&(annot_ptr->onset<chunk_endtime)
           &&(annots_written<datarecords_written))
        {
          fseeko(outputfile, (long long)(edfhdr->hdrsize + (annots_written * edfhdr->recordsize) + offset), SEEK_SET);

          for(cnt=1; ; cnt++)
          {
            if(fgetc(outputfile)==0)  break;
          }

          fseeko(outputfile, 0LL, SEEK_CUR);

          cnt += fprintf(outputfile, "+%i", (int)((annot_ptr->onset - new_hdr_timestamp) / TIME_FIXP_SCALING));

          if(annot_ptr->onset%TIME_FIXP_SCALING)
          {
            cnt += fprintf(outputfile, ".%07i", (int)((annot_ptr->onset - new_hdr_timestamp) % TIME_FIXP_SCALING));

            fflush(outputfile);

            fseeko(outputfile, -1LL, SEEK_CUR);

            while(fgetc(outputfile)=='0')
            {
              fseeko(outputfile, -2LL, SEEK_CUR);

              cnt--;
            }
          }

          fseeko(outputfile, 0LL, SEEK_CUR);

          if(annot_ptr->duration[0]!=0)
          {
            fputc(21, outputfile);

            cnt++;

            cnt += fprintf(outputfile, "%s", annot_ptr->duration);
          }

          fputc(20, outputfile);

          cnt++;

          for(i = 0; i < (annot_signal_size - cnt - 2); i++)
          {
           if(annot_ptr->description[i]==0)  break;

           fputc(annot_ptr->description[i], outputfile);

           cnt++;
          }

          fputc(20, outputfile);

          cnt++;

          for(i = cnt; i < annot_signal_size; i++)
          {
            fputc(0, outputfile);
          }

          annots_written++;
        }
      }

      fclose(outputfile);

      datarecords_written = 0;

      new_hdr_timestamp = next_timestamp;

      new_hdr_timestamp -= next_timestamp % TIME_FIXP_SCALING;

      chunk_starttime = next_timestamp;

      file_number++;

      strlcpy(output_path, inputpath, MAX_PATH_LENGTH);
      remove_extension_from_filename(output_path);
      if(edfhdr->edfplus)
      {
        snprintf(output_path + strlen(output_path), MAX_PATH_LENGTH - strlen(output_path), "_%04i.edf", file_number);
      }
      else
      {
        snprintf(output_path + strlen(output_path), MAX_PATH_LENGTH - strlen(output_path), "_%04i.bdf", file_number);
      }

      strlcpy(str1_2048, "Creating file ", 2048);
      len = strlen(str1_2048);
      get_filename_from_path(str1_2048 + len, output_path, MAX_PATH_LENGTH - len);
      textEdit1->append(str1_2048);

      outputfile = fopeno(output_path, "w+b");
      if(outputfile==NULL)
      {
        progress.reset();
        textEdit1->append("Error, cannot open outputfile for writing.\n");
        edfplus_annotation_empty_list(&edfhdr->annot_list);
        free(edfhdr->edfparam);
        free(edfhdr);
        free(fileheader);
        free(readbuf);
        fclose(inputfile);
        pushButton1->setEnabled(true);
        return;
      }

      if(fwrite(fileheader, edfhdr->hdrsize, 1, outputfile) != 1)
      {
        progress.reset();
        textEdit1->append("Write error.\n");
        edfplus_annotation_empty_list(&edfhdr->annot_list);
        free(edfhdr->edfparam);
        free(edfhdr);
        free(fileheader);
        free(readbuf);
        fclose(inputfile);
        fclose(outputfile);
        pushButton1->setEnabled(true);
        return;
      }
    }

    cnt = snprintf(tal, 16, "+%i", (int)((next_timestamp - new_hdr_timestamp) / TIME_FIXP_SCALING));

    if((next_timestamp - new_hdr_timestamp)%TIME_FIXP_SCALING)
    {
      cnt += snprintf(tal + cnt, 10, ".%07i", (int)((next_timestamp - new_hdr_timestamp) % TIME_FIXP_SCALING));

      for(i=cnt-1; i>0; i--)
      {
        if(tal[i]!='0')  break;

        cnt--;
      }
    }

    tal[cnt++] = 20;
    tal[cnt++] = 20;

    for(i=cnt; i<annot_signal_size; i++)
    {
      tal[i] = 0;
    }

    if(fwrite(readbuf, edfhdr->recordsize, 1, outputfile) != 1)
    {
      progress.reset();
      textEdit1->append("Write error.\n");
      edfplus_annotation_empty_list(&edfhdr->annot_list);
      free(edfhdr->edfparam);
      free(edfhdr);
      free(fileheader);
      free(readbuf);
      fclose(inputfile);
      fclose(outputfile);
      pushButton1->setEnabled(true);
      return;
    }

    datarecords_written++;

    former_timestamp = next_timestamp;
  }

  write_values_to_hdr(outputfile, new_hdr_timestamp, datarecords_written, edfhdr);

  annots_written = 0;

  for(j=0; j<annot_list_size; j++)
  {
    annot_ptr = edfplus_annotation_get_item(&edfhdr->annot_list, j);

    if((annot_ptr->onset>=chunk_starttime)&&(annots_written<datarecords_written))
    {
      fseeko(outputfile, (long long)(edfhdr->hdrsize + (annots_written * edfhdr->recordsize) + offset), SEEK_SET);

      for(cnt=1; ; cnt++)
      {
        if(fgetc(outputfile)==0)  break;
      }

      fseeko(outputfile, 0LL, SEEK_CUR);

      cnt += fprintf(outputfile, "+%i", (int)((annot_ptr->onset - new_hdr_timestamp) / TIME_FIXP_SCALING));

      if(annot_ptr->onset%TIME_FIXP_SCALING)
      {
        cnt += fprintf(outputfile, ".%07i", (int)((annot_ptr->onset - new_hdr_timestamp) % TIME_FIXP_SCALING));

        fflush(outputfile);

        fseeko(outputfile, -1LL, SEEK_CUR);

        while(fgetc(outputfile)=='0')
        {
          fseeko(outputfile, -2LL, SEEK_CUR);

          cnt--;
        }
      }

      fseeko(outputfile, 0LL, SEEK_CUR);

      if(annot_ptr->duration[0]!=0)
      {
        fputc(21, outputfile);

        cnt++;

        cnt += fprintf(outputfile, "%s", annot_ptr->duration);
      }

      fputc(20, outputfile);

      cnt++;

      for(i = 0; i < (annot_signal_size - cnt - 2); i++)
      {
        if(annot_ptr->description[i]==0)  break;

        fputc(annot_ptr->description[i], outputfile);

        cnt++;
      }

      fputc(20, outputfile);

      cnt++;

      for(i = cnt; i < annot_signal_size; i++)
      {
        fputc(0, outputfile);
      }

      annots_written++;
    }
  }

  edfplus_annotation_empty_list(&edfhdr->annot_list);
  free(edfhdr->edfparam);
  free(edfhdr);
  free(fileheader);
  free(readbuf);
  fclose(inputfile);
  fclose(outputfile);

  progress.reset();
  textEdit1->append("Done\n");

  pushButton1->setEnabled(true);
}



void UI_EDFDwindow::write_values_to_hdr(FILE *outputfile, long long timestamp, int datarecords, edfhdrblck_t *edfhdr)
{
  int i;

  date_time_t date_time;

  utc_to_date_time(edfhdr->utc_starttime + (timestamp / TIME_FIXP_SCALING), &date_time);

  fseeko(outputfile, 98LL, SEEK_SET);
  if(fgetc(outputfile) != 'X')
  {
    fseeko(outputfile, 98LL, SEEK_SET);
    fprintf(outputfile, "%02i-%s-%04i",
            date_time.day,
            date_time.month_str,
            date_time.year);
  }

  fseeko(outputfile, 168LL, SEEK_SET);
  fprintf(outputfile, "%02i.%02i.%02i%02i.%02i.%02i",
          date_time.day,
          date_time.month,
          date_time.year % 100,
          date_time.hour,
          date_time.minute,
          date_time.second);

  fseeko(outputfile, 192LL, SEEK_SET);
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF+C");
  }
  else
  {
    fprintf(outputfile, "BDF+C");
  }

  for(i=0; i<39; i++)
  {
    fputc(' ', outputfile);
  }

  fseeko(outputfile, 236LL, SEEK_SET);
  fprintf(outputfile, "%-8i", datarecords);
}



long long UI_EDFDwindow::get_datarecord_timestamp(char *str)
{
  int i, len=8, hasdot=0, dotposition=0;

  long long value=0LL, radix;

  for(i=0; ; i++)
  {
    if(str[i]==20)
    {
      len = i;
      break;
    }
  }

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

    for(i=dotposition-1; i>=1; i--)
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

    for(i=len-1; i>=1; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  if(str[0]=='-')
  {
    value = -value;
  }

  return value;
}










