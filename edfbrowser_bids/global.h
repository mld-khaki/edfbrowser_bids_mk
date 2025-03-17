/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2024 Teunis van Beelen
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


#ifndef EDFBROWSER_GLOBAL_H
#define EDFBROWSER_GLOBAL_H

#if defined(__GNUC__)

#define GCC_VERSION ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)

#if (GCC_VERSION >= 70100)
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif

#else
#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__)
/* nothing here */
#else
#error "You need the GNU C compiler!"
#endif
#endif

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#if defined(__LP64__) || defined(__MINGW64__)
#define THIS_APP_BITS_W   "64-bit"
#else
#define THIS_APP_BITS_W   "32-bit"
#endif

#if defined(_FORTIFY_SOURCE)
#error "configuration error"
#endif

#include <stdio.h>

#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__) || defined(__HAIKU__)
#define fopeno fopen
#else
#define fseeko fseeko64
#define ftello ftello64
#define fopeno fopen64
#endif

#define PROGRAM_NAME                "EDFbrowser"
#define PROGRAM_VERSION                   "2.12"
#define PROGRAM_BETA_SUFFIX                   ""
#define MINIMUM_QT4_VERSION           (0x040807)
#define MINIMUM_QT5_VERSION           (0x050C06)
#define MINIMUM_QT6_VERSION           (0x060402)
#define MAXFILES                            (32)
#define MAXSIGNALS                        (4096)
#define MAXFILTERS                          (16)
#define MAXMATHFUNCS                        (16)
#define TIME_FIXP_SCALING           (10000000LL)
#define TIME_FIXP_SCALING_HR        (TIME_FIXP_SCALING * 1000LL)
#define MAX_ANNOTATION_LEN                 (512)
#define MAX_UNIQUE_ANNOTATIONS              (64)
#define VIEWTIME_SYNCED_OFFSET               (0)
#define VIEWTIME_SYNCED_ABSOLUT              (1)
#define VIEWTIME_UNSYNCED                    (2)
#define VIEWTIME_USER_DEF_SYNCED             (3)
#define MAX_PATH_LENGTH                   (1024)
#define MAX_RECENTFILES                     (32)
#define MAX_ACTIVE_ANNOT_MARKERS           (128)
#define MAXSPECTRUMDIALOGS                  (32)
#define MAXSPECTRUMDOCKS                     (8)
#define MAXCDSADOCKS                        (64)
#define MAXAEEGDOCKS                        (64)
#define MAXHYPNOGRAMDOCKS                   (64)
#define MAXHRVDOCKS                         (64)
#define MAXPREDEFINEDMONTAGES               (12)
#define MAXAVERAGECURVEDIALOGS              (32)
#define MAXZSCOREDIALOGS                    (32)
#define MAXZOOMHISTORY                      (16)
#define MAX_ANNOTEDIT_SIDE_MENU_ANNOTS       (8)
#define MAX_MC_ANNOT_OV_COLORS               (8)
/* 200 bpm x 60 min. x 24 hours x 2 days */
#define STATISTICS_IVAL_LIST_SZ         (576000)


#define ANNOT_ID_NK_TRIGGER       (0)
#define ANNOT_ID_BS_TRIGGER       (1)

#define VIDEO_STATUS_STOPPED      (0)
#define VIDEO_STATUS_STARTUP_1    (1)
#define VIDEO_STATUS_STARTUP_2    (2)
#define VIDEO_STATUS_STARTUP_3    (3)
#define VIDEO_STATUS_STARTUP_4    (4)
#define VIDEO_STATUS_STARTUP_5    (5)
#define VIDEO_STATUS_STARTUP_6    (6)
#define VIDEO_STATUS_PLAYING     (16)
#define VIDEO_STATUS_PAUSED      (17)
#define VIDEO_STATUS_ENDED       (18)

#define VIEWTIME_INDICATOR_TYPE_RELATIVE             (0)
#define VIEWTIME_INDICATOR_TYPE_REAL_RELATIVE        (1)
#define VIEWTIME_INDICATOR_TYPE_DATE_REAL_RELATIVE   (2)

#define RC_CMD_LIST          (0)
#define RC_CMD_IDN           (1)
#define RC_CMD_QUIT          (2)
#define RC_CMD_RST           (3)
#define RC_CMD_CLS           (4)
#define RC_CMD_OPC           (5)
#define RC_CMD_ERROR         (6)
#define RC_CMD_FILE          (7)
#define RC_CMD_OPEN          (8)
#define RC_CMD_CLOSE         (9)
#define RC_CMD_ALL          (10)
#define RC_CMD_MONTAGE      (11)
#define RC_CMD_LOAD         (12)
#define RC_CMD_SIGNAL       (13)
#define RC_CMD_ADD          (14)
#define RC_CMD_LABEL        (15)
#define RC_CMD_AMPLITUDE    (16)
#define RC_CMD_FIT          (17)
#define RC_CMD_OFFSET       (18)
#define RC_CMD_ADJUST       (19)
#define RC_CMD_ZERO         (20)
#define RC_CMD_INVERT       (21)
#define RC_CMD_REMOVE       (22)
#define RC_CMD_TIMESCALE    (23)
#define RC_CMD_MMSECOND     (24)
#define RC_CMD_VIEWTIME     (25)
#define RC_CMD_TIMELOCK     (26)
#define RC_CMD_REFERENCE    (27)
#define RC_CMD_SYSTEM       (28)
#define RC_CMD_LOCKED       (29)
#define RC_CMD_MODE         (30)
#define RC_CMD_SESSION      (31)
#define RC_CMD_PLAY         (32)
#define RC_CMD_STOP         (33)
#define RC_CMD_STREAM       (34)

#define RC_CMD_LIST_SZ      (35)
#define RC_CMD_QUERY    (1 << 8)
#define RC_ERR_QUEUE_SZ     (64)

#define RC_CMD_MASK  (0xffffffff & (~RC_CMD_QUERY))

#define PRINT_LINE_WIDTH           (6)

#define PRINT_FORMAT_NONE          (0)
#define PRINT_FORMAT_PDF_A3        (1)
#define PRINT_FORMAT_PDF_A4        (2)
#define PRINT_FORMAT_PDF_LETTER    (3)
#define PRINT_FORMAT_PRINTER       (4)
#define PRINT_FORMAT_IMG           (5)


#include "edf_annot_list.h"
#include "filt/ecg_filter.h"
#include "filt/filter.h"
#include "filt/fir_filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/z_ratio_filter.h"
#include "third_party/fidlib/fidlib.h"
#include "filt/math_func.h"


typedef struct
{
  int r;
  int g;
  int b;
  int a;
} rgbac_t;

typedef struct
{
  char   label[17];
  char   transducer[81];
  char   physdimension[9];
  double phys_min;
  double phys_max;
  int    dig_min;
  int    dig_max;
  char   prefilter[81];
  int    smp_per_record;
  char   reserved[33];
  double offset;
  int    datrec_offset;
  double bitvalue;
  int    annotation;
  int    sf_int;
  double sf_f;
  long long smpls;
} edfpar_t;

typedef struct
{
  FILE      *file_hdl;
  char      version[32];
  char      filename[MAX_PATH_LENGTH];
  char      patient[81];
  char      recording[81];
  char      plus_patientcode[81];
  char      plus_sex[16];
  char      plus_birthdate[16];
  char      plus_patient_name[81];
  char      plus_patient_additional[81];
  char      plus_startdate[16];
  char      plus_admincode[81];
  char      plus_technician[81];
  char      plus_equipment[81];
  char      plus_recording_additional[81];
  int       plus_patientcode_offset;
  int       plus_sex_offset;
  int       plus_birthdate_offset;
  int       plus_patient_name_offset;
  int       plus_patient_additional_offset;
  int       plus_startdate_offset;
  int       plus_admincode_offset;
  int       plus_technician_offset;
  int       plus_equipment_offset;
  int       plus_recording_additional_offset;
  char      reserved[45];
  int       hdrsize;
  int       edfsignals;
  long long datarecords;
  int       recordsize;
  int       annot_ch[256];
  int       nr_annot_chns;
  int       edf;
  int       edfplus;
  int       bdf;
  int       bdfplus;
  int       discontinuous;
  int       genuine_nk;
  int       nk_triggers_read;
  int       genuine_biosemi;
  double    data_record_duration;
  unsigned long long long_data_record_duration;
  long long viewtime;          /* file position expressed in units of 1/TIME_FIXP_SCALING seconds, relative to the first sample in the recording */
  long long prefiltertime;
  long long starttime_hr;      /* high resolution clocktime (24 hours, no date), multiplied by TIME_FIXP_SCALING (includes the subsecond starttime) */
  long long utc_starttime;     /* seconds since the epoch january 1, 1970 */
  long long utc_starttime_hr;  /* high resolution, multiplied by TIME_FIXP_SCALING (includes the subsecond starttime) */
  long long starttime_subsec;  /* subsecond starttime (used by EDF+ and BDF+), multiplied by TIME_FIXP_SCALING */
  long long utc_endtime_hr;    /* high resolution, multiplied by TIME_FIXP_SCALING (includes the subsecond starttime) */
  int       recording_len_sec; /* duration of the recording in seconds */
  long long recording_duration_hr;  /* duration of the recording, high resolution, multiplied by TIME_FIXP_SCALING */
  int       annots_not_read;
  edfpar_t   *edfparam;
  annotlist_t annot_list;
  char      unique_annotations_list[MAX_UNIQUE_ANNOTATIONS][MAX_ANNOTATION_LEN];
  int       hypnogram_idx[MAXHYPNOGRAMDOCKS];
  int       hrv_dock[MAXHRVDOCKS];
} edfhdrblck_t;

typedef struct
{
  unsigned long long uid;
  edfhdrblck_t *edfhdr;
  int num_of_signals;
  int edfsignal[MAXSIGNALS];
  double factor[MAXSIGNALS];
  edfpar_t *edfparam_0;    /* points to edfparam[0] */

  int polarity;
  char signallabel[512];
  int signallabel_type_stripped;
  char alias[17];
  int signallabellen;
  char signallabel_bu[512];
  int signallabellen_bu;

  unsigned long long viewbufsize;
  unsigned long long viewbufoffset;
  unsigned long long records_in_viewbuf;
  unsigned long long samples_in_viewbuf;
  unsigned long long samples_on_screen;
  unsigned long long sample_start;
  unsigned long long sample_stop;
  int timeoffset;
  int sample_timeoffset;
  int pixels_shift;
  double sample_timeoffset_part;
  double sample_pixel_ratio;
  double sensitivity;
  int oldvalue;
  long long oldsmplnr;
  long long file_duration;
  int hascursor1;
  int hascursor2;
  int hasoffsettracking;
  int hasgaintracking;
  int hasruler;
  double screen_offset_pix;
  double screen_offset_unit;
  double voltpercm;
  char physdimension[9];
  char physdimension_bu[9];
  int color;
  int filter_cnt;
  int math_func_cnt_before;
  int math_func_cnt_after;
  int samples_in_prefilterbuf;
  long long prefilter_starttime;
  int prefilter_reset_sample;
  double filterpreset_a[MAXFILTERS];
  double filterpreset_b[MAXFILTERS];
  filtset_t *filter[MAXFILTERS];
  mathfuncset_t *math_func_before[MAXMATHFUNCS];
  mathfuncset_t *math_func_after[MAXMATHFUNCS];
  int max_dig_value;
  int min_dig_value;
  int fidfilter_cnt;
  int fidfilter_type[MAXFILTERS];
  double fidfilter_freq[MAXFILTERS];
  double fidfilter_freq2[MAXFILTERS];
  double fidfilter_ripple[MAXFILTERS];
  int fidfilter_order[MAXFILTERS];
  int fidfilter_model[MAXFILTERS];
  FidFilter *fidfilter[MAXFILTERS];
  FidRun *fid_run[MAXFILTERS];
  FidFunc *fidfuncp[MAXFILTERS];
  void *fidbuf[MAXFILTERS];
  void *fidbuf2[MAXFILTERS];
  int fidfilter_setup[MAXFILTERS];
  int stat_cnt;
  int stat_zero_crossing_cnt;
  double stat_sum;
  double stat_sum_sqr;
  double stat_sum_rectified;
  int draw_rectangle_sum_y;
  int draw_rectangle_sum_cnt;
  int ravg_filter_cnt;
  int ravg_filter_type[MAXFILTERS];
  int ravg_filter_size[MAXFILTERS];
  int ravg_filter_setup[MAXFILTERS];
  ravgfiltset_t *ravg_filter[MAXFILTERS];
  ecgfiltset_t *ecg_filter;
  int fir_filter_cnt;
  firfiltset_t *fir_filter[MAXFILTERS];
  int fir_filter_setup[MAXFILTERS];
  double spike_filter_velocity;
  int spike_filter_holdoff;
  spikefiltset_t *spike_filter;
  int plif_ecg_subtract_filter_plf;
  plifecgfilt_t *plif_ecg_filter;
  plifecgfilt_t *plif_ecg_filter_sav;
  int plif_eeg_subtract_filter_plf;
  plifeegfilt_t *plif_eeg_filter;
  plifeegfilt_t *plif_eeg_filter_sav;
  zratiofiltset_t *zratio_filter;
  double zratio_crossoverfreq;
  int spectr_dialog[MAXSPECTRUMDIALOGS];
  int cdsa_idx[MAXCDSADOCKS];
  int aeeg_idx[MAXAEEGDOCKS];
  int avg_dialog[MAXAVERAGECURVEDIALOGS];
  int zscoredialog[MAXZSCOREDIALOGS];
} sigcompblck_t;

typedef struct
{
  int idx;
  int history_size_tail;
  int history_size_head;
  long long viewtime[MAXZOOMHISTORY][MAXFILES];
  long long pagetime[MAXZOOMHISTORY];
  double voltpercm[MAXZOOMHISTORY][MAXSIGNALS];
  double sensitivity[MAXZOOMHISTORY][MAXSIGNALS];
  double screen_offset_unit[MAXZOOMHISTORY][MAXSIGNALS];
} zoomhistblck_t;

typedef struct
{
  edfhdrblck_t *edf_hdr;
  annotblck_t *list[MAX_ACTIVE_ANNOT_MARKERS];
  int count;
  int selected_idx;
} activemarkersblck_t;

typedef struct
{
  int x1;
  int y1;
  int x2;
  int y2;
} graphline_t;

typedef struct
{
  graphline_t graphic_line[MAXSIGNALS];
} graphbuf_t;

typedef struct
{
  int color;
  edfhdrblck_t *edf_hdr;
  int active;
  int moving;
  int position;
  int x_position;
  int y_position;
  int y_value;
  double value;
  long long time_clock;     /* clocktime (24 hours), multiplied by TIME_FIXP_SCALING */
  long long time_relative;  /* time relative to start of recording, multiplied by TIME_FIXP_SCALING */
  int has_hor_line;
  int dot_sz;
} crosshair_t;

typedef struct
{
  int status;
  int cntdwn_timer;
  int poll_timer;
  long long utc_starttime;
  int starttime_diff;
  int stop_det_counter;
  int fpos;
  int speed;
} videoplayer_t;

typedef struct
{
int tmin;
int tmax;
int invert;
int hide_other;
int hide_in_list_only;
} annotfilter_t;

#endif






