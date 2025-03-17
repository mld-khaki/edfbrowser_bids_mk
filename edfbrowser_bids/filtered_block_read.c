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


#include "filtered_block_read.h"


//#define FBR_DEBUG

#ifdef FBR_DEBUG

#include "edflib.h"

int edf_hdl=-1, edf_chan=0, *edf_ibuf=NULL;

edflib_hdr_t edfhdr;

#endif


flt_blck_rd_t * create_flt_blck_rd(sigcompblck_t *i_signalcomp, int datarecord_or_smpls_cnt, int skip_f, int mode, int *err_descr, double **b_processed_samples_buf)
{
  if(i_signalcomp == NULL)  return NULL;

  flt_blck_rd_t *fbr = (flt_blck_rd_t *)calloc(1, sizeof(flt_blck_rd_t));
  if(fbr == NULL)  return NULL;

  fbr->datarecord_cnt = -1;

  fbr->total_samples = -1LL;

  fbr->signalcomp = i_signalcomp;

  fbr->hdr = fbr->signalcomp->edfhdr;

  fbr->inputfile = fbr->hdr->file_hdl;

  fbr->skip_filters = skip_f;

  fbr->smpl_mode = mode;

  fbr->samples_per_datrec = fbr->signalcomp->edfparam_0->smp_per_record;

  fbr->samples_in_file = fbr->signalcomp->edfparam_0->smpls;

  if(fbr->smpl_mode)
  {
    fbr->total_samples = datarecord_or_smpls_cnt;

    fbr->datarecord_cnt = fbr->total_samples / fbr->samples_per_datrec;

    if(fbr->total_samples % fbr->samples_per_datrec)  fbr->datarecord_cnt++;

    if((fbr->datarecord_cnt > fbr->hdr->datarecords) || (fbr->total_samples < 1))
    {
      free(fbr);
      return NULL;
    }
  }
  else
  {
    fbr->datarecord_cnt = datarecord_or_smpls_cnt;

    if((fbr->datarecord_cnt > fbr->hdr->datarecords) || (fbr->datarecord_cnt < 1))
    {
      free(fbr);
      return NULL;
    }

    fbr->total_samples = (long long)(fbr->samples_per_datrec) * (long long)(fbr->datarecord_cnt);
  }
#ifdef FBR_DEBUG
  printf("init_signalcomp(): samples_per_datrec: %i\n", fbr->samples_per_datrec);

  printf("init_signalcomp(): total_samples: %lli\n", fbr->total_samples);

  printf("init_signalcomp(): datarecord_cnt: %i\n", fbr->datarecord_cnt);
#endif
  *b_processed_samples_buf = (double *)malloc(fbr->total_samples * sizeof(double));
  if(*b_processed_samples_buf == NULL)
  {
    if(err_descr != NULL)  *err_descr = 1;
    free(fbr);
    return NULL;
  }
  fbr->processed_samples_buf = *b_processed_samples_buf;

  fbr->readbuf = (char *)malloc(fbr->hdr->recordsize  * (fbr->datarecord_cnt + 1));
  if(fbr->readbuf == NULL)
  {
    if(err_descr != NULL)  *err_descr = 1;
    free(*b_processed_samples_buf);
    *b_processed_samples_buf = NULL;
    free(fbr);
    return NULL;
  }

  fbr->bitvalue = fbr->signalcomp->edfparam_0->bitvalue;
#ifdef FBR_DEBUG
  fbr->edf_hdl = -1;

  if(fbr->smpl_mode)
  {
    if(!edfopen_file_readonly(fbr->signalcomp->edfhdr->filename, &(fbr->edfhdr), 0))
    {
      fbr->edf_hdl = fbr->edfhdr.handle;

      printf("init_signalcomp(): datarecord_duration: %lli    smp_in_datarecord: %i    sf: %f Hz   total samples in file: %lli\n",
             fbr->edfhdr.datarecord_duration, fbr->edfhdr.signalparam[0].smp_in_datarecord,
             (fbr->edfhdr.signalparam[0].smp_in_datarecord * EDFLIB_TIME_FIXP_SCALING) / (double)(fbr->edfhdr.datarecord_duration),
              fbr->edfhdr.signalparam[0].smp_in_file);

      fbr->edf_ibuf = (int *)malloc(fbr->total_samples * sizeof(int));
      if(fbr->edf_ibuf == NULL)
      {
        edfclose_file(fbr->edf_hdl);
        fbr->edf_hdl = -1;
        printf("malloc error line %i file %s\n", __LINE__, __FILE__);
      }
    }
  }
#endif
  return fbr;
}


long long get_samples_flt_blck_rd(flt_blck_rd_t *fbr)
{
  if(fbr == NULL)  return 0;

  return fbr->total_samples;
}


void free_flt_blck_rd(flt_blck_rd_t *fbr)
{
  if(fbr == NULL)  return;
#ifdef FBR_DEBUG
  if(fbr->edf_hdl >= 0)
  {
    edfclose_file(fbr->edf_hdl);
  }
  free(fbr->edf_ibuf);
#endif
  free(fbr->readbuf);
  free(fbr);
}


int run_flt_blck_rd(flt_blck_rd_t *fbr, int datarecord_or_sample_start)
{
  int j, k, datarecord_start, tmp;

  long long s, s_end, sample_start, s_off=0;

  double dig_value=0,
         f_tmp=0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;
#ifdef FBR_DEBUG
  printf("process_signalcomp(): datarecord_or_sample_start: %i\n", datarecord_or_sample_start);
#endif
  if((fbr->total_samples < 1) || (fbr->datarecord_cnt < 1))
  {
    return -1;
  }

  if(fbr->smpl_mode)
  {
    sample_start = datarecord_or_sample_start;

    if((sample_start < 0) || (sample_start >= fbr->samples_in_file))
    {
      return -2;
    }

    if(fbr->total_samples > (fbr->samples_in_file - sample_start))
    {
      return -3;
    }

    datarecord_start = sample_start / (long long)(fbr->samples_per_datrec);

    s_off = sample_start % (long long)(fbr->samples_per_datrec);

    fbr->datarecord_cnt = ((sample_start + fbr->total_samples) /  (long long)(fbr->samples_per_datrec)) - datarecord_start;

    if((sample_start + fbr->total_samples) % (long long)(fbr->samples_per_datrec))
    {
      fbr->datarecord_cnt++;
    }
  }
  else
  {
    datarecord_start = datarecord_or_sample_start;

    if((datarecord_start < 0) || (datarecord_start >= fbr->hdr->datarecords))
    {
      return -2;
    }

    if(fbr->datarecord_cnt > (fbr->hdr->datarecords - datarecord_start))
    {
      return -3;
    }

    sample_start = (long long)(fbr->datarecord_cnt) * (long long)(fbr->samples_per_datrec);
  }
#ifdef FBR_DEBUG
  printf("process_signalcomp(): sample_start: %lli\n", sample_start);

  printf("process_signalcomp(): s_off: %lli\n", s_off);

  printf("process_signalcomp(): hdr->hdrsize: %i\n", fbr->hdr->hdrsize);

  printf("process_signalcomp(): datarecord_start: %i\n", datarecord_start);

  printf("process_signalcomp(): hdr->recordsize: %i\n", fbr->hdr->recordsize);

  printf("process_signalcomp(): fseek: %lli\n", ((long long)(fbr->hdr->hdrsize)) + (((long long)(datarecord_start) * ((long long) fbr->hdr->recordsize)));

  printf("process_signalcomp(): datarecord_cnt: %i\n", fbr->datarecord_cnt);
#endif
  if(fseeko(fbr->inputfile, ((long long)(fbr->hdr->hdrsize)) + (((long long)datarecord_start) * ((long long)(fbr->hdr->recordsize))), SEEK_SET))
  {
    return -4;
  }

  if(fread(fbr->readbuf, fbr->hdr->recordsize * fbr->datarecord_cnt, 1, fbr->inputfile) != 1)
  {
#ifdef FBR_DEBUG
  printf("process_signalcomp(): error: -5   datarecord_cnt: %i   datarecord_start: %i\n\n", datarecord_cnt, datarecord_start);
#endif
    return -5;
  }

  if((fbr->readbuf == NULL) || (fbr->processed_samples_buf == NULL))
  {
    return -6;
  }
#ifdef FBR_DEBUG
  if(fbr->smpl_mode)
  {
    if(fbr->edf_hdl >= 0)
    {
      if(edfseek(fbr->edf_hdl, 0, sample_start, EDFSEEK_SET) != sample_start)
      {
        printf("error: edfseek(%lli)\n", sample_start);
      }

      if(edfread_digital_samples(fbr->edf_hdl, 0, fbr->total_samples, edf_ibuf) != fbr->total_samples)
      {
        printf("error: edfread_digital_samples(%lli)\n", fbr->total_samples);
      }
    }
  }
#endif
  s_end = fbr->total_samples + s_off;

  for(s=s_off; s<s_end; s++)
  {
    dig_value = 0;

    for(j=0; j<fbr->signalcomp->num_of_signals; j++)
    {
      if(fbr->signalcomp->edfhdr->bdf)
      {
        var.two[0] = *((unsigned short *)(
          fbr->readbuf
          + (fbr->signalcomp->edfhdr->recordsize * (s / fbr->samples_per_datrec))
          + fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].datrec_offset
          + ((s % fbr->samples_per_datrec) * 3LL)));

        var.four[2] = *((unsigned char *)(
          fbr->readbuf
          + (fbr->signalcomp->edfhdr->recordsize * (s / fbr->samples_per_datrec))
          + fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].datrec_offset
          + ((s % fbr->samples_per_datrec) * 3LL)
          + 2LL));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        if(var.one_signed > fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_max)
        {
          var.one_signed = fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_max;
        }
        else if(var.one_signed < fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_min)
          {
            var.one_signed = fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_min;
          }

        f_tmp = var.one_signed;
      }

      if(fbr->signalcomp->edfhdr->edf)
      {
        tmp = *(((short *)(
          fbr->readbuf
          + (fbr->signalcomp->edfhdr->recordsize * (s / fbr->samples_per_datrec))
          + fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].datrec_offset))
          + (s % fbr->samples_per_datrec));

          if(tmp > fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_max)
          {
            tmp = fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_max;
          }
          else if(tmp < fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_min)
            {
              tmp = fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].dig_min;
            }

          f_tmp = tmp;
      }

      f_tmp += fbr->signalcomp->edfhdr->edfparam[fbr->signalcomp->edfsignal[j]].offset;
      f_tmp *= fbr->signalcomp->factor[j];

      dig_value += f_tmp;
    }
#ifdef FBR_DEBUG
//     if(((dig_value - edf_ibuf[s - s_off]) > 1) || ((dig_value - edf_ibuf[s - s_off]) < -1 ))
//     {
//       printf("dig_value: %f   edf_ibuf[%lli]: %i    datarecord_start: %i\n", dig_value, s - s_off, edf_ibuf[s - s_off], datarecord_start);
//     }
#endif
    if(!fbr->skip_filters)
    {
      if(fbr->signalcomp->spike_filter)
      {
        dig_value = run_spike_filter(dig_value, fbr->signalcomp->spike_filter);
      }

      for(k=0; k<fbr->signalcomp->math_func_cnt_before; k++)
      {
        dig_value = run_math_func(dig_value, fbr->signalcomp->math_func_before[k]);
      }

      for(k=0; k<fbr->signalcomp->filter_cnt; k++)
      {
        dig_value = first_order_filter(dig_value, fbr->signalcomp->filter[k]);
      }

      for(k=0; k<fbr->signalcomp->ravg_filter_cnt; k++)
      {
        dig_value = run_ravg_filter(dig_value, fbr->signalcomp->ravg_filter[k]);
      }

      for(k=0; k<fbr->signalcomp->fidfilter_cnt; k++)
      {
        dig_value = fbr->signalcomp->fidfuncp[k](fbr->signalcomp->fidbuf[k], dig_value);
      }

      for(k=0; k<fbr->signalcomp->fir_filter_cnt; k++)
      {
        dig_value = run_fir_filter(dig_value, fbr->signalcomp->fir_filter[k]);
      }

      for(k=0; k<fbr->signalcomp->math_func_cnt_after; k++)
      {
        dig_value = run_math_func(dig_value, fbr->signalcomp->math_func_after[k]);
      }

      if(fbr->signalcomp->plif_ecg_filter)
      {
        dig_value = plif_ecg_run_subtract_filter(dig_value, fbr->signalcomp->plif_ecg_filter);
      }

      if(fbr->signalcomp->plif_eeg_filter)
      {
        dig_value = plif_eeg_run_subtract_filter(dig_value, fbr->signalcomp->plif_eeg_filter);
      }

      if(fbr->signalcomp->ecg_filter)
      {
        dig_value = run_ecg_filter(dig_value, fbr->signalcomp->ecg_filter);
      }

      if(fbr->signalcomp->zratio_filter)
      {
        dig_value = run_zratio_filter(dig_value, fbr->signalcomp->zratio_filter);
      }

      dig_value *= fbr->signalcomp->polarity;
    }

    fbr->processed_samples_buf[s - s_off] = (dig_value * fbr->bitvalue);
  }

  return 0;
}















