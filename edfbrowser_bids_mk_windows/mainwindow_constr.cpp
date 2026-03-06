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



#include "mainwindow.h"



UI_Mainwindow::UI_Mainwindow()
{
  int i, j;

  bool ret;

  normfont = new QFont;
  monofont = new QFont;
#if QT_VERSION >= 0x050200
  *normfont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
  *monofont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
#else
#ifdef Q_OS_WIN32
  normfont->setFamily("Tahoma");
  normfont->setPixelSize(11);

  monofont->setFamily("courier");
  monofont->setPixelSize(12);
#else
  normfont->setFamily("Arial");
  normfont->setPixelSize(11);

  monofont->setFamily("Monospace");
  monofont->setPixelSize(12);
#endif
#endif

  splash_pixmap = NULL;

  splash = NULL;

  startup_timer = NULL;

  setMinimumSize(640, 480);
  setWindowTitle(PROGRAM_NAME);
  setWindowIcon(QIcon(":/images/edf.png"));
  setDockNestingEnabled(true);

  setlocale(LC_NUMERIC, "C");

  uid_seq = 1LL;

  x_pixelsizefactor = 0.0294382;
  y_pixelsizefactor = 0.0294382;

  w_scaling = 1;
  h_scaling = 1;

  viewtime_indicator_type = VIEWTIME_INDICATOR_TYPE_REAL_RELATIVE;

  mainwindow_title_type = 1;

  check_for_updates = 1;

  last_check_for_updates_time = 0LL;

  use_threads = 1;

  auto_dpi = 1;

  show_annot_markers = 1;

  show_baselines = 1;

  clip_to_pane = 0;

  annotations_onset_relative = 1;

  annotations_show_duration = 1;

  annotations_duration_background_type = 0;

  annot_onset_at_start_of_page_on_jump = 0;

  annotlist_scrolltoitem_while_browsing = 0;

  auto_reload_mtg = 1;

  read_biosemi_status_signal = 1;

  read_nk_trigger_signal = 1;

  maxfilesize_to_readin_annotations = 10485760000LL;

  exit_in_progress = 0;

  live_stream_active = 0;

  live_stream_active_paused = 0;

  playback_realtime_active = 0;

  signal_averaging_active = 0;

  live_stream_update_interval = 500;

  powerlinefreq = 50;

  mousewheelsens = 10;

  amplitude_doubler = 10;

  timescale_doubler = 10;

  default_amplitude = 100;

  linear_interpol = 0;

  annot_editor_highres = 0;

  auto_update_annot_onset = 0;

  annot_editor_user_button_update_annot_description = 0;

  annot_editor_user_button_update_annot_onset = 0;

  annot_editor_user_button_update_annot_duration = 0;

  annot_editor_user_button_jump_to_next_page = 0;

  annot_editor_user_button_stay_on_epoch_boundary = 0;

  annot_editor_user_button_epoch_len = 30LL * TIME_FIXP_SCALING;

  annot_editor_user_button_page_len = 30LL * TIME_FIXP_SCALING;

  hypnogram_epoch_len_threshold = TIME_FIXP_SCALING / 10LL;

  hypnogram_use_epoch_len = 0;

  hypnogram_use_overlays = 0;

  channel_linked_annotations = 1;

  use_diverse_signal_colors = 1;

  dig_min_max_overflow = 0;

  dig_min_max_overflow_warning_showed = 0;

  options_dialog_idx = 0;

  edf_debug = 0;

  strip_label_types = 1;

  signal_plotorder = 0;

  display_pagetime_mmsec = 1;

  window_width_sav_rest = 0;
  window_height_sav_rest = 0;

  mc_v_scrollarea_ratio = 1;
  mc_v_scrollarea_auto = 0;
  mc_v_scrollarea_max_signals = 5;

  annot_created_by_rect_draw_active = 0;
  annot_created_by_rect_draw_numchan = -1;
  annot_created_by_rect_draw_onset = 0;
  annot_created_by_rect_draw_duration = 0;
  annot_created_by_rect_draw_crosshair_delta = 0;

  for(i=0; i<8; i++)
  {
    annot_edit_user_button_enabled[i] = 0;
    annot_editor_user_button_onset_on_page_middle[i] = 0;
    annot_edit_user_button_name[i][0] = 0;
  }
  strlcpy(annot_edit_user_button_name[0], "W", 32);
  strlcpy(annot_edit_user_button_name[1], "R", 32);
  strlcpy(annot_edit_user_button_name[2], "N1", 32);
  strlcpy(annot_edit_user_button_name[3], "N2", 32);
  strlcpy(annot_edit_user_button_name[4], "N3", 32);
  strlcpy(annot_edit_user_button_name[5], "N4", 32);
  strlcpy(annot_edit_user_button_name[6], "N5", 32);
  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    annot_by_rect_draw_description[i][0] = 0;
  }

  drop_path[0] = 0;

  toolbar_stats.sz = 0;
  toolbar_stats.active = 0;
  toolbar_stats.annot_label[0] = 0;
  toolbar_stats.annot_list = NULL;
  toolbar_stats.ival = (double *)calloc(1, sizeof(double) * STATISTICS_IVAL_LIST_SZ);

  recent_montagedir[0] = 0;
  recent_savedir[0] = 0;
  recent_opendir[0] = 0;
  recent_video_opendir[0] = 0;
  recent_colordir[0] = 0;

  cfg_app_version_str_17[0] = 0;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    spectrumdialog[i] = NULL;
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    cdsa_dock[i] = NULL;
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    aeeg_dock[i] = NULL;
  }

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    hypnogram_dock[i] = NULL;
  }

  for(i=0; i<MAXHRVDOCKS; i++)
  {
    hrv_dock[i] = NULL;
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    averagecurvedialog[i] = NULL;
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    zscoredialog[i] = NULL;
  }

  spectrum_colorbar = (struct spectrum_markersblock *)calloc(1, sizeof(struct spectrum_markersblock));
  for(i=0; i < MAXSPECTRUMMARKERS; i++)
  {
    spectrum_colorbar->freq[i] = 1.0;
    spectrum_colorbar->color[i] = Qt::white;
  }
  spectrum_colorbar->items = 5;
  spectrum_colorbar->freq[0] = 4.0;
  spectrum_colorbar->freq[1] = 8.0;
  spectrum_colorbar->freq[2] = 12.0;
  spectrum_colorbar->freq[3] = 30.0;
  spectrum_colorbar->freq[4] = 100.0;
  spectrum_colorbar->color[0] = Qt::darkRed;
  spectrum_colorbar->color[1] = Qt::darkGreen;
  spectrum_colorbar->color[2] = Qt::darkBlue;
  spectrum_colorbar->color[3] = Qt::darkCyan;
  spectrum_colorbar->color[4] = Qt::darkMagenta;
  spectrum_colorbar->method = 0;
  spectrum_colorbar->auto_adjust = 1;
  spectrum_colorbar->max_colorbar_value = 10.0;

  import_annotations_var = (import_annotations_var_t *)calloc(1, sizeof(import_annotations_var_t));
  strlcpy(import_annotations_var->separator, "tab", 4);
  import_annotations_var->format = 1;
  import_annotations_var->onsettimeformat = 0;
  import_annotations_var->onsetcolumn = 1;
  import_annotations_var->descriptioncolumn = 2;
  import_annotations_var->useduration = 0;
  import_annotations_var->usestoptime = 0;
  import_annotations_var->durationcolumn = 3;
  import_annotations_var->datastartline = 1;
  import_annotations_var->dceventbittime = 10;
  import_annotations_var->triggerlevel = 500.0;
  import_annotations_var->manualdescription = 0;
  import_annotations_var->description[0] = 0;
  import_annotations_var->ignoreconsecutive = 0;
  import_annotations_var->csv_equal_filename = 0;

  export_annotations_var = (export_annotations_var_t *)calloc(1, sizeof(export_annotations_var_t));
  export_annotations_var->separator = 0;
  export_annotations_var->format = 1;
  export_annotations_var->duration = 0;

  average_period = 300;

  average_ratio = 0;

  average_upsidedown = 0;

  average_bw = 0;

  spectrum_bw = 0;

  spectrum_sqrt = 0;

  spectrum_vlog = 0;

  spectrum_window = 0;

  spectrum_blocksize_predefined = 0;

  spectrum_blocksize_userdefined = 200;

  spectrum_overlap = 0;

  spectrumdock_sqrt = 0;

  spectrumdock_vlog = 0;

  spectrumdock_window = 0;

  spectrumdock_blocksize_predefined = 0;

  spectrumdock_blocksize_userdefined = 200;

  spectrumdock_overlap = 0;

  spectrumdock_colorbars = 0;

  default_amplitude_physmax_div = 1;

  default_amplitude_use_physmax_div = 0;

  rc_host_server_port = 0;

  rc_host_server_public = 0;

  rc_file_open_requested = 0;

  rc_host_server = NULL;

  rc_host_sock = NULL;

  rc_cmd_in_progress = 0;

  rc_file_open_err = 0;

  rc_load_mtg_err = 0;

  rc_load_session_err = 0;

  rc_load_mtg_file_num = 0;

  rc_err_queue_idx = 0;
  for(i=0; i<RC_ERR_QUEUE_SZ; i++)
  {
    rc_err_queue[i] = 0;
  }

  rc_system_locked = 0;

  default_invert_eeg_signals = 0;

  default_time_scale = 10;

  default_fit_signals_to_pane = 0;

  ecg_view_mode = 0;

  session_start_video = 0;

  session_video_starttime = 0;

  session_video_seek = 0;

  session_relative_paths = 0;

  video_pause_requested = 0;

  mc_annot_ov_color_predefined[0] = QColor(255, 0, 0, 32);
  mc_annot_ov_color_predefined[1] = QColor(0, 0, 128, 32);
  mc_annot_ov_color_predefined[2] = QColor(128, 128, 0, 32);
  mc_annot_ov_color_predefined[3] = QColor(255, 85, 255, 32);
  mc_annot_ov_color_predefined[4] = QColor(255, 192, 0, 32);
  mc_annot_ov_color_predefined[5] = QColor(128, 192, 64, 32);
  mc_annot_ov_color_predefined[6] = QColor(64, 0, 192, 32);
  mc_annot_ov_color_predefined[7] = QColor(0, 255, 255, 32);

  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    mc_annot_ov_name_predefined[i][0] = 0;
  }

  cdsa_segmentlen = 30;
  cdsa_blocklen = 2;
  cdsa_overlap = 5;
  cdsa_window_func = 9;
  cdsa_min_hz = 1;
  cdsa_max_hz = 30;
  cdsa_max_pwr = 13;
  cdsa_min_pwr = -5;
  cdsa_max_voltage = 5;
  cdsa_log = 1;
  cdsa_pwr_voltage = 1;

  aeeg_segmentlen = 15;
  aeeg_bp_min_hz = 2;
  aeeg_bp_max_hz = 15;
  aeeg_lp_hz = 0.5;
  aeeg_max_nearby_pct = 93;
  aeeg_min_nearby_pct = 9;
  aeeg_scale_max_amp = 100;
  aeegdock_height = 150;
  aeeg_plot_margins = 1;

  strlcpy(hypnogram_stage_name[0], "W", 32);
  strlcpy(hypnogram_stage_name[1], "R", 32);
  strlcpy(hypnogram_stage_name[2], "N1", 32);
  strlcpy(hypnogram_stage_name[3], "N2", 32);
  strlcpy(hypnogram_stage_name[4], "N3", 32);
  strlcpy(hypnogram_stage_name[5], "N4", 32);

  strlcpy(hypnogram_annot_name[0], "W", 32);
  strlcpy(hypnogram_annot_name[1], "R", 32);
  strlcpy(hypnogram_annot_name[2], "N1", 32);
  strlcpy(hypnogram_annot_name[3], "N2", 32);
  strlcpy(hypnogram_annot_name[4], "N3", 32);
  strlcpy(hypnogram_annot_name[5], "N4", 32);

  for(i=0; i<6; i++)
  {
    hypnogram_annot_ov_name[i][0] = 0;
  }

  hypnogram_annot_ov_color[0] = QColor(0, 128, 0, 32);
  hypnogram_annot_ov_color[1] = QColor(0, 0, 128, 32);
  hypnogram_annot_ov_color[2] = QColor(128, 128, 0, 32);
  hypnogram_annot_ov_color[3] = QColor(128, 0, 128, 32);
  hypnogram_annot_ov_color[4] = QColor(0, 64, 192, 32);
  hypnogram_annot_ov_color[5] = QColor(128, 192, 64, 32);

  hypnogramdock_height = 150;

  strlcpy(ecg_qrs_rpeak_descr, "R-peak", 32);

  use_signallabel_in_annot_descr = 1;

  hrvdock_max_bpm = 180;
  hrvdock_min_bpm =  40;
  hrvdock_height = 150;
  hrvdock_trace_color = Qt::red;

  z_score_var.crossoverfreq = 7.5;
  z_score_var.z_threshold = 0.0;
  z_score_var.zscore_page_len = 30;
  z_score_var.zscore_error_detection = 80;
  z_score_var.z_hysteresis = 0.0;

  raw2edf_var.sf = 256;
  raw2edf_var.chns = 1;
  raw2edf_var.phys_max = 3000;
  raw2edf_var.straightbinary = 0;
  raw2edf_var.endianness = 0;
  raw2edf_var.samplesize = 2;
  raw2edf_var.offset = 0;
  raw2edf_var.skipblocksize = 0;
  raw2edf_var.skipbytes = 1;
  strlcpy(raw2edf_var.phys_dim, "uV", 16);

  annot_filter = (annotfilter_t *)calloc(1, sizeof(annotfilter_t));
  annot_filter->tmin = 1000;
  annot_filter->tmax = 2000;
  annot_filter->invert = 0;
  annot_filter->hide_other = 1;
  annot_filter->hide_in_list_only = 0;

  annot_list_edited_txt_color.setRed(0);
  annot_list_edited_txt_color.setGreen(255);
  annot_list_edited_txt_color.setBlue(170);

  vlc_sock = NULL;

  font_size = normfont->pointSize();
  sys_font_size = normfont->pointSize();

  monofont_size = monofont->pointSize();
  sys_monofont_size = monofont->pointSize();

  read_general_settings();

  normfont->setPointSize(font_size);
  monofont->setPointSize(monofont_size);

  set_font_metrics(1);

  QApplication::setFont(*normfont);

  maincurve = new ViewCurve(this);

  dpix = maincurve->physicalDpiX();

  dpiy = maincurve->physicalDpiY();

//  printf("dpix: %i    dpiy: %i\n", dpix, dpiy);

  if(auto_dpi)
  {
    y_pixelsizefactor = 2.54 / dpiy;  /* cm per pixel vertical */

    x_pixelsizefactor = 2.54 / dpix;  /* cm per pixel horizontal */
  }

  maincurve->setAcceptDrops(true);

  QObject::connect(maincurve, SIGNAL(annot_created_by_rect_draw()), this, SLOT(create_annot_by_rect_draw()));

  read_color_settings();

  video_player = (videoplayer_t *)calloc(1, sizeof(videoplayer_t));
  video_player->poll_timer = 100;

  live_stream_timer = new QTimer;
  live_stream_timer->setSingleShot(true);
  QObject::connect(live_stream_timer, SIGNAL(timeout()), this, SLOT(live_stream_timer_func()));

  video_poll_timer = new QTimer;
  video_poll_timer->setSingleShot(true);
  QObject::connect(video_poll_timer, SIGNAL(timeout()), this, SLOT(video_poll_timer_func()));

  playback_realtime_time = new QElapsedTimer;

  playback_realtime_timer = new QTimer;
  playback_realtime_timer->setInterval(20);
  QObject::connect(playback_realtime_timer, SIGNAL(timeout()), this, SLOT(playback_realtime_timer_func()));

  dig_min_max_overflow_timer = new QTimer;
  dig_min_max_overflow_timer->setSingleShot(true);
  QObject::connect(dig_min_max_overflow_timer, SIGNAL(timeout()), this, SLOT(dig_min_max_overflow_timer_func()));

#if QT_VERSION >= 0x050000
  live_stream_timer->setTimerType(Qt::PreciseTimer);
  video_poll_timer->setTimerType(Qt::PreciseTimer);
  playback_realtime_timer->setTimerType(Qt::PreciseTimer);
#endif

  mc_v_scrollbar = new QScrollBar;
  mc_v_scrollbar->setOrientation(Qt::Vertical);
  mc_v_scrollbar->setMinimum(0);
  mc_v_scrollbar->setMaximum(100000);
  mc_v_scrollbar->setEnabled(false);
  if(!mc_v_scrollarea_auto)
  {
   mc_v_scrollbar->setVisible(false);
  }

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(maincurve, 1000);
  hlayout1->addWidget(mc_v_scrollbar);
  hlayout1->setSpacing(0);
  hlayout1->setContentsMargins(0, 0, 0, 0);

  QFrame *frame1 = new QFrame;
  frame1->setFrameStyle(QFrame::NoFrame);
  frame1->setFrameShape(QFrame::NoFrame);
  frame1->setLineWidth(0);
  frame1->setMidLineWidth(0);
  frame1->setContentsMargins(0, 0, 0, 0);
  frame1->setLayout(hlayout1);

  setCentralWidget(frame1);

  connect(mc_v_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(mc_v_scrollbar_changed(int)));

  menubar = menuBar();

  recent_filesmenu = new QMenu(this);
  recent_filesmenu->setTitle("Recent files");
  connect(recent_filesmenu, SIGNAL(triggered(QAction *)), this, SLOT(recent_file_action_func(QAction *)));

  close_filemenu = new QMenu(this);
  close_filemenu->setTitle("Close");
  connect(close_filemenu, SIGNAL(triggered(QAction *)), this, SLOT(close_file_action_func(QAction *)));

  print_img_menu = new QMenu(this);
  print_img_menu->setTitle("to Image");
  print_img_menu->addAction("640 x 480",   this, SLOT(print_to_img_640x480()));
  print_img_menu->addAction("800 x 600",   this, SLOT(print_to_img_800x600()));
  print_img_menu->addAction("1024 x 768",  this, SLOT(print_to_img_1024x768()));
  print_img_menu->addAction("1280 x 1024", this, SLOT(print_to_img_1280x1024()));
  print_img_menu->addAction("1600 x 1200", this, SLOT(print_to_img_1600x1200()));
  print_img_menu->addAction("1920 x 1080", this, SLOT(print_to_img_1920x1080()));
  print_img_menu->addAction("2560 x 1440", this, SLOT(print_to_img_2560x1440()));
  print_img_menu->addAction("3840 x 2160", this, SLOT(print_to_img_3840x2160()));

  print_pdf_menu = new QMenu(this);
  print_pdf_menu->setTitle("to PDF");
  print_pdf_menu->addAction("Letter", maincurve, SLOT(print_to_pdf_Letter()));
  print_pdf_menu->addAction("A4",     maincurve, SLOT(print_to_pdf_A4()));
  print_pdf_menu->addAction("A3",     maincurve, SLOT(print_to_pdf_A3()));

  printmenu = new QMenu(this);
  printmenu->setTitle("Print");
#if QT_VERSION < 0x060000
  printmenu->addAction("to Printer",    maincurve, SLOT(print_to_printer()), QKeySequence::Print);
#else
  printmenu->addAction("to Printer", QKeySequence::Print, maincurve, SLOT(print_to_printer()));
#endif
  printmenu->addMenu(print_pdf_menu);
  printmenu->addMenu(print_img_menu);
  printmenu->addAction("to EDF",        this, SLOT(print_to_edf()));
  printmenu->addAction("to BDF",        this, SLOT(print_to_bdf()));

  save_act = new QAction("Save as", this);
  save_act->setShortcut(QKeySequence::Save);
  save_act->setEnabled(false);
  connect(save_act, SIGNAL(triggered()), this, SLOT(save_file()));

  video_act = new QAction("Start video", this);
  connect(video_act, SIGNAL(triggered()), this, SLOT(start_stop_video()));
  video_act->setShortcut(QKeySequence("Ctrl+Shift+V"));

  filemenu = new QMenu(this);
  filemenu->setTitle("&File");
#if QT_VERSION < 0x060000
  filemenu->addAction("Open", this, SLOT(open_new_file()), QKeySequence::Open);
#else
  filemenu->addAction("Open", QKeySequence::Open, this, SLOT(open_new_file()));
#endif
  filemenu->addSeparator();
#if QT_VERSION < 0x060000
  filemenu->addAction("Open stream",  this, SLOT(open_stream()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
#else
  filemenu->addAction("Open stream", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O), this, SLOT(open_stream()));
#endif
  filemenu->addSeparator();
  filemenu->addAction(video_act);
  filemenu->addSeparator();
#if QT_VERSION < 0x060000
  filemenu->addAction("Playback file", this, SLOT(playback_file()), QKeySequence("Ctrl+Space"));
#else
  filemenu->addAction("Playback file", QKeySequence("Ctrl+Space"), this, SLOT(playback_file()));
#endif
  filemenu->addSeparator();
  filemenu->addAction(save_act);
  filemenu->addMenu(recent_filesmenu);
  filemenu->addMenu(printmenu);
  filemenu->addAction("Info",         this, SLOT(show_file_info()));
  filemenu->addMenu(close_filemenu);
#if QT_VERSION < 0x060000
  filemenu->addAction("Close all",    this, SLOT(close_all_files()), QKeySequence::Close);
  filemenu->addAction("Exit",         this, SLOT(exit_program()), QKeySequence::Quit);
#else
  filemenu->addAction("Close all", QKeySequence::Close, this, SLOT(close_all_files()));
  filemenu->addAction("Exit",      QKeySequence::Quit,  this, SLOT(exit_program()));
#endif
  menubar->addMenu(filemenu);

  recent_session_menu = new QMenu(this);
  recent_session_menu->setTitle("Recent sessions");
  connect(recent_session_menu, SIGNAL(triggered(QAction *)), this, SLOT(recent_session_action_func(QAction *)));

  save_session_act = new QAction("Save", this);
  save_session_act->setEnabled(false);
  connect(save_session_act, SIGNAL(triggered()), this, SLOT(save_session()));

  load_session_act = new QAction("Load", this);
  connect(load_session_act, SIGNAL(triggered()), this, SLOT(load_session()));

  sessionmenu = new QMenu(this);
  sessionmenu->setTitle("Sessio&n");
  sessionmenu->addAction("View saved sessions", this, SLOT(view_session()));
  sessionmenu->addAction(save_session_act);
  sessionmenu->addAction(load_session_act);
  sessionmenu->addMenu(recent_session_menu);
  menubar->addMenu(sessionmenu);

  signalmenu = new QMenu(this);
  signalmenu->setTitle("&Signals");
  signalmenu->addAction("Properties", this, SLOT(signalproperties_dialog()));
  signalmenu->addAction("Add",        this, SLOT(add_signals_dialog()));
  signalmenu->addAction("Organize",   this, SLOT(organize_signals()));
  signalmenu->addAction("Remove all", this, SLOT(remove_all_signals()));
  menubar->addMenu(signalmenu);

  timescale_mmsec_menu = new QMenu(this);
  timescale_mmsec_menu->setTitle("mm/sec");
  timescale_mmsec_menu->addAction(" 10 mm/min", this, SLOT(page_10mmmin()));
  timescale_mmsec_menu->addAction(" 15 mm/min", this, SLOT(page_15mmmin()));
  timescale_mmsec_menu->addAction(" 30 mm/min", this, SLOT(page_30mmmin()));
  timescale_mmsec_menu->addAction("  1 mm/sec", this, SLOT(page_1mmsec()));
  timescale_mmsec_menu->addAction("  2 mm/sec", this, SLOT(page_2mmsec()));
  timescale_mmsec_menu->addAction("  5 mm/sec", this, SLOT(page_5mmsec()));
  timescale_mmsec_menu->addAction(" 10 mm/sec", this, SLOT(page_10mmsec()));
  timescale_mmsec_menu->addAction(" 15 mm/sec", this, SLOT(page_15mmsec()));
  timescale_mmsec_menu->addAction(" 25 mm/sec", this, SLOT(page_25mmsec()));
  timescale_mmsec_menu->addAction(" 30 mm/sec", this, SLOT(page_30mmsec()));
  timescale_mmsec_menu->addAction(" 50 mm/sec", this, SLOT(page_50mmsec()));
  timescale_mmsec_menu->addAction(" 60 mm/sec", this, SLOT(page_60mmsec()));
  timescale_mmsec_menu->addAction("100 mm/sec", this, SLOT(page_100mmsec()));
  timescale_mmsec_menu->addAction("120 mm/sec", this, SLOT(page_120mmsec()));
  timescale_mmsec_menu->addAction("250 mm/sec", this, SLOT(page_250mmsec()));
  timescale_mmsec_menu->addAction("500 mm/sec", this, SLOT(page_500mmsec()));
  timescale_mmsec_menu->addSeparator();
  timescale_mmsec_menu->addAction("user defined", this, SLOT(set_user_defined_display_time()));

  timescalemenu = new QMenu(this);
  timescalemenu->setTitle("&Timescale");
  timescalemenu->addMenu(timescale_mmsec_menu);
  timescalemenu->addSeparator();

  page_10u = new QAction("10 uSec/page", this);
  timescalemenu->addAction(page_10u);

  page_20u = new QAction("20 uSec/page", this);
  timescalemenu->addAction(page_20u);

  page_50u = new QAction("50 uSec/page", this);
  timescalemenu->addAction(page_50u);

  page_100u = new QAction("100 uSec/page", this);
  timescalemenu->addAction(page_100u);

  page_200u = new QAction("200 uSec/page", this);
  timescalemenu->addAction(page_200u);

  page_500u = new QAction("500 uSec/page", this);
  timescalemenu->addAction(page_500u);

  page_1m = new QAction("1 mSec/page", this);
  timescalemenu->addAction(page_1m);

  page_2m = new QAction("2 mSec/page", this);
  timescalemenu->addAction(page_2m);

  page_5m = new QAction("5 mSec/page", this);
  timescalemenu->addAction(page_5m);

  page_10m = new QAction("10 mSec/page", this);
  timescalemenu->addAction(page_10m);

  page_20m = new QAction("20 mSec/page", this);
  timescalemenu->addAction(page_20m);

  page_50m = new QAction("50 mSec/page", this);
  timescalemenu->addAction(page_50m);

  page_100m = new QAction("100 mSec/page", this);
  timescalemenu->addAction(page_100m);

  page_200m = new QAction("200 mSec/page", this);
  timescalemenu->addAction(page_200m);

  page_500m = new QAction("500 mSec/page", this);
  timescalemenu->addAction(page_500m);

  page_1 = new QAction("1 Sec/page", this);
  timescalemenu->addAction(page_1);

  page_2 = new QAction("2 Sec/page", this);
  timescalemenu->addAction(page_2);

  page_5 = new QAction("5 Sec/page", this);
  timescalemenu->addAction(page_5);

  page_10 = new QAction("10 Sec/page", this);
  timescalemenu->addAction(page_10);

  page_15 = new QAction("15 Sec/page", this);
  timescalemenu->addAction(page_15);

  page_20 = new QAction("20 Sec/page", this);
  timescalemenu->addAction(page_20);

  page_30 = new QAction("30 Sec/page", this);
  page_30->setChecked(true);
  timescalemenu->addAction(page_30);

  page_60 = new QAction("60 Sec/page", this);
  timescalemenu->addAction(page_60);

  page_300 = new QAction("5 min/page", this);
  timescalemenu->addAction(page_300);

  page_1200 = new QAction("20 min/page", this);
  timescalemenu->addAction(page_1200);

  page_3600 = new QAction("1 hour/page", this);
  timescalemenu->addAction(page_3600);

  page_8h = new QAction("8 hour/page", this);
  timescalemenu->addAction(page_8h);

  page_12h = new QAction("12 hour/page", this);
  timescalemenu->addAction(page_12h);

  page_24h = new QAction("24 hour/page", this);
  timescalemenu->addAction(page_24h);

  page_48h = new QAction("48 hour/page", this);
  timescalemenu->addAction(page_48h);

  timescalemenu->addSeparator();

  page_div2 = new QAction("Timescale / 2", this);
  page_div2->setShortcut(QKeySequence::ZoomIn);
  connect(page_div2, SIGNAL(triggered()), this, SLOT(set_page_div2()));
  timescalemenu->addAction(page_div2);

  page_mult2 = new QAction("Timescale x 2", this);
  page_mult2->setShortcut(QKeySequence::ZoomOut);
  connect(page_mult2, SIGNAL(triggered()), this, SLOT(set_page_mult2()));
  timescalemenu->addAction(page_mult2);

  timescalemenu->addSeparator();

  page_user_defined = new QAction("user defined", this);
  connect(page_user_defined, SIGNAL(triggered()), this, SLOT(set_user_defined_display_time()));
  timescalemenu->addAction(page_user_defined);

  timescalemenu->addSeparator();

  page_whole_rec = new QAction("whole recording", this);
  connect(page_whole_rec, SIGNAL(triggered()), this, SLOT(set_display_time_whole_rec()));
  timescalemenu->addAction(page_whole_rec);

  menubar->addMenu(timescalemenu);

  DisplayGroup = new QActionGroup(this);
  DisplayGroup->addAction(page_10u);
  DisplayGroup->addAction(page_20u);
  DisplayGroup->addAction(page_50u);
  DisplayGroup->addAction(page_100u);
  DisplayGroup->addAction(page_200u);
  DisplayGroup->addAction(page_500u);
  DisplayGroup->addAction(page_1m);
  DisplayGroup->addAction(page_2m);
  DisplayGroup->addAction(page_5m);
  DisplayGroup->addAction(page_10m);
  DisplayGroup->addAction(page_20m);
  DisplayGroup->addAction(page_50m);
  DisplayGroup->addAction(page_100m);
  DisplayGroup->addAction(page_200m);
  DisplayGroup->addAction(page_500m);
  DisplayGroup->addAction(page_1);
  DisplayGroup->addAction(page_2);
  DisplayGroup->addAction(page_5);
  DisplayGroup->addAction(page_10);
  DisplayGroup->addAction(page_15);
  DisplayGroup->addAction(page_20);
  DisplayGroup->addAction(page_30);
  DisplayGroup->addAction(page_60);
  DisplayGroup->addAction(page_300);
  DisplayGroup->addAction(page_1200);
  DisplayGroup->addAction(page_3600);
  DisplayGroup->addAction(page_8h);
  DisplayGroup->addAction(page_12h);
  DisplayGroup->addAction(page_24h);
  DisplayGroup->addAction(page_48h);
  connect(DisplayGroup, SIGNAL(triggered(QAction *)), this, SLOT(set_display_time(QAction *)));

  amplitudemenu = new QMenu(this);
  amplitudemenu->setTitle("&Amplitude");

  fit_to_pane = new QAction("Fit to pane", this);
  connect(fit_to_pane, SIGNAL(triggered()), this, SLOT(fit_signals_to_pane()));
  amplitudemenu->addAction(fit_to_pane);

  fit_to_dc = new QAction("Adjust offset", this);
  connect(fit_to_dc, SIGNAL(triggered()), this, SLOT(fit_signals_dc_offset()));
  amplitudemenu->addAction(fit_to_dc);

  amplitudemenu->addAction("Offset -> 0", this, SLOT(set_dc_offset_to_zero()));

  amplitudemenu->addSeparator();

  amp_50000 = new QAction("50000", this);
  amplitudemenu->addAction(amp_50000);

  amp_20000 = new QAction("20000", this);
  amplitudemenu->addAction(amp_20000);

  amp_10000 = new QAction("10000", this);
  amplitudemenu->addAction(amp_10000);

  amp_5000 = new QAction("5000", this);
  amplitudemenu->addAction(amp_5000);

  amp_2000 = new QAction("2000", this);
  amplitudemenu->addAction(amp_2000);

  amp_1000 = new QAction("1000", this);
  amplitudemenu->addAction(amp_1000);

  amp_500 = new QAction("500", this);
  amplitudemenu->addAction(amp_500);

  amp_200 = new QAction("200", this);
  amplitudemenu->addAction(amp_200);

  amp_100 = new QAction("100", this);
  amplitudemenu->addAction(amp_100);

  amp_50 = new QAction("50", this);
  amplitudemenu->addAction(amp_50);

  amp_20 = new QAction("20", this);
  amplitudemenu->addAction(amp_20);

  amp_10 = new QAction("10", this);
  amplitudemenu->addAction(amp_10);

  amp_5 = new QAction("5", this);
  amplitudemenu->addAction(amp_5);

  amp_2 = new QAction("2", this);
  amplitudemenu->addAction(amp_2);

  amp_1 = new QAction("1", this);
  amplitudemenu->addAction(amp_1);

  amp_05 = new QAction("0.5", this);
  amplitudemenu->addAction(amp_05);

  amp_02 = new QAction("0.2", this);
  amplitudemenu->addAction(amp_02);

  amp_01 = new QAction("0.1", this);
  amplitudemenu->addAction(amp_01);

  amp_005 = new QAction("0.05", this);
  amplitudemenu->addAction(amp_005);

  amp_002 = new QAction("0.02", this);
  amplitudemenu->addAction(amp_002);

  amp_001 = new QAction("0.01", this);
  amplitudemenu->addAction(amp_001);

  amp_0005 = new QAction("0.005", this);
  amplitudemenu->addAction(amp_0005);

  amp_0002 = new QAction("0.002", this);
  amplitudemenu->addAction(amp_0002);

  amp_0001 = new QAction("0.001", this);
  amplitudemenu->addAction(amp_0001);

  amp_00005 = new QAction("0.0005", this);
  amplitudemenu->addAction(amp_00005);

  amp_00002 = new QAction("0.0002", this);
  amplitudemenu->addAction(amp_00002);

  amp_00001 = new QAction("0.0001", this);
  amplitudemenu->addAction(amp_00001);

  amplitudemenu->addSeparator();

  amplitude_user_defined = new QAction("user defined", this);
  connect(amplitude_user_defined, SIGNAL(triggered()), this, SLOT(set_user_defined_amplitude()));
  amplitudemenu->addAction(amplitude_user_defined);

  amplitudemenu->addSeparator();

  amp_plus = new QAction("Amplitude x 2", this);
  amp_plus->setShortcut(Qt::Key_Minus);
  connect(amp_plus, SIGNAL(triggered()), this, SLOT(set_amplitude_mult2()));
  amplitudemenu->addAction(amp_plus);

  amp_minus = new QAction("Amplitude / 2", this);
  amp_minus->setShortcut(Qt::Key_Plus);
  connect(amp_minus, SIGNAL(triggered()), this, SLOT(set_amplitude_div2()));
  amplitudemenu->addAction(amp_minus);

  menubar->addMenu(amplitudemenu);

  AmplitudeGroup = new QActionGroup(this);
  AmplitudeGroup->addAction(amp_00001);
  AmplitudeGroup->addAction(amp_00002);
  AmplitudeGroup->addAction(amp_00005);
  AmplitudeGroup->addAction(amp_0001);
  AmplitudeGroup->addAction(amp_0002);
  AmplitudeGroup->addAction(amp_0005);
  AmplitudeGroup->addAction(amp_001);
  AmplitudeGroup->addAction(amp_002);
  AmplitudeGroup->addAction(amp_005);
  AmplitudeGroup->addAction(amp_01);
  AmplitudeGroup->addAction(amp_02);
  AmplitudeGroup->addAction(amp_05);
  AmplitudeGroup->addAction(amp_1);
  AmplitudeGroup->addAction(amp_2);
  AmplitudeGroup->addAction(amp_5);
  AmplitudeGroup->addAction(amp_10);
  AmplitudeGroup->addAction(amp_20);
  AmplitudeGroup->addAction(amp_50);
  AmplitudeGroup->addAction(amp_100);
  AmplitudeGroup->addAction(amp_200);
  AmplitudeGroup->addAction(amp_500);
  AmplitudeGroup->addAction(amp_1000);
  AmplitudeGroup->addAction(amp_2000);
  AmplitudeGroup->addAction(amp_5000);
  AmplitudeGroup->addAction(amp_10000);
  AmplitudeGroup->addAction(amp_20000);
  AmplitudeGroup->addAction(amp_50000);
  connect(AmplitudeGroup, SIGNAL(triggered(QAction *)), this, SLOT(set_amplitude(QAction *)));

  view_markers_act = new QAction("Annotation Markers", this);
  view_markers_act->setCheckable(true);
#if QT_VERSION < 0x060000
  view_markers_act->setShortcut(Qt::CTRL + Qt::Key_M);
#else
  view_markers_act->setShortcut(Qt::CTRL | Qt::Key_M);
#endif
  if(show_annot_markers)
  {
    view_markers_act->setChecked(true);
  }
  else
  {
    view_markers_act->setChecked(false);
  }
  connect(view_markers_act, SIGNAL(toggled(bool)), this, SLOT(set_view_markers(bool)));

  vert_scrollbar_act = new QAction("Vertical scrollbar", this);
  vert_scrollbar_act->setCheckable(true);
  if(mc_v_scrollarea_auto)
  {
    vert_scrollbar_act->setChecked(true);
  }
  else
  {
    vert_scrollbar_act->setChecked(false);
  }
  connect(vert_scrollbar_act, SIGNAL(toggled(bool)), this, SLOT(set_vert_scrollbar(bool)));

  linear_interpol_act = new QAction("Linear interpolation", this);
  linear_interpol_act->setCheckable(true);
  if(linear_interpol)
  {
    linear_interpol_act->setChecked(true);
  }
  else
  {
    linear_interpol_act->setChecked(false);
  }
  connect(linear_interpol_act, SIGNAL(toggled(bool)), this, SLOT(set_linear_interpol(bool)));

  viewmenu = new QMenu(this);
  viewmenu->setTitle("&View");
  viewmenu->addAction(view_markers_act);
  viewmenu->addAction(vert_scrollbar_act);
  viewmenu->addAction(linear_interpol_act);
  menubar->addMenu(viewmenu);

  math_func_menu = new QMenu(this);
  math_func_menu->setTitle("Math");
  math_func_menu->addAction("New", this, SLOT(add_new_math_func()));
  math_func_menu->addAction("Remove all", this, SLOT(remove_all_math_funcs()));

  filtermenu = new QMenu(this);
  filtermenu->setTitle("Filte&r");
  filtermenu->addAction("Classic Butterworth, Bessel, Chebyshev, etc.", this, SLOT(add_new_filter()));
  filtermenu->addAction("Adjust classic filters", this, SLOT(filterproperties_dialog()));
  filtermenu->addAction("Remove all classic filters", this, SLOT(remove_all_filters()));
  filtermenu->addSeparator();
  filtermenu->addAction("Powerline interference removal for ECG", this, SLOT(add_plif_ecg_filter()));
  filtermenu->addAction("Remove all ECG Powerline interference filters", this, SLOT(remove_all_plif_ecg_filters()));
  filtermenu->addSeparator();
  filtermenu->addAction("Powerline interference removal for EEG", this, SLOT(add_plif_eeg_filter()));
  filtermenu->addAction("Remove all EEG Powerline interference filters", this, SLOT(remove_all_plif_eeg_filters()));
  filtermenu->addSeparator();
  filtermenu->addAction("Custom FIR filter", this, SLOT(add_fir_filter()));
  filtermenu->addAction("Remove all custom FIR filters", this, SLOT(remove_all_fir_filters()));
  filtermenu->addSeparator();
  filtermenu->addAction("Spike", this, SLOT(add_spike_filter()));
  filtermenu->addAction("Remove all spike filters", this, SLOT(remove_all_spike_filters()));
  filtermenu->addSeparator();
  filtermenu->addMenu(math_func_menu);
  menubar->addMenu(filtermenu);

  load_predefined_mtg_act[0] = new QAction("Empty", this);
  load_predefined_mtg_act[0]->setShortcut(Qt::Key_F1);
  load_predefined_mtg_act[1] = new QAction("Empty", this);
  load_predefined_mtg_act[1]->setShortcut(Qt::Key_F2);
  load_predefined_mtg_act[2] = new QAction("Empty", this);
  load_predefined_mtg_act[2]->setShortcut(Qt::Key_F3);
  load_predefined_mtg_act[3] = new QAction("Empty", this);
  load_predefined_mtg_act[3]->setShortcut(Qt::Key_F4);
  load_predefined_mtg_act[4] = new QAction("Empty", this);
  load_predefined_mtg_act[4]->setShortcut(Qt::Key_F5);
  load_predefined_mtg_act[5] = new QAction("Empty", this);
  load_predefined_mtg_act[5]->setShortcut(Qt::Key_F6);
  load_predefined_mtg_act[6] = new QAction("Empty", this);
  load_predefined_mtg_act[6]->setShortcut(Qt::Key_F7);
  load_predefined_mtg_act[7] = new QAction("Empty", this);
  load_predefined_mtg_act[7]->setShortcut(Qt::Key_F8);
  load_predefined_mtg_act[8] = new QAction("Empty", this);
  load_predefined_mtg_act[8]->setShortcut(Qt::Key_F9);
  load_predefined_mtg_act[9] = new QAction("Empty", this);
  load_predefined_mtg_act[9]->setShortcut(Qt::Key_F10);
  load_predefined_mtg_act[10] = new QAction("Empty", this);
  load_predefined_mtg_act[10]->setShortcut(Qt::Key_F11);
  load_predefined_mtg_act[11] = new QAction("Empty", this);
  load_predefined_mtg_act[11]->setShortcut(Qt::Key_F12);
  load_predefined_mtg_group = new QActionGroup(this);
  for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
  {
    load_predefined_mtg_group->addAction(load_predefined_mtg_act[i]);
  }
  connect(load_predefined_mtg_group, SIGNAL(triggered(QAction *)), this, SLOT(load_predefined_mtg(QAction *)));

  montagemenu = new QMenu(this);
  montagemenu->setTitle("&Montage");
  montagemenu->addAction("View this montage", this, SLOT(show_this_montage()));
  montagemenu->addAction("View saved montages", this, SLOT(view_montage()));
  montagemenu->addAction("Save", this, SLOT(save_montage()));
  montagemenu->addAction("Load", this, SLOT(load_montage()));
  montagemenu->addSeparator();
  montagemenu->addAction("Edit key-bindings for montages", this, SLOT(edit_predefined_montages()));
  montagemenu->addSeparator();
  for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
  {
    montagemenu->addAction(load_predefined_mtg_act[i]);
  }
  menubar->addMenu(montagemenu);

//   patternmenu = new QMenu(this);
//   patternmenu->setTitle("&Pattern");
//   patternmenu->addAction("Search", this, SLOT(search_pattern()));
//   menubar->addMenu(patternmenu);

  toolsmenu = new QMenu(this);
  toolsmenu->setTitle("T&ools");
  toolsmenu->addAction("Check EDF/BDF compatibility", this, SLOT(check_edf_compatibility()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Header editor & repair", this, SLOT(edit_header()));
  toolsmenu->addAction("Reduce signals, duration or samplerate", this, SLOT(reduce_signals()));
  toolsmenu->addAction("Remove duplicates in annotations", this, SLOT(edfplus_remove_duplicate_annotations()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Import annotations/events", this, SLOT(import_annotations()));
  toolsmenu->addAction("Export annotations/events", this, SLOT(export_annotations()));
  toolsmenu->addAction("Export EDF/BDF to ASCII (CSV)", this, SLOT(export_to_ascii()));
  toolsmenu->addAction("Export/Import ECG RR-interval", this, SLOT(export_ecg_rr_interval_to_ascii()));
  toolsmenu->addAction("QRS detector", this, SLOT(qrs_detector()));
  toolsmenu->addAction("Export Filtered Signals", this, SLOT(export_filtered_signals()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Convert EDF+D to EDF+C", this, SLOT(edfd_converter()));
  toolsmenu->addAction("Convert BDF to EDF", this, SLOT(bdf2edf_converter()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Convert ASCII (CSV) to EDF/BDF", this, SLOT(convert_ascii_to_edf()));
  toolsmenu->addAction("Convert Nihon Kohden to EDF+", this, SLOT(nk2edf_converter()));
  toolsmenu->addAction("Convert Manscan to EDF+", this, SLOT(convert_manscan_to_edf()));
  toolsmenu->addAction("Convert SCP ECG to EDF+", this, SLOT(convert_scpecg_to_edf()));
  toolsmenu->addAction("Convert MIT (PhysioBank) to EDF+", this, SLOT(convert_mit_to_edf()));
  toolsmenu->addAction("Convert Finometer to EDF", this, SLOT(convert_fino_to_edf()));
  toolsmenu->addAction("Convert Nexfin to EDF", this, SLOT(convert_nexfin_to_edf()));
  toolsmenu->addAction("Convert Emsa to EDF+", this, SLOT(convert_emsa_to_edf()));
  toolsmenu->addAction("Convert Biosemi to BDF+", this, SLOT(biosemi2bdfplus_converter()));
  toolsmenu->addAction("Convert Unisens to EDF+", this, SLOT(unisens2edf_converter()));
  toolsmenu->addAction("Convert BI9800TL+3 to EDF", this, SLOT(BI98002edf_converter()));
  toolsmenu->addAction("Convert Wave to EDF", this, SLOT(convert_wave_to_edf()));
  toolsmenu->addAction("Convert Biox CB-1305-C to EDF", this, SLOT(convert_biox_to_edf()));
  toolsmenu->addAction("Convert FM Audio ECG to EDF", this, SLOT(convert_fm_audio_to_edf()));
  toolsmenu->addAction("Convert Mortara ECG XML to EDF", this, SLOT(convert_mortara_to_edf()));
  toolsmenu->addAction("Convert ISHNE ECG to EDF", this, SLOT(convert_ishne_to_edf()));
  toolsmenu->addAction("Convert CardioTrak EDF to EDF+", this, SLOT(convert_cardiotrak_to_edf()));
  toolsmenu->addAction("Convert Binary/raw data to EDF", this, SLOT(convert_binary_to_edf()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Unify resolution", this, SLOT(unify_resolution()));
  toolsmenu->addSeparator();
  toolsmenu->addAction("Options", this, SLOT(show_options_dialog()));
  menubar->addMenu(toolsmenu);


  settingsmenu = new QMenu(this);
  settingsmenu->setTitle("S&ettings");
  settingsmenu->addAction("Options", this, SLOT(show_options_dialog()));
  menubar->addMenu(settingsmenu);

  no_timesync_act = new QAction("no timelock", this);
  no_timesync_act->setCheckable(true);

  offset_timesync_act = new QAction("synchronize start of files (offset)", this);
  offset_timesync_act->setCheckable(true);

  absolut_timesync_act = new QAction("synchronize absolute time", this);
  absolut_timesync_act->setCheckable(true);

  user_def_sync_act = new QAction("user defined synchronizing", this);
  user_def_sync_act->setCheckable(true);

  timelock_act_group = new QActionGroup(this);
  timelock_act_group->addAction(no_timesync_act);
  timelock_act_group->addAction(offset_timesync_act);
  timelock_act_group->addAction(absolut_timesync_act);
  timelock_act_group->addAction(user_def_sync_act);
  absolut_timesync_act->setChecked(true);
  connect(timelock_act_group, SIGNAL(triggered(QAction *)), this, SLOT(set_timesync(QAction *)));

  sel_viewtime_act_group = new QActionGroup(this);
  connect(sel_viewtime_act_group, SIGNAL(triggered(QAction *)), this, SLOT(set_timesync_reference(QAction *)));

  timemenu = new QMenu(this);
  timemenu->setTitle("T&imesync");
#if QT_VERSION < 0x060000
  timemenu->addAction("Go to start of file", this, SLOT(jump_to_start()), QKeySequence::MoveToStartOfDocument);
  timemenu->addAction("Go to end of file",   this, SLOT(jump_to_end()), QKeySequence::MoveToEndOfDocument);
#else
  timemenu->addAction("Go to start of file", QKeySequence::MoveToStartOfDocument, this, SLOT(jump_to_start()));
  timemenu->addAction("Go to end of file",   QKeySequence::MoveToEndOfDocument,   this, SLOT(jump_to_end()));
#endif
  timemenu->addAction("Jump to", this, SLOT(jump_to_dialog()));
  timemenu->addSeparator()->setText("Timelock");
  timemenu->addAction(no_timesync_act);
  timemenu->addAction(offset_timesync_act);
  timemenu->addAction(absolut_timesync_act);
  timemenu->addAction(user_def_sync_act);
  timemenu->addSeparator();
  timemenu->addAction("synchronize by crosshairs", this, SLOT(sync_by_crosshairs()));
  timemenu->addSeparator()->setText("Time reference");
  menubar->addMenu(timemenu);

  windowmenu = new QMenu(this);
  windowmenu->setTitle("&Window");
  windowmenu->addAction("Annotations", this, SLOT(show_annotations()));
#if QT_VERSION < 0x060000
  windowmenu->addAction("Annotation editor", this, SLOT(annotation_editor()), QKeySequence(Qt::CTRL + Qt::Key_E));
#else
  windowmenu->addAction("Annotation editor", QKeySequence(Qt::CTRL | Qt::Key_E), this, SLOT(annotation_editor()));
#endif
  windowmenu->addAction("Power Spectrum", this, SLOT(show_spectrum_dock()));
  windowmenu->addAction("Color Density Spectral Array", this, SLOT(show_cdsa_dock()));
  windowmenu->addAction("amplitude EEG", this, SLOT(show_aeeg_dock()));
  windowmenu->addAction("Hypnogram", this, SLOT(show_hypnogram()));
  menubar->addMenu(windowmenu);

  helpmenu = new QMenu(this);
  helpmenu->setTitle("&Help");
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
  helpmenu->addAction("Manual", this, SLOT(show_help()));
#endif
  helpmenu->addAction("Keyboard shortcuts", this, SLOT(show_kb_shortcuts()));
  helpmenu->addAction("About EDFbrowser", this, SLOT(show_about_dialog()));
  helpmenu->addAction("Show splashscreen", this, SLOT(show_splashscreen()));
  menubar->addMenu(helpmenu);


  lhsc_uwo_bids_menu = new QMenu(this);
  lhsc_uwo_bids_menu->setTitle("LHSC UWO BID&S");
  lhsc_uwo_bids_menu->addSeparator();
  lhsc_uwo_bids_menu->addAction("Reduce signals, duration or samplerate", this, SLOT(reduce_signals()));
  lhsc_uwo_bids_menu->addSeparator();
  menubar->addMenu(lhsc_uwo_bids_menu);



  navtoolbar = new QToolBar("Navigation Bar");
  navtoolbar->setFloatable(false);
  navtoolbar->setAllowedAreas(Qt::AllToolBarAreas);
  navtoolbar->setIconSize(QSize(20 * h_scaling, 20 * h_scaling));
  addToolBar(Qt::BottomToolBarArea, navtoolbar);

  former_page_Act = new QAction(QIcon(":/images/go-first-symbolic.symbolic.png"), "Former page", this);
  former_page_Act->setShortcut(QKeySequence::MoveToPreviousPage);
  connect(former_page_Act, SIGNAL(triggered()), this, SLOT(former_page()));
  navtoolbar->addAction(former_page_Act);

  stop_playback_realtime_Act = new QAction(QIcon(":/images/media-playback-stop-symbolic.symbolic.png"), "Stop", this);
  connect(stop_playback_realtime_Act, SIGNAL(triggered()), this, SLOT(stop_playback()));
  navtoolbar->addAction(stop_playback_realtime_Act);

  playback_file_Act = new QAction(QIcon(":/images/media-playback-start-symbolic.symbolic.png"), "Playback", this);
  connect(playback_file_Act, SIGNAL(triggered()), this, SLOT(playback_file()));
  navtoolbar->addAction(playback_file_Act);

  shift_page_up_Act = new QAction(QIcon(":/images/go-up-symbolic.symbolic.png"), "Shift up", this);
//  shift_page_up_Act->setShortcut(QKeySequence::MoveToPreviousLine);
  connect(shift_page_up_Act, SIGNAL(triggered()), this, SLOT(shift_page_up()));
  navtoolbar->addAction(shift_page_up_Act);

  shift_page_down_Act = new QAction(QIcon(":/images/go-down-symbolic.symbolic.png"), "Shift down", this);
//  shift_page_down_Act->setShortcut(QKeySequence::MoveToNextLine);
  connect(shift_page_down_Act, SIGNAL(triggered()), this, SLOT(shift_page_down()));
  navtoolbar->addAction(shift_page_down_Act);

  shift_page_left_Act = new QAction(QIcon(":/images/go-next-symbolic-rtl.symbolic.png"), "Shift left", this);
//  shift_page_left_Act->setShortcut(QKeySequence::MoveToPreviousChar);
  connect(shift_page_left_Act, SIGNAL(triggered()), this, SLOT(shift_page_left()));
  navtoolbar->addAction(shift_page_left_Act);

  shift_page_right_Act = new QAction(QIcon(":/images/go-next-symbolic.symbolic.png"), "Shift Right", this);
//  shift_page_right_Act->setShortcut(QKeySequence::MoveToNextChar);
  connect(shift_page_right_Act, SIGNAL(triggered()), this, SLOT(shift_page_right()));
  navtoolbar->addAction(shift_page_right_Act);

  next_page_Act = new QAction(QIcon(":/images/go-first-symbolic-rtl.symbolic.png"), "Next page", this);
  next_page_Act->setShortcut(QKeySequence::MoveToNextPage);
  connect(next_page_Act, SIGNAL(triggered()), this, SLOT(next_page()));
  navtoolbar->addAction(next_page_Act);

  slower_Act = new QAction(QIcon(":/images/media-seek-backward-symbolic.symbolic.png"), "Slower", this);
//  slower_Act->setShortcut(QKeySequence::MoveToNextPage);
  connect(slower_Act, SIGNAL(triggered()), this, SLOT(video_player_slower()));
  navtoolbar->addAction(slower_Act);
  slower_Act->setVisible(false);

  faster_Act = new QAction(QIcon(":/images/media-seek-forward-symbolic.symbolic.png"), "Faster", this);
//  faster_Act->setShortcut(QKeySequence::MoveToNextPage);
  connect(faster_Act, SIGNAL(triggered()), this, SLOT(video_player_faster()));
  navtoolbar->addAction(faster_Act);
  faster_Act->setVisible(false);
//  faster_Act->setToolTip("Faster");

  zoomback_Act = new QAction(QIcon(":/images/zoom-out-symbolic.symbolic.png"), "Zoom Out", this);
  zoomback_Act->setShortcut(Qt::Key_Backspace);
  connect(zoomback_Act, SIGNAL(triggered()), this, SLOT(zoomback()));
  navtoolbar->addAction(zoomback_Act);

  zoomforward_Act = new QAction(QIcon(":/images/zoom-in-symbolic.symbolic.png"), "Zoom In", this);
  zoomforward_Act->setShortcut(Qt::Key_Insert);
  connect(zoomforward_Act, SIGNAL(triggered()), this, SLOT(forward()));
  navtoolbar->addAction(zoomforward_Act);

  nav_toolbar_label = new QLabel;
  navtoolbar->addWidget(nav_toolbar_label);

  Escape_act = new QAction(this);
  Escape_act->setShortcut(Qt::Key_Escape);
  connect(Escape_act, SIGNAL(triggered()), this, SLOT(Escape_fun()));
  maincurve->addAction(Escape_act);

  next_crosshair_act = new QAction("Next Crosshair", this);
#if QT_VERSION < 0x060000
  next_crosshair_act->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_C);
#else
  next_crosshair_act->setShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_C);
#endif
  connect(next_crosshair_act, SIGNAL(triggered()), maincurve, SLOT(next_crosshair_act_triggered()));
  maincurve->addAction(next_crosshair_act);

  shift_vert_scrollbar_up_act = new QAction(this);
#if QT_VERSION < 0x060000
  shift_vert_scrollbar_up_act->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Up));
#else
  shift_vert_scrollbar_up_act->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Up));
#endif
  connect(shift_vert_scrollbar_up_act, SIGNAL(triggered()), this, SLOT(shift_vert_scrollbar_up_act_triggered()));
  maincurve->addAction(shift_vert_scrollbar_up_act);

  shift_vert_scrollbar_down_act = new QAction(this);
#if QT_VERSION < 0x060000
  shift_vert_scrollbar_down_act->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Down));
#else
  shift_vert_scrollbar_down_act->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Down));
#endif
  connect(shift_vert_scrollbar_down_act, SIGNAL(triggered()), this, SLOT(shift_vert_scrollbar_down_act_triggered()));
  maincurve->addAction(shift_vert_scrollbar_down_act);

  positionslider = new QSlider(Qt::Horizontal);
  positionslider->setRange(0, 1000000);
  positionslider->setSingleStep(10000);
  positionslider->setPageStep(100000);

  slidertoolbar = new QToolBar("Position Slider");
  slidertoolbar->setFloatable(false);
  slidertoolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  slidertoolbar->addWidget(positionslider);
  addToolBar(Qt::BottomToolBarArea, slidertoolbar);
  QObject::connect(positionslider, SIGNAL(valueChanged(int)), this, SLOT(slider_moved(int)));
  slidertoolbar->setEnabled(false);
  positionslider->blockSignals(true);

  insertToolBarBreak(slidertoolbar);

  files_open = 0;
  signalcomps = 0;
  sel_viewtime = 0;
  viewtime_sync = VIEWTIME_SYNCED_ABSOLUT;
  pagetime = 10 * TIME_FIXP_SCALING;
  viewtime_string_128[0] = 0;
  pagetime_string_128[0] = 0;
  totalviewbufsize_bytes = 0LL;
  print_to_edf_active = 0;
  annot_editor_active = 0;
  annotations_edited = 0;

  viewbuf = NULL;

  for(i=0; i<MAXFILES; i++)
  {
    annotations_dock[i] = NULL;
  }

  annotationlist_backup = NULL;

  zoomhistory = (zoomhistblck_t *)calloc(1, sizeof(zoomhistblck_t));
  zoomhistory->idx = 0;
  zoomhistory->history_size_tail = 0;
  zoomhistory->history_size_head = 0;
  for(i=0; i<MAXZOOMHISTORY; i++)
  {
    zoomhistory->pagetime[i] = 10 * TIME_FIXP_SCALING;
    for(j=0; j<MAXFILES; j++)
    {
      zoomhistory->viewtime[i][j] = 0;
    }
    for(j=0; j<MAXSIGNALS; j++)
    {
      zoomhistory->voltpercm[i][j] = 70;
      zoomhistory->screen_offset_unit[i][j] = 0;
      zoomhistory->sensitivity[i][j] = 0.0475;
    }
  }

  path[0] = 0;
  recent_montagedir[0] = 0;
  recent_savedir[0] = 0;
  recent_opendir[0] = 0;
  recent_video_opendir[0] = 0;
  recent_sessiondir[0] = 0;
  montagepath[0] = 0;
  session_path[0] = 0;
  option_str[0] = 0;

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    recent_file_path[i][0] = 0;
    recent_file_mtg_path[i][0] = 0;
    recent_session_path[i][0] = 0;
  }

  for(i=0; i<MAXPREDEFINEDMONTAGES; i++)
  {
    predefined_mtg_path[i][0] = 0;
  }

  read_recent_file_settings();

  for(i=0; i<MAXPREDEFINEDMONTAGES; i++)
  {
    if(predefined_mtg_path[i][0] != 0)
    {
      load_predefined_mtg_act[i]->setText(predefined_mtg_path[i]);
    }
  }

  annotationEditDock = NULL;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    spectrumdock[i] = new UI_SpectrumDockWindow(this);

    addDockWidget(Qt::TopDockWidgetArea, spectrumdock[i]->dock, Qt::Horizontal);

    spectrumdock[i]->dock->hide();
  }

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

  char tmp_str[MAX_PATH_LENGTH]={""};

  cmdlineargument = 0;

  cmdlineoption = 0;

  if(QCoreApplication::arguments().size()>1)
  {
    strlcpy(tmp_str, QCoreApplication::arguments().at(1).toLocal8Bit().data(), MAX_PATH_LENGTH);

    if((strlen(tmp_str) > 2) && (!strncmp(tmp_str, "--", 2)))
    {
      strlcpy(option_str, tmp_str, MAX_PATH_LENGTH);

      cmdlineoption++;
    }
    else
    {
      strlcpy(path, tmp_str, MAX_PATH_LENGTH);

      cmdlineargument++;
    }

    if(QCoreApplication::arguments().size()>2)
    {
      strlcpy(tmp_str, QCoreApplication::arguments().at(2).toLocal8Bit().data(), MAX_PATH_LENGTH);

      if(!cmdlineargument)
      {
        strlcpy(path, tmp_str, MAX_PATH_LENGTH);
      }
      else
      {
        strlcpy(montagepath, tmp_str, MAX_PATH_LENGTH);
      }

      cmdlineargument++;

      if(cmdlineargument == 1)
      {
        if(QCoreApplication::arguments().size()>3)
        {
          strlcpy(montagepath, QCoreApplication::arguments().at(3).toLocal8Bit().data(), MAX_PATH_LENGTH);

          cmdlineargument++;
        }
      }
    }
  }

#if QT_VERSION >= 0x050B00
//   if((window_width_sav_rest > (QGuiApplication::screens()[0]->availableGeometry().width())) ||
//      (window_height_sav_rest > (QGuiApplication::screens()[0]->availableGeometry().height())))
  if((window_width_sav_rest > (QGuiApplication::primaryScreen()->availableGeometry().width())) ||
     (window_height_sav_rest > (QGuiApplication::primaryScreen()->availableGeometry().height())))
  {
    window_width_sav_rest = 0;
    window_height_sav_rest = 0;
  }
#else
  if((window_width_sav_rest > (QApplication::desktop()->availableGeometry().width())) ||
     (window_height_sav_rest > (QApplication::desktop()->availableGeometry().height())))
  {
    window_width_sav_rest = 0;
    window_height_sav_rest = 0;
  }
#endif
  if((!window_width_sav_rest) || (!window_height_sav_rest))
  {
    showMaximized();
  }
  else
  {
    resize(window_width_sav_rest, window_height_sav_rest);
    show();
  }

  maincurve->setFocus(Qt::OtherFocusReason);

  oldwindowheight = height();

  splash_pixmap = new QPixmap(":/images/splash.png");
  QPainter p(splash_pixmap);
  QFont sansFont("Noto Sans", 10);
  p.setFont(sansFont);
  p.setPen(Qt::black);
  if(!strcmp(PROGRAM_BETA_SUFFIX, ""))
  {
    p.drawText(250, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "    " THIS_APP_BITS_W);
  }
  else
  {
    p.drawText(50, 240, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "  " THIS_APP_BITS_W);
    p.drawText(50, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, PROGRAM_BETA_SUFFIX);
  }
#if QT_VERSION >= 0x050F00
  splash = new QSplashScreen(this->screen(), *splash_pixmap);
#else
  splash = new QSplashScreen(this, *splash_pixmap, Qt::WindowStaysOnTopHint);
#endif

  if( (QT_VERSION < MINIMUM_QT4_VERSION) ||
     ((QT_VERSION >= 0x050000) && (QT_VERSION < MINIMUM_QT5_VERSION)) ||
     ((QT_VERSION >= 0x060000) && (QT_VERSION < MINIMUM_QT6_VERSION)))
  {
    cmdlineargument = 0;

    QMessageBox messagewindow(QMessageBox::Warning, "Error", "EDFbrowser has been compiled with a version of Qt\n"
                                                             "which is too old and can cause problems.");
    messagewindow.exec();
  }

  int v_nr;

  char v_str[32];

  strlcpy(v_str, qVersion(), 32);

  v_nr = 0x10000 * atoi(v_str);
  v_nr += 0x100 * atoi(v_str + 2);
  if(v_str[3] == '.')
  {
    v_nr += atoi(v_str + 4);
  }
  else
  {
    v_nr += atoi(v_str + 5);
  }

  if((v_nr < MINIMUM_QT4_VERSION) ||
    ((v_nr >= 0x050000) && (v_nr < MINIMUM_QT5_VERSION)) ||
    ((v_nr >= 0x060000) && (v_nr < MINIMUM_QT6_VERSION)))
  {
    cmdlineargument = 0;

    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Your version of Qt is too old\n"
                                                              "and can cause problems.");
    messagewindow.exec();
  }

  if(edflib_version() != 127)
  {
    cmdlineargument = 0;

    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There's a version problem with EDFlib.\n"
                                                              "Cannot continue.");
    messagewindow.exec();

    menubar->setEnabled(false);
    navtoolbar->setEnabled(false);

    return;
  }

  update_checker = NULL;

  if(check_for_updates)
  {
    if((time(NULL) - last_check_for_updates_time) > (24LL * 3600LL))
    {
      update_checker = new Check_for_updates(this);
    }
  }

  if(cmdlineoption)
  {
    if((!strncmp(option_str, "--rc-host-port=", 15)) && (strlen(option_str) > 18))
    {
      rc_host_server_port = atoi(option_str + 15);

      if((rc_host_server_port < 1024) || (rc_host_server_port > 65535))
      {
        rc_host_server_port = 0;
      }
      else
      {
        rc_host_server = new QTcpServer;
        rc_host_server->setMaxPendingConnections(1);
        if(rc_host_server_public == 1)
        {
          ret = rc_host_server->listen(QHostAddress::Any, rc_host_server_port);
        }
        else
        {
          ret = rc_host_server->listen(QHostAddress::LocalHost, rc_host_server_port);
        }
        if(ret == true)
        {
          printf("rc host server listening at port %i\n", rc_host_server_port);

          QObject::connect(rc_host_server, SIGNAL(newConnection()), this, SLOT(rc_host_server_new_connection()), Qt::QueuedConnection);
        }
        else
        {
          printf("rc host server failed at port %i, port is occupied by another process?\n", rc_host_server_port);
        }
      }
    }
  }

  if(cmdlineargument)
  {
    startup_timer = new QTimer;
    startup_timer->setSingleShot(true);

    if(cmdlineoption)
    {
      if(!strcmp(option_str, "--stream"))
      {
        QObject::connect(startup_timer, SIGNAL(timeout()), this, SLOT(open_stream()));
        startup_timer->start(50);
      }
    }
    else
    {
      QObject::connect(startup_timer, SIGNAL(timeout()), this, SLOT(open_new_file()));
      startup_timer->start(50);
    }
  }

  QObject::connect(maincurve,                        SIGNAL(file_dropped()),                  this, SLOT(open_new_file()));
#if QT_VERSION >= 0x050000
  QObject::connect(this->window()->windowHandle(),   SIGNAL(screenChanged(QScreen*)),         this, SLOT(desktop_resized()));
#endif
#if QT_VERSION < 0x060000
  QObject::connect(QApplication::desktop(),          SIGNAL(resized(int)),                    this, SLOT(desktop_resized()));
#else
  QObject::connect(QGuiApplication::primaryScreen(), SIGNAL(geometryChanged(QRect)),          this, SLOT(desktop_resized()));
  QObject::connect(qGuiApp,                          SIGNAL(primaryScreenChanged(QScreen *)), this, SLOT(desktop_resized()));
#endif
}



















