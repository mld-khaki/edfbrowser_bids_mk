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

#include "remez.h"


#define FLT_ROUNDS 1

#define T_RZ_MAX_BANDS              (32)

#define T_RZ_MAX_TAPS             (2000)

#define T_RZ_MAX_FREQS          ((T_RZ_MAX_TAPS / 2) + 2)

#define T_RZ_DELTALIMIT          (1e-12)

#define T_RZ_MAX_MSG_LEN          (2048)


typedef struct
{
  double freq;
  double error;
} t_rz_fq_er;


typedef struct
{
  double start;
  double stop;
  double start_hz;
  double stop_hz;
  double gain;
  double ripple;
  double weight;
  double upper;
  double lower;
} t_rz_band;


typedef struct
{
  t_rz_band band[T_RZ_MAX_BANDS];
  int num_bands;
  double sf;
  int num_taps;
  int desired_taps;
  int done;

  int r;
  double sum_bands;
  double frequencies[T_RZ_MAX_FREQS];
  int num_frequencies;
  double delta;
  double x[T_RZ_MAX_FREQS];
  double y[T_RZ_MAX_FREQS];
  double di[T_RZ_MAX_FREQS];
  double dn[T_RZ_MAX_FREQS];
  double dy[T_RZ_MAX_FREQS];
  double w[T_RZ_MAX_FREQS];
  double d[T_RZ_MAX_FREQS];

  int no_convergence;
  int requirements_met;
  int delta_too_small;
  double maxerror;

  int max_resolution_alloc;

  t_rz_fq_er *extrema;
  int num_extrema;

  t_rz_fq_er *allextrema;
  int num_allextrema;

  int error;
  char err_msg[T_RZ_MAX_MSG_LEN];
  int *abort;
} t_rz_spec;



static int dblcmp(double val1, double val2)
{
  long double diff = (long double)val1 - (long double)val2;

  if(diff > 1e-24l)
  {
    return 1;
  }
  else if(-diff > 1e-24l)
    {
      return -1;
    }
    else
    {
      return 0;
    }
}


static double get_linear_freq(t_rz_spec *spec, double w)
{
  int i;

  for(i=0; i<spec->num_bands; i++)
  {
    if(w < spec->band[i].stop - spec->band[i].start)
    {
      return spec->band[i].start + w;
    }
    else
    {
      w -= spec->band[i].stop - spec->band[i].start;
    }
  }

  return 0;
}


static int init_frequencies(t_rz_spec *spec)
{
  int i;

  double step;

  spec->sum_bands = 0;

  for(i=0; i<spec->num_bands; i++)
  {
    spec->sum_bands += spec->band[i].stop - spec->band[i].start;
  }

  step = spec->sum_bands / (spec->r + 2);

  for(i=0; i<(spec->r + 1); i++)
  {
    if(*(spec->abort))  return -1;

    if(i >= T_RZ_MAX_FREQS)
    {
      spec->error = -2;
      snprintf(spec->err_msg, T_RZ_MAX_MSG_LEN, "init_frequencies(): reached max frequencies: %i   line: %i  file: %s\n", spec->r, __LINE__, __FILE__);
      fprintf(stderr, "%s", spec->err_msg);
      return -2;
    }

    spec->frequencies[i] = get_linear_freq(spec, step * (i + 1));

//    fprintf(stdout, "spec->frequencies[%i]: %f\n", i, spec->frequencies[i]);
  }

  spec->num_frequencies = i;

//  fprintf(stdout, "spec->num_frequencies: %i\n", spec->num_frequencies);

  return 0;
}


static int binary_search(t_rz_spec *spec, int min, int max, int (*is_good)(t_rz_spec *, int c))
{
  int c;

  while(1)
  {
    if(spec->error | *(spec->abort))  return 0;

    c = (min + max) / 2;

    if(!(c % 2))  c++;

    if(is_good(spec, c))
    {
      min = c;
    }
    else
    {
      max = c;
    }

    if((max - min) < 3)
    {
      return min;
    }
  }

  return 0;
}


/* performs a barycentric Lagrange interpolation based on the
 * precalculated variables and the delta property
 */
static double get_actual_gain(t_rz_spec *spec, double w)
{
  int i;

  double numerator=0,
         denominator=0,
         cosw,
         c;

  cosw = cos(w * M_PI);

  for(i=0; i<spec->num_frequencies; i++)
  {
    if(*(spec->abort))  return -1;

    c = cosw - spec->x[i];
    if(fabs(c) < T_RZ_DELTALIMIT)
    {
      numerator = spec->y[i];
      denominator = 1;
      break;
    }
    c = spec->dn[i] / c;
    denominator += c;
    numerator += c * spec->y[i];
  }

  return numerator / denominator;
}


static int get_taps(t_rz_spec *spec, double *h)
{
  int i, k;

  double c,
         M,
         val,
         x,
         *taps=NULL;

  taps = (double *)malloc(sizeof(double) * spec->num_taps);
  if(taps == NULL)
  {
    fprintf(stderr, "malloc() error    line: %i    file: %s\n", __LINE__, __FILE__);
    return -1;
  }

  for(i=0; i<=(spec->num_taps / 2); i++)
  {
    if(*(spec->abort))  return -2;

    if(spec->num_taps % 2 == 0)
      c = cos(M_PI * i / spec->num_taps);
    else
      c = 1;
    taps[i] = get_actual_gain(spec, i * 2.0 / spec->num_taps) * c;
  }

  M = (spec->num_taps - 1) / 2.0;

  if(!(spec->num_taps % 2))
  {
    for(i=0; i<spec->num_taps; i++)
    {
      if(*(spec->abort))  return -3;

      val = taps[0];

      x = M_PI * 2 * (i - M) / spec->num_taps;

      for(k=1; k<=((spec->num_taps / 2) - 1); k++)
      {
        val += 2.0 * taps[k] * cos(x * k);
      }

      h[i] = val / spec->num_taps;
    }
  }
  else
  {
    for(i=0; i<spec->num_taps; i++)
    {
      if(*(spec->abort))  return -3;

      val = taps[0];

      x = M_PI * 2 * (i - M) / spec->num_taps;

      for(k=1; k<=M; k++)
      {
         val += 2.0 * taps[k] * cos(x * k);
      }

      h[i] = val / spec->num_taps;
    }
  }

  free(taps);

  return 0;
}


static int is_in_limits(t_rz_spec *spec, double w)
{
  int i;

  double gain;

  for(i=0;  i<spec->num_bands; i++)
  {
    if((w >= spec->band[i].start) && (w <= spec->band[i].stop))
    {
      gain = fabs(get_actual_gain(spec, w));

      if(dblcmp(spec->band[i].gain, 0))
      {
        return ((gain >= spec->band[i].lower) && (gain <= spec->band[i].upper));
      }
      else
      {
        return (gain <= spec->band[i].upper);
      }
    }
  }

  return 1;
}


static double get_weight(t_rz_spec *spec, double w)
{
  int i;

  for(i=0; i<spec->num_bands; i++)
  {
    if(w >= spec->band[i].start && w <= spec->band[i].stop)
    {
      return((spec->num_taps % 2 != 0) ? spec->band[i].weight : spec->band[i].weight * cos(w * M_PI / 2));
    }
  }

  return 0;
}


static double get_desired_gain(t_rz_spec *spec, double w)
{
  int i;

  for(i=0; i<spec->num_bands; i++)
  {
    if(w >= spec->band[i].start && w <= spec->band[i].stop)
    {
      return((spec->num_taps % 2 != 0) ? spec->band[i].gain : spec->band[i].gain / cos(w * M_PI / 2));
    }
  }

  return 0;
}


static double get_error(t_rz_spec *spec, double w)
{
  return get_weight(spec, w) * (get_desired_gain(spec, w) - get_actual_gain(spec, w));
}


static void calc_delta(t_rz_spec *spec)
{
  int i, j, sign;

  double numerator=0,
         denominator=0;

  for(i=0; i<spec->num_frequencies; i++)
  {
    spec->x[i] = cos(spec->frequencies[i] * M_PI);
  }

  if(*(spec->abort))  return;

  for(i=0; i<spec->num_frequencies; i++)
  {
    spec->di[i] = 1;
    for(j=0; j<spec->num_frequencies; j++)
    {
      if(i != j)
      {
        spec->di[i] *= (spec->x[j] - spec->x[i]);
      }
    }
    spec->dn[i] = 1.0 / spec->di[i];
  }

  if(*(spec->abort))  return;

  for(i=0, sign=1; i<spec->num_frequencies; i++)
  {
    numerator += get_desired_gain(spec, spec->frequencies[i]) * spec->dn[i];
    denominator += sign / get_weight(spec, spec->frequencies[i]) * spec->dn[i];
    sign = -sign;
  }

  spec->delta = numerator / denominator;

  if(*(spec->abort))  return;

  for(i=0, sign=1; i<spec->num_frequencies; i++)
  {
    spec->w[i] =  get_weight(spec, spec->frequencies[i]);
    spec->dy[i] = sign * spec->delta / spec->w[i];
    spec->d[i] =  get_desired_gain(spec, spec->frequencies[i]);
    spec->y[i] =  spec->d[i] - spec->dy[i];

    sign = -sign;
  }
}


static double calc_max_error(t_rz_fq_er *extrema, int num_extrema)
{
  int i;

  double maxerror = 0;

  for(i=0; i<num_extrema; i++)
  {
    if(fabs(extrema[i].error) > maxerror)
    {
      maxerror = fabs(extrema[i].error);
    }
  }

  return maxerror;
}


static double make_alternating(int firstOneSign, t_rz_fq_er *candidates, int *num_candidates, int r)
{
  int i, j, min, req_length;

//   fprintf(stdout, "entering make_alternating() ################################################################\n");
//
//   fprintf(stdout, "*num_candidates 1: %i\n", *num_candidates);

  while(*num_candidates > 0 && candidates[0].error * firstOneSign < 0)
  {
    for(i=1; i<*num_candidates; i++)
    {
      candidates[i-1] = candidates[i];
    }

    (*num_candidates)--;
  }

//  fprintf(stdout, "*num_candidates 2: %i\n", *num_candidates);

  req_length = r + 1;

  while(*num_candidates > req_length)
  {
//    fprintf(stdout, "*num_candidates 3: %i\n", *num_candidates);

    for(i=1; i<*num_candidates; i++)
    {
      if(candidates[i].error * candidates[i-1].error >= 0)
      {
        if(fabs(candidates[i].error) > fabs(candidates[i-1].error))
        {
          for(j=i; j<*num_candidates; j++)
          {
            candidates[j-1] = candidates[j];
          }

          (*num_candidates)--;
        }
        else
        {
          for(j=(i + 1); j<*num_candidates; j++)
          {
            candidates[j-1] = candidates[j];
          }

          (*num_candidates)--;
        }

        i--;
      }
    }

    if(*num_candidates == (req_length + 1))
    {
      if(fabs(candidates[0].error) > fabs(candidates[*num_candidates - 1].error))
      {
        (*num_candidates)--;
      }
      else
      {
        for(j=1; j<*num_candidates; j++)
        {
          candidates[j-1] = candidates[j];
        }

        (*num_candidates)--;
      }
    }
    else if(*num_candidates > req_length)
      {
        for(i=0, min=0; i<*num_candidates; i++)
        {
          if(fabs(candidates[i].error) < fabs(candidates[min].error))
          {
            min = i;
          }
        }

        for(j=(min + 1); j<*num_candidates; j++)
        {
          candidates[j-1] = candidates[j];
        }

        (*num_candidates)--;
      }
  }

//  fprintf(stdout, "*num_candidates 4: %i\n", *num_candidates);

  if(*num_candidates < req_length)
  {
//    fprintf(stdout, "exiting make_alternating() return -1 ################################################################\n");
    return -1;
  }

//  fprintf(stdout, "exiting make_alternating() ################################################################\n");

  return calc_max_error(candidates, *num_candidates);
}


static int get_extrema(t_rz_spec *spec, int resolution)
{
  int i, j, err=0,
      num_candidates=0,
      num_firstUp=0,
      num_firstDown=0;

  long long alloc_sz=0LL;

  double *E=NULL,
         *F=NULL,
         firstUpError,
         firstDownError;

  t_rz_fq_er *candidates=NULL,
             *firstUp=NULL,
             *firstDown=NULL,
             fq_er_tmp;

  spec->num_extrema = 0;

  alloc_sz = (long long)resolution + (spec->num_bands * 2LL) + 16LL;

  E = (double *)malloc(sizeof(double) * alloc_sz);
  F = (double *)malloc(sizeof(double) * alloc_sz);

  candidates = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * alloc_sz);
  firstUp    = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * alloc_sz);
  firstDown  = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * alloc_sz);

  if((E == NULL) || (F == NULL) || (candidates == NULL) || (firstUp == NULL) || (firstDown == NULL))
  {
    err = -1;
    fprintf(stderr, "get_extrema(): malloc() error   line: %i  file: %s\n", __LINE__, __FILE__);
    goto OUT_GET_EXTREMA;
  }

  for(i=0; i<resolution; i++)
  {
    if(*(spec->abort))
    {
      err = -2;
      goto OUT_GET_EXTREMA;
    }

    F[i] = get_linear_freq(spec, spec->sum_bands * (i + 1.0) / (resolution + 1.0));
    E[i] = get_error(spec, F[i]);
  }

//  fprintf(stdout, "num_candidates 81: %i\n", num_candidates);

  if(((E[0] > 0) && (E[0] > E[1])) ||
     ((E[0] < 0) && (E[0] < E[1])))
  {
    candidates[num_candidates].freq = F[0];
    candidates[num_candidates].error = E[0];
    num_candidates++;
  }

  j = resolution - 1;

  if(((E[j] > 0) && (E[j] > E[j-1])) ||
     ((E[j] < 0) && (E[j] < E[j-1])))
  {
    candidates[num_candidates].freq = F[j];
    candidates[num_candidates].error = E[j];
    num_candidates++;
  }

  for(i=1; i<resolution-1; i++)
  {
    if(*(spec->abort))
    {
      err = -3;
      goto OUT_GET_EXTREMA;
    }

    if((E[i] >= E[i-1] && E[i] > E[i+1] && E[i] > 0) ||
       (E[i] <= E[i-1] && E[i] < E[i+1] && E[i] < 0))
    {
      candidates[num_candidates].freq = F[i];
      candidates[num_candidates].error = E[i];
      num_candidates++;
    }
  }

  for(i=0; i<spec->num_bands; i++)
  {
    if(*(spec->abort))
    {
      err = -4;
      goto OUT_GET_EXTREMA;
    }

    candidates[num_candidates].freq = spec->band[i].start;
    candidates[num_candidates].error = get_error(spec, spec->band[i].start);
    num_candidates++;

    candidates[num_candidates].freq = spec->band[i].stop;
    candidates[num_candidates].error = get_error(spec, spec->band[i].stop);
    num_candidates++;
  }

//  fprintf(stdout, "num_candidates 82: %i\n", num_candidates);

  for(i=1; i<num_candidates; i++)  /* sort the list */
  {
    if(candidates[i].freq < candidates[i-1].freq)
    {
      for(j=i; j>0; j--)
      {
        if(candidates[j].freq >= candidates[j-1].freq)  break;

        fq_er_tmp = candidates[j];
        candidates[j] = candidates[j-1];
        candidates[j-1] = fq_er_tmp;
      }

      i--;
    }
  }

//  fprintf(stdout, "num_candidates 83: %i   resolution: %i\n", num_candidates, resolution);

  spec->num_allextrema = 0;

  for(i=0; i<num_candidates; i++)
  {
    spec->allextrema[spec->num_allextrema++] = candidates[i];

    firstUp[num_firstUp++] = candidates[i];

    firstDown[num_firstDown++] = candidates[i];
  }

//   fprintf(stdout, "num_firstUp: %i    num_firstDown: %i\n", num_firstUp, num_firstDown);
//
//   fprintf(stdout, "*num_candidates 91: %i\n", num_candidates);

  firstUpError = make_alternating(1, firstUp, &num_firstUp, spec->r);
  firstDownError = make_alternating(-1, firstDown, &num_firstDown, spec->r);

//   fprintf(stdout, "*num_candidates 92: %i\n", num_candidates);
//
//   fprintf(stdout, "firstUpError: %.19f   firstDownError: %.19f\n", firstUpError, firstDownError);
//
//   fprintf(stdout, "num_firstUp: %i    num_firstDown: %i\n", num_firstUp, num_firstDown);

  if(firstUpError > firstDownError)
  {
    for(i=0; i<num_firstUp; i++)
    {
      spec->extrema[i] = firstUp[i];
    }

    spec->num_extrema = num_firstUp;
  }
  else
  {
    for(i=0; i<num_firstDown; i++)
    {
      spec->extrema[i] = firstDown[i];
    }

    spec->num_extrema = num_firstDown;
  }

//  fprintf(stdout, "spec->num_extrema: %i\n", spec->num_extrema);

  spec->maxerror = calc_max_error(spec->allextrema, spec->num_allextrema);

//  fprintf(stdout, "spec->maxerror: %.19f\n", spec->maxerror);

OUT_GET_EXTREMA:

  free(E);
  free(F);
  free(candidates);
  free(firstUp);
  free(firstDown);

  if(err)  return err;

  return spec->num_extrema;
}


static int update_frequencies(t_rz_spec *spec)
{
  int i, j, num_e=0;

  double d_tmp;

  t_rz_fq_er *e=NULL, fq_er_tmp;

  spec->num_frequencies = 0;

  e = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * spec->max_resolution_alloc);
  if(e == NULL)
  {
    fprintf(stderr, "update_frequencies(): malloc() error   line: %i   file: %s\n", __LINE__, __FILE__);
    return -1;
  }

  for(num_e=0; num_e<spec->num_extrema; num_e++)
  {
    if(num_e >= spec->max_resolution_alloc)
    {
      fprintf(stderr, "update_frequencies(): error   line: %i   file: %s\n", __LINE__, __FILE__);
      free(e);
      return -2;
    }

    e[num_e] = spec->extrema[num_e];
  }

  for(i=1; i<num_e; i++)  /* sort the list */
  {
    if(fabs(e[i].error) < fabs(e[i-1].error))
    {
      for(j=i; j>0; j--)
      {
        if(fabs(e[j].error) >= fabs(e[j-1].error))  break;

        fq_er_tmp = e[j];
        e[j] = e[j-1];
        e[j-1] = fq_er_tmp;
      }

      i--;
    }
  }

  for(i=0, spec->num_frequencies=0; i<num_e; i++)
  {
    spec->frequencies[spec->num_frequencies++] = e[i].freq;
    if(spec->num_frequencies == (spec->r + 1))  break;
  }

  for(i=1; i<spec->num_frequencies; i++)  /* sort the list */
  {
    if(fabs(spec->frequencies[i]) < fabs(spec->frequencies[i-1]))
    {
      for(j=i; j>0; j--)
      {
        if(fabs(spec->frequencies[j]) >= fabs(spec->frequencies[j-1]))  break;

        d_tmp = spec->frequencies[j];
        spec->frequencies[j] = spec->frequencies[j-1];
        spec->frequencies[j-1] = d_tmp;
      }

      i--;
    }
  }

  free(e);

  return 0;
}


static int get_extrema_adaptively(t_rz_spec *spec)
{
  int resolution,
      num_extrema=0;

  resolution = spec->r * 16;

  while(num_extrema < spec->r + 1)
  {
    if(*(spec->abort))
    {
      free(spec->extrema);
      spec->extrema = NULL;
      free(spec->allextrema);
      spec->allextrema = NULL;
      spec->max_resolution_alloc = 0;
      return -1;
    }

    if((resolution + 16) > spec->max_resolution_alloc)
    {
      fprintf(stdout, "mem re-alloc: %.2f MiB\n", (resolution + 16.0) / (1024 * 1024));

      free(spec->extrema);
      spec->extrema = NULL;

      free(spec->allextrema);
      spec->allextrema = NULL;

      spec->max_resolution_alloc = resolution + 16;

      spec->extrema = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * spec->max_resolution_alloc);
      if(spec->extrema == NULL)
      {
        spec->max_resolution_alloc = 0;
        return -2;
      }

      spec->allextrema = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * spec->max_resolution_alloc);
      if(spec->allextrema == NULL)
      {
        free(spec->extrema);
        spec->extrema = NULL;
        spec->max_resolution_alloc = 0;
        return -3;
      }
    }

    num_extrema = get_extrema(spec, resolution);

    if(num_extrema >= spec->r + 1)  break;

    if((long long)resolution * 10LL > (256LL * 1024LL * 1024LL))
    {
      fprintf(stdout, "memory limit reached...   line: %i  file: %s\n", __LINE__, __FILE__);

      free(spec->extrema);
      free(spec->allextrema);
      spec->extrema = NULL;
      spec->allextrema = NULL;
      spec->max_resolution_alloc = 0;
      return -4;
    }

    resolution *= 10;
  }

  return 0;
}


static int design_filter(t_rz_spec *spec)
{
  int i, err, iter, maxiter=100;

  double errorchange,
         lasterror=1;

  spec->no_convergence = 0;
  spec->requirements_met = 0;
  spec->delta_too_small = 0;

  if(!(spec->num_taps % 2))
  {
    spec->r = spec->num_taps / 2;
  }
  else
  {
    spec->r = (spec->num_taps + 1) / 2;
  }

  err = init_frequencies(spec);
  if(err)
  {
    return -1;
  }

  for(iter=0; iter<maxiter; iter++)
  {
    if(*(spec->abort))  return -2;

    calc_delta(spec);

//    fprintf(stdout, "delta: %.19f\n", spec->delta);

    err = get_extrema_adaptively(spec);
    if(err)
    {
      spec->no_convergence = 1;
      fprintf(stderr, "design_filter(): ERROR: not enough extrema: error: %i   line: %i  file: %s\n", err, __LINE__, __FILE__);
      return -3;
    }

    errorchange = lasterror - spec->maxerror;

//    fprintf(stderr, "maxerror: %e   errorchange: %e   line: %i  file: %s\n", spec->maxerror, errorchange, __LINE__, __FILE__);

    if((errorchange >= 0) && (errorchange < (lasterror * 0.001)) && (errorchange < 1e-12))
    {
//      fprintf(stderr, "wtf quitting   line: %i  file: %s\n", __LINE__, __FILE__);
      break;
    }

    err = update_frequencies(spec);
    if(err)
    {
      return -4;
    }

    lasterror = spec->maxerror;
  }

  spec->requirements_met = 1;

  for(i=0; i<spec->num_allextrema; i++)
  {
    if(!is_in_limits(spec, spec->allextrema[i].freq))
    {
      spec->requirements_met = 0;
      break;
    }
  }

  if(!spec->requirements_met && iter == maxiter)
  {
    spec->no_convergence = 1;
    fprintf(stderr, "design_filter(): error, no convergence  line: %i  file: %s\n", __LINE__, __FILE__);
    return -5;
  }

  return 0;
}


static double get_only_delta(t_rz_spec *spec)
{
  int err;

  if(spec->num_taps % 2)
  {
    spec->r = (spec->num_taps + 1) / 2;
  }
  else
  {
    spec->r = spec->num_taps / 2;
  }

  err = init_frequencies(spec);
  if(err)
  {
    return 0;
  }

  calc_delta(spec);

  return spec->delta;
}


static int func_a(t_rz_spec *spec, int n)
{
  spec->num_taps = n;

  return(fabs(get_only_delta(spec)) > T_RZ_DELTALIMIT);
}


static int func_b(t_rz_spec *spec, int n)
{
  spec->num_taps = n;

  int err = design_filter(spec);
  if(err)
  {
    snprintf(spec->err_msg, T_RZ_MAX_MSG_LEN, "func_b() error: %i   line: %i  file: %s\n", err, __LINE__, __FILE__);
    spec->error = -1;
    fprintf(stderr, "%s", spec->err_msg);
    err = 1;
  }

  return((!spec->requirements_met) || spec->no_convergence || err);
}


/*  Stopband must have a gain of zero.
 *  Passband must have a gain more than zero (positive), it's usually 1.
 */
int spec_init(t_rz_spec *spec)
{
  int i;

  if(!spec)  return -1;

  if((spec->num_bands < 1) || (spec->num_bands > T_RZ_MAX_BANDS))  return -2;

  if(spec->sf < 1e-12)  return -3;

//  fprintf(stdout, "num_bands: %i\n", spec->num_bands);

  for(i=0; i<spec->num_bands; i++)
  {
//     fprintf(stdout, "start_hz: %f    stop_hz: %f   gain: %f   ripple: %f\n",
//             spec->band[i].start_hz,
//             spec->band[i].stop_hz,
//             spec->band[i].gain,
//             spec->band[i].ripple);

    if(spec->band[i].gain < 0)  return -4;

    if(spec->band[i].stop_hz > ((spec->sf / 2) + 1e-12))  return -5;

    if(spec->band[i].stop_hz <= spec->band[i].start_hz)  return -6;

    if(i)
    {
      if(spec->band[i].start_hz <= spec->band[i-1].stop_hz)  return -7;
    }

    spec->band[i].stop = spec->band[i].stop_hz / spec->sf * 2;

    spec->band[i].start = spec->band[i].start_hz / spec->sf * 2;

    if(!dblcmp(spec->band[i].gain, 0))
    {
      if(spec->band[i].ripple >= 0)  return -8;

      spec->band[i].weight = pow(10, -spec->band[i].ripple / 20);

      spec->band[i].upper = pow(10, spec->band[i].ripple / 20);
    }
    else
    {
      if(spec->band[i].ripple <= 0)  return -9;

      spec->band[i].weight = pow(10, -spec->band[i].ripple / M_E / 20);

//      fprintf(stdout, "spec->band[i].weight: %20f\n", spec->band[i].weight);

      spec->band[i].upper = 1.0 / spec->band[i].weight * spec->band[i].gain;
      spec->band[i].lower = (1.0 - (1.0 / spec->band[i].weight - 1.0)) * spec->band[i].gain;
      spec->band[i].weight = 1.0 / (1.0 / spec->band[i].weight - 1.0) / spec->band[i].gain;

//       fprintf(stdout, "spec->band[i].gain: %20f\n", spec->band[i].gain);
//       fprintf(stdout, "spec->band[i].upper: %20f\n", spec->band[i].upper);
//       fprintf(stdout, "spec->band[i].lower: %20f\n", spec->band[i].lower);
//       fprintf(stdout, "spec->band[i].weight: %20f\n", spec->band[i].weight);
    }
  }

  spec->done = 0;

  spec->extrema = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * 20000);
  if(spec->extrema == NULL)
  {
    return -10;
  }

  spec->allextrema = (t_rz_fq_er *)malloc(sizeof(t_rz_fq_er) * 20000);
  if(spec->allextrema == NULL)
  {
    free(spec->extrema);
    return -11;
  }

  spec->max_resolution_alloc = 20000;

  return 0;
}


int optimize(t_rz_spec *spec)
{
  int opt, err;

  double delta;

  spec->num_taps = 21;

  while(1)
  {
    if(*(spec->abort))  return -1;

    delta = fabs(get_only_delta(spec));
    if((delta < T_RZ_DELTALIMIT) && (delta > 0))
    {
      break;
    }
    else
    {
      spec->num_taps = spec->num_taps * 1.5 + 0.5;
    }

    if(spec->num_taps > T_RZ_MAX_TAPS)
    {
      fprintf(stderr, "optimize(): cannot design filter, requires more than %i taps.   line: %i  file: %s\n", T_RZ_MAX_TAPS, __LINE__, __FILE__);
      return -2;
    }
  }

//  fprintf(stdout, "spec->num_taps: %i\n", spec->num_taps);

  opt = binary_search(spec, 1, spec->num_taps, &func_a) + 2;

  if(spec->error)
  {
    fprintf(stderr, "optimize(): cannot design filter, XXX   line: %i  file: %s\n", __LINE__, __FILE__);
    return -3;
  }

//  fprintf(stdout, "spec->num_taps: %i   opt: %i\n", spec->num_taps, opt);

  if(spec->desired_taps)
  {
    if(opt > spec->desired_taps)
    {
      opt = spec->desired_taps;
    }
    else
    {
      fprintf(stderr, "optimize(): the specified number of taps cannot be achieved. Try specifying narrower transition bands.   line: %i  file: %s\n", __LINE__, __FILE__);
      return -4;
    }

    spec->num_taps = opt;
  }
  else
  {
    opt = binary_search(spec, 1, opt, &func_b);

    if(spec->error)
    {
      fprintf(stderr, "optimize(): cannot design filter, XXX   line: %i  file: %s\n", __LINE__, __FILE__);
      return -5;
    }

    spec->num_taps = opt + 2;
  }

  if(spec->num_taps > T_RZ_MAX_TAPS)
  {
    fprintf(stderr, "optimize(): cannot design filter, requires more than %i taps.   line: %i  file: %s\n", T_RZ_MAX_TAPS, __LINE__, __FILE__);
    return -6;
  }

//  fprintf(stdout, "spec->num_taps: %i   spec->desired_taps: %i\n", spec->num_taps, spec->desired_taps);

  err = design_filter(spec);
  if(err)
  {
    fprintf(stderr, "optimize(): design_filter() error: %i  line %i   file: %s\n", err, __LINE__, __FILE__);
    return -7;
  }

  return 0;
}


int remez(double sf, int num_bands, double *start_hz, double *stop_hz, double *gain, double *ripple, double **taps, int *abort)
{
  int i, err=0, num_taps=0;

  t_rz_spec *spec=NULL;

  *taps = NULL;

  if(abort == NULL)
  {
    num_taps = -99;
    goto OUT_REMEZ;
  }

  spec = (t_rz_spec *)calloc(1, sizeof(t_rz_spec));
  if(spec == NULL)
  {
    fprintf(stderr, "calloc() error    line: %i   file: %s\n", __LINE__, __FILE__);
    num_taps = -1;
    goto OUT_REMEZ;
  }

  spec->sf = sf;
  spec->num_bands = num_bands;
  spec->abort = abort;

  for(i=0; i<num_bands; i++)
  {
    spec->band[i].start_hz = start_hz[i];
    spec->band[i].stop_hz = stop_hz[i];
    spec->band[i].gain = gain[i];
    spec->band[i].ripple = ripple[i];
  }

  err = spec_init(spec);
  if(err)
  {
    fprintf(stderr, "spec_init() error: %i\n", err);
    num_taps = -2;
    goto OUT_REMEZ;
  }

  err = optimize(spec);
  if(err)
  {
    fprintf(stderr, "optimize() error: %i\n", err);
    num_taps = -3;
    goto OUT_REMEZ;
  }

  num_taps = spec->num_taps;

  *taps = (double *)malloc(sizeof(double) * num_taps);
  if(*taps == NULL)
  {
    fprintf(stderr, "malloc() error   line: %i  file: %s\n", __LINE__, __FILE__);
    num_taps = -4;
    goto OUT_REMEZ;
  }

  err = get_taps(spec, *taps);
  if(err)
  {
    fprintf(stderr, "get_taps() error: %i\n", err);
    num_taps = -5;
    goto OUT_REMEZ;
  }

OUT_REMEZ:

  if(spec != NULL)
  {
    free(spec->extrema);
    free(spec->allextrema);
    free(spec);
  }

  return num_taps;
}


















