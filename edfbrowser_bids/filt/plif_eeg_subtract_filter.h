/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 - 2024 Teunis van Beelen
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


#ifndef PLIF_EEG_subtract_filter_INCLUDED
#define PLIF_EEG_subtract_filter_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  int sf;
  int pwrcycles;
  int pwrcyclesmpls;
  double *buf;
  int sz;
  int idx;
} plifeegfilt_t;


plifeegfilt_t * plif_eeg_create_subtract_filter(int, int);
double plif_eeg_run_subtract_filter(double, plifeegfilt_t *);
void plif_eeg_reset_subtract_filter(plifeegfilt_t *);
void plif_eeg_free_subtract_filter(plifeegfilt_t *);
void plif_eeg_subtract_filter_state_copy(plifeegfilt_t *, plifeegfilt_t *);
plifeegfilt_t * plif_eeg_subtract_filter_create_copy(plifeegfilt_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif















