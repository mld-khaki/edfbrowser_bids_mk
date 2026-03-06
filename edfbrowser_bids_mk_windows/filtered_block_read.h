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


#ifndef filtered_block_read_INCLUDED
#define filtered_block_read_INCLUDED


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "edf_helper.h"

#include "filt/filter.h"
#include "filt/fir_filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/math_func.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"

#include "third_party/fidlib/fidlib.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  FILE *inputfile;
  edfhdrblck_t *hdr;
  sigcompblck_t *signalcomp;
  int datarecord_cnt;
  int samples_per_datrec;
  int skip_filters;
  int smpl_mode;
  long long total_samples;
  long long samples_in_file;
  char *readbuf;
  double *processed_samples_buf;
  double bitvalue;
} flt_blck_rd_t;


flt_blck_rd_t * create_flt_blck_rd(sigcompblck_t *, int, int, int, int *, double **);
int run_flt_blck_rd(flt_blck_rd_t *, int);
long long get_samples_flt_blck_rd(flt_blck_rd_t *);
void free_flt_blck_rd(flt_blck_rd_t *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif





