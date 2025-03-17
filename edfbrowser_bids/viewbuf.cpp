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


#include "mainwindow.h"


void UI_Mainwindow::setup_viewbuf()
{
  int i, j, k, r, s,
      temp=0,
      skip,
      hasprefilter=0,
      precision=3,
      prec_scale=10000;

  double pre_time=0.0,
         d_temp=0.0,
         dig_value;

  long long l_temp,
            datarecords,
            dif;

  unsigned long long totalsize=0LL,
                     readsize=0LL;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  date_time_t date_time_str;


  for(i=0; i<files_open; i++) edfheaderlist[i]->prefiltertime = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->filter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->filter_cnt; k++)
      {
        if(pre_time < (1.0 / signalcomp[i]->filter[k]->cutoff_frequency))
        {
          pre_time = (1.0 / signalcomp[i]->filter[k]->cutoff_frequency);
        }
      }
    }

    if(signalcomp[i]->spike_filter)
    {
      hasprefilter = 1;

      if(pre_time < 5.0)
      {
        pre_time = 5.0;
      }
    }

    if(signalcomp[i]->plif_ecg_filter)
    {
      hasprefilter = 1;

      if(pre_time < 2.0)
      {
        pre_time = 2.0;
      }
    }

    if(signalcomp[i]->plif_eeg_filter)
    {
      hasprefilter = 1;

      if(pre_time < 1.0)
      {
        pre_time = 1.0;
      }
    }

    if(signalcomp[i]->ravg_filter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->ravg_filter_cnt; k++)
      {
        if(pre_time < ((double)(signalcomp[i]->ravg_filter[k]->size + 3) / signalcomp[i]->edfparam_0->sf_f))
        {
          pre_time = (double)(signalcomp[i]->ravg_filter[k]->size + 3) / signalcomp[i]->edfparam_0->sf_f;
        }

        reset_ravg_filter(0, signalcomp[i]->ravg_filter[k]);
      }
    }

    if(signalcomp[i]->fir_filter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->fir_filter_cnt; k++)
      {
        if(pre_time < ((double)(fir_filter_size(signalcomp[i]->fir_filter[k]) + 3) / signalcomp[i]->edfparam_0->sf_f))
        {
          pre_time = (double)(fir_filter_size(signalcomp[i]->fir_filter[k]) + 3) / signalcomp[i]->edfparam_0->sf_f;
        }
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      hasprefilter = 1;

      if(pre_time < 10.0)
      {
        pre_time = 10.0;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      hasprefilter = 1;

      if(pre_time < 4.0)
      {
        pre_time = 4.0;
      }
    }

    if(signalcomp[i]->fidfilter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->fidfilter_cnt; k++)
      {
        if(pre_time < ((2.0 * signalcomp[i]->fidfilter_order[k]) / signalcomp[i]->fidfilter_freq[k]))
        {
          pre_time = (2.0 * signalcomp[i]->fidfilter_order[k]) / signalcomp[i]->fidfilter_freq[k];
        }
      }
    }
  }

  if(hasprefilter)
  {
    for(i=0; i<signalcomps; i++)
    {
      if((signalcomp[i]->filter_cnt) || (signalcomp[i]->spike_filter) || (signalcomp[i]->ravg_filter_cnt) ||
         (signalcomp[i]->fidfilter_cnt) || (signalcomp[i]->fir_filter_cnt) || (signalcomp[i]->plif_ecg_filter != NULL) ||
         (signalcomp[i]->plif_eeg_filter != NULL) || (signalcomp[i]->ecg_filter != NULL) || (signalcomp[i]->zratio_filter != NULL))
      {
        signalcomp[i]->edfhdr->prefiltertime = (long long)(pre_time * ((double)TIME_FIXP_SCALING));
        if(signalcomp[i]->edfhdr->prefiltertime>signalcomp[i]->edfhdr->viewtime)
        {
          signalcomp[i]->edfhdr->prefiltertime = signalcomp[i]->edfhdr->viewtime;
          if(signalcomp[i]->edfhdr->prefiltertime<0) signalcomp[i]->edfhdr->prefiltertime = 0;
        }
      }
    }

    totalsize = 0LL;

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->edfhdr->prefiltertime)  signalcomp[i]->records_in_viewbuf = (signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration) - ((signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;
      else signalcomp[i]->records_in_viewbuf = 0;

      signalcomp[i]->viewbufsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

      if(signalcomp[i]->edfhdr->prefiltertime)
      {
        signalcomp[i]->samples_in_prefilterbuf = (signalcomp[i]->records_in_viewbuf - 1) * signalcomp[i]->edfparam_0->smp_per_record;

        signalcomp[i]->samples_in_prefilterbuf
        += (int)(((double)(signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration)
        / (double)signalcomp[i]->edfhdr->long_data_record_duration)
        * (double)signalcomp[i]->edfparam_0->smp_per_record);
      }
      else
      {
        signalcomp[i]->samples_in_prefilterbuf = 0;
      }

      if(!i)
      {
        signalcomp[i]->viewbufoffset = 0;
        totalsize = signalcomp[i]->viewbufsize;
      }
      else
      {
        skip = 0;

        for(j=0; j<i; j++)
        {
          if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
          {
            skip = 1;
            signalcomp[i]->viewbufoffset = signalcomp[j]->viewbufoffset;
            signalcomp[i]->records_in_viewbuf = signalcomp[j]->records_in_viewbuf;
            signalcomp[i]->viewbufsize = signalcomp[j]->viewbufsize;
            break;
          }
        }

        if(!skip)
        {
          signalcomp[i]->viewbufoffset = totalsize;
          totalsize += signalcomp[i]->viewbufsize;
        }
      }
    }

    if(viewbuf!=NULL)
    {
      free(viewbuf);
      viewbuf = NULL;
      totalviewbufsize_bytes = 0;
    }

//    printf("debug: totalsize is: %llu\n", totalsize);
#if defined(__LP64__) || defined(__MINGW64__)
    if(totalsize >= (UINT_MAX * 32LL))
    {
      live_stream_active = 0;

      QMessageBox::critical(this, "Error", "Somehow you hit the memory limit...\n"
                                           "Decrease the timescale and/or number of traces and try again.");
      remove_all_signals();
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
#else
    if(totalsize >= UINT_MAX)
    {
      live_stream_active = 0;
      QMessageBox::critical(this, "Error", "You have hit the memory limit of 4.2GB.\n"
                                           "Decrease the timescale and/or number of traces and try again.\n"
                                           "Consider switching to the 64-bit version.");
      remove_all_signals();
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
#endif
    viewbuf = (char *)malloc(totalsize);
    if(viewbuf==NULL)
    {
      live_stream_active = 0;
      QMessageBox::critical(this, "Error", "Internal error: Memory allocation error:\n\"prefilterbuf\"");
      remove_all_signals();
      totalviewbufsize_bytes = 0;
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
    totalviewbufsize_bytes = totalsize;

    for(i=0; i<signalcomps; i++)
    {
      if(!i)
      {
        datarecords = (signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration;

        signalcomp[i]->prefilter_starttime = datarecords * signalcomp[i]->edfhdr->long_data_record_duration;

        if((signalcomp[i]->viewbufsize > 0) && (datarecords < signalcomp[i]->edfhdr->datarecords))
        {
          fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)(signalcomp[i]->edfhdr->hdrsize + (datarecords * signalcomp[i]->edfhdr->recordsize)), SEEK_SET);

          if(signalcomp[i]->viewbufsize > (unsigned long long)((signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize))
          {
            signalcomp[i]->viewbufsize = (signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize;
          }

          if(fread(viewbuf + signalcomp[i]->viewbufoffset, signalcomp[i]->viewbufsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
          {
            live_stream_active = 0;
            QMessageBox::critical(this, "Error", "A read error occurred. 2");
            remove_all_signals();
            return;
          }
        }
      }
      else
      {
        skip = 0;

        for(j=0; j<i; j++)
        {
          if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
          {
            skip = 1;
            break;
          }
        }

        if(!skip)
        {
          datarecords = (signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration;

          signalcomp[i]->prefilter_starttime = datarecords * signalcomp[i]->edfhdr->long_data_record_duration;

          if((signalcomp[i]->viewbufsize > 0) && (datarecords<signalcomp[i]->edfhdr->datarecords))
          {
            fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)(signalcomp[i]->edfhdr->hdrsize + (datarecords * signalcomp[i]->edfhdr->recordsize)), SEEK_SET);

            if(signalcomp[i]->viewbufsize > (unsigned long long)((signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize))
            {
              signalcomp[i]->viewbufsize = (signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize;
            }

            if(fread(viewbuf + signalcomp[i]->viewbufoffset, signalcomp[i]->viewbufsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
            {
              live_stream_active = 0;
              QMessageBox::critical(this, "Error", "A read error occurred. 3");
              remove_all_signals();
              return;
            }
          }
        }
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->zratio_filter != NULL)
      {
        l_temp = signalcomp[i]->prefilter_starttime % (TIME_FIXP_SCALING * 2LL); // necessary for the Z-ratio filter
        if(l_temp != 0L)
        {
          temp = (TIME_FIXP_SCALING * 2LL) - l_temp;

          l_temp = temp;

          signalcomp[i]->prefilter_reset_sample = (l_temp / signalcomp[i]->edfhdr->long_data_record_duration)
          * signalcomp[i]->edfparam_0->smp_per_record;

          signalcomp[i]->prefilter_reset_sample
          += (int)(((double)(l_temp % signalcomp[i]->edfhdr->long_data_record_duration)
          / (double)signalcomp[i]->edfhdr->long_data_record_duration)
          * (double)signalcomp[i]->edfparam_0->smp_per_record);
        }
        else
        {
          signalcomp[i]->prefilter_reset_sample = 0;
        }

// printf("records_in_viewbuf is %lli\n"
//       "samples_in_prefilterbuf is %i\n"
//       "l_temp is %lli\n"
//       "temp is %i\n"
//       "prefilter_reset_sample is %i\n\n",
//       signalcomp[i]->records_in_viewbuf,
//       signalcomp[i]->samples_in_prefilterbuf,
//       l_temp,
//       temp,
//       signalcomp[i]->prefilter_reset_sample);

      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if((!signalcomp[i]->filter_cnt) && (!signalcomp[i]->spike_filter) && (!signalcomp[i]->ravg_filter_cnt) &&
         (!signalcomp[i]->fidfilter_cnt) && (!signalcomp[i]->fir_filter_cnt) && (!signalcomp[i]->plif_ecg_filter) &&
         (!signalcomp[i]->plif_eeg_filter) && (signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL)) continue;

      for(s=0; s<signalcomp[i]->samples_in_prefilterbuf; s++)
      {
        dig_value = 0.0;

        for(k=0; k<signalcomp[i]->num_of_signals; k++)
        {
          if(signalcomp[i]->edfhdr->bdf)
          {
            var.two[0] = *((unsigned short *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].datrec_offset
              + ((s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record) * 3)));

            var.four[2] = *((unsigned char *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].datrec_offset
              + ((s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record) * 3)
              + 2));

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            d_temp = var.one_signed;
          }

          if(signalcomp[i]->edfhdr->edf)
          {
            d_temp = *(((short *)(
            viewbuf
            + signalcomp[i]->viewbufoffset
            + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
            + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].datrec_offset))
            + (s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record));
          }

          d_temp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].offset;
          d_temp *= signalcomp[i]->factor[k];

          dig_value += d_temp;
        }

        if(signalcomp[i]->spike_filter)
        {
          dig_value = run_spike_filter(dig_value, signalcomp[i]->spike_filter);
        }

        for(j=0; j<signalcomp[i]->math_func_cnt_before; j++)
        {
          dig_value = run_math_func(dig_value, signalcomp[i]->math_func_before[j]);
        }

        for(j=0; j<signalcomp[i]->filter_cnt; j++)
        {
          dig_value = first_order_filter(dig_value, signalcomp[i]->filter[j]);
        }

        for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
        {
          dig_value = run_ravg_filter(dig_value, signalcomp[i]->ravg_filter[j]);
        }

        for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
        {
          dig_value = signalcomp[i]->fidfuncp[j](signalcomp[i]->fidbuf[j], dig_value);
        }

        for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
        {
          dig_value = run_fir_filter(dig_value, signalcomp[i]->fir_filter[j]);
        }

        for(j=0; j<signalcomp[i]->math_func_cnt_after; j++)
        {
          dig_value = run_math_func(dig_value, signalcomp[i]->math_func_after[j]);
        }

        if(signalcomp[i]->plif_ecg_filter)
        {
          dig_value = plif_ecg_run_subtract_filter(dig_value, signalcomp[i]->plif_ecg_filter);
        }

        if(signalcomp[i]->plif_eeg_filter)
        {
          dig_value = plif_eeg_run_subtract_filter(dig_value, signalcomp[i]->plif_eeg_filter);
        }

        if(signalcomp[i]->ecg_filter != NULL)
        {
          if(s == 0)
          {
            reset_ecg_filter(signalcomp[i]->ecg_filter);
          }

          dig_value = run_ecg_filter(dig_value, signalcomp[i]->ecg_filter);
        }

        if(signalcomp[i]->zratio_filter != NULL)
        {
          if(s == signalcomp[i]->prefilter_reset_sample)
          {
            reset_zratio_filter(signalcomp[i]->zratio_filter);
          }

          dig_value = run_zratio_filter(dig_value, signalcomp[i]->zratio_filter);
        }
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->samples_in_prefilterbuf > 0)
      {
        if(signalcomp[i]->spike_filter)
        {
          spike_filter_save_buf(signalcomp[i]->spike_filter);
        }

        for(j=0; j<signalcomp[i]->filter_cnt; j++)
        {
          signalcomp[i]->filterpreset_a[j] = signalcomp[i]->filter[j]->old_input;
          signalcomp[i]->filterpreset_b[j] = signalcomp[i]->filter[j]->old_output;
        }

        for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
        {
          ravg_filter_save_buf(signalcomp[i]->ravg_filter[j]);
        }

        for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
        {
          memcpy(signalcomp[i]->fidbuf2[j], signalcomp[i]->fidbuf[j], fid_run_bufsize(signalcomp[i]->fid_run[j]));
        }

        for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
        {
          fir_filter_save_buf(signalcomp[i]->fir_filter[j]);
        }

        if(signalcomp[i]->plif_ecg_filter)
        {
          plif_ecg_subtract_filter_state_copy(signalcomp[i]->plif_ecg_filter_sav, signalcomp[i]->plif_ecg_filter);
        }

        if(signalcomp[i]->plif_eeg_filter)
        {
          plif_eeg_subtract_filter_state_copy(signalcomp[i]->plif_eeg_filter_sav, signalcomp[i]->plif_eeg_filter);
        }

        if(signalcomp[i]->ecg_filter != NULL)
        {
          ecg_filter_save_buf(signalcomp[i]->ecg_filter);
        }

        if(signalcomp[i]->zratio_filter != NULL)
        {
          zratio_filter_save_buf(signalcomp[i]->zratio_filter);
        }
      }
    }
  }

  totalsize = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfhdr->viewtime>=0)  signalcomp[i]->records_in_viewbuf = ((pagetime + (signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration)) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;
    else  signalcomp[i]->records_in_viewbuf = ((pagetime + ((-(signalcomp[i]->edfhdr->viewtime)) % signalcomp[i]->edfhdr->long_data_record_duration)) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;

    signalcomp[i]->viewbufsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

//     printf("viewbuf test: signalcomp: %i  records_in_viewbuf: %lli  recordsize: %i  viewbufsize: %i\n",
//            i, signalcomp[i]->records_in_viewbuf, signalcomp[i]->edfhdr->recordsize, signalcomp[i]->viewbufsize);

    signalcomp[i]->samples_on_screen = (int)(((double)pagetime / (double)signalcomp[i]->edfhdr->long_data_record_duration) * (double)signalcomp[i]->edfparam_0->smp_per_record);

    if(signalcomp[i]->edfhdr->viewtime<0)
    {
      d_temp =
        (((double)(-(signalcomp[i]->edfhdr->viewtime)))
        / (double)signalcomp[i]->edfhdr->long_data_record_duration)
        * (double)signalcomp[i]->edfparam_0->smp_per_record;

      signalcomp[i]->sample_start = d_temp + 0.5;

      if(signalcomp[i]->sample_start > 0x7fffffffLL)
      {
        signalcomp[i]->sample_start = 0x7fffffffLL;
      }
    }
    else
    {
      signalcomp[i]->sample_start = 0;
    }

    if(signalcomp[i]->edfhdr->viewtime>=0)
    {
      signalcomp[i]->sample_timeoffset_part = ((double)(signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration) /
                                               (double)signalcomp[i]->edfhdr->long_data_record_duration) * (double)signalcomp[i]->edfparam_0->smp_per_record;

      signalcomp[i]->sample_timeoffset = (int)(signalcomp[i]->sample_timeoffset_part);

      signalcomp[i]->sample_timeoffset_part -= signalcomp[i]->sample_timeoffset;
    }
    else
    {
      signalcomp[i]->sample_timeoffset_part = ((double)(-signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration) /
                                               (double)signalcomp[i]->edfhdr->long_data_record_duration) * (double)signalcomp[i]->edfparam_0->smp_per_record;

      signalcomp[i]->sample_timeoffset = (int)(signalcomp[i]->sample_timeoffset_part);

      signalcomp[i]->sample_timeoffset_part -= signalcomp[i]->sample_timeoffset;

      if(signalcomp[i]->sample_timeoffset_part >= 0.5)  signalcomp[i]->sample_timeoffset_part -= 1;

      signalcomp[i]->sample_timeoffset_part = -signalcomp[i]->sample_timeoffset_part;

      signalcomp[i]->sample_timeoffset = 0;
    }

    if(!i)
    {
      signalcomp[i]->viewbufoffset = 0;
      totalsize = signalcomp[i]->viewbufsize;
    }
    else
    {
      skip = 0;

      for(j=0; j<i; j++)
      {
        if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
        {
          skip = 1;
          signalcomp[i]->viewbufoffset = signalcomp[j]->viewbufoffset;
          signalcomp[i]->records_in_viewbuf = signalcomp[j]->records_in_viewbuf;
          signalcomp[i]->viewbufsize = signalcomp[j]->viewbufsize;
          break;
        }
      }

      if(!skip)
      {
        signalcomp[i]->viewbufoffset = totalsize;
        totalsize += signalcomp[i]->viewbufsize;
      }
    }
  }

  if(viewbuf!=NULL)
  {
    free(viewbuf);
    viewbuf = NULL;
    totalviewbufsize_bytes = 0;
  }

  if(totalsize)
  {
//    printf("debug: totalsize is: %llu\n", totalsize;
#if defined(__LP64__) || defined(__MINGW64__)
    if(totalsize >= (0xffffffffULL * 32ULL))
    {
      live_stream_active = 0;

      QMessageBox::critical(this, "Error", "Somehow you hit the memory limit...\n"
                                           "Decrease the timescale and/or number of traces and try again.");
      remove_all_signals();
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
#else
    if(totalsize >= 0xffffffffULL)
    {
      live_stream_active = 0;
      QMessageBox::critical(this, "Error", "You have hit the memory limit of 4.2GB.\n"
                                           "Decrease the timescale and/or number of traces and try again.\n"
                                           "Consider switching to the 64-bit version.");
      remove_all_signals();
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
#endif
    viewbuf = (char *)malloc(totalsize);
    if(viewbuf==NULL)
    {
      live_stream_active = 0;
      QMessageBox::critical(this, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.\n"
                                           "Decrease the timescale and try again.");
      remove_all_signals();
      totalviewbufsize_bytes = 0;
      if(pagetime > TIME_FIXP_SCALING)  pagetime = TIME_FIXP_SCALING;
      return;
    }
    totalviewbufsize_bytes = totalsize;
  }

  for(i=0; i<signalcomps; i++)
  {
    if(!i)
    {
      if(signalcomp[i]->edfhdr->viewtime>=0)
      {
        datarecords = signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration;
      }
      else
      {
        datarecords = 0;
      }

      dif = signalcomp[i]->edfhdr->datarecords - datarecords;

//       printf("signalcomp[%i]->viewbufoffset: %llu\n"
//              "signalcomp[%i]->records_in_viewbuf: %llu\n"
//              "signalcomp[%i]->edfhdr->recordsize: %i\n"
//              "dif: %lli\n"
//              "datarecords: %lli\n",
//              i, signalcomp[i]->viewbufoffset, i, signalcomp[i]->records_in_viewbuf, i, signalcomp[i]->edfhdr->recordsize, dif, datarecords);

      if(dif <= 0)
      {
        memset(viewbuf + signalcomp[i]->viewbufoffset, 0, signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize);

        signalcomp[i]->sample_stop = 0;
      }
      else
      {
        if(dif < (long long)signalcomp[i]->records_in_viewbuf)
        {
          readsize = dif * signalcomp[i]->edfhdr->recordsize;

          memset(viewbuf + signalcomp[i]->viewbufoffset + readsize, 0, (signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize) - readsize);

          signalcomp[i]->sample_stop = (dif * signalcomp[i]->edfparam_0->smp_per_record) - signalcomp[i]->sample_timeoffset;
        }
        else
        {
          readsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

          signalcomp[i]->sample_stop = signalcomp[i]->samples_on_screen;
        }

        l_temp = signalcomp[i]->edfhdr->hdrsize;
        l_temp += (datarecords * signalcomp[i]->edfhdr->recordsize);

        fseeko(signalcomp[i]->edfhdr->file_hdl, l_temp, SEEK_SET);

        if(fread(viewbuf + signalcomp[i]->viewbufoffset, readsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
        {
          live_stream_active = 0;
          QMessageBox::critical(this, "Error", "A read error occurred. 5");
          remove_all_signals();
          return;
        }
      }
    }
    else
    {
      skip = 0;

      for(j=0; j<i; j++)
      {
        if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
        {
          skip = 1;
          break;
        }
      }

      if(signalcomp[i]->edfhdr->viewtime>=0)
      {
        datarecords = signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration;
      }
      else
      {
        datarecords = 0;
      }

      dif = signalcomp[i]->edfhdr->datarecords - datarecords;

      if(dif<=0)
      {
        if(!skip)
        {
          memset(viewbuf + signalcomp[i]->viewbufoffset, 0, signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize);
        }

        signalcomp[i]->sample_stop = 0;
      }
      else
      {
        if((unsigned long long)dif < signalcomp[i]->records_in_viewbuf)
        {
          if(!skip)
          {
            readsize = dif * signalcomp[i]->edfhdr->recordsize;

//             printf("viewbuf test: signalcomp: %i  viewbufoffset: %i  readsize: %i  records_in_viewbuf: %lli  recordsize: %i\n"
//                    "viewtime: %lli  datarecords: %lli  dif: %i  readsize: %i  (records_in_viewbuf * recordsize): %lli\n"
//                    "viewbufsize: %i\n",
//                    i, signalcomp[i]->viewbufoffset, readsize, signalcomp[i]->records_in_viewbuf, signalcomp[i]->edfhdr->recordsize,
//                    signalcomp[i]->edfhdr->viewtime, signalcomp[i]->edfhdr->datarecords, dif, readsize,
//                    signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize, signalcomp[i]->viewbufsize);

            memset(viewbuf + signalcomp[i]->viewbufoffset + readsize, 0, (signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize) - readsize);
          }

          signalcomp[i]->sample_stop = (dif * signalcomp[i]->edfparam_0->smp_per_record) - signalcomp[i]->sample_timeoffset;
        }
        else
        {
          if(!skip)
          {
            readsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;
          }

          signalcomp[i]->sample_stop = signalcomp[i]->samples_on_screen;
        }

        if(!skip)
        {
          l_temp = signalcomp[i]->edfhdr->hdrsize;
          l_temp += (datarecords * signalcomp[i]->edfhdr->recordsize);

          fseeko(signalcomp[i]->edfhdr->file_hdl, l_temp, SEEK_SET);

          if(fread(viewbuf + signalcomp[i]->viewbufoffset, readsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
          {
            live_stream_active = 0;
            QMessageBox::critical(this, "Error", "A read error occurred. 6");
            remove_all_signals();
            return;
          }
        }
      }
    }

    signalcomp[i]->sample_stop += signalcomp[i]->sample_start;
  }

  if(files_open && (!signal_averaging_active))
  {
    viewtime_string_128[0] = 0;

    pagetime_string_128[0] = 0;

    if(annot_editor_highres)
    {
      precision = 6;
      prec_scale = 10;
    }
    else
    {
      precision = 3;
      prec_scale = 10000;
    }

    if(viewtime_indicator_type == VIEWTIME_INDICATOR_TYPE_DATE_REAL_RELATIVE)
    {
      utc_to_date_time((edfheaderlist[sel_viewtime]->utc_starttime_hr + edfheaderlist[sel_viewtime]->viewtime) / TIME_FIXP_SCALING, &date_time_str);

      snprintf(viewtime_string_128, 128, "%2i-%s ", date_time_str.day, date_time_str.month_str);
    }

    if(edfheaderlist[sel_viewtime]->viewtime>=0LL)
    {
      if(viewtime_indicator_type != VIEWTIME_INDICATOR_TYPE_RELATIVE)
      {
        snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), "%2i:%02i:%02i.%0*i (",
                (int)((((edfheaderlist[sel_viewtime]->starttime_hr + edfheaderlist[sel_viewtime]->viewtime) / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                (int)((((edfheaderlist[sel_viewtime]->starttime_hr + edfheaderlist[sel_viewtime]->viewtime) / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                (int)(((edfheaderlist[sel_viewtime]->starttime_hr + edfheaderlist[sel_viewtime]->viewtime) / TIME_FIXP_SCALING) % 60LL),
                precision,
                (int)(((edfheaderlist[sel_viewtime]->starttime_hr + edfheaderlist[sel_viewtime]->viewtime) % TIME_FIXP_SCALING) / prec_scale));
      }

      snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), "%i:%02i:%02i.%0*i",
              (int)((edfheaderlist[sel_viewtime]->viewtime / TIME_FIXP_SCALING)/ 3600LL),
              (int)(((edfheaderlist[sel_viewtime]->viewtime / TIME_FIXP_SCALING) % 3600LL) / 60LL),
              (int)((edfheaderlist[sel_viewtime]->viewtime / TIME_FIXP_SCALING) % 60LL),
              precision,
              (int)((edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING) / prec_scale));

      if(viewtime_indicator_type != VIEWTIME_INDICATOR_TYPE_RELATIVE)
      {
        snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), ")");
      }
    }
    else
    {
      l_temp = (edfheaderlist[sel_viewtime]->starttime_hr + edfheaderlist[sel_viewtime]->viewtime) % (86400LL * TIME_FIXP_SCALING);
      if(l_temp<=0)
      {
        l_temp += (86400LL * TIME_FIXP_SCALING);
      }

      if(viewtime_indicator_type != VIEWTIME_INDICATOR_TYPE_RELATIVE)
      {
        snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), "%2i:%02i:%02i.%0*i (",
                (int)((((l_temp) / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                (int)((((l_temp) / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                (int)(((l_temp) / TIME_FIXP_SCALING) % 60LL),
                precision,
                (int)(((l_temp) % TIME_FIXP_SCALING) / prec_scale));
      }

      l_temp = -edfheaderlist[sel_viewtime]->viewtime;

      snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), "-%i:%02i:%02i.%0*i",
              (int)((l_temp / TIME_FIXP_SCALING)/ 3600LL),
              (int)(((l_temp / TIME_FIXP_SCALING) % 3600LL) / 60LL),
              (int)((l_temp / TIME_FIXP_SCALING) % 60LL),
              precision,
              (int)((l_temp % TIME_FIXP_SCALING) / prec_scale));

      if(viewtime_indicator_type != VIEWTIME_INDICATOR_TYPE_RELATIVE)
      {
        snprintf(viewtime_string_128 + strlen(viewtime_string_128), 128 - strlen(viewtime_string_128), ")");
      }
    }

    if(pagetime >= (3600LL * TIME_FIXP_SCALING))
    {
      snprintf(pagetime_string_128, 128, "%i:%02i:%02i.%0*i",
              ((int)(pagetime / TIME_FIXP_SCALING)) / 3600,
              (((int)(pagetime / TIME_FIXP_SCALING)) % 3600) / 60,
              ((int)(pagetime / TIME_FIXP_SCALING)) % 60,
              precision,
              (int)((pagetime % TIME_FIXP_SCALING) / prec_scale));
    }
    else if(pagetime > (600LL * TIME_FIXP_SCALING))
      {
        if(display_pagetime_mmsec)
        {
          snprintf(pagetime_string_128, 128, "%i:%02i.%0*i  (%i mm/min)",
                  ((int)(pagetime / TIME_FIXP_SCALING)) / 60,
                  ((int)(pagetime / TIME_FIXP_SCALING)) % 60,
                  precision,
                  (int)((pagetime % TIME_FIXP_SCALING) / prec_scale),
                  (int)((maincurve->width() * x_pixelsizefactor * 600) / (((double)pagetime) / TIME_FIXP_SCALING) + 0.5));
        }
        else
        {
          snprintf(pagetime_string_128, 128, "%i:%02i.%0*i",
                  ((int)(pagetime / TIME_FIXP_SCALING)) / 60,
                  ((int)(pagetime / TIME_FIXP_SCALING)) % 60,
                  precision,
                  (int)((pagetime % TIME_FIXP_SCALING) / prec_scale));
        }
      }
      else if(pagetime >= (60LL * TIME_FIXP_SCALING))
        {
          if(display_pagetime_mmsec)
          {
            snprintf(pagetime_string_128, 128, "%i:%02i.%0*i  (%.1f mm/sec)",
                    ((int)(pagetime / TIME_FIXP_SCALING)) / 60,
                    ((int)(pagetime / TIME_FIXP_SCALING)) % 60,
                    precision,
                    (int)((pagetime % TIME_FIXP_SCALING) / prec_scale),
                    (maincurve->width() * x_pixelsizefactor * 10) / (((double)pagetime) / TIME_FIXP_SCALING));
          }
          else
          {
            snprintf(pagetime_string_128, 128, "%i:%02i.%0*i",
                    ((int)(pagetime / TIME_FIXP_SCALING)) / 60,
                    ((int)(pagetime / TIME_FIXP_SCALING)) % 60,
                    precision,
                    (int)((pagetime % TIME_FIXP_SCALING) / prec_scale));
          }
        }
        else if(pagetime >= TIME_FIXP_SCALING)
          {
            if(display_pagetime_mmsec)
            {
              snprintf(pagetime_string_128, 128, "%i.%0*i sec  (%i mm/sec)",
                      (int)(pagetime / TIME_FIXP_SCALING),
                      precision,
                      (int)((pagetime % TIME_FIXP_SCALING) / prec_scale),
                      (int)((maincurve->width() * x_pixelsizefactor * 10) / (((double)pagetime) / TIME_FIXP_SCALING) + 0.5));
            }
            else
            {
              snprintf(pagetime_string_128, 128, "%i.%0*i sec",
                      (int)(pagetime / TIME_FIXP_SCALING),
                      precision,
                      (int)((pagetime % TIME_FIXP_SCALING) / prec_scale));
            }
          }
          else
          {
            convert_to_metric_suffix(pagetime_string_128, (double)pagetime / TIME_FIXP_SCALING, 3, 64);

            strlcat(pagetime_string_128, "S", 128);

            if((pagetime >= (TIME_FIXP_SCALING / 10)) && display_pagetime_mmsec)
            {
              snprintf(pagetime_string_128 + strlen(pagetime_string_128), 128 - strlen(pagetime_string_128), "  (%i mm/sec)",
                      (int)((maincurve->width() * x_pixelsizefactor * 10) / (((double)pagetime) / TIME_FIXP_SCALING) + 0.5));
            }
          }

    remove_trailing_zeros(viewtime_string_128);
    remove_trailing_zeros(pagetime_string_128);
  }

  if(!signal_averaging_active)
  {
    if(print_to_edf_active)
    {
      print_to_edf_active = 0;
    }
    else
    {
      if(signalcomps && (!live_stream_active))
      {
        positionslider->blockSignals(true);

        long long record_duration = edfheaderlist[sel_viewtime]->recording_duration_hr;

        record_duration -= pagetime;

        emit file_position_changed(edfheaderlist[sel_viewtime]->viewtime);

        if(edfheaderlist[sel_viewtime]->viewtime <= 0)
        {
          positionslider->setValue(0);
        }
        else
        {
          if(edfheaderlist[sel_viewtime]->viewtime >= record_duration)
          {
            positionslider->setValue(1000000);
          }
          else
          {
            if(record_duration < (long long)pagetime)
            {
              positionslider->setValue(1000000);
            }
            else
            {
              if(record_duration > 1e12)
              {
                positionslider->setValue(edfheaderlist[sel_viewtime]->viewtime / (record_duration / 1000000LL));
              }
              else
              {
                positionslider->setValue(edfheaderlist[sel_viewtime]->viewtime * 1000000LL / record_duration);
              }
            }
          }
        }

        slidertoolbar->setEnabled(true);
      }
      else
      {
        slidertoolbar->setEnabled(false);

        positionslider->blockSignals(true);
      }

      maincurve->drawCurve_stage_1();

      if(signalcomps && (!live_stream_active))
      {
        positionslider->blockSignals(false);
      }
    }

    for(r=0; r<MAXSPECTRUMDOCKS; r++)
    {
      if(spectrumdock[r]->dock->isVisible())
      {
        spectrumdock[r]->rescan();
      }
    }
  }

//   printf("\n");
//
//   for(int n=0; n<signalcomps; n++)
//   {
//     printf("signalcomp: %i  filenum: %i  signal: %i  viewbufoffset: %i  buf_offset: %i\n",
//            n,
//            signalcomp[n]->filenum, signalcomp[n]->edfsignal[0],
//            signalcomp[n]->viewbufoffset,
//            signalcomp[n]->edfhdr->edfparam[signalcomp[n]->edfsignal[0]].datrec_offset);
//   }
}

















