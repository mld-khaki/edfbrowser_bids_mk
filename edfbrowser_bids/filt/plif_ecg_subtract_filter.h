/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 - 2024 Teunis van Beelen
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


#ifndef PLIF_ECG_subtract_filter_INCLUDED
#define PLIF_ECG_subtract_filter_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define PLIF_ECG_NBUFS   (40)


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  int sf;
  int tpl;
  double *ravg_buf;
  double *ravg_noise_buf[PLIF_ECG_NBUFS];
  double *input_buf[PLIF_ECG_NBUFS];
  double *ref_buf;
  double linear_threshold;
  double linear_diff[PLIF_ECG_NBUFS];
  int ravg_idx;
  int buf_idx;
} plifecgfilt_t;



plifecgfilt_t * plif_ecg_create_subtract_filter(int, int, double);
double plif_ecg_run_subtract_filter(double, plifecgfilt_t *);
void plif_ecg_reset_subtract_filter(plifecgfilt_t *, double);
void plif_ecg_free_subtract_filter(plifecgfilt_t *);
void plif_ecg_subtract_filter_state_copy(plifecgfilt_t *, plifecgfilt_t *);
plifecgfilt_t * plif_ecg_subtract_filter_create_copy(plifecgfilt_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif















