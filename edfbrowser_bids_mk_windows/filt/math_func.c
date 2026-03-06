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


#include "math_func.h"


static const char math_func_descr[MATH_MAX_FUNCS][32]=
{
  "None",
  "Square",
  "Square Root",
  "Absolute",
  "Peak Hold",
};


mathfuncset_t * create_math_func(int func_f, int pk_smpls)
{
  mathfuncset_t *st;

  if((func_f < 0) || (func_f >= MATH_MAX_FUNCS))
  {
    return NULL;
  }

  st = (mathfuncset_t *) calloc(1, sizeof(mathfuncset_t));
  if(st==NULL)  return NULL;

  st->func = func_f;

  strlcpy(st->descr, math_func_descr[func_f], 32);

  if(func_f == MATH_FUNC_PK_HOLD)
  {
    if(pk_smpls < 1)
    {
      free(st);
      return NULL;
    }

    st->pk_hold_smpls_set = pk_smpls;

    snprintf(st->descr + strlen(st->descr), 32 - strlen(st->descr), " %i smpls", pk_smpls);
  }

  return st;
}


int get_math_func_descr(int func_f, char *dest, int sz)
{
  if((func_f < 0) || (func_f >= MATH_MAX_FUNCS) || (dest == NULL) || (sz < 1))
  {
    return -1;
  }

  strlcpy(dest, math_func_descr[func_f], sz);

  return 0;
}


void free_math_func(mathfuncset_t *st)
{
  free(st);
}


double run_math_func(double val, mathfuncset_t *st)
{
  if(st->func == MATH_FUNC_SQUARE)
  {
    if(val < 0)
    {
      return (-val * val);
    }
    else
    {
      return (val * val);
    }
  }
  else if(st->func == MATH_FUNC_SQRT)
    {
      if(val < 0)
        {
          return (-sqrt(-val));
        }
        else
        {
          return sqrt(val);
        }
    }
    else if(st->func == MATH_FUNC_ABS)
      {
        return fabs(val);
      }
      else if(st->func == MATH_FUNC_PK_HOLD)
        {
          st->pk_hold_smpl_cntr++;

          if((val > st->pk_hold_val) || (st->pk_hold_smpl_cntr >= st->pk_hold_smpls_set))
          {
            st->pk_hold_val = val;
            st->pk_hold_smpl_cntr = 0;
            return val;
          }

          return st->pk_hold_val;
        }
        else if(st->func == MATH_FUNC_NONE)
          {
            return val;
          }

  return 0;
}















