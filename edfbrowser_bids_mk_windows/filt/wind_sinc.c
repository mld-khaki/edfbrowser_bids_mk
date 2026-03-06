/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2023 - 2024 Teunis van Beelen
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "wind_sinc.h"


#define t_sinc(x)   (sin(x * M_PI) / (x * M_PI))


/*
 * zero order, first kind, modified Bessel function
 * http://jean-pierre.moreau.pagesperso-orange.fr/c_bessel.html
 */
static double t_bessi0(double x)
{
  double y,p1,p2,p3,p4,p5,p6,p7,q1,q2,q3,q4,q5,q6,q7,q8,q9,ax,bx;

  p1=1.0;
  p2=3.5156229;
  p3=3.0899424;
  p4=1.2067492;
  p5=0.2659732;
  p6=0.360768e-1;
  p7=0.45813e-2;
  q1=0.39894228;
  q2=0.1328592e-1;
  q3=0.225319e-2;
  q4=-0.157565e-2;
  q5=0.916281e-2;
  q6=-0.2057706e-1;
  q7=0.2635537e-1;
  q8=-0.1647633e-1;
  q9=0.392377e-2;

  if(fabs(x) < 3.75)
  {
    y = (x / 3.75) * (x / 3.75);

    return(p1 + y * (p2 + y * (p3 + y * (p4 + y * (p5 + y * (p6 + y * p7))))));
  }
  else
  {
    ax = fabs(x);
    y = 3.75 / ax;
    bx = exp(ax) / sqrt(ax);
    ax = q1 + y * (q2 + y * (q3 + y * (q4 + y * (q5 + y * (q6 + y * (q7 + y * (q8 + y * q9)))))));

    return(ax * bx);
  }
}


static void ws_create_coefficients(int sz, double *buf, double c, int inv)
{
  int i;

  double sum;

  for(i=0; i<sz; i++)
  {
    buf[i] = t_sinc(2 * c * (i - (sz / 2)));  /* create the coefficients */
  }

  buf[sz / 2] = 1;  /* prevent NaN */

  for(i=0; i<sz; i++)  /* Kaiser 5 window */
  {
    buf[i] *= ((t_bessi0(M_PI * 5.0 * sqrt(1.0 - ((((2.0 * i) / (double)(sz - 1)) - 1.0) * (((2.0 * i) / (double)(sz - 1)) - 1.0))))) / (t_bessi0(M_PI * 5.0)));
  }

  for(i=0, sum=0; i<sz; i++)
  {
    sum += buf[i];
  }

  for(i=0; i<sz; i++)  /* unity gain */
  {
    buf[i] /= sum;
  }

  if(inv)
  {
    for(i=0; i<sz; i++)  /*  spectral inversion */
    {
      buf[i] *= -1;
    }

    buf[sz / 2] += 1;
  }
}


/* Returns the discrete, linear convolution of two one-dimensional arrays.
 * Size of the new array is sz_a + sz_b - 1.
 * The caller is responsible to free the new array.
 */
static double * t_convolve(double *a, double *b, int sz_a, int sz_b)
{
  int i, j, len;

  double *c;

  if((sz_a < 1) || (sz_b < 1) || (a == NULL) || (b == NULL))  return NULL;

  len = sz_a + sz_b - 1;

  c = (double *)calloc(1, sizeof(double) * len);
  if(c == NULL)  return NULL;

  for(i=0; i<len; i++)
  {
    for(j=0; j<sz_b; j++)
    {
      if(((i - j) >= 0) && ((i - j) < sz_a))
      {
        c[i] += a[i - j] * b[j];
      }
    }
  }

  return c;
}


int wind_sinc(double sf, int filt_type, double cutoff1_hz, double cutoff2_hz, double trans_bw_hz, double **taps1)
{
  int i, err=0,
      num_taps;

  double cutoff1,
         cutoff2=0,
         trans_bw,
         *taps2=NULL,
         *taps3=NULL;

  *taps1 = NULL;

  if(sf < 1e-6)  return -1;

  if((filt_type < 0) || (filt_type >=T_FIR_MAX_TYPES))  return -2;

  if((cutoff1_hz <= 0) || (cutoff1_hz >= (sf / 2)))  return -3;

  if((trans_bw_hz <= 0) || (trans_bw_hz >= (sf / 2)))  return -4;

  cutoff1 = cutoff1_hz / sf;

  if((filt_type == T_FIR_TYPE_BS) || (filt_type == T_FIR_TYPE_BP))
  {
    if((cutoff2_hz <= cutoff1_hz) || (cutoff2_hz >= (sf / 2)))  return -5;

    cutoff2 = cutoff2_hz / sf;
  }

  trans_bw = trans_bw_hz / sf;

  if(trans_bw < 1e-3)  return -6;

  num_taps = ceil(4 / trans_bw) + 0.5;

  if(!(num_taps % 2))  num_taps++;

  if(filt_type == T_FIR_TYPE_BP)
  {
    if(num_taps > 2000)  return -7;
  }
  else
  {
    if(num_taps > 4000)  return -7;
  }

  *taps1 = (double *)malloc(sizeof(double) * num_taps);
  if(*taps1 == NULL)  return -8;

  if((filt_type == T_FIR_TYPE_LP) || (filt_type == T_FIR_TYPE_BS))
  {
    ws_create_coefficients(num_taps, *taps1, cutoff1, 0);
  }
  else
  {
    ws_create_coefficients(num_taps, *taps1, cutoff1, 1);
  }

  if((filt_type == T_FIR_TYPE_BS) || (filt_type == T_FIR_TYPE_BP))
  {
    taps2 = (double *)malloc(sizeof(double) * num_taps);
    if(taps2 == NULL)
    {
      free(*taps1);
      *taps1 = NULL;
      return -9;
    }

    if(filt_type == T_FIR_TYPE_BP)
    {
      ws_create_coefficients(num_taps, taps2, cutoff2, 0);

      taps3 = t_convolve(*taps1, taps2, num_taps, num_taps);

      num_taps = num_taps * 2 - 1;

      if(taps3 == NULL)  err = -10;

      free(*taps1);

      *taps1 = taps3;
    }
    else
    {
      ws_create_coefficients(num_taps, taps2, cutoff2, 1);

      for(i=0; i<num_taps; i++)
      {
        (*taps1)[i] += taps2[i];
      }
    }
  }

  free(taps2);

  if(err)  return err;

  return num_taps;
}















