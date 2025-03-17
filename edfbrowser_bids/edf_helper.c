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



#include "edf_helper.h"


#define EDF_LABEL_TYPE_NUM  (7)


static const char edf_label_types[EDF_LABEL_TYPE_NUM][32]=
{
  "EEG             ",
  "ECG             ",
  "EOG             ",
  "ERG             ",
  "EMG             ",
  "MEG             ",
  "MCG             "
};


int edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdrblck_t *hdr)
{
  int timestamp_digits;

  char scratchpad_256[256]={""};

  long long time;


  if(hdr==NULL)
  {
    return -1;
  }

  time = (hdr->datarecords * hdr->long_data_record_duration) / TIME_FIXP_SCALING;

#ifdef _WIN32
  timestamp_digits = __mingw_snprintf(scratchpad_256, 256, "%lli", time);
#else
  timestamp_digits = snprintf(scratchpad_256, 256, "%lli", time);
#endif

  return timestamp_digits;
}


int edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdrblck_t *hdr)
{
  int i, j,
      timestamp_decimals;


  if(hdr==NULL)
  {
    return -1;
  }

  j = 10;

  for(timestamp_decimals=7; timestamp_decimals>0; timestamp_decimals--)
  {
    if(hdr->long_data_record_duration % j)
    {
      break;
    }

    j *= 10;
  }

  if((hdr->edfplus==1)||(hdr->bdfplus==1))
  {
    j = 10;

    for(i=7; i>0; i--)
    {
      if(hdr->starttime_subsec % j)
      {
        break;
      }

      j *= 10;
    }

    if(i > timestamp_decimals)
    {
      timestamp_decimals = i;
    }
  }

  return timestamp_decimals;
}


int strip_types_from_label(char *label)
{
  int i, type, len;

  len = strlen(label);
  if(len<5)
  {
    return 0;
  }

  for(type=0; type<EDF_LABEL_TYPE_NUM; type++)
  {
    if(!strncmp(label, edf_label_types[type], 4))
    {
      break;
    }
  }
  if(type == EDF_LABEL_TYPE_NUM)
  {
    return 0;
  }

  if(label[4] == ' ')
  {
    return 0;
  }

  for(i=0; i<(len-4); i++)
  {
    label[i] = label[i+4];
  }

  for(; i<len; i++)
  {
    label[i] = ' ';
  }

  return (type + 1);
}


int utc_to_edf_startdate(long long utc_time, char *dest)
{
  date_time_t dt;

  utc_to_date_time(utc_time, &dt);

  if((dt.year >= 1985) && (dt.year <= 2084))
  {
    snprintf(dest, 12, "%02i-%.3s-%04i", dt.day, dt.month_str, dt.year);

    return 0;
  }
  else
  {
    return -1;
  }
}


int to_edf_startdate(int day, int month, int year, char *dest)
{
  char month_str[4]="";

  if((year < 1985) || (year > 2084) ||
     (month < 1)   || (month > 12)  ||
     (day < 1)     || (day > 31))
  {
    return -1;
  }

  switch(month)
  {
    case  1 : strlcpy(month_str, "JAN", 4);
              break;
    case  2 : strlcpy(month_str, "FEB", 4);
              break;
    case  3 : strlcpy(month_str, "MAR", 4);
              break;
    case  4 : strlcpy(month_str, "APR", 4);
              break;
    case  5 : strlcpy(month_str, "MAY", 4);
              break;
    case  6 : strlcpy(month_str, "JUN", 4);
              break;
    case  7 : strlcpy(month_str, "JUL", 4);
              break;
    case  8 : strlcpy(month_str, "AUG", 4);
              break;
    case  9 : strlcpy(month_str, "SEP", 4);
              break;
    case 10 : strlcpy(month_str, "OCT", 4);
              break;
    case 11 : strlcpy(month_str, "NOV", 4);
              break;
    case 12 : strlcpy(month_str, "DEC", 4);
              break;
    default : strlcpy(month_str, "ERR", 4);
              break;
  }

  snprintf(dest, 12, "%02i-%.3s-%04i", day, month_str, year);

  return 0;
}


void run_filter_list(sigcompblck_t *sig_comp, unsigned long long s, double *val)
{
  int k, n, runin_samples;

  if(sig_comp->spike_filter)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime <= 0)
      {
        reset_spike_filter(sig_comp->spike_filter);
      }
      else
      {
        spike_filter_restore_buf(sig_comp->spike_filter);
      }
    }

    *val = run_spike_filter(*val, sig_comp->spike_filter);
  }

  for(k=0; k<sig_comp->math_func_cnt_before; k++)
  {
    *val = run_math_func(*val, sig_comp->math_func_before[k]);
  }

  for(k=0; k<sig_comp->filter_cnt; k++)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime == 0)
      {
        reset_filter(*val, sig_comp->filter[k]);
      }
      else
      {
        sig_comp->filter[k]->old_input = sig_comp->filterpreset_a[k];
        sig_comp->filter[k]->old_output = sig_comp->filterpreset_b[k];
      }
    }

    *val = first_order_filter(*val, sig_comp->filter[k]);
  }

  for(k=0; k<sig_comp->ravg_filter_cnt; k++)
  {
    if(s==sig_comp->sample_start)
    {
      if((sig_comp->edfhdr->viewtime <= 0) && sig_comp->ravg_filter_setup[k])
      {
        reset_ravg_filter(*val, sig_comp->ravg_filter[k]);
      }
      else
      {
        ravg_filter_restore_buf(sig_comp->ravg_filter[k]);
      }

      sig_comp->ravg_filter_setup[k] = 0;
    }

    *val = run_ravg_filter(*val, sig_comp->ravg_filter[k]);
  }

  for(k=0; k<sig_comp->fidfilter_cnt; k++)
  {
    if(s==sig_comp->sample_start)
    {
      if((sig_comp->edfhdr->viewtime <= 0) && sig_comp->fidfilter_setup[k])
      {
        runin_samples = sig_comp->edfparam_0->sf_f / sig_comp->fidfilter_freq[k];

        runin_samples *= 26;

        if(runin_samples < 10)
        {
          runin_samples = 10;
        }

        for(n=0; n<runin_samples; n++)
        {
          sig_comp->fidfuncp[k](sig_comp->fidbuf[k], *val);
        }

        memcpy(sig_comp->fidbuf2[k], sig_comp->fidbuf[k], fid_run_bufsize(sig_comp->fid_run[k]));
      }
      else
      {
        memcpy(sig_comp->fidbuf[k], sig_comp->fidbuf2[k], fid_run_bufsize(sig_comp->fid_run[k]));
      }

      sig_comp->fidfilter_setup[k] = 0;
    }

    *val = sig_comp->fidfuncp[k](sig_comp->fidbuf[k], *val);
  }

  for(k=0; k<sig_comp->fir_filter_cnt; k++)
  {
    if(s==sig_comp->sample_start)
    {
      if((sig_comp->edfhdr->viewtime<=0) && sig_comp->fir_filter_setup[k])
      {
        reset_fir_filter(*val, sig_comp->fir_filter[k]);
      }
      else
      {
        fir_filter_restore_buf(sig_comp->fir_filter[k]);
      }

      sig_comp->fir_filter_setup[k] = 0;
    }

    *val = run_fir_filter(*val, sig_comp->fir_filter[k]);
  }

  for(k=0; k<sig_comp->math_func_cnt_after; k++)
  {
    *val = run_math_func(*val, sig_comp->math_func_after[k]);
  }

  if(sig_comp->plif_ecg_filter)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime<=0)
      {
        plif_ecg_reset_subtract_filter(sig_comp->plif_ecg_filter, 0);
      }
      else
      {
        plif_ecg_subtract_filter_state_copy(sig_comp->plif_ecg_filter, sig_comp->plif_ecg_filter_sav);
      }
    }

    *val = plif_ecg_run_subtract_filter(*val, sig_comp->plif_ecg_filter);
  }

  if(sig_comp->plif_eeg_filter)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime<=0)
      {
        plif_eeg_reset_subtract_filter(sig_comp->plif_eeg_filter);
      }
      else
      {
        plif_eeg_subtract_filter_state_copy(sig_comp->plif_eeg_filter, sig_comp->plif_eeg_filter_sav);
      }
    }

    *val = plif_eeg_run_subtract_filter(*val, sig_comp->plif_eeg_filter);
  }

  if(sig_comp->ecg_filter != NULL)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime <= 0LL)
      {
        reset_ecg_filter(sig_comp->ecg_filter);
      }
      else
      {
        ecg_filter_restore_buf(sig_comp->ecg_filter);
      }
    }

    *val = run_ecg_filter(*val, sig_comp->ecg_filter);
  }

  if(sig_comp->zratio_filter != NULL)
  {
    if(s==sig_comp->sample_start)
    {
      if(sig_comp->edfhdr->viewtime <= 0LL)
      {
        reset_zratio_filter(sig_comp->zratio_filter);
      }
      else
      {
        zratio_filter_restore_buf(sig_comp->zratio_filter);
      }
    }

    *val = run_zratio_filter(*val, sig_comp->zratio_filter);
  }
}


