/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2024 Teunis van Beelen
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


#include "ecg_filter.h"


#define ECG_FILTER_STAT_BUFSIZE  (16384)


ecgfiltset_t * create_ecg_filter(double sf, double bitval, double sense)
{
  ecgfiltset_t *st;

  if(sf < 199.999)  return NULL;

  st = (ecgfiltset_t *)calloc(1, sizeof(ecgfiltset_t));
  if(st == NULL)  return NULL;

  st->sf = sf;

  st->bpm = 60.0;

  st->bitvalue = bitval;

  st->pt_qrs = create_pt_qrs(sf, fabs(bitval * sense));
  if(st->pt_qrs == NULL)
    goto CREATE_OUT_ERROR;

  st->pt_qrs_bu = create_pt_qrs(sf, fabs(bitval * sense));
  if(st->pt_qrs_bu == NULL)
    goto CREATE_OUT_ERROR;

  st->stat_buf = (double *)malloc(sizeof(double) * ECG_FILTER_STAT_BUFSIZE);
  if(st->stat_buf == NULL)
    goto CREATE_OUT_ERROR;

  st->stat_smpl_buf = (long long *)malloc(sizeof(long long) * ECG_FILTER_STAT_BUFSIZE);
  if(st->stat_smpl_buf == NULL)
    goto CREATE_OUT_ERROR;

  st->stat_buf_sz = ECG_FILTER_STAT_BUFSIZE;

  return st;

CREATE_OUT_ERROR:

  free_ecg_filter(st);

  return NULL;
}


void free_ecg_filter(ecgfiltset_t *st)
{
  if(st == NULL)  return;

  free_pt_qrs(st->pt_qrs);
  free_pt_qrs(st->pt_qrs_bu);

  free(st->stat_buf);
  free(st->stat_smpl_buf);

  free(st);
}


double run_ecg_filter(double new_sample, ecgfiltset_t *st)
{
  int rr_smpl_ival,
      qrs_smpl_offset;

  long long *lptr;

  double *dptr;

  qrs_smpl_offset = run_pt_qrs(new_sample, st->pt_qrs);

  if(qrs_smpl_offset == 0)
  {
    st->sample_cntr++;

    st->smpl_n++;

    return (st->bpm / st->bitvalue);
  }

  rr_smpl_ival = st->smpl_n + st->qrs_smpl_offset_old - qrs_smpl_offset;

  st->bpm = (st->sf * 60.0) / rr_smpl_ival;

  if(st->stat_buf_idx >= st->stat_buf_sz)
  {
    dptr = st->stat_buf;

    lptr= st->stat_smpl_buf;

    if(st->stat_buf_idx >= 0x7fffffff - (ECG_FILTER_STAT_BUFSIZE * 2))  goto RUN_OUT_ERROR;

    st->stat_buf = (double *)realloc(st->stat_buf, sizeof(double) * (st->stat_buf_sz + ECG_FILTER_STAT_BUFSIZE));
    if(st->stat_buf == NULL)
      goto RUN_OUT_ERROR;

    st->stat_smpl_buf = (long long *)realloc(st->stat_smpl_buf, sizeof(long long) * (st->stat_buf_sz + ECG_FILTER_STAT_BUFSIZE));
    if(st->stat_smpl_buf == NULL)
      goto RUN_OUT_ERROR;

    st->stat_buf_sz += ECG_FILTER_STAT_BUFSIZE;
  }

  st->stat_buf[st->stat_buf_idx] = ((double)rr_smpl_ival) / st->sf;

  if(st->stat_buf_idx > 0)
  {
    st->stat_smpl_buf[st->stat_buf_idx] = st->stat_smpl_buf[st->stat_buf_idx - 1] + rr_smpl_ival;
  }
  else
  {
    st->stat_smpl_buf[st->stat_buf_idx] = st->sample_cntr - qrs_smpl_offset;
  }

  st->stat_buf_idx++;

  st->qrs_smpl_offset_old = qrs_smpl_offset;

  st->sample_cntr = 1;

  st->smpl_n = 1;

  return (st->bpm / st->bitvalue);

RUN_OUT_ERROR:

  st->stat_buf = dptr;
  st->stat_smpl_buf = lptr;
  st->malloc_err = 1;
  return 0;
}


void ecg_filter_save_buf(ecgfiltset_t *st)
{
  int i;

  double *d_ptr;

  if(st == NULL)  return;

  st->smpl_n_bu = st->smpl_n;

  st->bpm_bu = st->bpm;

  d_ptr = st->pt_qrs_bu->ds_ravg_buf;

  *st->pt_qrs_bu = *st->pt_qrs;

  st->pt_qrs_bu->ds_ravg_buf = d_ptr;

  if(st->pt_qrs->ds_ravg_len > 1)
  {
    for(i=0; i<st->pt_qrs->ds_ravg_len; i++)
    {
      st->pt_qrs_bu->ds_ravg_buf[i] = st->pt_qrs->ds_ravg_buf[i];
    }
  }

  st->bu_filled = 1;
}


void ecg_filter_restore_buf(ecgfiltset_t *st)
{
  int i;

  double *d_ptr;

  if(st == NULL)  return;

  st->stat_buf_idx = 0;

  st->sample_cntr = 0;

  if(st->bu_filled == 0)  return;

  st->smpl_n = st->smpl_n_bu;

  st->bpm = st->bpm_bu;

  d_ptr = st->pt_qrs->ds_ravg_buf;

  *st->pt_qrs = *st->pt_qrs_bu;

  st->pt_qrs->ds_ravg_buf = d_ptr;

  if(st->pt_qrs->ds_ravg_len > 1)
  {
    for(i=0; i<st->pt_qrs->ds_ravg_len; i++)
    {
      st->pt_qrs->ds_ravg_buf[i] = st->pt_qrs_bu->ds_ravg_buf[i];
    }
  }
}


void reset_ecg_filter(ecgfiltset_t *st)
{
  if(st == NULL)  return;

  st->smpl_n = 0;

  st->bu_filled = 0;

  st->bpm = 60.0;

  st->stat_buf_idx = 0;

  st->sample_cntr = 0;
}


int ecg_filter_get_beat_cnt(ecgfiltset_t *st)
{
  if(st == NULL)  return 0;

  return st->stat_buf_idx;
}


long long * ecg_filter_get_onset_beatlist(ecgfiltset_t *st)
{
  if(st == NULL)  return 0LL;

  return st->stat_smpl_buf;
}


double * ecg_filter_get_interval_beatlist(ecgfiltset_t *st)
{
  if(st == NULL)  return NULL;

  return st->stat_buf;
}











