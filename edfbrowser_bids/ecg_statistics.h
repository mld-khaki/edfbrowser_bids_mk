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

#ifndef ecg_statistics_INCLUDED
#define ecg_statistics_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "utils.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  int beat_cnt;
  double mean_rr;
  double sdnn_rr;
  double rmssd_rr;
  double mean_hr;
  double sdnn_hr;
  int NN20;
  double pNN20;
  int NN50;
  double pNN50;
} ecg_hr_statistics_t;


int ecg_get_hr_statistics(double *, int, ecg_hr_statistics_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif








