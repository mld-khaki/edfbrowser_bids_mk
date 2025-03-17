/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 - 2024 Teunis van Beelen
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



#ifndef fft_wrap_INCLUDED
#define fft_wrap_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "third_party/kiss_fft/kiss_fftr.h"

#define FFT_WNDW_TYPE_RECT                    (0)
#define FFT_WNDW_TYPE_HAMMING                 (1)
#define FFT_WNDW_TYPE_4TERM_BLACKMANHARRIS    (2)
#define FFT_WNDW_TYPE_7TERM_BLACKMANHARRIS    (3)
#define FFT_WNDW_TYPE_NUTTALL3B               (4)
#define FFT_WNDW_TYPE_NUTTALL4C               (5)
#define FFT_WNDW_TYPE_HANN                    (6)
#define FFT_WNDW_TYPE_HFT223D                 (7)
#define FFT_WNDW_TYPE_HFT95                   (8)
#define FFT_WNDW_TYPE_KAISER_A2               (9)
#define FFT_WNDW_TYPE_KAISER_A3              (10)
#define FFT_WNDW_TYPE_KAISER_A4              (11)
#define FFT_WNDW_TYPE_KAISER_A5              (12)


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  int sz_in;
  int dft_sz;
  int sz_out;
  int blocks;
  int smpls_left;
  int wndw_type;
  int overlap;
  int blocks_processed;
  double *buf_in;
  double *buf_wndw;
  double *buf_wndw_coef;
  double *buf_out;
  kiss_fftr_cfg cfg;
  kiss_fft_cpx *kiss_fftbuf;
} fft_wrap_setting_t;


fft_wrap_setting_t * fft_wrap_create(double *, int, int, int, int);
void fft_wrap_run(fft_wrap_setting_t *);
void free_fft_wrap(fft_wrap_setting_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif







