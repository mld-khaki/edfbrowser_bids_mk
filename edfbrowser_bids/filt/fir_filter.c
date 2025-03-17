/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 - 2024 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
**************************************************************************
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
*
*/


#include "fir_filter.h"


#define atan_deg(x)   (atan(x) * (180 / M_PI))


static void rect_to_polar(double, double, double *, double *);


firfiltset_t * create_fir_filter(double *vars, int sz, const char *label)
{
  firfiltset_t *settings;

  if((vars == NULL) || (sz < 1) || (sz > 4000))  return NULL;

  settings = (firfiltset_t *)calloc(1, sizeof(firfiltset_t));
  if(settings == NULL)
  {
    return NULL;
  }

  settings->vars = (double *)calloc(1, sizeof(double) * sz);
  if(settings->vars == NULL)
  {
    free(settings);
    return NULL;
  }

  settings->buf = (double *)calloc(1, sizeof(double) * sz);
  if(settings->buf == NULL)
  {
    free(settings->vars);
    free(settings);
    return NULL;
  }

  settings->buf_sav = (double *)calloc(1, sizeof(double) * sz);
  if(settings->buf_sav == NULL)
  {
    free(settings->buf);
    free(settings->vars);
    free(settings);
    return NULL;
  }

  settings->sz = sz;

  settings->idx = 0;

  settings->idx_sav = 0;

  memcpy(settings->vars, vars, sizeof(double) * sz);

  if(label != NULL)
  {
    strlcpy(settings->label, label, 512);
  }

  return settings;
}


double run_fir_filter(double val, firfiltset_t *settings)
{
  int i, j;

  double result = 0;

  if(settings == NULL)  return 0;

  j = settings->idx;

  settings->buf[settings->idx++] = val;

  settings->idx %= settings->sz;

  for(i=0; i<settings->sz; i++)
  {
    result += settings->buf[j--] * settings->vars[i];

    if(j < 0)  j = settings->sz - 1;
  }

  return result;
}


void free_fir_filter(firfiltset_t *settings)
{
  if(settings == NULL)  return;

  free(settings->vars);
  free(settings->buf);
  free(settings->buf_sav);
  free(settings);
}


void reset_fir_filter(double val, firfiltset_t *settings)
{
  int i;

  if(settings == NULL)  return;

  for(i=0; i<settings->sz; i++)
  {
    settings->buf[i] = val;
  }
}


firfiltset_t * create_fir_filter_copy(firfiltset_t *settings)
{
  int i;

  firfiltset_t *new_settings;

  if(settings == NULL)  return NULL;

  new_settings = create_fir_filter(settings->vars, settings->sz, settings->label);

  if(new_settings == NULL)  return NULL;

  for(i=0; i<settings->sz; i++)
  {
    new_settings->buf[i] = settings->buf[i];
  }

  return new_settings;
}


int fir_filter_size(firfiltset_t *settings)
{
  if(settings == NULL)  return 0;

  return settings->sz;
}


double fir_filter_tap(int idx, firfiltset_t *settings)
{
  if(settings == NULL)  return 0;

  if((idx < 0) || (idx >= settings->sz))  return 0;

  return settings->vars[idx];
}


void fir_filter_save_buf(firfiltset_t *settings)
{
  int i;

  for(i=0; i<settings->sz; i++)
  {
    settings->buf_sav[i] = settings->buf[i];
  }

  settings->idx_sav = settings->idx;
}


void fir_filter_restore_buf(firfiltset_t *settings)
{
  int i;

  for(i=0; i<settings->sz; i++)
  {
    settings->buf[i] = settings->buf_sav[i];
  }

  settings->idx = settings->idx_sav;
}


const char * fir_filter_label(firfiltset_t *settings)
{
  return settings->label;
}


/*
 *  Calculates the frequency and phase response of a FIR filter.
 *  h_arr is a pointer to an array with filter taps.
 *  n_taps is the length of the array of filter taps.
 *  z is a pointer to an array of size pnts where the frequency response will be stored.
 *  deg is a pointer to an array of size pnts where the phase response will be stored.
 *  pnts is the number of measuring points from 0 Hz to Nyquist,
 *  so, frequency step (resolution) will be: (fs / 2) / pnts
 */
int fir_filter_freqz(double *h_arr, int n_taps, double *z, double *deg, int pnts)
{
  int i, j;

  double w, *re=NULL, *im=NULL;

  if((h_arr == NULL) || (z == NULL) || (deg == NULL) || (n_taps < 2) || (pnts < 2))  return -1;

  re = (double *)malloc(sizeof(double) * pnts);
  if(re == NULL)  return -2;

  im = (double *)malloc(sizeof(double) * pnts);
  if(im == NULL)
  {
    free(re);
    return -3;
  }

  w = M_PI / pnts;

/*
 *  H(jw)=h(0)[cos(0w) - jsin(0w)] + h(1)[cos(1w) - jsin(1w)] + ... h(N-1)[cos((N-1)w) - jsin((N-1)w)]
 */
  for(i=0; i<pnts; i++)
  {
    re[i] = 0;
    im[i] = 0;

    for(j=0; j<n_taps; j++)
    {
      re[i] += h_arr[j] * cos(j * i * w);
      im[i] += h_arr[j] * sin(j * i * w);
    }
  }

  for(i=0; i<pnts; i++)
  {
    rect_to_polar(re[i], im[i], &z[i], &deg[i]);

    if(deg[i] > 180)  deg[i] -= 360;

    deg[i] *= -1;

//    printf("re: %f    im: %f    z: %f    deg: %f\n", re[i], im[i], z[i], deg[i]);
  }

  free(re);
  free(im);

  return 0;
}


static void rect_to_polar(double x, double y, double *v, double *g)
{
  if(!dblcmp(y, 0))
  {
    *v = fabs(x);

    if(x < 0)
    {
      *g = 180;
    }
    else
    {
      *g = 0;
    }

    return;
  }

  if(!dblcmp(x, 0))
  {
    *v = fabs(y);

    if(y < 0)
    {
      *g = 270;
    }
    else
    {
      *g = 90;
    }

    return;
  }

  *v = sqrt((x * x) + (y * y));

  if((x < 0) && (y >= 0))
  {
    *g = atan_deg(y / x) + 180;
  }
  else if((x < 0) && (y < 0))
    {
      *g = atan_deg(y / x) + 180;
    }
    else if((x >= 0) && (y < 0))
      {
        *g = 360 + atan_deg(y / x);
      }
      else
      {
        *g = atan_deg(y / x);
      }
}



















