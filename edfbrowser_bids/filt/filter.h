/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2024 Teunis van Beelen
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







#ifndef filter_INCLUDED
#define filter_INCLUDED



#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  double old_input;
  double old_output;
  double factor;
  double sample_frequency;
  double cutoff_frequency;
  int is_LPF;
  int first_sample;
} filtset_t;



filtset_t * create_filter(int, double, double);
void reset_filter(double, filtset_t *);
double first_order_filter(double, filtset_t *);
double get_filter_factor(double, double);
filtset_t * create_filter_copy(filtset_t *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif


