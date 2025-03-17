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


#ifndef ecg_filter_INCLUDED
#define ecg_filter_INCLUDED


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pt_qrs.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  ptqrsset_t *pt_qrs,
             *pt_qrs_bu;

  int qrs_smpl_offset_old;

  int smpl_n;
  double sf;
  double bpm;
  double bitvalue;

  int smpl_n_bu;
  double bpm_bu;
  int bu_filled;

  double *stat_buf;
  long long *stat_smpl_buf;
  int stat_buf_idx;
  int stat_buf_sz;
  int sample_cntr;

  int malloc_err;
} ecgfiltset_t;


ecgfiltset_t * create_ecg_filter(double, double, double);
double run_ecg_filter(double, ecgfiltset_t *);
void free_ecg_filter(ecgfiltset_t *);
void ecg_filter_save_buf(ecgfiltset_t *);
void ecg_filter_restore_buf(ecgfiltset_t *);
void reset_ecg_filter(ecgfiltset_t *);
int ecg_filter_get_beat_cnt(ecgfiltset_t *);
long long * ecg_filter_get_onset_beatlist(ecgfiltset_t *);
double * ecg_filter_get_interval_beatlist(ecgfiltset_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif







