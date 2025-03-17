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


#ifndef UI_VIEW_MAINFORM_H
#define UI_VIEW_MAINFORM_H

#include "qt_headers.h"

#ifdef Q_OS_WIN32
  #include <windows.h>
  #include <io.h>
  #ifndef CSIDL_COMMON_APPDATA
    #define CSIDL_COMMON_APPDATA 0x0023 // All Users\Application Data
  #endif
  #ifndef CSIDL_APPDATA
    #define CSIDL_APPDATA 0x001a // <username>\Application Data
  #endif
  #ifndef CSIDL_PROGRAM_FILES
    #define CSIDL_PROGRAM_FILES 0x0026 // C:\Program Files
  #endif
  #if QT_VERSION >= 0x050000
    #define QT_WA(unicode, ansi) unicode
  #endif
#else
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <limits.h>

#include "global.h"
#include "viewcurve.h"
#include "tmessagebox.h"
#include "check_edf_file.h"
#include "show_edf_hdr.h"
#include "signals_dialog.h"
#include "signal_chooser.h"
#include "edf_compat.h"
#include "filter_dialog.h"
#include "jump_dialog.h"
#include "about_dialog.h"
#include "edf_annotations.h"
#include "annotations_dock.h"
#include "options_dialog.h"
#include "xml.h"
#include "save_montage_dialog.h"
#include "load_montage_dialog.h"
#include "view_montage_dialog.h"
#include "view_session_dialog.h"
#include "show_actual_montage_dialog.h"
#include "print_to_edf.h"
#include "pagetime_dialog.h"
#include "amplitude_dialog.h"
#include "print_to_bdf.h"
#include "edit_annotation_dock.h"
#include "save_annots.h"
#include "utils.h"
#include "spectrumanalyzer.h"
#include "export_annotations.h"
#include "edit_predefined_mtg.h"
#include "spectrum_dock.h"
#include "signalcurve.h"
#include "edf_annot_list.h"
#include "reduce_signals.h"
#include "header_editor.h"
#include "bdf_triggers.h"
#include "import_annotations.h"
#include "averager_curve_wnd.h"
#include "ecg_export.h"
#include "z_score_dialog.h"
#include "check_for_updates.h"
#include "date_time_stamp_parser.h"
#include "spike_filter_dialog.h"
#include "plif_ecg_subtract_filter_dialog.h"
#include "plif_eeg_subtract_filter_dialog.h"
#include "export_filtered_signals.h"
#include "fir_filter_dialog.h"
#include "cdsa_dialog.h"
#include "cdsa_dock.h"
#include "aeeg_dialog.h"
#include "aeeg_dock.h"
#include "hypnogram_dialog.h"
#include "hypnogram_dock.h"
#include "run_qrs_detector.h"
#include "hrv_dock.h"
#include "unify_resolution.h"
#include "math_f_dialog.h"

#include "cnvs/ascii_export.h"
#include "cnvs/ascii2edf.h"
#include "cnvs/bdf2edf.h"
#include "cnvs/biosemi2bdfplus.h"
#include "cnvs/biox2edf.h"
#include "cnvs/bi9800.h"
#include "cnvs/edfplusd_cnv.h"
#include "cnvs/emsa2edf.h"
#include "cnvs/fino2edf.h"
#include "cnvs/fma_ecg2edf.h"
#include "cnvs/ishne2edf.h"
#include "cnvs/manscan2edf.h"
#include "cnvs/mit2edf.h"
#include "cnvs/mortara2edf.h"
#include "cnvs/nexfin2edf.h"
#include "cnvs/nk2edf.h"
#include "cnvs/raw2edf.h"
#include "cnvs/scp_ecg2edf.h"
#include "cnvs/unisens2edf.h"
#include "cnvs/wav2edf.h"
#include "cnvs/cardiotrak2edf.h"

#include "filt/ecg_filter.h"
#include "filt/filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"
#include "filt/ravg_filter.h"
#include "filt/spike_filter.h"
#include "filt/z_ratio_filter.h"
#include "filt/math_func.h"

#include "third_party/fidlib/fidlib.h"




class ViewCurve;
class UI_Signalswindow;
class UI_Annotationswindow;
class UI_AnnotationEditwindow;
class UI_SpectrumDockWindow;
class UI_FreqSpectrumWindow;
class UI_AverageCurveWindow;
class UI_ZScoreWindow;
class UI_cdsa_dock;
class UI_aeeg_dock;
class UI_hypnogram_dock;
class UI_hrv_dock;
class Check_for_updates;


#define CMD_PARSE_STR_LEN    (64)

#define CMD_MAX_SUB_CMDS      (8)


class UI_Mainwindow : public QMainWindow
{
  Q_OBJECT

public:
  UI_Mainwindow();
  ~UI_Mainwindow();

  int files_open,
      signalcomps,
      sel_viewtime,
      viewtime_sync,
      print_to_edf_active,
      annot_editor_active,
      annot_editor_highres,
      show_annot_markers,
      show_baselines,
      annotations_edited,
      annotations_onset_relative,
      annotations_show_duration,
      annotations_duration_background_type,
      annot_onset_at_start_of_page_on_jump,
      annotlist_scrolltoitem_while_browsing,
      exit_in_progress,
      dpix,
      dpiy,
      auto_dpi,
      clip_to_pane,
      auto_reload_mtg,
      read_biosemi_status_signal,
      read_nk_trigger_signal,
      live_stream_active,
      live_stream_active_paused,
      playback_realtime_active,
      live_stream_update_interval,
      signal_averaging_active,
      powerlinefreq,
      mousewheelsens,
      average_ratio,
      average_upsidedown,
      average_bw,
      spectrum_bw,
      spectrum_sqrt,
      spectrum_vlog,
      spectrum_window,
      spectrum_blocksize_predefined,
      spectrum_blocksize_userdefined,
      spectrum_overlap,
      spectrumdock_sqrt,
      spectrumdock_vlog,
      spectrumdock_window,
      spectrumdock_blocksize_predefined,
      spectrumdock_blocksize_userdefined,
      spectrumdock_overlap,
      spectrumdock_colorbars,
      cdsa_segmentlen,
      cdsa_blocklen,
      cdsa_overlap,
      cdsa_window_func,
      cdsa_min_hz,
      cdsa_max_hz,
      cdsa_log,
      cdsa_pwr_voltage,
      cdsa_max_pwr,
      cdsa_min_pwr,
      aeeg_segmentlen,
      aeeg_max_nearby_pct,
      aeeg_min_nearby_pct,
      aeeg_plot_margins,
      use_threads,
      check_for_updates,
      amplitude_doubler,
      timescale_doubler,
      viewtime_indicator_type,
      mainwindow_title_type,
      linear_interpol,
      auto_update_annot_onset,
      average_period,
      font_size,
      monofont_size,
      sys_font_size,
      sys_monofont_size,
      use_diverse_signal_colors,
      use_signallabel_in_annot_descr,
      hrvdock_max_bpm,
      hrvdock_min_bpm,
      hrvdock_height,
      aeegdock_height,
      annot_edit_user_button_enabled[8],
      annot_editor_user_button_onset_on_page_middle[8],
      annot_editor_user_button_update_annot_description,
      annot_editor_user_button_update_annot_onset,
      annot_editor_user_button_update_annot_duration,
      annot_editor_user_button_jump_to_next_page,
      annot_editor_user_button_stay_on_epoch_boundary,
      channel_linked_annotations,
      hypnogram_use_epoch_len,
      hypnogram_use_overlays,
      hypnogramdock_height,
      dig_min_max_overflow,
      dig_min_max_overflow_warning_showed,
      options_dialog_idx,
      edf_debug,
      strip_label_types,
      default_amplitude_physmax_div,
      default_amplitude_use_physmax_div,
      rc_host_server_port,
      rc_host_server_public,
      rc_file_open_requested,
      rc_file_open_err,
      rc_load_mtg_err,
      rc_load_session_err,
      rc_load_mtg_file_num,
      rc_err_queue_idx,
      rc_err_queue[RC_ERR_QUEUE_SZ],
      rc_system_locked,
      window_width_sav_rest,
      window_height_sav_rest,
      session_start_video,
      session_video_seek,
      session_relative_paths,
      video_pause_requested,
      display_pagetime_mmsec,
      mc_v_scrollarea_ratio,
      mc_v_scrollarea_auto,
      mc_v_scrollarea_max_signals,
      default_invert_eeg_signals,
      default_time_scale,
      default_fit_signals_to_pane,
      ecg_view_mode,
      signal_plotorder;

volatile int rc_cmd_in_progress;

long long annot_editor_user_button_epoch_len,
          annot_editor_user_button_page_len,
          hypnogram_epoch_len_threshold,
          session_video_starttime,
          last_check_for_updates_time;

  int annot_created_by_rect_draw_active,
      annot_created_by_rect_draw_numchan;
  long long annot_created_by_rect_draw_onset,
            annot_created_by_rect_draw_duration;
  double annot_created_by_rect_draw_crosshair_delta;

  QColor hrvdock_trace_color,
         annot_list_edited_txt_color,
         hypnogram_annot_ov_color[6],
         mc_annot_ov_color_predefined[MAX_MC_ANNOT_OV_COLORS];

  unsigned long long pagetime,
                     maxfilesize_to_readin_annotations,
                     uid_seq,
                     totalviewbufsize_bytes;

  char *viewbuf,
       viewtime_string_128[128],
       pagetime_string_128[128],
       recent_montagedir[MAX_PATH_LENGTH],
       recent_savedir[MAX_PATH_LENGTH],
       recent_opendir[MAX_PATH_LENGTH],
       recent_video_opendir[MAX_PATH_LENGTH],
       recent_sessiondir[MAX_PATH_LENGTH],
       predefined_mtg_path[MAXPREDEFINEDMONTAGES][MAX_PATH_LENGTH],
       recent_file_mtg_path[MAX_RECENTFILES][MAX_PATH_LENGTH],
       recent_session_path[MAX_RECENTFILES][MAX_PATH_LENGTH],
       recent_colordir[MAX_PATH_LENGTH],
       cfg_app_version_str_17[17],
       drop_path[MAX_PATH_LENGTH],
       hypnogram_stage_name[6][32],
       hypnogram_annot_name[6][32],
       hypnogram_annot_ov_name[6][32],
       ecg_qrs_rpeak_descr[32],
       annot_edit_user_button_name[8][32],
       annot_by_rect_draw_description[MAX_ANNOTEDIT_SIDE_MENU_ANNOTS][32],
       mc_annot_ov_name_predefined[MAX_MC_ANNOT_OV_COLORS][32];

  double y_pixelsizefactor,
         x_pixelsizefactor,
         norm_font_pixel_height,
         norm_font_pixel_width,
         mono_font_pixel_height,
         mono_font_pixel_width,
         default_amplitude,
         cdsa_max_voltage,
         w_scaling,
         h_scaling,
         aeeg_bp_min_hz,
         aeeg_bp_max_hz,
         aeeg_lp_hz,
         aeeg_scale_max_amp;

  struct
  {
    double crossoverfreq;
    double z_threshold;
    double z_hysteresis;
    int zscore_page_len;
    int zscore_error_detection;
  } z_score_var;

  struct
  {
    int sz;
    int active;
    double *ival;
    char annot_label[MAX_ANNOTATION_LEN + 1];
    annotlist_t *annot_list;
  } toolbar_stats;

  typedef struct
  {
    int separator;
    int format;
    int duration;
    int txt_encoding;
  } export_annotations_var_t;

  typedef struct
  {
    char separator[4];
    int format;
    int onsettimeformat;
    int onsetcolumn;
    int descriptioncolumn;
    int manualdescription;
    int useduration;
    int usestoptime;
    int durationcolumn;
    int stopcolumn;
    char description[21];
    int datastartline;
    int dceventbittime;
    double triggerlevel;
    int ignoreconsecutive;
    int ascii_txt_encoding;
    int csv_equal_filename;
  } import_annotations_var_t;

  raw2edf_var_t raw2edf_var;

  edfhdrblck_t *edfheaderlist[MAXFILES];

  sigcompblck_t *signalcomp[MAXSIGNALS];

  zoomhistblck_t *zoomhistory;

  annotlist_t *annotationlist_backup;

  struct spectrum_markersblock *spectrum_colorbar;

  import_annotations_var_t *import_annotations_var;

  export_annotations_var_t *export_annotations_var;

  videoplayer_t *video_player;

  annotfilter_t *annot_filter;

  UI_Annotationswindow *annotations_dock[MAXFILES];

  UI_AnnotationEditwindow *annotationEditDock;

  UI_FreqSpectrumWindow *spectrumdialog[MAXSPECTRUMDIALOGS];

  UI_cdsa_dock *cdsa_dock[MAXCDSADOCKS];

  UI_aeeg_dock *aeeg_dock[MAXAEEGDOCKS];

  UI_hypnogram_dock *hypnogram_dock[MAXHYPNOGRAMDOCKS];

  UI_hrv_dock *hrv_dock[MAXHRVDOCKS];

  UI_SpectrumDockWindow *spectrumdock[MAXSPECTRUMDOCKS];

  UI_AverageCurveWindow *averagecurvedialog[MAXAVERAGECURVEDIALOGS];

  UI_ZScoreWindow *zscoredialog[MAXZSCOREDIALOGS];

  void setup_viewbuf();

  void setMainwindowTitle(edfhdrblck_t *);

  int file_is_opened(const char *);

  void remove_recent_file_mtg_path(const char *);

  void get_rgbcolor_settings(xml_hdl_t *, const char *, int, QColor *);

  void annot_dock_updated(void);

  int get_filenum(edfhdrblck_t *);

  void get_unique_annotations(edfhdrblck_t *);

  sigcompblck_t * create_signalcomp_copy(sigcompblck_t *);

  void enable_hrv_stats_toolbar(const char *, annotlist_t *, sigcompblck_t *);

  void remove_signalcomp(int);

  int get_signalcomp_number(const char *);

  void register_rc_err(int);

  int rc_get_last_cmd_args_token(char *, char **);

  int rc_cmd2key(const char *);

  void set_font_metrics(int);

#ifdef Q_OS_WIN32
  QString specialFolder(int);
#endif

  ViewCurve    *maincurve;

  QFont        *normfont,
               *monofont;

  QAction      *save_act,
               *save_session_act,
               *load_session_act,
               *load_predefined_mtg_act[MAXPREDEFINEDMONTAGES],
               *Escape_act,
               *video_act,
               *shift_vert_scrollbar_up_act,
               *shift_vert_scrollbar_down_act,
               *view_markers_act,
               *vert_scrollbar_act,
               *linear_interpol_act;

  QActionGroup *timelock_act_group,
               *sel_viewtime_act_group;

  QLabel       *nav_toolbar_label;

  QTimer       *dig_min_max_overflow_timer;

  QTcpServer   *rc_host_server;

  QTcpSocket   *rc_host_sock;

  QScrollBar *mc_v_scrollbar;

signals:
     void file_position_changed(long long);
     void annot_docklist_changed(void);
     void shown();

private:

  QMenuBar     *menubar;

  QMenu        *filemenu,
               *sessionmenu,
               *signalmenu,
               *timescalemenu,
               *timescale_mmsec_menu,
               *amplitudemenu,
               *viewmenu,
               *toolsmenu,
               *settingsmenu,
               *helpmenu,
               *printmenu,
               *filtermenu,
               *math_func_menu,
               *timemenu,
               *windowmenu,
               *recent_filesmenu,
               *recent_session_menu,
               *close_filemenu,
               *montagemenu,
               *patternmenu,
               *print_img_menu,
               *lhsc_uwo_bids_menu,
               *print_pdf_menu;

  char path[MAX_PATH_LENGTH],
       videopath[MAX_PATH_LENGTH],
       montagepath[MAX_PATH_LENGTH],
       session_path[MAX_PATH_LENGTH],
       recent_file_path[MAX_RECENTFILES][MAX_PATH_LENGTH],
       option_str[MAX_PATH_LENGTH];

  QAction  *former_page_Act,
           *shift_page_left_Act,
           *shift_page_right_Act,
           *playback_file_Act,
           *stop_playback_realtime_Act,
           *next_page_Act,
           *shift_page_up_Act,
           *shift_page_down_Act,
           *page_div2,
           *page_mult2,
           *page_10u,
           *page_20u,
           *page_50u,
           *page_100u,
           *page_200u,
           *page_500u,
           *page_1m,
           *page_2m,
           *page_5m,
           *page_10m,
           *page_20m,
           *page_50m,
           *page_100m,
           *page_200m,
           *page_500m,
           *page_1,
           *page_2,
           *page_5,
           *page_10,
           *page_15,
           *page_20,
           *page_30,
           *page_60,
           *page_300,
           *page_1200,
           *page_3600,
           *page_8h,
           *page_12h,
           *page_24h,
           *page_48h,
           *page_user_defined,
           *page_whole_rec,
           *fit_to_pane,
           *fit_to_dc,
           *amp_00001,
           *amp_00002,
           *amp_00005,
           *amp_0001,
           *amp_0002,
           *amp_0005,
           *amp_001,
           *amp_002,
           *amp_005,
           *amp_01,
           *amp_02,
           *amp_05,
           *amp_1,
           *amp_2,
           *amp_5,
           *amp_10,
           *amp_20,
           *amp_50,
           *amp_100,
           *amp_200,
           *amp_500,
           *amp_1000,
           *amp_2000,
           *amp_5000,
           *amp_10000,
           *amp_20000,
           *amp_50000,
           *amp_plus,
           *amp_minus,
           *zoomback_Act,
           *zoomforward_Act,
           *amplitude_user_defined,
           *slower_Act,
           *faster_Act,
           *recent_file[MAX_RECENTFILES],
           *offset_timesync_act,
           *absolut_timesync_act,
           *no_timesync_act,
           *user_def_sync_act,
           *sel_viewtime_act[MAXFILES],
           *next_crosshair_act;

  QActionGroup *AmplitudeGroup,
               *DisplayGroup,
               *load_predefined_mtg_group;

  QTimer   *live_stream_timer,
           *video_poll_timer,
           *playback_realtime_timer,
           *startup_timer;

  QElapsedTimer *playback_realtime_time;

  QSplashScreen *splash;

  QPixmap  *splash_pixmap;

  QToolBar *slidertoolbar,
           *navtoolbar;

  QSlider  *positionslider;

  QProcess *video_process;

  QTcpSocket *vlc_sock;

  int cmdlineargument,
      cmdlineoption,
      oldwindowheight;

  Check_for_updates *update_checker;

  void read_color_settings();
  void read_recent_file_settings();
  void read_general_settings();
  void write_settings();
  int read_session_file(const char *);
  int session_format_error(const char *, int, sigcompblck_t *, xml_hdl_t *);
  long long check_edf_file_datarecords(edfhdrblck_t *);
  void mpr_write(const char *);
  int mpr_read(char *, int);
  int parse_rc_command(const char *, char [CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], int *, char *, int);
  int process_rc_cmd_file(const char *, int *, int);
  int process_rc_cmd_stream(const char *, int *, int);
  int process_rc_cmd_montage(const char *, int *, int);
  int process_rc_cmd_session(const char *, int *, int);
  int process_rc_cmd_signal(const char *, int *, int);
  int process_rc_cmd_timescale(const char *, int *, int);
  int process_rc_cmd_viewtime(const char *, int *, int);
  int process_rc_cmd_timelock(const char *, int *, int);
  int process_rc_cmd_system(const char *, int *, int);

public slots:
  void remove_all_signals();
  void edfplus_remove_duplicate_annotations();
  void video_player_seek(int);
  void video_player_toggle_pause();
  void shift_page_left();
  void shift_page_right();
  void shift_page_up();
  void shift_page_down();
  void next_page();
  void next_epoch(long long);
  void dig_min_max_overflow_timer_func();
  void set_viewtime(long long);
  void fit_signals_to_pane(int n=-1);
  void fit_signals_dc_offset(int n=-1);
  void set_dc_offset_to_zero(int n=-1);
  void signalcomp_invert(int, int n=-1);
  void set_timesync_reference(int);
  void set_timesync(int);
  void shift_vert_scrollbar_up_act_triggered();
  void shift_vert_scrollbar_down_act_triggered();
  void open_manual(const char *);
  void set_page_div2();
  void set_page_mult2();
  void open_edf_file(const QString &file_path);


private slots:
  void open_new_file();
  void show_file_info();
  void close_file_action_func(QAction *);
  void close_all_files();
  void exit_program();
  void signalproperties_dialog();
  void filterproperties_dialog();
  void add_signals_dialog();
  void show_about_dialog();
  void set_display_time(QAction *);
  void set_display_time_whole_rec();
  void set_amplitude(QAction *);
  void set_amplitude_div2();
  void set_amplitude_mult2();
  void set_vert_scrollbar(bool);
  void set_view_markers(bool);
  void set_linear_interpol(bool);
  void former_page();
  void playback_file();
  void stop_playback();
  void zoomback();
  void forward();
  void show_splashscreen();
  void export_to_ascii();
  void check_edf_compatibility();
  void add_new_filter();
  void add_plif_ecg_filter();
  void add_plif_eeg_filter();
  void add_fir_filter();
  void add_spike_filter();
  void remove_all_filters();
  void remove_all_plif_ecg_filters();
  void remove_all_plif_eeg_filters();
  void remove_all_fir_filters();
  void remove_all_spike_filters();
  void add_new_math_func();
  void remove_all_math_funcs();
  void jump_to_dialog();
  void jump_to_start();
  void jump_to_end();
  void jump_to_time_millisec(long long);
  void show_annotations();
  void show_options_dialog();
  long long get_long_time(char *);
  void nk2edf_converter();
  void set_timesync(QAction *);
  void set_timesync_reference(QAction *);
  void recent_file_action_func(QAction *);
  void recent_session_action_func(QAction *);
  void sync_by_crosshairs();
  void save_montage();
  void load_montage();
  void view_montage();
  void view_session();
  void show_this_montage();
  void show_help();
  void show_kb_shortcuts();
  void print_to_edf();
  void set_user_defined_display_time();
  void set_user_defined_amplitude();
  void print_to_bdf();
  void print_to_img_640x480();
  void print_to_img_1920x1080();
  void print_to_img_2560x1440();
  void print_to_img_3840x2160();
  void print_to_img_800x600();
  void print_to_img_1024x768();
  void print_to_img_1280x1024();
  void print_to_img_1600x1200();
  void convert_ascii_to_edf();
  void convert_fino_to_edf();
  void convert_wave_to_edf();
  void convert_fm_audio_to_edf();
  void convert_mortara_to_edf();
  void convert_ishne_to_edf();
  void convert_cardiotrak_to_edf();
  void convert_nexfin_to_edf();
  void edfd_converter();
  void slider_moved(int);
  void convert_emsa_to_edf();
  void bdf2edf_converter();
  void annotation_editor();
  void save_file();
  void save_session();
  void load_session();
  void unisens2edf_converter();
  void BI98002edf_converter();
  void export_annotations();
  void unify_resolution();
  void load_predefined_mtg(QAction *);
  void edit_predefined_montages();
  void show_spectrum_dock();
  void show_cdsa_dock();
  void show_aeeg_dock();
  void show_hypnogram();
  void page_10mmmin();
  void page_15mmmin();
  void page_30mmmin();
  void page_1mmsec();
  void page_2mmsec();
  void page_5mmsec();
  void page_10mmsec();
  void page_15mmsec();
  void page_25mmsec();
  void page_30mmsec();
  void page_50mmsec();
  void page_60mmsec();
  void page_100mmsec();
  void page_120mmsec();
  void page_250mmsec();
  void page_500mmsec();
  void reduce_signals();
  void edit_header();
  void biosemi2bdfplus_converter();
  void import_annotations();
  void open_stream();
  void start_stop_video();
  void stop_video_generic(int);
  void live_stream_timer_func();
  void video_poll_timer_func();
  void playback_realtime_timer_func();
  void organize_signals();
  void Escape_fun();
  void export_ecg_rr_interval_to_ascii();
  void qrs_detector();
  void convert_binary_to_edf();
  void convert_manscan_to_edf();
  void convert_scpecg_to_edf();
  void convert_mit_to_edf();
  void convert_biox_to_edf();
  void video_process_error(QProcess::ProcessError);
  void vlc_sock_error(QAbstractSocket::SocketError);
  void export_filtered_signals();
  void video_player_faster();
  void video_player_slower();
  void create_annot_by_rect_draw();
  void rc_host_server_new_connection();
  void rc_host_sock_rxdata_handler();
  void rc_host_sock_disconnected_handler();
  void desktop_resized();
  void mc_v_scrollbar_changed(int);
//  void search_pattern();

protected:
  void closeEvent(QCloseEvent *);
  void showEvent(QShowEvent *);
};


#endif



