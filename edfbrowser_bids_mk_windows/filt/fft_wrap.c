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


#include "fft_wrap.h"


static void window_func(const double *, double *, double *, int, int);
static void set_gain_unity(double *, int);
static double bessi0(double);


fft_wrap_setting_t * fft_wrap_create(double *buf, int buf_size, int dft_size, int window_type, int overlap)
{
  fft_wrap_setting_t *st;

  if(buf == NULL)  return NULL;
  if(buf_size < 4)  return NULL;
  if(dft_size < 4)  return NULL;
  if(dft_size & 1)  dft_size--;
  if((window_type < 0) || (window_type > 12))  return NULL;
  if((overlap < 1) || (overlap > 5))  return NULL;

  st = (fft_wrap_setting_t *)calloc(1, sizeof(fft_wrap_setting_t));
  if(st == NULL)  return NULL;
  st->sz_in = buf_size;
  st->dft_sz = dft_size;
  st->wndw_type = window_type;
  st->overlap = overlap;

  st->blocks = 1;

  if(st->dft_sz < st->sz_in)
  {
    st->blocks = st->sz_in / st->dft_sz;
  }
  else
  {
    st->dft_sz = st->sz_in;
  }

  if(st->dft_sz & 1)  st->dft_sz--;

  st->smpls_left = st->sz_in % st->dft_sz;
  if(st->smpls_left & 1)  st->smpls_left--;

  st->sz_out = st->dft_sz / 2;
  st->buf_in = buf;
  if(st->wndw_type)
  {
    st->buf_wndw = (double *)malloc(sizeof(double) * (st->dft_sz + 2));
    if(st->buf_wndw == NULL)
    {
      free(st);
      return NULL;
    }
    st->buf_wndw_coef = (double *)malloc(sizeof(double) * (st->dft_sz / 2 + 2));
    if(st->buf_wndw_coef == NULL)
    {
      free(st->buf_wndw);
      free(st);
      return NULL;
    }
  }
  st->buf_out = (double *)malloc(sizeof(double) * (st->sz_out + 2));
  if(st->buf_out == NULL)
  {
    free(st->buf_wndw_coef);
    free(st->buf_wndw);
    free(st);
    return NULL;
  }
  st->kiss_fftbuf = (kiss_fft_cpx *)malloc((st->sz_out + 1) * sizeof(kiss_fft_cpx));
  if(st->kiss_fftbuf == NULL)
  {
    free(st->buf_out);
    free(st->buf_wndw_coef);
    free(st->buf_wndw);
    free(st);
    return NULL;
  }
  st->cfg = kiss_fftr_alloc(st->dft_sz, 0, NULL, NULL);

  return st;
}


void fft_wrap_run(fft_wrap_setting_t *st)
{
  int i, j, k;

  if(st == NULL)  return;
  if(st->sz_in < 4)  return;
  if(st->dft_sz < 4) return;
  if(st->sz_out < 1) return;
  if(st->buf_in == NULL)  return;
  if(st->buf_out == NULL)  return;
  if(st->kiss_fftbuf == NULL)  return;

  st->blocks_processed = 0;

  if(st->wndw_type)
  {
    if(st->buf_wndw == NULL)  return;

    window_func(st->buf_in, st->buf_wndw, st->buf_wndw_coef, st->dft_sz, st->wndw_type);

    kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
  }
  else
  {
    kiss_fftr(st->cfg, st->buf_in, st->kiss_fftbuf);
  }

  for(i=0; i<st->sz_out; i++)
  {
    st->buf_out[i] = ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
  }

  st->blocks_processed++;

  for(j=1; j<st->blocks; j++)
  {
    for(k=1; k<st->overlap; k++)
    {
      if(st->wndw_type)
      {
        window_func(st->buf_in + (j * st->dft_sz) - ((st->dft_sz / st->overlap) * k), st->buf_wndw, st->buf_wndw_coef, st->dft_sz, st->wndw_type);

        kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
      }
      else
      {
        kiss_fftr(st->cfg, st->buf_in + (j * st->dft_sz) - ((st->dft_sz / st->overlap) * k), st->kiss_fftbuf);
      }

      for(i=0; i<st->sz_out; i++)
      {
        st->buf_out[i] += ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
      }

      st->blocks_processed++;
    }

    if(st->wndw_type)
    {
      window_func(st->buf_in + (j * st->dft_sz), st->buf_wndw, st->buf_wndw_coef, st->dft_sz, st->wndw_type);

      kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
    }
    else
    {
      kiss_fftr(st->cfg, st->buf_in + (j * st->dft_sz), st->kiss_fftbuf);
    }

    for(i=0; i<st->sz_out; i++)
    {
      st->buf_out[i] += ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
    }

    st->blocks_processed++;
  }

  if(st->smpls_left)
  {
    if(st->wndw_type)
    {
      window_func(st->buf_in + ((j-1) * st->dft_sz) + st->smpls_left, st->buf_wndw, st->buf_wndw_coef, st->dft_sz, st->wndw_type);

      kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
    }
    else
    {
      kiss_fftr(st->cfg, st->buf_in + ((j-1) * st->dft_sz) + st->smpls_left, st->kiss_fftbuf);
    }

    for(i=0; i<st->sz_out; i++)
    {
      st->buf_out[i] += ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
    }

    st->blocks_processed++;
  }

  if(st->blocks_processed > 1)
  {
    for(i=0; i<st->sz_out; i++)
    {
      st->buf_out[i] /= st->blocks_processed;
    }
  }
}


void free_fft_wrap(fft_wrap_setting_t *st)
{
  if(st == NULL)  return;
  free(st->cfg);
  free(st->kiss_fftbuf);
  free(st->buf_out);
  free(st->buf_wndw);
  free(st->buf_wndw_coef);
  memset(st, 0, sizeof(fft_wrap_setting_t));
  free(st);
}


static void window_func(const double *src, double *dest, double *coef, int sz, int type)
{
  int i, sz2;

  static int old_sz=-1,
             old_type=-1;

  static double *old_coef=NULL;


  sz2 = sz / 2;

  if((sz != old_sz) || (type != old_type) || (coef != old_coef))
  {
    old_sz = sz;
    old_type = type;
    old_coef = coef;

    if(type == FFT_WNDW_TYPE_HAMMING)
    {
      for(i=0; i<sz2; i++)
      {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
        coef[i] = 0.54 - (0.46 * cos((2.0 * M_PI * i) / (sz - 1)));  /* Hamming (original) */
//        coef[i] = 0.53836 - (0.46164 * cos((2.0 * M_PI * i) / (sz - 1)));  /* Hamming marginally optimized */
      }
    }
    else if(type == FFT_WNDW_TYPE_NUTTALL3B)
      {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
        for(i=0; i<sz2; i++)
        {
//          coef[i] = 0.42 - (0.5 * cos((2.0 * M_PI * i) / (sz - 1))) + (0.08 * cos((4.0 * M_PI * i) / (sz - 1)));  /* Blackman */
          coef[i] = 0.4243801 - (0.4973406 * cos((2.0 * M_PI * i) / (sz - 1))) + (0.0782793 * cos((4.0 * M_PI * i) / (sz - 1)));  /* Nuttall3b */
        }
      }
      else if(type == FFT_WNDW_TYPE_4TERM_BLACKMANHARRIS)
        {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
          for(i=0; i<sz2; i++)
          {
            coef[i] = 0.35875 - (0.48829 * cos((2.0 * M_PI * i) / (sz - 1))) + (0.14128 * cos((4.0 * M_PI * i) / (sz - 1))) - (0.01168 * cos((6.0 * M_PI * i) / (sz - 1)));  /* 4-term Blackman-Harris */
          }
        }
        else if(type == FFT_WNDW_TYPE_7TERM_BLACKMANHARRIS)
          {  /* The use of DFT windows in signal-to-noise ratio and harmonic distortion computations IEEE */
            for(i=0; i<sz2; i++)
            {
              coef[i] = 0.271051400693424 - (0.433297939234485 * cos((2.0 * M_PI * i) / (sz - 1))) + (0.218122999543110 * cos((4.0 * M_PI * i) / (sz - 1))) - (0.65925446388031e-1 * cos((6.0 * M_PI * i) / (sz - 1)))
              + (0.10811742098371e-1 * cos((8.0 * M_PI * i) / (sz - 1))) - (0.776584825226e-3 * cos((10.0 * M_PI * i) / (sz - 1))) + (0.13887217352e-4 * cos((12.0 * M_PI * i) / (sz - 1)));  /* 7-term Blackman-Harris */
            }
          }
          else if(type == FFT_WNDW_TYPE_NUTTALL4C)
            {
              for(i=0; i<sz2; i++)
              {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                coef[i] =  0.3635819 - (0.4891775 * cos((2.0 * M_PI * i) / (sz - 1))) + ( 0.1365995 * cos((4.0 * M_PI * i) / (sz - 1))) - (0.0106411 * cos((6.0 * M_PI * i) / (sz - 1)));  /* Nuttall4c */
              }
            }
            else if(type == FFT_WNDW_TYPE_HANN)
              {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                for(i=0; i<sz2; i++)
                {                           /* both are the same */
                  coef[i] = (1.0 - cos((2.0 * M_PI * i) / (sz - 1))) / 2.0;  /* Hann */
//                  coef[i] = 0.5 - (0.5 * cos((2.0 * M_PI * i) / (sz - 1)));  /* Hann */
                }
              }
              else if(type == FFT_WNDW_TYPE_HFT223D)
                {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                  for(i=0; i<sz2; i++)
                  {
                    coef[i] = 1.0 - (1.98298997309 * cos((2.0 * M_PI * i) / (sz - 1))) + (1.75556083063 * cos((4.0 * M_PI * i) / (sz - 1))) - (1.19037717712 * cos((6.0 * M_PI * i) / (sz - 1)))
                    + (0.56155440797 * cos((8.0 * M_PI * i) / (sz - 1))) - (0.17296769663 * cos((10.0 * M_PI * i) / (sz - 1))) + (0.3233247087e-1 * cos((12.0 * M_PI * i) / (sz - 1)))
                    - (0.324954578e-2 * cos((14.0 * M_PI * i) / (sz - 1))) + (0.13801040e-3 * cos((16.0 * M_PI * i) / (sz - 1))) - (0.132725e-5 * cos((18.0 * M_PI * i) / (sz - 1)));  /* 9-term HFT223D */
                  }
                }
                else if(type == FFT_WNDW_TYPE_HFT95)
                  {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                    for(i=0; i<sz2; i++)
                    {
                      coef[i] = 1.0 - (1.9383379 * cos((2.0 * M_PI * i) / (sz - 1))) + (1.3045202 * cos((4.0 * M_PI * i) / (sz - 1)))
                      - (0.4028270 * cos((6.0 * M_PI * i) / (sz - 1))) + (0.0350665 * cos((8.0 * M_PI * i) / (sz - 1)));
                    }
                  }
                  else if(type == FFT_WNDW_TYPE_KAISER_A2)
                    {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                      for(i=0; i<sz2; i++)
                      {
                        coef[i] = (bessi0(M_PI * 2.0 * sqrt(1.0 - ((((2.0 * i) / (double)(sz - 1)) - 1.0) * (((2.0 * i) / (double)(sz - 1)) - 1.0))))) / (bessi0(M_PI * 2.0));
                      }
                    }
                    else if(type == FFT_WNDW_TYPE_KAISER_A3)
                      {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                        for(i=0; i<sz2; i++)
                        {
                          coef[i] = (bessi0(M_PI * 3.0 * sqrt(1.0 - ((((2.0 * i) / (double)(sz - 1)) - 1.0) * (((2.0 * i) / (double)(sz - 1)) - 1.0))))) / (bessi0(M_PI * 3.0));
                        }
                      }
                      else if(type == FFT_WNDW_TYPE_KAISER_A4)
                        {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                          for(i=0; i<sz2; i++)
                          {
                            coef[i] = (bessi0(M_PI * 4.0 * sqrt(1.0 - ((((2.0 * i) / (double)(sz - 1)) - 1.0) * (((2.0 * i) / (double)(sz - 1)) - 1.0))))) / (bessi0(M_PI * 4.0));
                          }
                        }
                        else if(type == FFT_WNDW_TYPE_KAISER_A5)
                          {  /* Spectrum and spectral density estimation by the Discrete Fourier transform (DFT), including a comprehensive list of window functions and some new at-top windows Max Planck Institute */
                            for(i=0; i<sz2; i++)
                            {
                              coef[i] = (bessi0(M_PI * 5.0 * sqrt(1.0 - ((((2.0 * i) / (double)(sz - 1)) - 1.0) * (((2.0 * i) / (double)(sz - 1)) - 1.0))))) / (bessi0(M_PI * 5.0));
                            }
                          }
                          else
                          {
                            for(i=0; i<sz2; i++)
                            {
                              coef[i] = 0;
                            }
                          }

    set_gain_unity(coef, sz2);
  }

  for(i=0; i<sz2; i++)
  {
    dest[i] = coef[i] * src[i];

    dest[(sz - 1) - i] = coef[i] * src[(sz - 1) - i];
  }
}


static void set_gain_unity(double *arr, int sz)
{
  int i;

  double total = 0.0;

  if(sz < 4)  return;

  for(i=0; i<sz; i++)
  {
    total += arr[i];
  }

  total /= sz;

  if(total != 0.0)
  {
    for(i=0; i<sz; i++)
    {
      arr[i] /= total;
    }
  }
}


/*
 * zero order, first kind, modified Bessel function
 * http://jean-pierre.moreau.pagesperso-orange.fr/c_bessel.html
 */
static double bessi0(double x)
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










