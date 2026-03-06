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


#ifndef FIR_filter_INCLUDED
#define FIR_filter_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  double *vars;
  double *buf;
  double *buf_sav;
  int sz;
  int idx;
  int idx_sav;
  char label[512];
} firfiltset_t;


firfiltset_t * create_fir_filter(double *, int, const char *);
double run_fir_filter(double, firfiltset_t *);
void free_fir_filter(firfiltset_t *);
void reset_fir_filter(double, firfiltset_t *);
firfiltset_t * create_fir_filter_copy(firfiltset_t *);
int fir_filter_size(firfiltset_t *);
double fir_filter_tap(int, firfiltset_t *);
void fir_filter_restore_buf(firfiltset_t *);
void fir_filter_save_buf(firfiltset_t *);
int fir_filter_freqz(double *, int, double *, double *, int);
const char * fir_filter_label(firfiltset_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif







