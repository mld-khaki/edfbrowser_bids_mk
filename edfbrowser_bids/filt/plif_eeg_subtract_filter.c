/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 - 2024 Teunis van Beelen
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
* Inspired by:
*
* - A simple method for the removal of mains interference from pre-recorded electrophysiological data
*   W.J. Heitler
*
***************************************************************************
*/


#include "plif_eeg_subtract_filter.h"


/*
 *
 * sf: samplefrequency (must be >= 100Hz and must be an integer multiple of the powerline frequency)
 *
 * pwlf: powerline frequency (must be set to 50Hz or 60Hz)
 *
 */
plifeegfilt_t * plif_eeg_create_subtract_filter(int sf, int pwlf)
{
  plifeegfilt_t *st;

/* perform some sanity checks */
  if(sf < 100)  return NULL;

  if((pwlf != 50) && (pwlf != 60))  return NULL;  /* powerline frequency must be either 50 or 60Hz */

  if(sf % pwlf)  return NULL;  /* ratio between the samplefrequency and the powerline frequency must be an integer multiple */

  st = (plifeegfilt_t *)calloc(1, sizeof(plifeegfilt_t));
  if(st==NULL)  return NULL;

  st->sf = sf;
  st->pwrcycles = pwlf / 2;
  st->pwrcyclesmpls = sf / pwlf;
  st->sz = sf / 2;
  st->buf = (double *)calloc(1, sizeof(double) * st->sz);
  if(st->buf == NULL)
  {
    free(st);
    return NULL;
  }

  return st;
}


double plif_eeg_run_subtract_filter(double new_input, plifeegfilt_t *st)
{
  int i;

  double val=0,
         avg=0;

  if(st == NULL)
  {
    return 0;
  }

  st->buf[st->idx] = new_input;

  for(i=0; i<st->sz; i++)
  {
    avg += st->buf[i];
  }
  avg /= st->sz;

  for(i=0; i<st->pwrcycles; i++)
  {
    val += st->buf[(st->idx + (i * st->pwrcyclesmpls)) % st->sz];
  }
  val /= st->pwrcycles;

  st->idx++;
  st->idx %= st->sz;

  return (new_input - (val - avg));
}


plifeegfilt_t * plif_eeg_subtract_filter_create_copy(plifeegfilt_t *st_ori)
{
  plifeegfilt_t *st;

  if(st_ori == NULL)
  {
    return NULL;
  }

  st = (plifeegfilt_t *)calloc(1, sizeof(plifeegfilt_t));
  if(st==NULL)  return NULL;

  *st = *st_ori;

  st->buf = (double *)calloc(1, sizeof(double) * st->sz);
  if(st->buf == NULL)
  {
    free(st);
    return NULL;
  }
  memcpy(st->buf, st_ori->buf, sizeof(double) * st->sz);

  return st;
}


void plif_eeg_free_subtract_filter(plifeegfilt_t *st)
{
  if(st == NULL)
  {
    return;
  }

  free(st->buf);
  free(st);
}


void plif_eeg_reset_subtract_filter(plifeegfilt_t *st)
{
  if(st == NULL)
  {
    return;
  }

  st->idx = 0;

  memset(st->buf, 0, sizeof(double) * st->sz);
}


void plif_eeg_subtract_filter_state_copy(plifeegfilt_t *dest, plifeegfilt_t *src)
{
  if((dest == NULL) || (src == NULL))  return;

  if(dest->sf != src->sf)  return;

  if(dest->sz != src->sz)  return;

  dest->idx = src->idx;

  memcpy(dest->buf, src->buf, sizeof(double) * dest->sz);
}



















