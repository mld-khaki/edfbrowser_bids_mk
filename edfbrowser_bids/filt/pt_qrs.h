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



#ifndef PT_QRS_INCLUDED
#define PT_QRS_INCLUDED



#ifdef __cplusplus
extern "C" {
#endif


#define PT_LPF_LEN         (13)
#define PT_HPF_LEN         (33)
#define PT_DIFF_LEN         (5)
#define PT_QRS_RR_AVG_LEN   (8)
#define PT_MWI_LEN         (30)


typedef struct
{
/******** begin down sampling section ********/
  double ds_ratio;
  double ds_smpl_pos;
  double ds_old_val;

  long long ds_num_smpl;

  int ds_ravg_len;
  int ds_ravg_idx;
  int ds_sf_out;

  double *ds_ravg_buf;
/******** end down sampling section ********/

  /* high threshold max peak index */
  int idx_ht;
  /* low threshold max peak index */
  int idx_lt;

  int pk_det_start;

  double slope_last;
  double slope_lt;

  int del_comp;

  int lpf_idx;
  double lpfx[PT_LPF_LEN];
  double lpfy[PT_LPF_LEN];

  int hpf_idx;
  double hpfx[PT_HPF_LEN];
  double hpfy[PT_HPF_LEN];
  double hpf_out;
  double hpf_out_old;

  /* max peak with low threshold */
  double hpf_out_lt;

  int diff_idx;
  double diffx[PT_DIFF_LEN];

  double sqr_out;
  double sqr_out_clip;

  int mwi_idx;
  double mwix[PT_MWI_LEN];
  double mwi_out;
  double mwi_out_old;

  /* max peak with low threshold */
  double mwi_out_lt;

  double peaki;
  double spki;
  double npki;

  double threshold_i1;
  double threshold_i2;

  int smpls_last_pk;

  double peakf;
  double spkf;
  double npkf;

  double threshold_f1;
  double threshold_f2;

  int rr_avg_idx1;
  int rr_avg_idx2;
  int rr_avg_x1[PT_QRS_RR_AVG_LEN];
  int rr_avg_x2[PT_QRS_RR_AVG_LEN];
  int rr_average1;
  int rr_average2;
  int rr_low_limit;
  int rr_high_limit;
  int rr_missed_limit;
  int rr_regular;
} ptqrsset_t;


ptqrsset_t * create_pt_qrs(double, double);
void free_pt_qrs(ptqrsset_t *);
int run_pt_qrs(double, ptqrsset_t *);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif














