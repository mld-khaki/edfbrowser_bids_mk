/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 - 2024 Teunis van Beelen
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




#include "spike_filter.h"




spikefiltset_t * create_spike_filter(int sf, double sv, int ho, int *pace_detected)
{
  spikefiltset_t *st;

  if((sf < 4000) || (sf > 128000)) return NULL;

  if((sv < 1E-10) || (sv > 1E13))  return NULL;

  if((ho < 10) || (ho > 1000))  return NULL;

  st = (spikefiltset_t *) calloc(1, sizeof(spikefiltset_t));
  if(st==NULL)  return NULL;

  st->sf = sf;    // samplefrequency

  st->n_max = sf / 4000;  // Velocity is measured between sample n and sample n - n_max (tdiff is 0.25 mSec.)

  st->bufsz = (sf * 12) / 1000;  // buffer length is 12 milli-Seconds

  st->flank_det_set = (sf * 3) / 1000;  // accept only spikes with a maximum width of 3 milli-Seconds

  st->holdoff_set = (sf * ho) / 1000;  // 100 milliSec. holdoff time (do not try to detect another spike during this time)

  st->velocity = (sv / (sf / 4000)) * st->n_max;

  st->pd_sig = pace_detected;

  st->spike_pos = -1;

  return st;
}


void reset_spike_filter(spikefiltset_t *st)
{
  int i;

  st->cutoff = 0;

  st->cutoff_sav = 0;

  st->holdoff = 0;

  st->holdoff_sav = 0;

  st->flank_det = 0;

  st->flank_det_sav = 0;

  st->polarity = 0;

  st->polarity_sav = 0;

  for(i=0; i<st->n_max; i++)  st->array[i] = 0;

  st->idx = 0;

  st->idx_sav = 0;

  st->run_in = 0;

  st->run_in_sav = 0;

  st->spikewidth = 0;

  st->spikewidth_sav = 0;

  st->spike_pos = -1;

  st->spike_pos_sav = -1;

  if(st->pd_sig)  *st->pd_sig = SPIKEFILTER_SPIKE_NO;
}


void spike_filter_save_buf(spikefiltset_t *st)
{
  int i;

  st->holdoff_sav = st->holdoff;
  st->cutoff_sav = st->cutoff;
  for(i=0; i<st->n_max; i++) st->array_sav[i] = st->array[i];
  st->idx_sav = st->idx;
  st->base_smpl_sav = st->base_smpl;
  st->polarity_sav = st->polarity;
  st->flank_det_sav = st->flank_det;
  st->run_in_sav = st->run_in;
  st->spikewidth_sav = st->spikewidth;
  st->spike_pos_sav = st->spike_pos;
}


void spike_filter_restore_buf(spikefiltset_t *st)
{
  int i;

  st->holdoff = st->holdoff_sav;
  st->cutoff = st->cutoff_sav;
  for(i=0; i<st->n_max; i++) st->array[i] = st->array_sav[i];
  st->idx = st->idx_sav;
  st->base_smpl = st->base_smpl_sav;
  st->polarity = st->polarity_sav;
  st->flank_det = st->flank_det_sav;
  st->run_in = st->run_in_sav;
  st->spikewidth = st->spikewidth_sav;
  st->spike_pos = st->spike_pos_sav;
}


double run_spike_filter(double val, spikefiltset_t *st)
{
  int k,
      p_det=0,
      pol=1,
      tmp;

  if(st == NULL) return 0.0;

  if(st->run_in < st->bufsz)
  {
    st->run_in++;

    st->array[st->idx] = val;

    st->idx++;

    st->idx %= st->bufsz;

    if(st->pd_sig != NULL)
    {
      *st->pd_sig = SPIKEFILTER_SPIKE_NO;
    }

    return val;
  }

  if(st->flank_det)  st->flank_det--;

  if(st->holdoff)  st->holdoff--;

  if(st->cutoff)
  {
    st->cutoff--;

    st->array[st->idx] = val;

    st->idx++;

    st->idx %= st->bufsz;

    if(st->pd_sig != NULL)
    {
      *st->pd_sig = SPIKEFILTER_SPIKE_NO;
    }

    return st->base_smpl;  // replace the sample with the baseline value because we are in the spike period
  }

  if(st->spike_pos == st->idx)
  {
    st->spike_pos = -1;

    st->cutoff = st->spikewidth;

    st->array[st->idx] = val;

    st->idx++;

    st->idx %= st->bufsz;

    if(st->pd_sig != NULL)
    {
      *st->pd_sig = SPIKEFILTER_SPIKE_ONSET;
    }

    return st->base_smpl;  // replace the sample with the baseline value because we are in the spike period
  }

  if(!st->holdoff)
  {
    k = (st->idx + st->bufsz - st->n_max) % st->bufsz;

    tmp = val - st->array[k];  // check if there is a fast change between two samples

    if(tmp < 0)
    {
      tmp *= -1;  // we want an absolute value

      pol = -1;
    }

    if(tmp > st->velocity)  // change is fast enough to trigger a spike detect?
    {
      p_det = pol;
    }

    if(p_det)
    {
      if(st->flank_det)
      {
        if(pol != st->polarity)  // we found the second flank of the spike
        {
          st->spikewidth = st->flank_det_set - st->flank_det + (st->n_max * 2);

          st->spike_pos = (st->idx + st->bufsz - st->spikewidth) % st->bufsz;

          st->spikewidth += (st->n_max * 4);

          st->holdoff = st->holdoff_set;  // set the holdoff timer

          st->base_smpl = st->array[st->spike_pos];

          st->flank_det = 0;
        }
      }
      else  // we found the first flank of a new spike
      {
        st->polarity = pol;

        st->flank_det = st->flank_det_set;
      }
    }
  }

  st->array[st->idx] = val;

  st->idx++;

  st->idx %= st->bufsz;

  if(st->pd_sig != NULL)
  {
    *st->pd_sig = SPIKEFILTER_SPIKE_NO;
  }

  return st->array[st->idx];
}


spikefiltset_t * create_spike_filter_copy(spikefiltset_t *src)
{
  spikefiltset_t *settings;

  settings = (spikefiltset_t *) malloc(sizeof(spikefiltset_t));
  if(settings==NULL)
  {
    return NULL;
  }
  memcpy(settings, src, sizeof(spikefiltset_t));

  return settings;
}


void free_spike_filter(spikefiltset_t *st)
{
  free(st);
}













