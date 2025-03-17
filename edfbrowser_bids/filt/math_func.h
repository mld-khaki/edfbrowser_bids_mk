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


#ifndef math_func_INCLUDED
#define math_func_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MATH_FUNC_NONE      (0)
#define MATH_FUNC_SQUARE    (1)
#define MATH_FUNC_SQRT      (2)
#define MATH_FUNC_ABS       (3)
#define MATH_FUNC_PK_HOLD   (4)
#define MATH_MAX_FUNCS      (5)


typedef struct
{
  int func;
  char descr[32];
  int pk_hold_smpls_set;
  int pk_hold_smpl_cntr;
  double pk_hold_val;
} mathfuncset_t;


mathfuncset_t * create_math_func(int, int);
double run_math_func(double, mathfuncset_t *);
void free_math_func(mathfuncset_t *);
int get_math_func_descr(int, char *, int);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif







