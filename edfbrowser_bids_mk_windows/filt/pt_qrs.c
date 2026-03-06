/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2024 Teunis van Beelen
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

/* This is an implementation of the Pan-Tompkins QRS detector algorithm.
 * The performance has been tested on an annotated arrhythmia database (MIT-BIH):
 *
 * sensitivity: 99.76 %    positive predictivity: 99.47 %
 *
 * Se = TP / (TP + FN)   sensitivity
 *
 * +P = TP / (TP + FP)   positive predictivity
 *
 * TP: true positive detections
 * FN: false negatives
 * FP: false positives
 *
 * The following conditions apply:
 *
 * 1: maximum allowed misalignment (time skew) of the detected R-peak relative to
 *    the annotation in order to be a correctly detected beat, is 110 milli-Sec.
 *
 * 2: in case parts of the signal with ventricular flutter/fibrillation were excluded
 *    during the evaluation, the rates are:
 *    sensitivity: 99.76 %    positive predictivity: 99.80 %
 *
 * 3: For every record in the database, the lead with best signal quality was chosen.
 *    The first signal in the records was chosen, except for records 108, 113, 114, 116,
 *    117, 202, 222, 228, 232 for which the second signal was selected.
 *    In case always the first signal is selected for every record, the rates are:
 *    sensitivity: 99.42 %    positive predictivity: 99.37 %
 *
 * https://courses.cs.washington.edu/courses/cse474/18wi/labs/l8/QRSdetection.pdf
 *
 * https://en.wikipedia.org/wiki/Pan%E2%80%93Tompkins_algorithm
 *
 * https://archive.physionet.org/physiobank/database/mitdb/
 */

#include <stdlib.h>
#include <math.h>

#include "pt_qrs.h"


/* The algorithm only works correctly when a sample rate of 200Hz is used.
 * This implementation has a downsampling routine added so that also higher
 * samplingrates work correctly.
 */
#define SMPL_FREQ     (200)

/* Once a valid QRS complex is recognized, there is a 200 ms refractory period
 * before the next one can be detected since QRS complexes cannot occur more
 * closely than this physiologically. This refractory period eliminates the
 * possibility of a false detection such as multiple triggering on the same
 * QRS complex during this time interval.
 */
#define REFRACT_PERIOD    (40)

/* When a QRS detection occurs following the end of the refractory period but
 * within 360 ms of the previous complex, we must determine if it is a valid
 * QRS complex or a T-wave.
 */
#define T_WAVE_PERIOD    (72)

/* Used to find the highest peak in the squared differentiator output array.
 */
#define SLOPE_SEARCH_WIDTH   (15)

/* Constants for the analog filters.
 */
#define LPF_C1    (1)
#define LPF_C2    (2)
#define LPF_C3    (6)
#define LPF_C4   (12)

#define HPF_C1    (1)
#define HPF_C2   (16)
#define HPF_C3   (17)
#define HPF_C4   (32)

#define DIFF_C1   (1)
#define DIFF_C2   (3)
#define DIFF_C3   (4)


static inline void pt_qrs_bpf_diff_sqr(double, ptqrsset_t *);
static inline void pt_qrs_adjust_thresholds(int, int, ptqrsset_t *);
static inline void pt_qrs_adjust_rr_averages(ptqrsset_t *, int);
static inline double pt_qrs_get_slope(ptqrsset_t *);
static inline int pt_qrs_search_back(ptqrsset_t *);

/* Creates a new qrs detector object.
 * ds_sf_in is the input sample rate
 * sense is the sensitivity expressed in uVolts/bit
 * if your input samples are expressed in uVolts, set it to 1
 * if your input samples are expressed in mVolts, set it to 1000
 * Returns a pointer to the qrs detector object.
 */
ptqrsset_t * create_pt_qrs(double ds_sf_in, double sense)
{
  int i;

  ptqrsset_t *st;

  if((ds_sf_in < 199.999999) || (sense < 1e-6))  return NULL;  /* sanity check */

  st = (ptqrsset_t *)calloc(1, sizeof(ptqrsset_t));
  if(st==NULL)  return NULL;

/* The compensation for the delay of the filters.
 */
  st->del_comp = 21;

  st->rr_low_limit = 0.92 * SMPL_FREQ;
  st->rr_high_limit = 1.16 * SMPL_FREQ;
  st->rr_missed_limit = 1.66 * SMPL_FREQ;

  for(i=0; i<PT_QRS_RR_AVG_LEN; i++)
  {
    st->rr_avg_x1[i] = SMPL_FREQ;
    st->rr_avg_x2[i] = SMPL_FREQ + 10;
  }

/* The article does not say if and how to initialize the variables
 * used in the algorithm. Based on results during validation using
 * the MIT-BIH arrhythmia database, I decided to use the following values.
 */
  st->spki = 2000.0 / sense;
  st->spkf = 7000.0 / sense;

  st->sqr_out_clip = (5000.0 / sense) * (5000.0 / sense);

/******** begin down sampling section ********/
  st->ds_ratio = ds_sf_in / SMPL_FREQ;

  st->ds_ravg_len = st->ds_ratio;

  st->ds_sf_out = SMPL_FREQ;

  if(st->ds_ravg_len > 1)
  {
    st->ds_ravg_buf = (double *)calloc(1, st->ds_ravg_len * sizeof(double));
    if(st->ds_ravg_buf == NULL)
      goto CREATE_OUT_ERROR;
  }
/******** end down sampling section ********/

  return st;

CREATE_OUT_ERROR:

  free_pt_qrs(st);

  return NULL;
}


void free_pt_qrs(ptqrsset_t *st)
{
  if(st == NULL)  return;

  free(st->ds_ravg_buf);
  free(st);
}

/* new_val is the new input sample.
 * st is a pointer to a qrs detector object.
 * Returns the position of the R-peak expressed in samples
 * relative to this sample when a beat has been detected.
 * Otherwise it returns 0.
 */
int run_pt_qrs(double new_val, ptqrsset_t *st)
{
  int i,
      r_peak_detected_1st_run=0,
      r_peak_detected_2nd_run=0,
      r_peak_pos=0;

  double hpf_out, mwi_out, slope;

  if(st == NULL)  return 0;

/**************************************************************/
/* start with the downsampling (the algorithm requires 200Hz) */
/**************************************************************/

  double ds_avg, ds_tmp, ds_fract, ds_val_out;

  if(st->ds_ravg_buf != NULL)
  {
    st->ds_ravg_buf[st->ds_ravg_idx++] = new_val;
    st->ds_ravg_idx %= st->ds_ravg_len;

    for(i=0, ds_avg=0; i<st->ds_ravg_len; i++)
    {
      ds_avg += st->ds_ravg_buf[i];
    }

    ds_avg /= st->ds_ravg_len;
  }
  else
  {
    ds_avg = new_val;
  }

  if(st->ds_num_smpl++ == (((long long)st->ds_smpl_pos) + 1))
  {
    ds_fract = modf(st->ds_smpl_pos, &ds_tmp);

    ds_val_out = (ds_avg * ds_fract) + (st->ds_old_val * (1.0 - ds_fract));

    st->ds_smpl_pos += st->ds_ratio;

    st->ds_old_val = ds_avg;
  }
  else
  {
    st->ds_old_val = ds_avg;

    return 0;
  }

/**************************************************************/
/*     start with the filter part of the algorithm            */
/**************************************************************/

  pt_qrs_bpf_diff_sqr(ds_val_out, st);

/**************************************************************/
/*   start with the rule-based part of the algorithm          */
/**************************************************************/

  hpf_out = st->hpf_out;
/*
 * Take the absolute value of the bandpass filter output.
 * It avoids problems in case of an inverted signal.
 */
  if(hpf_out < 0)
  {
    hpf_out *= -1;
  }
  st->hpf_out = hpf_out;

  mwi_out = st->mwi_out;

/* if it's within 200 mSec of the last peak, do nothing
 */
  if(st->smpls_last_pk <= REFRACT_PERIOD)
  {
    goto RUN_OUT_RETURN;
  }

/* check also the lower thresholds in case we need to do a second run
 */
  if((mwi_out >= st->threshold_i2) && (hpf_out >= st->threshold_f2))
  {
    if(hpf_out > st->hpf_out_lt)
    {
      st->idx_lt = st->smpls_last_pk;

      st->mwi_out_lt = mwi_out;

      st->hpf_out_lt = hpf_out;

      slope = pt_qrs_get_slope(st);

      if(st->slope_lt < slope)
      {
        st->slope_lt = slope;
      }
    }
  }

/* A peak is a local maximum determined by observing when the signal
 * changes direction within a predefined time interval.
 * So, after detecting a candidate peak, continue checking the next samples until
 * MWI or the filtered ECG starts to decline. This way we find the real peak of
 * the waveform.
 */
/* To be identified as a QRS complex, a peak must be recognized as such a complex
 * by both the integration and bandpass-filtered waveforms.
 */
  if(mwi_out >= st->threshold_i1)
  {
    if(hpf_out >= st->threshold_f1)
    {
/* A peak is a local maximum determined by observing when the signal
 * changes direction within a predefined time interval.
 * So, after detecting a candidate peak, continue checking the next samples until
 * MWI or the filtered ECG starts to decline. This way we find the real peak of
 * the waveform.
 */
      if(((st->pk_det_start) && (mwi_out >= st->mwi_out_old) && (hpf_out >= st->hpf_out_old)) || (!st->pk_det_start))
      {
        st->mwi_out_old = mwi_out;

        st->hpf_out_old = hpf_out;

        st->idx_ht = st->smpls_last_pk;

        st->pk_det_start++;

        goto RUN_OUT_RETURN;
      }
      else if(st->pk_det_start && (st->pk_det_start < SLOPE_SEARCH_WIDTH))
        {
          st->pk_det_start++;

          goto RUN_OUT_RETURN;
        }
    }
    else if(st->pk_det_start && (st->pk_det_start < SLOPE_SEARCH_WIDTH))
      {
        st->pk_det_start++;

        goto RUN_OUT_RETURN;
      }
  }
  else if(st->pk_det_start && (st->pk_det_start < SLOPE_SEARCH_WIDTH))
    {
      st->pk_det_start++;

      goto RUN_OUT_RETURN;
    }

  if(st->pk_det_start)  /* we found the highest peak */
  {
    st->pk_det_start = 0;

    st->peaki = st->mwi_out_old;
    st->peakf = st->hpf_out_old;

    slope = pt_qrs_get_slope(st);

/* When an RR interval is less than 360 ms (it must be greater than the 200 ms latency),
 * a judgement is made to determine whether the current QRS complex has been correctly
 * identified or whether it is really a T wave. If the maximal slope that occurs during
 * this waveform is less than half that of the QRS waveform that preceded it, it is
 * identified to be a T wave; otherwise it is called a QRS complex.
 */
    if((st->smpls_last_pk > T_WAVE_PERIOD) || (slope >= (st->slope_last / 2.0)))
    {
      st->slope_last = slope;

      pt_qrs_adjust_thresholds(0, 0, st);

      pt_qrs_adjust_rr_averages(st, 0);

      st->mwi_out_lt = 0;

      st->hpf_out_lt = 0;

      st->idx_lt = 0;

      st->slope_lt = 0;

      r_peak_detected_1st_run = 1;
    }
  }

  if(!r_peak_detected_1st_run)
  {
    if((st->smpls_last_pk > st->rr_missed_limit) && (st->smpls_last_pk > REFRACT_PERIOD))
    {
      r_peak_detected_2nd_run = pt_qrs_search_back(st);
    }
  }

RUN_OUT_RETURN:

  if((!r_peak_detected_1st_run) && (!r_peak_detected_2nd_run))
  {
    st->peaki = mwi_out;
    st->peakf = hpf_out;

    pt_qrs_adjust_thresholds(1, 0, st);
  }

  if((!r_peak_detected_1st_run) && (!r_peak_detected_2nd_run))
  {
    r_peak_pos = 0;
/* If no QRS complex has been recognized for 5 seconds, lower the thresholds.
 * This avoids that, after recurring artefacts, the thresholds become too high and
 * no more beats will be detected.
 */
    if(++st->smpls_last_pk >= (5 * SMPL_FREQ))
    {
      if(st->smpls_last_pk < (15 * SMPL_FREQ))
      {
        if(!(st->smpls_last_pk % SMPL_FREQ))
        {
          pt_qrs_adjust_thresholds(0, 1, st);
        }
      }
    }
  }
  else if(r_peak_detected_1st_run)
    {
      r_peak_pos = st->smpls_last_pk - st->idx_ht + st->del_comp;

      st->smpls_last_pk -= st->idx_ht;

      st->idx_ht = 0;
    }
    else if(r_peak_detected_2nd_run)
      {
        r_peak_pos = st->smpls_last_pk - st->idx_lt + st->del_comp;

        st->smpls_last_pk -= st->idx_lt;

        st->idx_lt = 0;

        st->slope_lt = 0;
      }
      else  /* catch all */
      {
        r_peak_pos = 0;

        st->smpls_last_pk++;
      }

  return ((int)(((double)r_peak_pos * st->ds_ratio) + 0.5));
}


static inline void pt_qrs_bpf_diff_sqr(double new_val, ptqrsset_t *st)
{
  int i;

  double mwi_out, hpf_out, diff_out;

  double lpf_out;

  st->lpfx[st->lpf_idx] = new_val;

  lpf_out = (2.0 * st->lpfy[(st->lpf_idx + PT_LPF_LEN - LPF_C1) % PT_LPF_LEN]) -
            st->lpfy[(st->lpf_idx + PT_LPF_LEN - LPF_C2) % PT_LPF_LEN] +
            st->lpfx[st->lpf_idx] -
            (2.0 * st->lpfx[(st->lpf_idx + PT_LPF_LEN - LPF_C3) % PT_LPF_LEN]) +
            st->lpfx[(st->lpf_idx + PT_LPF_LEN - LPF_C4) % PT_LPF_LEN];

  st->lpfy[st->lpf_idx] = lpf_out;

  st->lpf_idx++;
  st->lpf_idx %= PT_LPF_LEN;

  st->hpfx[st->hpf_idx] = lpf_out;

  hpf_out = st->hpfy[(st->hpf_idx + PT_HPF_LEN - HPF_C1) % PT_HPF_LEN] -
            (st->hpfx[st->hpf_idx] / 32.0) +
            st->hpfx[(st->hpf_idx + PT_HPF_LEN - HPF_C2) % PT_HPF_LEN] -
            st->hpfx[(st->hpf_idx + PT_HPF_LEN - HPF_C3) % PT_HPF_LEN] +
            (st->hpfx[(st->hpf_idx + PT_HPF_LEN - HPF_C4) % PT_HPF_LEN] / 32.0);

  st->hpfy[st->hpf_idx] = hpf_out;

  st->hpf_idx++;
  st->hpf_idx %= PT_HPF_LEN;

  st->hpf_out = hpf_out;

  st->diffx[st->diff_idx] = hpf_out;

  diff_out = ((2 * st->diffx[st->diff_idx]) +
              st->diffx[(st->diff_idx + PT_DIFF_LEN - DIFF_C1) % PT_DIFF_LEN] -
              st->diffx[(st->diff_idx + PT_DIFF_LEN - DIFF_C2) % PT_DIFF_LEN] -
              (2 * st->diffx[(st->diff_idx + PT_DIFF_LEN - DIFF_C3) % PT_DIFF_LEN])) / 8.0;

  st->diff_idx++;
  st->diff_idx %= PT_DIFF_LEN;

  st->sqr_out = diff_out * diff_out;

/* The output of the squaring function was hardlimited to a maximal value of 255.
 */
  if(st->sqr_out > st->sqr_out_clip)
  {
    st->sqr_out = st->sqr_out_clip;
  }

  st->mwix[st->mwi_idx] = st->sqr_out;

  for(i=0, mwi_out=0; i<PT_MWI_LEN; i++)
  {
    mwi_out += st->mwix[i];
  }
  mwi_out /= PT_MWI_LEN;

  st->mwi_idx++;
  st->mwi_idx %= PT_MWI_LEN;

  st->mwi_out = mwi_out;
}

/* If a QRS complex is not found during the interval specified by the rr_missed_limit,
 * the maximal peak reserved between the two established thresholds is considered to
 * be a QRS candidate.
 */
static inline int pt_qrs_search_back(ptqrsset_t *st)
{
  if(st->idx_lt < 1)  return 0;

  if(st->slope_lt < (st->slope_last * 0.15))  return 0;

  st->peaki = st->mwi_out_lt;
  st->peakf = st->hpf_out_lt;

  st->pk_det_start = 0;

  pt_qrs_adjust_thresholds(0, 1, st);

  pt_qrs_adjust_rr_averages(st, 1);

  st->mwi_out_lt = 0;

  st->hpf_out_lt = 0;

  return 1;
}

/* Adjusting the thresholds.
 *
 *  The thresholds are automatically adjusted to float over the noise.
 * Low thresholds are possible because of the improvements of the
 * signal-to-noise ratio by the band-pass filter. The higher of the two
 * thresholds is used for the first analysis of the signal. The lower
 * threshold is used if no QRS is detected in a certain time interval
 * so that a search-back technique is necessary to look back in time
 * for the QRS complex.
 *
 * peaki is the overall peak
 * spki is the running estimate of the signal peak
 * npki is the running estimate of the noise peak
 * threshold_i1 is the first threshold applied
 * threshold_i2 is the second threshold applied
 *
 * A peak is a local maximum determined by observing when the signal
 * changes direction within a predefined time interval. The signal
 * spki is a peak that the algorithm has already established to be a
 * QRS complex. The noise peak npki is any peak that is not related
 * to the QRS (e.g., the T-wave). The thresholds are based upon running
 * estimates of spki and npki. That is, new values of these variables
 * are computed in part from their prior values. When a new peak is
 * detected, it must first be classified as a noise peak or a signal
 * peak. To be a signal peak, the peak must exceed threshold_i1 as the
 * signal is first analyzed or threshold_i2 if searchback is required
 * to find the QRS.
 *
 * the variables referring to the filtered ECG:
 *
 * peakf is the overall peak
 * spkf is the running estimate of the signal peak
 * npkf is the running estimate of the noise peak
 * threshold_f1 is the first threshold applied
 * threshold_f2 is the second threshold applied
 *
 */
static inline void pt_qrs_adjust_thresholds(int noise, int thr2, ptqrsset_t *st)
{
  if(noise)
  {
    st->npki = (0.125 * st->peaki) + (0.875 * st->npki);
    st->npkf = (0.125 * st->peakf) + (0.875 * st->npkf);
  }
  else
  {
    if(thr2)  /* When the QRS complex is found using the second threshold */
    {
      st->spki = (0.25 * st->peaki) + (0.75 * st->spki);
      st->spkf = (0.25 * st->peakf) + (0.75 * st->spkf);
    }
    else
    {
      st->spki = (0.125 * st->peaki) + (0.875 * st->spki);
      st->spkf = (0.125 * st->peakf) + (0.875 * st->spkf);
    }
  }

  st->threshold_i1 = st->npki + (0.25 * (st->spki - st->npki));
  st->threshold_f1 = st->npkf + (0.25 * (st->spkf - st->npkf));

  st->threshold_i2 = st->threshold_i1 / 2.0;
  st->threshold_f2 = st->threshold_f1 / 2.0;
}

/* Adjusting the Average RR Interval and Rate Limits
 *
 *  Two RR-interval averages are maintained. One is the average
 * of the eight most-recent beats. The other is the average of the
 * eight most-recent beats that fall within certain limits. The
 * reason is to be able to adapt to quickly changing or irregular
 * heart rates. The first average is the mean of the eight most-
 * recent sequential RR intervals regardless of their values.
 * The second average is based on selected beats that fell between
 * the acceptable low and high RR-interval limits.
 */
static inline void pt_qrs_adjust_rr_averages(ptqrsset_t *st, int bs)
{
  int i, idx;

  if(bs)
  {
    idx = st->idx_lt;
  }
  else
  {
    idx = st->smpls_last_pk;
  }

  st->rr_avg_x1[st->rr_avg_idx1++] = idx;
  st->rr_avg_idx1 %= PT_QRS_RR_AVG_LEN;

  for(i=0, st->rr_average1=0; i<PT_QRS_RR_AVG_LEN; i++)
  {
    st->rr_average1 += st->rr_avg_x1[i];
  }
  st->rr_average1 /= PT_QRS_RR_AVG_LEN;

  if((idx >= st->rr_low_limit) && (idx <= st->rr_high_limit))
  {
    st->rr_avg_x2[st->rr_avg_idx2++] = idx;
    st->rr_avg_idx2 %= PT_QRS_RR_AVG_LEN;

    for(i=0, st->rr_average2=0; i<PT_QRS_RR_AVG_LEN; i++)
    {
      st->rr_average2 += st->rr_avg_x2[i];
    }
    st->rr_average2 /= PT_QRS_RR_AVG_LEN;
  }

  if(st->rr_average1 == st->rr_average2)
  {
    st->rr_regular = 1;
  }
  else if(st->rr_regular)
    {
      st->rr_regular = 0;

/* For irregular heart rates, the first threshold of each set is reduced
 * by half so as to increase the detection sensitivity and to avoid
 * missing beats.
 */
      st->threshold_i1 /= 2.0;
      st->threshold_f1 /= 2.0;
    }

/* In the section "Adjusting the Average RR Interval and Rate Limits" is written that
 * the RR LOW LIMIT, RR HIGH LIMIT and RR MISSED LIMIT are derived from RR AVERAGE2.
 *
 * But RR AVERAGE2 is updated only with RR intervals that are within the RR LOW LIMIT and RR HIGH LIMIT.
 * That means that, if suddenly the RR interval changes to a value outside of the limits and stays there,
 * the limits will never be adjusted. So, we use RR AVERAGE1 to derive the limits.
 */
  st->rr_low_limit    = 0.92 * st->rr_average1;
  st->rr_high_limit   = 1.16 * st->rr_average1;
  st->rr_missed_limit = 1.66 * st->rr_average1;
}

/* search back and find the highest peak in the squared differentiator output array */
static inline double pt_qrs_get_slope(ptqrsset_t *st)
{
  int i;

  double slope=0;

  for(i=0; i<SLOPE_SEARCH_WIDTH; i++)
  {
    if(slope < st->mwix[(st->mwi_idx + PT_MWI_LEN - i) % PT_MWI_LEN])
    {
      slope = st->mwix[(st->mwi_idx + PT_MWI_LEN - i) % PT_MWI_LEN];
    }
  }

  return slope;
}












