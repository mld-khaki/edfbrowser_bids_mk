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


UI_Mainwindow::~UI_Mainwindow()
{
  delete splash_pixmap;
  delete splash;
  delete startup_timer;
  delete normfont;
  delete monofont;
  delete maincurve;
  delete annotationEditDock;
  for(int i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    delete spectrumdock[i];
  }
  delete live_stream_timer;
  delete video_poll_timer;
  delete playback_realtime_time;
  delete playback_realtime_timer;
  delete dig_min_max_overflow_timer;
  delete update_checker;
  free(toolbar_stats.ival);
  delete rc_host_server;
}


void UI_Mainwindow::exit_program()
{
  close();
}


void UI_Mainwindow::closeEvent(QCloseEvent *cl_event)
{
  int i, button_nr=0;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to quit?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    cl_event->ignore();

    return;
  }

  exit_in_progress = 1;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(spectrumdialog[i] != NULL)
    {
      delete spectrumdialog[i];

      spectrumdialog[i] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] != NULL)
    {
      delete cdsa_dock[i];

      cdsa_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(aeeg_dock[i] != NULL)
    {
      delete aeeg_dock[i];

      aeeg_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(averagecurvedialog[i] != NULL)
    {
      delete averagecurvedialog[i];

      averagecurvedialog[i] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(zscoredialog[i] != NULL)
    {
      delete zscoredialog[i];

      zscoredialog[i] = NULL;
    }
  }

  annotations_edited = 0;

  close_all_files();

  window_width_sav_rest = width();
  window_height_sav_rest = height();

  write_settings();

  free(spectrum_colorbar);
  free(zoomhistory);
  free(import_annotations_var);
  free(export_annotations_var);
  free(video_player);
  free(annot_filter);

  cl_event->accept();
}


// void UI_Mainwindow::search_pattern()
// {
//   if(!signalcomps)
//   {
//     return;
//   }
//
//
//
// }


void UI_Mainwindow::Escape_fun()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->hascursor1 = 0;
    signalcomp[i]->hascursor2 = 0;
    signalcomp[i]->hasoffsettracking = 0;
  }
  maincurve->crosshair_1.active = 0;
  maincurve->crosshair_2.active = 0;
  maincurve->crosshair_1.moving = 0;
  maincurve->crosshair_2.moving = 0;
  maincurve->use_move_events = 0;
  maincurve->setMouseTracking(false);

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->hasruler = 0;
  }
  maincurve->ruler_active = 0;
  maincurve->ruler_moving = 0;

  maincurve->update();
}


void UI_Mainwindow::open_stream()
{
  if(files_open)
  {
    QMessageBox::critical(this, "Error", "Close all files before opening a stream.");
    return;
  }

  live_stream_active = 1;

  live_stream_active_paused = 0;

  open_new_file();

  if(files_open == 1)
  {
    toolsmenu->setEnabled(false);
    lhsc_uwo_bids_menu->setEnabled(false);
    timemenu->setEnabled(false);
//    windowmenu->setEnabled(false);
    windowmenu->actions().at(0)->setEnabled(false);  // Annotations window
    windowmenu->actions().at(1)->setEnabled(false);  // Annotation editor
    former_page_Act->setEnabled(false);
//    shift_page_left_Act->setEnabled(false);
//    shift_page_right_Act->setEnabled(false);
    next_page_Act->setEnabled(false);
//     shift_page_up_Act->setEnabled(false);
//     shift_page_down_Act->setEnabled(false);
    printmenu->setEnabled(false);
    recent_filesmenu->setEnabled(false);
    recent_session_menu->setEnabled(false);
//    playback_file_Act->setEnabled(false);

    live_stream_timer->start(live_stream_update_interval);
    playback_file_Act->setIcon(QIcon(":/images/media-playback-pause-symbolic.symbolic.png"));
  }
  else
  {
    live_stream_active = 0;
  }
}


void UI_Mainwindow::live_stream_timer_func()
{
  long long datarecords_old,
            datarecords_new;


  if((!live_stream_active) || (files_open != 1))
  {
    return;
  }

  if(!signalcomps)
  {
    live_stream_timer->start(live_stream_update_interval);

    return;
  }

  datarecords_old = edfheaderlist[0]->datarecords;

  datarecords_new = check_edf_file_datarecords(edfheaderlist[0]);

  if((datarecords_new > datarecords_old) && (datarecords_new > 0))
  {
    jump_to_end();
  }

  live_stream_timer->start(live_stream_update_interval);
}


long long UI_Mainwindow::check_edf_file_datarecords(edfhdrblck_t *hdr)
{
  long long datarecords;


  if(fseeko(hdr->file_hdl, 0LL, SEEK_END) == -1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  datarecords = ftello(hdr->file_hdl);

  if(datarecords < 1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  datarecords -= (long long)hdr->hdrsize;

  datarecords /= (long long)hdr->recordsize;

  if(datarecords < 1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  hdr->datarecords = datarecords;

  return datarecords;
}


void UI_Mainwindow::save_file()
{
  char f_path[MAX_PATH_LENGTH]={""};

  edfhdrblck_t *hdr=NULL;

  FILE *outputfile=NULL;


  if((!annotations_edited)||(!files_open))
  {
    save_act->setEnabled(false);

    return;
  }

  hdr = edfheaderlist[0];

  strlcpy(f_path, recent_savedir, MAX_PATH_LENGTH);
  strlcat(f_path, "/", MAX_PATH_LENGTH);
  get_filename_from_path(f_path + strlen(f_path), hdr->filename, MAX_PATH_LENGTH - strlen(f_path));
  remove_extension_from_filename(f_path);
  if(hdr->edf)
  {
    strlcat(f_path, "_edited.edf", MAX_PATH_LENGTH);
  }
  else
  {
    strlcat(f_path, "_edited.bdf", MAX_PATH_LENGTH);
  }

  strlcpy(f_path, QFileDialog::getSaveFileName(this, "Save file", QString::fromLocal8Bit(f_path), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF *.rec *.REC)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(f_path, ""))
  {
    return;
  }

  get_directory_from_path(recent_savedir, f_path, MAX_PATH_LENGTH);

  if(file_is_opened(f_path))
  {
    QMessageBox::critical(this, "Error", "Selected file is in use.");
    return;
  }

  outputfile = fopeno(f_path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox::critical(this, "Error", "Cannot create a file for writing.");
    return;
  }

  if(save_annotations(this, outputfile, hdr))
  {
    QMessageBox::critical(this, "Error", "An error occurred during saving.");
    fclose(outputfile);
    return;
  }

  fclose(outputfile);

  edfplus_annotation_empty_list(&hdr->annot_list);

  if(annotationlist_backup != NULL)
  {
    hdr->annot_list = *annotationlist_backup;

    free(annotationlist_backup);

    annotationlist_backup = NULL;
  }

  annotations_dock[0]->updateList(0);

  annotations_edited = 0;

  save_act->setEnabled(false);

  delete annotationEditDock;
  annotationEditDock = NULL;

  maincurve->update();
}


void UI_Mainwindow::load_session()
{
  int i, button_nr=0, err, present=0, position=0;

  QAction *act=NULL;

  if(rc_cmd_in_progress)
  {
    if(!strlen(session_path))
    {
      return;
    }
  }
  else
  {
    session_path[0] = 0;

    if(annotations_edited)
    {
      QMessageBox messagewindow;
      messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
      messagewindow.setIcon(QMessageBox::Question);
      messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
      messagewindow.setDefaultButton(QMessageBox::Cancel);
      button_nr = messagewindow.exec();
    }
    else if(files_open)
      {
        QMessageBox messagewindow;
        messagewindow.setText("This will close all files, continue?");
        messagewindow.setIcon(QMessageBox::Question);
        messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Close);
        messagewindow.setDefaultButton(QMessageBox::Cancel);
        button_nr = messagewindow.exec();
      }

    if(button_nr == QMessageBox::Cancel)
    {
      return;
    }
  }

  annotations_edited = 0;

  session_start_video = 0;

  session_video_seek = 0;

  close_all_files();

  if(!rc_cmd_in_progress)
  {
    strlcpy(session_path, QFileDialog::getOpenFileName(0, "Load session", QString::fromLocal8Bit(recent_sessiondir), "Session files (*.esf *.ESF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(session_path, ""))
    {
      return;
    }
  }

  get_directory_from_path(recent_sessiondir, session_path, MAX_PATH_LENGTH);

  for(i=0, present=0; i<MAX_RECENTFILES; i++)
  {
//    printf("mainwindow.cpp: load_session(): recent_session_path[i]: ->%s<-\nsession_path: ->%s<-\n", recent_session_path[i], session_path);

    if(!strcmp(&recent_session_path[i][0], session_path))
    {
      present = 1;

      position = i;

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_session_path[0][0], session_path, MAX_PATH_LENGTH);

  recent_session_menu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_session_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
    act->setData(QVariant(i));
    recent_session_menu->addAction(act);
  }
  recent_session_menu->addSeparator();
  act = new QAction("Clear list", recent_session_menu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_session_menu->addAction(act);

  err = read_session_file(session_path);

  if(err)
  {
    printf("load_session() returned error: %i\n", err);
  }
}


void UI_Mainwindow::slider_moved(int value)
{
  long long new_viewtime, tmp;

  if(!signalcomps)  return;

  new_viewtime = edfheaderlist[sel_viewtime]->long_data_record_duration * edfheaderlist[sel_viewtime]->datarecords;

  new_viewtime -= pagetime;

  if(new_viewtime<0)
  {
    new_viewtime = 0;
  }
  else
  {
    new_viewtime /= 1000000;

    new_viewtime *= value;
  }

  if(pagetime >= (3600LL * TIME_FIXP_SCALING))
  {
    tmp = new_viewtime % (360LL * TIME_FIXP_SCALING);

    new_viewtime -= tmp;
  }
  else if(pagetime >= (1200LL * TIME_FIXP_SCALING))
    {
      tmp = new_viewtime % (120LL * TIME_FIXP_SCALING);

      new_viewtime -= tmp;
    }
    else if(pagetime >= (300LL * TIME_FIXP_SCALING))
      {
        tmp = new_viewtime % (30LL * TIME_FIXP_SCALING);

        new_viewtime -= tmp;
      }
      else if(pagetime >= (60LL * TIME_FIXP_SCALING))
        {
          tmp = new_viewtime % (6LL * TIME_FIXP_SCALING);

          new_viewtime -= tmp;
        }
        else if(pagetime >= (30LL * TIME_FIXP_SCALING))
          {
            tmp = new_viewtime % (3LL * TIME_FIXP_SCALING);

            new_viewtime -= tmp;
          }
          else if(pagetime >= (20LL * TIME_FIXP_SCALING))
            {
              tmp = new_viewtime % (2LL * TIME_FIXP_SCALING);

              new_viewtime -= tmp;
            }
            else if(pagetime >= (10LL * TIME_FIXP_SCALING))
              {
                tmp = new_viewtime % TIME_FIXP_SCALING;

                new_viewtime -= tmp;
              }
              else if(pagetime >= TIME_FIXP_SCALING)
                {
                  tmp = new_viewtime % (TIME_FIXP_SCALING / 10LL);

                  new_viewtime -= tmp;
                }
                else if(pagetime >= (TIME_FIXP_SCALING / 10LL))
                  {
                    tmp = new_viewtime % (TIME_FIXP_SCALING / 100LL);

                    new_viewtime -= tmp;
                  }
                  else if(pagetime >= (TIME_FIXP_SCALING / 100LL))
                    {
                      tmp = new_viewtime % (TIME_FIXP_SCALING / 1000LL);

                      new_viewtime -= tmp;
                    }

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((new_viewtime + (pagetime / 2LL)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((new_viewtime + (pagetime / 2LL)) / TIME_FIXP_SCALING));
  }

  set_viewtime(new_viewtime);
}


void UI_Mainwindow::set_timesync_reference(QAction *action)
{
  int i;

  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, action->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  sel_viewtime = i;

  setMainwindowTitle(edfheaderlist[sel_viewtime]);

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync_reference(int ref)
{
  if((ref < 0) || (ref >= files_open))
  {
    return;
  }

  sel_viewtime = ref;

  setMainwindowTitle(edfheaderlist[sel_viewtime]);

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync(QAction *)
{
  int i;

  if(no_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_UNSYNCED;
  }

  if(offset_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_SYNCED_OFFSET;

    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
    }
  }

  if(absolut_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_SYNCED_ABSOLUT;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime + (edfheaderlist[sel_viewtime]->utc_starttime_hr - edfheaderlist[i]->utc_starttime_hr);
      }
    }
  }

  if(user_def_sync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_USER_DEF_SYNCED;
  }

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync(int mode)
{
  int i;

  if(mode == VIEWTIME_UNSYNCED)
  {
    viewtime_sync = VIEWTIME_UNSYNCED;
  }
  else if(mode == VIEWTIME_SYNCED_OFFSET)
    {
      viewtime_sync = VIEWTIME_SYNCED_OFFSET;

      for(i=0; i<files_open; i++)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
      }
    }
    else if(mode == VIEWTIME_SYNCED_ABSOLUT)
      {
        viewtime_sync = VIEWTIME_SYNCED_ABSOLUT;

        for(i=0; i<files_open; i++)
        {
          if(i!=sel_viewtime)
          {
            edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime + (edfheaderlist[sel_viewtime]->utc_starttime_hr - edfheaderlist[i]->utc_starttime_hr);
          }
        }
      }
      else if(mode == VIEWTIME_USER_DEF_SYNCED)
        {
          viewtime_sync = VIEWTIME_USER_DEF_SYNCED;
        }

  switch(viewtime_sync)
  {
    case VIEWTIME_SYNCED_OFFSET :   offset_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_SYNCED_ABSOLUT :  absolut_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_UNSYNCED :        no_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_USER_DEF_SYNCED : user_def_sync_act->setChecked(true);
                                    break;
    default                       : break;
  }

  setup_viewbuf();
}


void UI_Mainwindow::sync_by_crosshairs()
{
  if((files_open < 2) || (signalcomps < 2))  return;

  if(!maincurve->crosshair_1.active || !maincurve->crosshair_2.active)  return;

  if(maincurve->crosshair_1.edf_hdr == maincurve->crosshair_2.edf_hdr)  return;

  viewtime_sync = VIEWTIME_USER_DEF_SYNCED;

  maincurve->crosshair_2.edf_hdr->viewtime -=
   ((maincurve->crosshair_1.time_clock - maincurve->crosshair_1.edf_hdr->starttime_hr - maincurve->crosshair_1.edf_hdr->viewtime) -
    (maincurve->crosshair_2.time_clock - maincurve->crosshair_2.edf_hdr->starttime_hr - maincurve->crosshair_2.edf_hdr->viewtime));

  maincurve->crosshair_2.x_position = maincurve->crosshair_1.x_position;

  user_def_sync_act->setChecked(true);

  viewtime_sync = VIEWTIME_USER_DEF_SYNCED;

  setup_viewbuf();
}


void UI_Mainwindow::show_options_dialog()
{
  UI_OptionsDialog OptionsDialog(this);
}


void UI_Mainwindow::nk2edf_converter()
{
  UI_NK2EDFwindow nk2edf(this, recent_opendir);
}


void UI_Mainwindow::convert_ascii_to_edf()
{
  UI_ASCII2EDFapp ascii2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_binary_to_edf()
{
  UI_RAW2EDFapp bin2edf(this, &raw2edf_var, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_fino_to_edf()
{
  UI_FINO2EDFwindow fino2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_wave_to_edf()
{
  UI_WAV2EDFwindow wav2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_fm_audio_to_edf()
{
  UI_FMaudio2EDFwindow fma2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_mortara_to_edf()
{
  UI_MortaraEDFwindow mortara2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_ishne_to_edf()
{
  UI_IshneEDFwindow ishne2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_cardiotrak_to_edf()
{
  UI_CardiotrakEDFwindow ctrak2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_nexfin_to_edf()
{
  UI_NEXFIN2EDFwindow nexfin2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_emsa_to_edf()
{
  UI_EMSA2EDFwindow emsa2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_manscan_to_edf()
{
  UI_MANSCAN2EDFwindow manscan2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_mit_to_edf()
{
  UI_MIT2EDFwindow mit2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_biox_to_edf()
{
  UI_BIOX2EDFwindow biox2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_scpecg_to_edf()
{
  UI_SCPECG2EDFwindow scpecg2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::edfd_converter()
{
  UI_EDFDwindow edfplusd2edfplusc(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::bdf2edf_converter()
{
  UI_BDF2EDFwindow bdf2edfconv(this);
}


void UI_Mainwindow::biosemi2bdfplus_converter()
{
  UI_BIOSEMI2BDFPLUSwindow biosemi2bdfplusconv(this);
}


void UI_Mainwindow::reduce_signals()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open the file first.");
    return;
  }

  UI_ReduceSignalsWindow reduceSignals(this);
}


void UI_Mainwindow::edit_header()
{
  UI_headerEditorWindow header_edit(this);
}


void UI_Mainwindow::unisens2edf_converter()
{
  UI_UNISENS2EDFwindow unisens2edfconv(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::BI98002edf_converter()
{
  UI_BI98002EDFwindow BI98002edfconv(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::edit_predefined_montages()
{
  UI_edit_predefined_mtg_window edit_predef_mtgs_app(this);
}


void UI_Mainwindow::jump_to_dialog()
{
  if(!files_open)  return;

  UI_JumpMenuDialog jumpmenu(this);
}


void UI_Mainwindow::jump_to_start()
{
  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((pagetime / 2LL) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((pagetime / 2LL) / TIME_FIXP_SCALING));
  }

  set_viewtime(0LL);
}


void UI_Mainwindow::jump_to_time_millisec(long long milliseconds)
{
  if(!files_open)  return;

  set_viewtime(milliseconds * (TIME_FIXP_SCALING / 1000));
}


void UI_Mainwindow::jump_to_end()
{
  long long new_viewtime;

  if(!files_open)  return;

  new_viewtime = edfheaderlist[sel_viewtime]->datarecords * edfheaderlist[sel_viewtime]->long_data_record_duration - pagetime;

  set_viewtime(new_viewtime);
}


void UI_Mainwindow::set_viewtime(long long new_viewtime)
{
  int i;

  if(!files_open)  return;

  if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = new_viewtime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;
  }

  if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
  {
    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime + (edfheaderlist[sel_viewtime]->utc_starttime_hr - edfheaderlist[i]->utc_starttime_hr);
      }
    }
  }

  if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
  {
    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime -= (edfheaderlist[sel_viewtime]->viewtime - new_viewtime);
      }
    }

    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::add_new_filter()
{
  if(!signalcomps)  return;

  UI_FilterDialog filterdialog(this);
}


void UI_Mainwindow::add_plif_ecg_filter()
{
  if(!signalcomps)  return;

  UI_PLIF_ECG_filter_dialog plifecgfilterdialog(this);
}


void UI_Mainwindow::add_plif_eeg_filter()
{
  if(!signalcomps)  return;

  UI_PLIF_EEG_filter_dialog plifeegfilterdialog(this);
}


void UI_Mainwindow::add_fir_filter()
{
  if(!signalcomps)  return;

  UI_FIRFilterDialog firfilterdialog(recent_opendir, recent_savedir, this);
}


void UI_Mainwindow::add_spike_filter()
{
  if(!signalcomps)  return;

  UI_SpikeFilterDialog spikefilterdialog(this);
}


void UI_Mainwindow::add_new_math_func()
{
  if(!signalcomps)  return;

  UI_MATH_func_dialog mathfuncdialog(this);
}


void UI_Mainwindow::remove_all_math_funcs()
{
  int i, j,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<signalcomp[i]->math_func_cnt_before; j++)
    {
      free_math_func(signalcomp[i]->math_func_before[j]);

      update_scr = 1;
    }

    signalcomp[i]->math_func_cnt_before = 0;

    for(j=0; j<signalcomp[i]->math_func_cnt_after; j++)
    {
      free_math_func(signalcomp[i]->math_func_after[j]);

      update_scr = 1;
    }

    signalcomp[i]->math_func_cnt_after = 0;
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::zoomback()
{
  int i;

  if(!files_open)  return;

  if(!zoomhistory->history_size_tail)
  {
    return;
  }
  zoomhistory->history_size_head++;
  zoomhistory->history_size_tail--;

  for(i=0; i<files_open; i++)
  {
    zoomhistory->viewtime[zoomhistory->idx][i] = edfheaderlist[i]->viewtime;
  }
  zoomhistory->pagetime[zoomhistory->idx] = pagetime;
  for(i=0; i<signalcomps; i++)
  {
    zoomhistory->voltpercm[zoomhistory->idx][i] = signalcomp[i]->voltpercm;
    zoomhistory->sensitivity[zoomhistory->idx][i] = signalcomp[i]->sensitivity;
    zoomhistory->screen_offset_unit[zoomhistory->idx][i] = signalcomp[i]->screen_offset_unit;
  }

  zoomhistory->idx--;
  zoomhistory->idx += MAXZOOMHISTORY;
  zoomhistory->idx %= MAXZOOMHISTORY;

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime = zoomhistory->viewtime[zoomhistory->idx][i];
  }
  pagetime = zoomhistory->pagetime[zoomhistory->idx];

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->voltpercm = zoomhistory->voltpercm[zoomhistory->idx][i];
    signalcomp[i]->sensitivity = zoomhistory->sensitivity[zoomhistory->idx][i];
    signalcomp[i]->screen_offset_unit = zoomhistory->screen_offset_unit[zoomhistory->idx][i];
    signalcomp[i]->screen_offset_pix = -signalcomp[i]->screen_offset_unit / (y_pixelsizefactor * signalcomp[i]->voltpercm);
  }

  setup_viewbuf();
}


void UI_Mainwindow::forward()
{
  int i;

  if(!files_open)  return;

  if(!zoomhistory->history_size_head)
  {
    return;
  }
  zoomhistory->history_size_head--;
  zoomhistory->history_size_tail++;

  zoomhistory->idx++;
  zoomhistory->idx %= MAXZOOMHISTORY;

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime = zoomhistory->viewtime[zoomhistory->idx][i];
  }
  pagetime = zoomhistory->pagetime[zoomhistory->idx];

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->voltpercm = zoomhistory->voltpercm[zoomhistory->idx][i];
    signalcomp[i]->sensitivity = zoomhistory->sensitivity[zoomhistory->idx][i];
    signalcomp[i]->screen_offset_unit = zoomhistory->screen_offset_unit[zoomhistory->idx][i];
    signalcomp[i]->screen_offset_pix = -signalcomp[i]->screen_offset_unit / (y_pixelsizefactor * signalcomp[i]->voltpercm);
  }

  setup_viewbuf();
}


void UI_Mainwindow::former_page()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime - (pagetime / 2LL)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime - (pagetime / 2LL)) / TIME_FIXP_SCALING));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime -= pagetime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime -= pagetime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_left()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) - (pagetime / 10)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) - (pagetime / 10)) / TIME_FIXP_SCALING));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime -= (pagetime / 10);
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime -= (pagetime / 10);
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_right()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) + (pagetime / 10)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) + (pagetime / 10)) / TIME_FIXP_SCALING));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime += (pagetime / 10);
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += (pagetime / 10);
  }

  setup_viewbuf();
}


void UI_Mainwindow::stop_playback()
{
  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;
  }
  else
  {
    if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
    {
      start_stop_video();
    }
  }
}


void UI_Mainwindow::playback_file()
{
  if(!signalcomps)
  {
    return;
  }

  if(live_stream_active)
  {
    if(live_stream_active_paused)
    {
      live_stream_timer->start(live_stream_update_interval);
      live_stream_active_paused = 0;
      playback_file_Act->setIcon(QIcon(":/images/media-playback-pause-symbolic.symbolic.png"));
    }
    else
    {
      live_stream_timer->stop();
      live_stream_active_paused = 1;
      playback_file_Act->setIcon(QIcon(":/images/media-playback-start-symbolic.symbolic.png"));
    }

    return;
  }

  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;

    return;
  }

  if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
  {
    video_player_toggle_pause();

    return;
  }

  playback_realtime_time->start();

  playback_realtime_timer->start();

  playback_realtime_active = 1;
}


void UI_Mainwindow::playback_realtime_timer_func()
{
  int i;

  long long elapsed_time;

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += (playback_realtime_time->restart() * 10000);
  }
  else if((viewtime_sync==VIEWTIME_SYNCED_OFFSET) ||
      (viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) ||
      (viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
      {
        elapsed_time = playback_realtime_time->restart() * 10000LL;

        for(i=0; i<files_open; i++)
        {
          edfheaderlist[i]->viewtime += elapsed_time;
        }

        playback_realtime_time->restart();
      }

  setup_viewbuf();
}


void UI_Mainwindow::next_page()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + pagetime + (pagetime / 2LL)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + pagetime + (pagetime / 2LL)) / TIME_FIXP_SCALING));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime += pagetime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += pagetime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::next_epoch(long long epoch_len)
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    if(epoch_len < 2)  return;

    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + epoch_len + (epoch_len / 2LL)) / TIME_FIXP_SCALING));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    if(epoch_len < 2)  return;

    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + epoch_len + (epoch_len / 2LL)) / TIME_FIXP_SCALING));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime += epoch_len;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += epoch_len;
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_up()
{
  int i;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->screen_offset_pix += (height() / 20.0);

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::shift_page_down()
{
  int i;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->screen_offset_pix -= (height() / 20.0);

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::show_annotations()
{
  int i;

  EDF_annotations annotations;

  if(!files_open)  return;

  for(i=0; i<files_open; i++)
  {
    if(edfheaderlist[i]->annots_not_read)
    {
      if((edfheaderlist[i]->edfplus) || (edfheaderlist[i]->bdfplus))
      {
        edfheaderlist[i]->annots_not_read = 0;

        annotations.get_annotations(edfheaderlist[i], read_nk_trigger_signal);

        if(edfheaderlist[i]->annots_not_read)
        {
          edfplus_annotation_empty_list(&edfheaderlist[i]->annot_list);
        }
        else
        {
          if(annotations_dock[i] == NULL)
          {
            annotations_dock[i] = new UI_Annotationswindow(edfheaderlist[i], this);

            addDockWidget(Qt::RightDockWidgetArea, annotations_dock[i]->docklist, Qt::Vertical);
          }
        }
      }
    }

    if(annotations_dock[i] != NULL)
    {
      annotations_dock[i]->docklist->show();
    }
  }
}


void UI_Mainwindow::annotation_editor()
{
//  stop_video_generic(0);

//  stop_playback();

  if(annotationEditDock)
  {
    delete annotationEditDock;
    annotationEditDock = NULL;
    return;
  }

  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;
  }

  if(files_open!=1)
  {
    QMessageBox::critical(this, "Error", "Editing annotations is possible when you have opened one file only.");
    return;
  }

  if(edfheaderlist[0]->annots_not_read)
  {
    edfplus_annotation_empty_list(&edfheaderlist[0]->annot_list);

    if(annotations_dock[0] != NULL)
    {
      annotations_dock[0]->docklist->close();
      delete annotations_dock[0];
      annotations_dock[0] = NULL;
    }

    edfheaderlist[0]->annots_not_read = 0;

    EDF_annotations annotations;

    annotations.get_annotations(edfheaderlist[0], read_nk_trigger_signal);

    if(edfheaderlist[0]->annots_not_read)
    {
      QMessageBox::critical(this, "Error", "Editing annotations is not possible when you abort the scanning of the file.");
      return;
    }
  }

  if(annotations_dock[0]==NULL)
  {
    annotations_dock[0] = new UI_Annotationswindow(edfheaderlist[0], this);

    addDockWidget(Qt::RightDockWidgetArea, annotations_dock[0]->docklist, Qt::Vertical);
  }

  maincurve->active_markers->edf_hdr = edfheaderlist[0];

  annotations_dock[0]->docklist->show();

  if(annotationEditDock == NULL)
  {
//    printf("annotationEditDock = new    file: %s  line: %i\n", __FILE__, __LINE__);

    annotationEditDock = new UI_AnnotationEditwindow(edfheaderlist[0], this);

    addToolBar(Qt::BottomToolBarArea, annotationEditDock->dockedit);

    insertToolBarBreak(annotationEditDock->dockedit);
  }
  else
  {
//    printf("annotationEditDock pointer re-used!    file: %s  line: %i\n", __FILE__, __LINE__);

    annotationEditDock->set_edf_header(edfheaderlist[0]);

    annotationEditDock->dockedit->show();
  }
}


void UI_Mainwindow::show_spectrum_dock()
{
  int i;

  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    for(i=0; i<MAXSPECTRUMDOCKS; i++)
    {
      if(spectrumdock[i]->dock->isHidden())  break;
    }

    if(i<MAXSPECTRUMDOCKS)
    {
      spectrumdock[i]->init(0);
    }
    else
    {
      QMessageBox::critical(this, "Error", "The maximum number of docked Power Spectrum windows has been reached.\n"
                                           "Close one first.");
    }

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 1);
}


void UI_Mainwindow::show_cdsa_dock()
{
  int i;

  if((!files_open) || (!signalcomps) || live_stream_active)  return;

  if(signalcomps == 1)
  {
    if(signalcomp[0]->ecg_filter != NULL)
    {
      return;
    }

    if(signalcomp[0]->edfparam_0->sf_int < 30)
    {
      QMessageBox::critical(this, "Error", "Samplefrequency must be at least 30Hz and must be an integer value.");
      return;
    }

    if(signalcomp[0]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox::critical(this, "Error", "Recording length must be at least 30 seconds.");
      return;
    }

    for(i=0; i<MAXCDSADOCKS; i++)
    {
      if(cdsa_dock[i] == NULL)
      {
        UI_cdsa_window wndw(this, signalcomp[0], i);
        break;
      }
    }

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 5);
}


void UI_Mainwindow::show_aeeg_dock()
{
  int i;

  char str1_32[32]={""};

  if((!files_open) || (!signalcomps) || live_stream_active)  return;

  if(signalcomps == 1)
  {
    if(signalcomp[0]->ecg_filter != NULL)
    {
      return;
    }

    if(signalcomp[0]->edfparam_0->sf_int < 100)
    {
      QMessageBox::critical(this, "Error", "Samplefrequency must be at least 100Hz and must be an integer value.");
      return;
    }

    strlcpy(str1_32, signalcomp[0]->edfparam_0->physdimension, 32);
    trim_spaces(str1_32);
    if((strcmp(str1_32, "uV")) && (strcmp(str1_32, "mV")) && (strcmp(str1_32, "V")))
    {
      QMessageBox::critical(this, "Error", "Unknown physical dimension (unit), expected uV or mV or V");
      return;
    }

    if(signalcomp[0]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox::critical(this, "Error", "Recording length must be at least 30 seconds.");
      return;
    }

    for(i=0; i<MAXAEEGDOCKS; i++)
    {
      if(aeeg_dock[i] == NULL)
      {
        UI_aeeg_window wndw(this, signalcomp[0], i);
        break;
      }
    }

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 6);
}


void UI_Mainwindow::show_hypnogram()
{
  int i, file_num=0;

  if((!files_open) || live_stream_active)  return;

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(hypnogram_dock[i] == NULL)
    {
      if(files_open > 1)
      {
        UI_activeFileChooserWindow choose_file_dialog(&file_num, this);

        if(file_num >= 0)
        {
          UI_hypnogram_window hypnogram_dialog(this, edfheaderlist[file_num], i);
        }
      }
      else
      {
        UI_hypnogram_window hypnogram_dialog(this, edfheaderlist[0], i);
      }

      break;
    }
  }
}


void UI_Mainwindow::open_new_file()
{
  FILE *newfile=NULL;

  int i, len, present, position, button_nr=0, err=0;

  char str1_2048[2048]="";

  QAction *act=NULL;

  edfhdrblck_t *edfhdr=NULL;

  rc_file_open_err = 0;

  if(edflib_version() != 127)  return;

  if(annot_editor_active && files_open)
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 101;
    }
    else
    {
      QMessageBox::critical(this, "Error", "You cannot open multiple files when editing annotations.\n"
                                           "Close the annotation edit window first.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if((files_open > 0) && (live_stream_active))
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 102;
    }
    else
    {
      QMessageBox::critical(this, "Error", "You cannot open multiple files while a streaming file is open.\n"
                                           "Close the streaming file first.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if(files_open >= MAXFILES)
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 103;
    }
    else
    {
      QMessageBox::critical(this, "Error", "There are too many files opened.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if((cmdlineargument == 0) && (drop_path[0] == 0) && (!rc_file_open_requested))
  {
    strlcpy(path, QFileDialog::getOpenFileName(this, "Open file", QString::fromLocal8Bit(recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF *.rec *.REC)").toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(path, ""))
    {
      return;
    }

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);
  }

  if((cmdlineargument == 0) && (drop_path[0] != 0))
  {
    strlcpy(path, drop_path, MAX_PATH_LENGTH);

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);
  }

  if((cmdlineargument == 0) || (cmdlineargument == 1))
  {
    montagepath[0] = 0;
  }

  present = 0;

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(!strcmp(&recent_file_path[i][0], path))
    {
      present = 1;

      position = i;

      if(cmdlineargument != 2)
      {
        strlcpy(montagepath, &recent_file_mtg_path[i][0], MAX_PATH_LENGTH);
      }

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_file_path[i][0], &recent_file_path[i-1][0], MAX_PATH_LENGTH);
      strlcpy(&recent_file_mtg_path[i][0], &recent_file_mtg_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_file_path[i][0], &recent_file_path[i-1][0], MAX_PATH_LENGTH);
      strlcpy(&recent_file_mtg_path[i][0], &recent_file_mtg_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_file_path[0][0], path, MAX_PATH_LENGTH);
  strlcpy(&recent_file_mtg_path[0][0], montagepath, MAX_PATH_LENGTH);

  recent_filesmenu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_file_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_file_path[i][0]), recent_filesmenu);
    act->setData(QVariant(i));
    recent_filesmenu->addAction(act);
  }
  recent_filesmenu->addSeparator();
  act = new QAction("Clear list", recent_filesmenu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_filesmenu->addAction(act);

  present = 0;

  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, path))
    {
      present = 1;

      break;
    }
  }

  if(!present)
  {
    len = strlen(path);

    err = 0;

    if(len < 4)
    {
      err = 1;
    }
    else
    {
      if((strcmp(path + (len - 4), ".edf"))
         &&(strcmp(path + (len - 4), ".EDF"))
         &&(strcmp(path + (len - 4), ".rec"))
         &&(strcmp(path + (len - 4), ".REC"))
         &&(strcmp(path + (len - 4), ".bdf"))
         &&(strcmp(path + (len - 4), ".BDF")))
      {
        err = 1;
      }
    }

    if(err)
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 104;
      }
      else
      {
        snprintf(str1_2048, 2048, "File has an unknown extension:\n \n%s", path);

        QMessageBox::critical(this, "Error", QString::fromLocal8Bit(str1_2048));
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    newfile = fopeno(path, "rb");
    if(newfile==NULL)
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 105;
      }
      else
      {
        snprintf(str1_2048, 2048, "Cannot open file for reading:\n \n%s\n \n"
                             "Check if you have the right permissions.", path);
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit(str1_2048));
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

    edfhdr = check_edf_file(newfile, str1_2048, 2048, live_stream_active, 0);
    if(edfhdr==NULL)
    {
      fclose(newfile);

      newfile = NULL;

      if(rc_file_open_requested)
      {
        rc_file_open_err = 106;
      }
      else
      {
        if(strstr(str1_2048, "If you want to import annotations from this file") != NULL)
        {
          QMessageBox::critical(this, "Error", str1_2048);
        }
        else
        {
          tmessagebox(QMessageBox::Critical, "Error", "File is not a valid EDF or BDF file.", str1_2048, this);
        }
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    if(edfhdr->discontinuous)
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 107;
      }
      else
      {
        if(edfhdr->edf)
        {
          tmessagebox(QMessageBox::Critical, "Error",
                      "EDFbrowser cannot show EDF+D (discontiguous) files",
                      "Convert this file to EDF+C first. You can find this converter\n"
                      "in the Tools menu (EDF+D to EDF+C converter).", this);
        }
        else if(edfhdr->bdf)
          {
            tmessagebox(QMessageBox::Critical, "Error",
                        "EDFbrowser cannot show BDF+D (discontiguous) files",
                        "Convert this file to BDF+C first. You can find this converter\n"
                        "in the Tools menu (EDF+D to EDF+C converter).", this);
          }
      }

      free(edfhdr->edfparam);
      edfhdr->edfparam = NULL;
      free(edfhdr);
      edfhdr = NULL;
      fclose(newfile);
      newfile = NULL;
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    strlcpy(edfhdr->filename, path, MAX_PATH_LENGTH);

    edfhdr->file_hdl = newfile;

    edfheaderlist[files_open] = edfhdr;

    memset(&edfheaderlist[files_open]->annot_list, 0, sizeof(annotlist_t));

    annotations_dock[files_open] = NULL;

    if((edfhdr->edfplus || edfhdr->bdfplus) && (!live_stream_active))
    {
      if((edfhdr->datarecords * (unsigned long long)edfhdr->recordsize) <= maxfilesize_to_readin_annotations)
      {
        EDF_annotations annotations;

        annotations.get_annotations(edfhdr, read_nk_trigger_signal);

        if(edfhdr->annots_not_read)
        {
          edfplus_annotation_empty_list(&edfheaderlist[files_open]->annot_list);
        }
        else
        {
          annotations_dock[files_open] = new UI_Annotationswindow(edfheaderlist[files_open], this);

          addDockWidget(Qt::RightDockWidgetArea, annotations_dock[files_open]->docklist, Qt::Vertical);

          if(!edfheaderlist[files_open]->annot_list.sz)
          {
            annotations_dock[files_open]->docklist->hide();
          }
        }
      }
      else
      {
        edfhdr->annots_not_read = 1;
      }
    }

    edfhdr->utc_starttime_hr = (edfhdr->utc_starttime * TIME_FIXP_SCALING) + edfhdr->starttime_subsec;

    edfhdr->utc_endtime_hr = edfhdr->utc_starttime_hr + (edfhdr->long_data_record_duration * edfhdr->datarecords);

    edfhdr->starttime_hr += edfhdr->starttime_subsec;

    if((edfhdr->bdf && (!edfhdr->bdfplus) && read_biosemi_status_signal) && (!live_stream_active))
    {
      if((edfhdr->datarecords * (unsigned long long)edfhdr->recordsize) <= maxfilesize_to_readin_annotations)
      {
        BDF_triggers bdf_triggers_obj;

        bdf_triggers_obj.get_triggers(edfhdr);

        annotations_dock[files_open] = new UI_Annotationswindow(edfheaderlist[files_open], this);

        addDockWidget(Qt::RightDockWidgetArea, annotations_dock[files_open]->docklist, Qt::Vertical);

        if(!edfhdr->annot_list.sz)
        {
          annotations_dock[files_open]->docklist->hide();
        }
      }
    }

    if(!files_open)
    {
      edfheaderlist[0]->viewtime = 0;

      setMainwindowTitle(edfhdr);
    }
    else
    {
      if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
      {
        edfheaderlist[files_open]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
      }

      if(viewtime_sync==VIEWTIME_UNSYNCED)
      {
        edfheaderlist[files_open]->viewtime = 0;
      }

      if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
      {
        edfheaderlist[files_open]->viewtime = edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->utc_starttime_hr - edfheaderlist[files_open]->utc_starttime_hr;
      }

      if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
      {
        edfheaderlist[files_open]->viewtime = 0;
      }
    }

    sel_viewtime_act[files_open] = new QAction(QString::fromLocal8Bit(edfhdr->filename), this);
    sel_viewtime_act[files_open]->setCheckable(true);
    if(!files_open)
    {
      sel_viewtime_act[files_open]->setChecked(true);
    }
    sel_viewtime_act_group->addAction(sel_viewtime_act[files_open]);
    timemenu->addAction(sel_viewtime_act[files_open]);

    files_open++;
  }

  if(files_open == 1)
  {
    if(default_time_scale > 0)
    {
      pagetime = default_time_scale * TIME_FIXP_SCALING;
    }
    else if(default_time_scale == 0)
    {
      pagetime = edfhdr->recording_duration_hr;
    }

    if(pagetime < 100LL)  pagetime = 10 * TIME_FIXP_SCALING;
  }

  if(!rc_file_open_requested)
  {
    if((montagepath[0]!=0)&&(cmdlineargument==2))
    {
      UI_LoadMontagewindow load_mtg(this, montagepath);
      strlcpy(&recent_file_mtg_path[0][0], montagepath, MAX_PATH_LENGTH);
    }
    else
    {
      if((recent_file_mtg_path[0][0] != 0) && (files_open == 1) && auto_reload_mtg)
      {
        QMessageBox messagewindow;
        messagewindow.setText("Load last used montage?");
        messagewindow.setInformativeText(QString::fromLocal8Bit(&recent_file_mtg_path[0][0]));
        messagewindow.setIcon(QMessageBox::Question);
        messagewindow.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messagewindow.setDefaultButton(QMessageBox::Yes);
        button_nr = messagewindow.exec();

        if(button_nr == QMessageBox::No)
        {
          recent_file_mtg_path[0][0] = 0;

          UI_Signalswindow signalwindow(this);

          if(!signalcomps)
          {
            setup_viewbuf();
          }
        }
        else
        {
          UI_LoadMontagewindow load_mtg(this, &recent_file_mtg_path[0][0]);

          if(recent_file_mtg_path[0][0] == 0)
          {
            UI_Signalswindow signalwindow(this);

            if(!signalcomps)
            {
              setup_viewbuf();
            }
          }
        }
      }
      else
      {
        UI_Signalswindow signalwindow(this);

        if(!signalcomps)
        {
          setup_viewbuf();
        }
      }
    }
  }

  if((files_open == 1) && (montagepath[0]==0))
  {
    if(default_fit_signals_to_pane)
    {
      fit_signals_to_pane(-1);
    }
  }

  cmdlineargument = 0;

  montagepath[0] = 0;

  drop_path[0] = 0;

  rc_file_open_requested = 0;

  if(files_open)
  {
    save_session_act->setEnabled(true);
  }

  close_filemenu->addAction(QString::fromLocal8Bit(path));
}


void UI_Mainwindow::remove_recent_file_mtg_path(const char *mtg_path)
{
  int i;


  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(!strcmp(&recent_file_mtg_path[i][0], mtg_path))
    {
      recent_file_mtg_path[i][0] = 0;
    }
  }
}


void UI_Mainwindow::setMainwindowTitle(edfhdrblck_t *edfhdr)
{
  int i, len;

  char str1[MAX_PATH_LENGTH + 128]={""};

  date_time_t date_time;


  if(edfhdr==NULL)
  {
    setWindowTitle(PROGRAM_NAME);

    return;
  }

  if(mainwindow_title_type == 0)
  {
    if(edfhdr->edfplus || edfhdr->bdfplus)
    {
      snprintf(str1, 256, PROGRAM_NAME "  subject %s  birthdate %s  startdate %s",
                    edfhdr->plus_patient_name,
                    edfhdr->plus_birthdate,
                    edfhdr->plus_startdate);
    }
    else
    {
      utc_to_date_time(edfhdr->utc_starttime, &date_time);

      date_time.month_str[0] += 32;
      date_time.month_str[1] += 32;
      date_time.month_str[2] += 32;

      snprintf(str1, 256, PROGRAM_NAME "  %s  startdate %i %s %i",
                    edfhdr->patient,
                    date_time.day,
                    date_time.month_str,
                    date_time.year);
      len = strlen(str1);
      for(i=0; i<len; i++)
      {
        if(str1[i]=='_')
        {
          str1[i] = ' ';
        }
      }
    }
  }

  if(mainwindow_title_type == 1)
  {
    get_filename_from_path(str1, QString::fromLocal8Bit(edfhdr->filename).toUtf8().data(), MAX_PATH_LENGTH);

    strlcat(str1, " - " PROGRAM_NAME, MAX_PATH_LENGTH + 128);
  }

  if(mainwindow_title_type == 2)
  {
    strlcpy(str1, QString::fromLocal8Bit(edfhdr->filename).toUtf8().data(), MAX_PATH_LENGTH + 128);

    strlcat(str1, " - " PROGRAM_NAME, MAX_PATH_LENGTH + 128);
  }

  setWindowTitle(QString::fromUtf8(str1));
}


void UI_Mainwindow::signalproperties_dialog()
{
  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    maincurve->exec_sidemenu(0);

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 0);
}


void UI_Mainwindow::filterproperties_dialog()
{
  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    AdjustFilterSettings filtersettings(signalcomp[0], maincurve);

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 2);
}


void UI_Mainwindow::add_signals_dialog()
{
  if(!files_open)  return;

  UI_Signalswindow signalwindow(this);
}


void UI_Mainwindow::show_splashscreen()
{
#if QT_VERSION >= 0x050F00
  splash = new QSplashScreen(this->screen(), *splash_pixmap);
#else
  splash = new QSplashScreen(this, *splash_pixmap, Qt::WindowStaysOnTopHint);
#endif
  splash->show();
}


void UI_Mainwindow::show_file_info()
{
  if(!files_open)  return;

  UI_EDFhdrwindow showhdr(this);
}


void UI_Mainwindow::remove_all_filters()
{
  int i, j,
      update_scr=0,
      remove_list[512],
      remove_cnt=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      free(signalcomp[i]->filter[j]);

      update_scr = 1;
    }

    signalcomp[i]->filter_cnt = 0;

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      free(signalcomp[i]->fidfilter[j]);
      fid_run_free(signalcomp[i]->fid_run[j]);
      fid_run_freebuf(signalcomp[i]->fidbuf[j]);
      fid_run_freebuf(signalcomp[i]->fidbuf2[j]);

      update_scr = 1;
    }

    signalcomp[i]->fidfilter_cnt = 0;

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      free_ravg_filter(signalcomp[i]->ravg_filter[j]);

      update_scr = 1;
    }

    signalcomp[i]->ravg_filter_cnt = 0;

    for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
    {
      free_fir_filter(signalcomp[i]->fir_filter[j]);

      signalcomp[i]->fir_filter[j] = NULL;
    }

    signalcomp[i]->fir_filter_cnt = 0;

    if(signalcomp[i]->ecg_filter != NULL)
    {
      free_ecg_filter(signalcomp[i]->ecg_filter);

      signalcomp[i]->ecg_filter = NULL;

      strlcpy(signalcomp[i]->signallabel, signalcomp[i]->signallabel_bu, 512);
      signalcomp[i]->signallabellen = signalcomp[i]->signallabellen_bu;
      strlcpy(signalcomp[i]->physdimension, signalcomp[i]->physdimension_bu, 9);

      if(remove_cnt < 512)
      {
        remove_list[remove_cnt] = i;
        remove_cnt++;
      }

      update_scr = 1;
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      free_zratio_filter(signalcomp[i]->zratio_filter);

      signalcomp[i]->zratio_filter = NULL;

      strlcpy(signalcomp[i]->signallabel, signalcomp[i]->signallabel_bu, 512);
      signalcomp[i]->signallabellen = signalcomp[i]->signallabellen_bu;
      strlcpy(signalcomp[i]->physdimension, signalcomp[i]->physdimension_bu, 9);
    }
  }

  if(remove_cnt)
  {
    for(i=0; i<remove_cnt; i++)
    {
      remove_signalcomp(remove_list[i]);
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_plif_ecg_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->plif_ecg_filter)
    {
      plif_ecg_free_subtract_filter(signalcomp[i]->plif_ecg_filter);

      signalcomp[i]->plif_ecg_filter = NULL;

      update_scr = 1;
    }

    if(signalcomp[i]->plif_ecg_filter_sav)
    {
      plif_ecg_free_subtract_filter(signalcomp[i]->plif_ecg_filter_sav);

      signalcomp[i]->plif_ecg_filter_sav = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_plif_eeg_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->plif_eeg_filter)
    {
      plif_eeg_free_subtract_filter(signalcomp[i]->plif_eeg_filter);

      signalcomp[i]->plif_eeg_filter = NULL;

      update_scr = 1;
    }

    if(signalcomp[i]->plif_eeg_filter_sav)
    {
      plif_eeg_free_subtract_filter(signalcomp[i]->plif_eeg_filter_sav);

      signalcomp[i]->plif_eeg_filter_sav = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_fir_filters()
{
  int i, j,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
    {
      free_fir_filter(signalcomp[i]->fir_filter[j]);

      signalcomp[i]->fir_filter[j] = NULL;

      update_scr = 1;
    }

    signalcomp[i]->fir_filter_cnt = 0;
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_spike_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->spike_filter)
    {
      free_spike_filter(signalcomp[i]->spike_filter);

      signalcomp[i]->spike_filter = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_signals()
{
  int i;

  stop_video_generic(0);

  stop_playback();

  session_start_video = 0;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    spectrumdock[i]->clear();
    spectrumdock[i]->dock->hide();
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(spectrumdialog[i] != NULL)
    {
      delete spectrumdialog[i];

      spectrumdialog[i] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] != NULL)
    {
      delete cdsa_dock[i];

      cdsa_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(aeeg_dock[i] != NULL)
    {
      delete aeeg_dock[i];

      aeeg_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(averagecurvedialog[i] != NULL)
    {
      delete averagecurvedialog[i];

      averagecurvedialog[i] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(zscoredialog[i] != NULL)
    {
      delete zscoredialog[i];

      zscoredialog[i] = NULL;
    }
  }

  maincurve->crosshair_1.active = 0;
  maincurve->crosshair_2.active = 0;
  maincurve->crosshair_1.moving = 0;
  maincurve->crosshair_2.moving = 0;

  remove_all_math_funcs();

  remove_all_filters();

  remove_all_spike_filters();

  remove_all_plif_ecg_filters();

  remove_all_plif_eeg_filters();

  for(i=0; i<signalcomps; i++)
  {
    free(signalcomp[i]);

    signalcomp[i] = NULL;
  }

  signalcomps = 0;

  free(viewbuf);
  viewbuf = NULL;

  slidertoolbar->setEnabled(false);
  positionslider->blockSignals(true);

  setup_viewbuf();
}


void UI_Mainwindow::close_file_action_func(QAction *action)
{
  int i, j, k, p,
      file_n,
      inst_num=0;

  char f_path[MAX_PATH_LENGTH]={""};

  if(files_open < 2)
  {
    close_all_files();

    return;
  }

  toolbar_stats.active = 0;

  nav_toolbar_label->setText("");

  strlcpy(f_path, action->text().toLocal8Bit().data(), MAX_PATH_LENGTH);

  for(file_n=0; file_n<files_open; file_n++)
  {
    if(!strcmp(f_path, edfheaderlist[file_n]->filename))
    {
      break;
    }
  }

  if(file_n == files_open)  return;

  delete action;

  stop_video_generic(0);

  stop_playback();

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    inst_num = edfheaderlist[file_n]->hypnogram_idx[i];

    if(inst_num > 0)
    {
      if(hypnogram_dock[inst_num - 1] != NULL)
      {
        delete hypnogram_dock[inst_num - 1];

        hypnogram_dock[inst_num - 1] = NULL;
      }

      edfheaderlist[file_n]->hypnogram_idx[i] = 0;
    }
  }

  for(i=0; i<MAXHRVDOCKS; i++)
  {
    inst_num = edfheaderlist[file_n]->hrv_dock[i];

    if(inst_num > 0)
    {
      if(hrv_dock[inst_num - 1] != NULL)
      {
        delete hrv_dock[inst_num - 1];

        hrv_dock[inst_num - 1] = NULL;
      }

      edfheaderlist[file_n]->hrv_dock[i] = 0;
    }
  }

  for(j=0; j<signalcomps; )
  {
    if(signalcomp[j]->edfhdr == edfheaderlist[file_n])
    {
      for(i=0; i<MAXSPECTRUMDOCKS; i++)
      {
        if(spectrumdock[i]->signalcomp == signalcomp[j])
        {
          spectrumdock[i]->clear();
          spectrumdock[i]->dock->hide();
        }
      }

      for(i=0; i<MAXSPECTRUMDIALOGS; i++)
      {
        p = signalcomp[j]->spectr_dialog[i];

        if(p != 0)
        {
          delete spectrumdialog[p - 1];

          spectrumdialog[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXCDSADOCKS; i++)
      {
        p = signalcomp[j]->cdsa_idx[i];

        if(p != 0)
        {
          delete cdsa_dock[p - 1];

          cdsa_dock[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXAEEGDOCKS; i++)
      {
        p = signalcomp[j]->aeeg_idx[i];

        if(p != 0)
        {
          delete aeeg_dock[p - 1];

          aeeg_dock[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
      {
        p = signalcomp[j]->avg_dialog[i];

        if(p != 0)
        {
          delete averagecurvedialog[p - 1];

          averagecurvedialog[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXZSCOREDIALOGS; i++)
      {
        p = signalcomp[j]->zscoredialog[i];

        if(p != 0)
        {
          delete zscoredialog[p - 1];

          zscoredialog[p - 1] = NULL;
        }
      }

      if(signalcomp[j]->hascursor2)
      {
        maincurve->crosshair_2.active = 0;
        maincurve->crosshair_2.moving = 0;
      }

      if(signalcomp[j]->hascursor1)
      {
        maincurve->crosshair_1.active = 0;
        maincurve->crosshair_2.active = 0;
        maincurve->crosshair_1.moving = 0;
        maincurve->crosshair_2.moving = 0;

        for(i=0; i<signalcomps; i++)
        {
          signalcomp[i]->hascursor2 = 0;
        }
      }

      if(signalcomp[j]->hasruler)
      {
        maincurve->ruler_active = 0;
        maincurve->ruler_moving = 0;
      }

      for(k=0; k<signalcomp[j]->filter_cnt; k++)
      {
        free(signalcomp[j]->filter[k]);
      }

      signalcomp[j]->filter_cnt = 0;

      if(signalcomp[j]->plif_ecg_filter)
      {
        plif_ecg_free_subtract_filter(signalcomp[j]->plif_ecg_filter);

        signalcomp[j]->plif_ecg_filter = NULL;
      }

      if(signalcomp[j]->plif_ecg_filter_sav)
      {
        plif_ecg_free_subtract_filter(signalcomp[j]->plif_ecg_filter_sav);

        signalcomp[j]->plif_ecg_filter_sav = NULL;
      }

      if(signalcomp[j]->plif_eeg_filter)
      {
        plif_eeg_free_subtract_filter(signalcomp[j]->plif_eeg_filter);

        signalcomp[j]->plif_eeg_filter = NULL;
      }

      if(signalcomp[j]->plif_eeg_filter_sav)
      {
        plif_eeg_free_subtract_filter(signalcomp[j]->plif_eeg_filter_sav);

        signalcomp[j]->plif_eeg_filter_sav = NULL;
      }

      if(signalcomp[j]->spike_filter)
      {
        free_spike_filter(signalcomp[j]->spike_filter);

        signalcomp[j]->spike_filter = NULL;
      }

      for(k=0; k<signalcomp[j]->ravg_filter_cnt; k++)
      {
        free_ravg_filter(signalcomp[j]->ravg_filter[k]);
      }

      signalcomp[j]->ravg_filter_cnt = 0;

      for(k=0; k<signalcomp[j]->fir_filter_cnt; k++)
      {
        free_fir_filter(signalcomp[j]->fir_filter[k]);

        signalcomp[j]->fir_filter[k] = NULL;
      }

      signalcomp[j]->fir_filter_cnt = 0;

      if(signalcomp[j]->ecg_filter != NULL)
      {
        free_ecg_filter(signalcomp[j]->ecg_filter);

        signalcomp[j]->ecg_filter = NULL;

        strlcpy(signalcomp[j]->signallabel, signalcomp[j]->signallabel_bu, 512);
        signalcomp[j]->signallabellen = signalcomp[j]->signallabellen_bu;
        strlcpy(signalcomp[j]->physdimension, signalcomp[j]->physdimension_bu, 9);
      }

      for(k=0; k<signalcomp[j]->fidfilter_cnt; k++)
      {
        free(signalcomp[j]->fidfilter[k]);
        fid_run_free(signalcomp[j]->fid_run[k]);
        fid_run_freebuf(signalcomp[j]->fidbuf[k]);
        fid_run_freebuf(signalcomp[j]->fidbuf2[k]);
      }

      signalcomp[j]->fidfilter_cnt = 0;

      signalcomp[j]->uid = 0LL;

      free(signalcomp[j]);

      signalcomp[j] = NULL;

      for(i=j; i<signalcomps - 1; i++)
      {
        signalcomp[i] = signalcomp[i + 1];
      }

      signalcomps--;
    }
    else
    {
      j++;
    }
  }

  edfplus_annotation_empty_list(&edfheaderlist[file_n]->annot_list);
  fclose(edfheaderlist[file_n]->file_hdl);
  free(edfheaderlist[file_n]->edfparam);
  edfheaderlist[file_n]->edfparam = NULL;
  free(edfheaderlist[file_n]);
  edfheaderlist[file_n] = NULL;

  if(annotations_dock[file_n] != NULL)
  {
    annotations_dock[file_n]->docklist->close();
    delete annotations_dock[file_n];
    annotations_dock[file_n] = NULL;
  }

  if(file_n == sel_viewtime)
  {
    sel_viewtime = 0;
  }

  delete sel_viewtime_act[file_n];

  for(i=file_n; i<files_open - 1; i++)
  {
    edfheaderlist[i] = edfheaderlist[i + 1];

    annotations_dock[i] = annotations_dock[i + 1];

    sel_viewtime_act[i] = sel_viewtime_act[i + 1];

    if((i + 1) == sel_viewtime)
    {
      sel_viewtime--;
    }
  }

  files_open--;

  sel_viewtime_act[sel_viewtime]->setChecked(true);

  setMainwindowTitle(edfheaderlist[sel_viewtime]);

  setup_viewbuf();

  if(!files_open)
  {
    save_session_act->setEnabled(false);
  }
}


void UI_Mainwindow::close_all_files()
{
  int i, j, button_nr=0, inst_num=0;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  toolbar_stats.active = 0;
  nav_toolbar_label->setText("");
  live_stream_active = 0;
  live_stream_active_paused = 0;
  live_stream_timer->stop();
  playback_file_Act->setIcon(QIcon(":/images/media-playback-start-symbolic.symbolic.png"));
  toolsmenu->setEnabled(true);
  timemenu->setEnabled(true);
  lhsc_uwo_bids_menu->setEnabled(true);
//  windowmenu->setEnabled(true);
  windowmenu->actions().at(0)->setEnabled(true);  // Annotations window
  windowmenu->actions().at(1)->setEnabled(true);  // Annotation editor
  former_page_Act->setEnabled(true);
  shift_page_left_Act->setEnabled(true);
  shift_page_right_Act->setEnabled(true);
  next_page_Act->setEnabled(true);
  shift_page_up_Act->setEnabled(true);
  shift_page_down_Act->setEnabled(true);
  printmenu->setEnabled(true);
  recent_filesmenu->setEnabled(true);
  recent_session_menu->setEnabled(true);
  playback_file_Act->setEnabled(true);

  save_session_act->setEnabled(false);

  annotations_edited = 0;

  session_start_video = 0;

  remove_all_signals();

  while(files_open)
  {
    files_open--;

    for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
    {
      inst_num = edfheaderlist[files_open]->hypnogram_idx[i];

      if(inst_num > 0)
      {
        if(hypnogram_dock[inst_num - 1] != NULL)
        {
          delete hypnogram_dock[inst_num - 1];

          hypnogram_dock[inst_num - 1] = NULL;
        }

        edfheaderlist[files_open]->hypnogram_idx[i] = 0;
      }
    }

    for(i=0; i<MAXHRVDOCKS; i++)
    {
      inst_num = edfheaderlist[files_open]->hrv_dock[i];

      if(inst_num > 0)
      {
        if(hrv_dock[inst_num - 1] != NULL)
        {
          delete hrv_dock[inst_num - 1];

          hrv_dock[inst_num - 1] = NULL;
        }

        edfheaderlist[files_open]->hrv_dock[i] = 0;
      }
    }

    if(edfheaderlist[files_open]->file_hdl != NULL)
    {
      fclose(edfheaderlist[files_open]->file_hdl);
    }
    edfplus_annotation_empty_list(&edfheaderlist[files_open]->annot_list);
    free(edfheaderlist[files_open]->edfparam);
    edfheaderlist[files_open]->edfparam = NULL;
    free(edfheaderlist[files_open]);
    edfheaderlist[files_open] = NULL;

    if(annotations_dock[files_open] != NULL)
    {
      annotations_dock[files_open]->docklist->close();
      delete annotations_dock[files_open];
      annotations_dock[files_open] = NULL;
    }

    delete sel_viewtime_act[files_open];
    sel_viewtime_act[files_open] = NULL;
  }

  if(annotationlist_backup != NULL)
  {
    edfplus_annotation_empty_list(annotationlist_backup);

    free(annotationlist_backup);

    annotationlist_backup = NULL;
  }

  sel_viewtime = 0;

  pagetime = 10 * TIME_FIXP_SCALING;

  timescale_doubler = 10;

  amplitude_doubler = 10;

  viewtime_string_128[0] = 0;

  pagetime_string_128[0] = 0;

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

  delete annotationEditDock;
  annotationEditDock = NULL;

  save_act->setEnabled(false);

  annotations_edited = 0;

  dig_min_max_overflow = 0;

  dig_min_max_overflow_warning_showed = 0;

  setWindowTitle(PROGRAM_NAME);

  close_filemenu->clear();

  mc_v_scrollbar->setValue(0);

  if(!exit_in_progress)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::show_about_dialog()
{
  UI_Aboutwindow aboutwindow(this);
}


void UI_Mainwindow::page_10mmmin()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 60;

  setup_viewbuf();
}


void UI_Mainwindow::page_15mmmin()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 40;

  setup_viewbuf();
}


void UI_Mainwindow::page_30mmmin()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 20;

  setup_viewbuf();
}


void UI_Mainwindow::page_1mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 10;

  setup_viewbuf();
}


void UI_Mainwindow::page_2mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 5;

  setup_viewbuf();
}


void UI_Mainwindow::page_5mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * 2;

  setup_viewbuf();
}


void UI_Mainwindow::page_10mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING;

  setup_viewbuf();
}


void UI_Mainwindow::page_15mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 1.5;

  setup_viewbuf();
}


void UI_Mainwindow::page_25mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 2.5;

  setup_viewbuf();
}


void UI_Mainwindow::page_30mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 3.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_50mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 5.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_60mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 6.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_100mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 10.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_120mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 12.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_250mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 25.0;

  setup_viewbuf();
}


void UI_Mainwindow::page_500mmsec()
{
  pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING / 50.0;

  setup_viewbuf();
}


void UI_Mainwindow::set_page_div2()
{
  int i;

//   long long l_tmp, trshld=100LL;

  if(pagetime <= 100LL)  return;

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      if(timescale_doubler == 50)
      {
        edfheaderlist[i]->viewtime += (pagetime * 0.3);
      }
      else
      {
        edfheaderlist[i]->viewtime += (pagetime / 4);
      }

//       if(viewtime_sync!=VIEWTIME_USER_DEF_SYNCED)
//       {
//         l_tmp = edfheaderlist[i]->viewtime % TIME_FIXP_SCALING;
//
//         if(l_tmp < trshld)
//         {
//           edfheaderlist[i]->viewtime -= l_tmp;
//         }
//
//         if(l_tmp > (TIME_FIXP_SCALING - trshld))
//         {
//           edfheaderlist[i]->viewtime += (TIME_FIXP_SCALING - l_tmp);
//         }
//       }
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    if(timescale_doubler == 50)
    {
      edfheaderlist[sel_viewtime]->viewtime += (pagetime * 0.3);
    }
    else
    {
      edfheaderlist[sel_viewtime]->viewtime += (pagetime / 4);
    }

//     l_tmp = edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING;
//
//     if(l_tmp < trshld)
//     {
//       edfheaderlist[sel_viewtime]->viewtime -= l_tmp;
//     }
//
//     if(l_tmp > (TIME_FIXP_SCALING - trshld))
//     {
//       edfheaderlist[sel_viewtime]->viewtime += (TIME_FIXP_SCALING - l_tmp);
//     }
  }

  if(timescale_doubler == 10)
  {
    timescale_doubler = 50;

    pagetime /= 2;
  }
  else
  {
    if(timescale_doubler == 50)
    {
      timescale_doubler = 20;

      pagetime /= 2.5;
    }
    else
    {
      timescale_doubler = 10;

      pagetime /= 2;
    }
  }

//  printf("pagetime: %12lli      viewtime: %12lli\n", pagetime, edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING);

  setup_viewbuf();
}


void UI_Mainwindow::set_page_mult2()
{
  int i;

//  long long l_tmp, trshld=100LL;

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      if(timescale_doubler == 20)
      {
        edfheaderlist[i]->viewtime -= (pagetime * 0.75);
      }
      else
      {
        edfheaderlist[i]->viewtime -= (pagetime / 2);
      }

      // if(viewtime_sync!=VIEWTIME_USER_DEF_SYNCED)
      // {
      //   l_tmp = edfheaderlist[i]->viewtime % TIME_FIXP_SCALING;
      //
      //   if(l_tmp < trshld)
      //   {
      //     edfheaderlist[i]->viewtime -= l_tmp;
      //   }
      //
      //   if(l_tmp > (TIME_FIXP_SCALING - trshld))
      //   {
      //     edfheaderlist[i]->viewtime += (TIME_FIXP_SCALING - l_tmp);
      //   }
      // }
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    if(timescale_doubler == 20)
    {
      edfheaderlist[sel_viewtime]->viewtime -= (pagetime * 0.75);
    }
    else
    {
      edfheaderlist[sel_viewtime]->viewtime -= (pagetime / 2);
    }

    // l_tmp = edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING;
    //
    // if(l_tmp < trshld)
    // {
    //   edfheaderlist[sel_viewtime]->viewtime -= l_tmp;
    // }
    //
    // if(l_tmp > (TIME_FIXP_SCALING - trshld))
    // {
    //   edfheaderlist[sel_viewtime]->viewtime += (TIME_FIXP_SCALING - l_tmp);
    // }
  }

  if(timescale_doubler == 10)
  {
    timescale_doubler = 20;

    pagetime *= 2;
  }
  else
  {
    if(timescale_doubler == 20)
    {
      timescale_doubler = 50;

      pagetime *= 2.5;
    }
    else
    {
      timescale_doubler = 10;

      pagetime *= 2;
    }
  }

//  printf("pagetime: %12lli      viewtime: %12lli\n", pagetime, edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING);

  setup_viewbuf();
}


void UI_Mainwindow::set_display_time(QAction *action)
{
  if(action==page_10u)
  {
    pagetime = TIME_FIXP_SCALING / 100000;

    timescale_doubler = 10;
  }
  else if(action==page_20u)
  {
    pagetime = TIME_FIXP_SCALING / 50000;

    timescale_doubler = 20;
  }
  else if(action==page_50u)
  {
    pagetime = TIME_FIXP_SCALING / 20000;

    timescale_doubler = 50;
  }
  else if(action==page_100u)
  {
    pagetime = TIME_FIXP_SCALING / 10000;

    timescale_doubler = 10;
  }
  else if(action==page_200u)
  {
    pagetime = TIME_FIXP_SCALING / 5000;

    timescale_doubler = 20;
  }
  else if(action==page_500u)
  {
    pagetime = TIME_FIXP_SCALING / 2000;

    timescale_doubler = 50;
  }
  else if(action==page_1m)
  {
    pagetime = TIME_FIXP_SCALING / 1000;

    timescale_doubler = 10;
  }
  else if(action==page_2m)
  {
    pagetime = TIME_FIXP_SCALING / 500;

    timescale_doubler = 20;
  }
  else if(action==page_5m)
  {
    pagetime = TIME_FIXP_SCALING / 200;

    timescale_doubler = 50;
  }
  else if(action==page_10m)
  {
    pagetime = TIME_FIXP_SCALING / 100;

    timescale_doubler = 10;
  }
  else if(action==page_20m)
  {
    pagetime = TIME_FIXP_SCALING / 50;

    timescale_doubler = 20;
  }
  else if(action==page_50m)
  {
    pagetime = TIME_FIXP_SCALING / 20;

    timescale_doubler = 50;
  }
  else if(action==page_100m)
  {
    pagetime = TIME_FIXP_SCALING / 10;

    timescale_doubler = 10;
  }
  else if(action==page_200m)
  {
    pagetime = TIME_FIXP_SCALING / 5;

    timescale_doubler = 20;
  }
  else if(action==page_500m)
  {
    pagetime = TIME_FIXP_SCALING / 2;

    timescale_doubler = 50;
  }
  else if(action==page_1)
  {
    pagetime = TIME_FIXP_SCALING;

    timescale_doubler = 10;
  }
  else if(action==page_2)
  {
    pagetime = TIME_FIXP_SCALING * 2;

    timescale_doubler = 20;
  }
  else if(action==page_5)
  {
    pagetime = TIME_FIXP_SCALING * 5;

    timescale_doubler = 50;
  }
  else if(action==page_10)
  {
    pagetime = TIME_FIXP_SCALING * 10;

    timescale_doubler = 10;
  }
  else if(action==page_15)    pagetime = TIME_FIXP_SCALING * 15;
  else if(action==page_20)
  {
    pagetime = TIME_FIXP_SCALING * 20;

    timescale_doubler = 20;
  }
  else if(action==page_30)    pagetime = TIME_FIXP_SCALING * 30;
  else if(action==page_60)    pagetime = TIME_FIXP_SCALING * 60;
  else if(action==page_300)   pagetime = TIME_FIXP_SCALING * 300;
  else if(action==page_1200)  pagetime = TIME_FIXP_SCALING * 1200;
  else if(action==page_3600)  pagetime = TIME_FIXP_SCALING * 3600;
  else if(action==page_8h)    pagetime = TIME_FIXP_SCALING * 3600 * 8;
  else if(action==page_12h)   pagetime = TIME_FIXP_SCALING * 3600 * 12;
  else if(action==page_24h)   pagetime = TIME_FIXP_SCALING * 3600 * 24;
  else if(action==page_48h)   pagetime = TIME_FIXP_SCALING * 3600 * 48;

  setup_viewbuf();
}


void UI_Mainwindow::set_user_defined_display_time()
{
  if(!files_open)  return;

  UI_Userdefined_timepage_Dialog set_displaytime_dialog(this);
}


void UI_Mainwindow::set_user_defined_amplitude()
{
  if(!files_open)  return;

  if(!signalcomps)  return;

  UI_Userdefined_amplitude_Dialog set_amplitude_dialog(this);
}


void UI_Mainwindow::set_display_time_whole_rec()
{
  int i;

  long long vtime=0LL;

  if(!files_open)  return;

  if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = 0;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime = 0;
  }

  if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
  {
    edfheaderlist[sel_viewtime]->viewtime = 0;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->utc_starttime_hr - edfheaderlist[i]->utc_starttime_hr;
      }
    }
  }

  if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
  {
    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime -= edfheaderlist[sel_viewtime]->viewtime;
      }
    }

    edfheaderlist[sel_viewtime]->viewtime = 0;
  }

  for(i=0; i<files_open; i++)
  {
    if(edfheaderlist[i]->viewtime > vtime)
    {
      vtime = edfheaderlist[i]->viewtime;
    }
  }

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime -= vtime;
  }

  pagetime = 0LL;

  for(i=0; i<files_open; i++)
  {
    if(((edfheaderlist[i]->datarecords * edfheaderlist[i]->long_data_record_duration) - edfheaderlist[i]->viewtime) > pagetime)
    {
      pagetime = (edfheaderlist[i]->datarecords * edfheaderlist[i]->long_data_record_duration) - edfheaderlist[i]->viewtime;
    }
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_vert_scrollbar_up_act_triggered()
{
  if(!signalcomps)  return;

  if(mc_v_scrollarea_ratio <= 1)  return;

  int val = mc_v_scrollbar->value() - (10000 / mc_v_scrollarea_ratio);

  mc_v_scrollbar->setValue(val);

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::shift_vert_scrollbar_down_act_triggered()
{
  if(!signalcomps)  return;

  if(mc_v_scrollarea_ratio <= 1)  return;

  int val = mc_v_scrollbar->value() + (10000 / mc_v_scrollarea_ratio);

  mc_v_scrollbar->setValue(val);

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::fit_signals_to_pane(int n)
{
  int i,
      pane_size;

  if(!signalcomps)  return;

  if(mc_v_scrollarea_auto && (signalcomps > mc_v_scrollarea_max_signals))
  {
    pane_size = maincurve->height() / (mc_v_scrollarea_max_signals + 1);
  }
  else
  {
    pane_size = maincurve->height() / (signalcomps + 1);
  }

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    if(signalcomp[i]->max_dig_value!=signalcomp[i]->min_dig_value)
    {
      signalcomp[i]->sensitivity = (double)pane_size / (double)(signalcomp[i]->max_dig_value - signalcomp[i]->min_dig_value);
    }
    else
    {
      signalcomp[i]->sensitivity = pane_size;
    }

    signalcomp[i]->voltpercm = signalcomp[i]->edfparam_0->bitvalue / (signalcomp[i]->sensitivity * y_pixelsizefactor);

    signalcomp[i]->screen_offset_pix = ((signalcomp[i]->max_dig_value + signalcomp[i]->min_dig_value) / 2.0) * signalcomp[i]->sensitivity * signalcomp[i]->polarity;

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::fit_signals_dc_offset(int n)
{
  int i;

  if(!signalcomps)  return;

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    signalcomp[i]->screen_offset_pix = ((signalcomp[i]->max_dig_value + signalcomp[i]->min_dig_value) / 2.0) * signalcomp[i]->sensitivity * signalcomp[i]->polarity;

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude(QAction *action)
{
  int i;

  double value=100;

  if(action==amp_00001)
  {
    value = 0.0001;
    amplitude_doubler = 10;
  }
  else if(action==amp_00002)
  {
    value = 0.0002;
    amplitude_doubler = 20;
  }
  else if(action==amp_00005)
  {
    value = 0.0005;
    amplitude_doubler = 50;
  }
  else if(action==amp_0001)
  {
    value = 0.001;
    amplitude_doubler = 10;
  }
  else if(action==amp_0002)
  {
    value = 0.002;
    amplitude_doubler = 20;
  }
  else if(action==amp_0005)
  {
    value = 0.005;
    amplitude_doubler = 50;
  }
  else if(action==amp_001)
  {
    value = 0.01;
    amplitude_doubler = 10;
  }
  else if(action==amp_002)
  {
    value = 0.02;
    amplitude_doubler = 20;
  }
  else if(action==amp_005)
  {
    value = 0.05;
    amplitude_doubler = 50;
  }
  else if(action==amp_01)
  {
    value = 0.1;
    amplitude_doubler = 10;
  }
  else if(action==amp_02)
  {
    value = 0.2;
    amplitude_doubler = 20;
  }
  else if(action==amp_05)
  {
    value = 0.5;
    amplitude_doubler = 50;
  }
  else if(action==amp_1)
  {
    value = 1.0;
    amplitude_doubler = 10;
  }
  else if(action==amp_2)
  {
    value = 2.0;
    amplitude_doubler = 20;
  }
  else if(action==amp_5)
  {
    value = 5.0;
    amplitude_doubler = 50;
  }
  else if(action==amp_10)
  {
    value = 10.0;
    amplitude_doubler = 10;
  }
  else if(action==amp_20)
  {
    value = 20.0;
    amplitude_doubler = 20;
  }
  else if(action==amp_50)
  {
    value = 50.0;
    amplitude_doubler = 50;
  }
  else if(action==amp_100)
  {
    value = 100.0;
    amplitude_doubler = 10;
  }
  else if(action==amp_200)
  {
    value = 200.0;
    amplitude_doubler = 20;
  }
  else if(action==amp_500)
  {
    value = 500.0;
    amplitude_doubler = 50;
  }
  else if(action==amp_1000)
  {
    value = 1000.0;
    amplitude_doubler = 10;
  }
  else if(action==amp_2000)
  {
    value = 2000.0;
    amplitude_doubler = 20;
  }
  else if(action==amp_5000)
  {
    value = 5000.0;
    amplitude_doubler = 50;
  }
  else if(action==amp_10000)
  {
    value = 10000.0;
    amplitude_doubler = 10;
  }
  else if(action==amp_20000)
  {
    value = 20000.0;
    amplitude_doubler = 20;
  }
  else if(action==amp_50000)
  {
    value = 50000.0;
    amplitude_doubler = 50;
  }

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfparam_0->bitvalue < 0.0)
    {
      value *= -1.0;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfparam_0->bitvalue / value) / y_pixelsizefactor;

    signalcomp[i]->screen_offset_pix *= (signalcomp[i]->voltpercm / value);

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;

    signalcomp[i]->voltpercm = value;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude_mult2()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->voltpercm < 0)
    {
      if(signalcomp[i]->voltpercm < -5000000.0)
      {
        return;
      }
    }
    else
    {
      if(signalcomp[i]->voltpercm > 5000000.0)
      {
        return;
      }
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(amplitude_doubler != 20)
    {
      signalcomp[i]->voltpercm *= 2;

      signalcomp[i]->screen_offset_pix /= 2;
    }
    else
    {
      signalcomp[i]->voltpercm *= 2.5;

      signalcomp[i]->screen_offset_pix /= 2.5;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfparam_0->bitvalue / signalcomp[i]->voltpercm) / y_pixelsizefactor;

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  if(amplitude_doubler == 10)
  {
    amplitude_doubler = 20;
  }
  else
  {
    if(amplitude_doubler == 20)
    {
      amplitude_doubler = 50;
    }
    else
    {
      amplitude_doubler = 10;
    }
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude_div2()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->voltpercm < 0)
    {
      if(signalcomp[i]->voltpercm > -0.000001)
      {
        return;
      }
    }
    else
    {
      if(signalcomp[i]->voltpercm < 0.000001)
      {
        return;
      }
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(amplitude_doubler != 50)
    {
      signalcomp[i]->voltpercm /= 2;

      signalcomp[i]->screen_offset_pix *= 2;
    }
    else
    {
      signalcomp[i]->voltpercm /= 2.5;

      signalcomp[i]->screen_offset_pix *= 2.5;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfparam_0->bitvalue / signalcomp[i]->voltpercm) / y_pixelsizefactor;

    signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
  }

  if(amplitude_doubler == 10)
  {
    amplitude_doubler = 50;
  }
  else
  {
    if(amplitude_doubler == 50)
    {
      amplitude_doubler = 20;
    }
    else
    {
      amplitude_doubler = 10;
    }
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::load_predefined_mtg(QAction *action)
{
  int i;

  if(!files_open)
  {
    return;
  }

  for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
  {
    if(action==load_predefined_mtg_act[i])
    {
      if(predefined_mtg_path[i][0] != 0)
      {
        strlcpy(montagepath, &predefined_mtg_path[i][0], MAX_PATH_LENGTH);

        UI_LoadMontagewindow load_mtg(this, montagepath);

        return;
      }
    }
  }
}


void UI_Mainwindow::export_to_ascii()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_AsciiExportwindow exportdialog(this);
}


void UI_Mainwindow::export_ecg_rr_interval_to_ascii()
{
  UI_ECGExport ECG_export(this);
}


void UI_Mainwindow::qrs_detector()
{
  int signal_nr=-2;

  char str1_2048[2048]={""};

  double sf=1;

  if((!files_open) || live_stream_active || (!signalcomps))  return;

  if(signalcomps > 1)
  {
    UI_SignalChooser signal_chooser(this, 4, &signal_nr);
  }
  else
  {
    signal_nr = 0;
  }

  if((signal_nr < 0) || (signal_nr >= signalcomps))  return;

  sf = signalcomp[signal_nr]->edfparam_0->sf_f;
  if(sf < 199.999)
  {
    snprintf(str1_2048, 2048, "Sample rate of selected signal is %.1f Hz."
                        "The QRS detector needs a samplerate of 200 Hz or higher.", sf);
    QMessageBox::critical(this, "Error", str1_2048);
    return;
  }

  strlcpy(str1_2048, signalcomp[signal_nr]->edfparam_0->physdimension, 2048);
  trim_spaces(str1_2048);
  if(strcmp(str1_2048, "uV") && strcmp(str1_2048, "mV") && strcmp(str1_2048, "V"))
  {
    snprintf(str1_2048, 2048, "Unknown unit (physical dimension): \"%s\", expected uV or mV or V",
             signalcomp[signal_nr]->edfparam_0->physdimension);
    QMessageBox::critical(this, "Error", str1_2048);
    return;
  }

  if(signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Z-ratio is active for this signal!");
    return;
  }

  UI_QRS_detector ui_qrs_det(this, signalcomp[signal_nr]);

  enable_hrv_stats_toolbar(ecg_qrs_rpeak_descr, &signalcomp[signal_nr]->edfhdr->annot_list, signalcomp[signal_nr]);
}


void UI_Mainwindow::export_filtered_signals()
{
  UI_ExportFilteredSignalsWindow filt_signalswdw(this);
}


void UI_Mainwindow::export_annotations()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_ExportAnnotationswindow exportAnnotsDialog(this);
}


void UI_Mainwindow::import_annotations()
{
  UI_ImportAnnotationswindow importAnnotsDialog(this);
}


void UI_Mainwindow::check_edf_compatibility()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open the file first.");
    return;
  }

  UI_EDFCompatwindow edfcompatdialog(this);
}


void UI_Mainwindow::unify_resolution()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_unify_resolution unify_resolution_dialog(this);
}


void UI_Mainwindow::print_to_img_640x480()
{
  maincurve->print_to_image(640, 480, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_800x600()
{
  maincurve->print_to_image(800, 600, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_1024x768()
{
  maincurve->print_to_image(1024, 768, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_1280x1024()
{
  maincurve->print_to_image(1280, 1024, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_1600x1200()
{
  maincurve->print_to_image(1600, 1200, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_1920x1080()
{
  maincurve->print_to_image(1920, 1080, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_2560x1440()
{
  maincurve->print_to_image(2560, 1440, PRINT_FORMAT_IMG);
}


void UI_Mainwindow::print_to_img_3840x2160()
{
  maincurve->print_to_image(3840, 2160, PRINT_FORMAT_IMG);
}


long long UI_Mainwindow::get_long_time(char *str)
{
  int i, len, hasdot=0, dotposition=0, neg=0;

  long long value=0, radix;

  if(str[0] == '+')
  {
    str++;
  }
  else if(str[0] == '-')
    {
      neg = 1;
      str++;
    }

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      hasdot = 1;
      dotposition = i;
      break;
    }
  }

  if(hasdot)
  {
    radix = TIME_FIXP_SCALING;

    for(i=dotposition-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }

    radix = TIME_FIXP_SCALING / 10;

    for(i=dotposition+1; i<len; i++)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix /= 10;
    }
  }
  else
  {
    radix = TIME_FIXP_SCALING;

    for(i=len-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  if(neg)  value = -value;

  return value;
}


void UI_Mainwindow::recent_file_action_func(QAction *action)
{
  int i, idx=-1;

  idx = action->data().toInt();

  if(idx == MAX_RECENTFILES)
  {
    recent_filesmenu->clear();

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      recent_file_path[i][0] = 0;
    }

    return;
  }

  if(idx < MAX_RECENTFILES)
  {
    strlcpy(path, &recent_file_path[idx][0], MAX_PATH_LENGTH);

    cmdlineargument = 1;

    open_new_file();
  }
}


void UI_Mainwindow::recent_session_action_func(QAction *action)
{
  int i, button_nr=0, err, present=0, position=0, idx=-1;

  char session_path_tmp[MAX_PATH_LENGTH]="";

  QAction *act=NULL;

  idx = action->data().toInt();

  if(idx == MAX_RECENTFILES)
  {
    recent_session_menu->clear();

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      recent_session_path[i][0] = 0;
    }

    return;
  }

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }
  else if(files_open)
    {
      QMessageBox messagewindow;
      messagewindow.setText("This will close all files, continue?");
      messagewindow.setIcon(QMessageBox::Question);
      messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Close);
      messagewindow.setDefaultButton(QMessageBox::Cancel);
      button_nr = messagewindow.exec();
    }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  annotations_edited = 0;

  close_all_files();

  strlcpy(session_path_tmp, &recent_session_path[idx][0], MAX_PATH_LENGTH);
  if(!strcmp(session_path_tmp, ""))
  {
    return;
  }

  get_directory_from_path(recent_sessiondir, session_path_tmp, MAX_PATH_LENGTH);

  for(i=0, present=0; i<MAX_RECENTFILES; i++)
  {
//    printf("mainwindow.cpp: load_session(): recent_session_path[i]: ->%s<-\nsession_path_tmp: ->%s<-\n", recent_session_path[i], session_path_tmp);

    if(!strcmp(&recent_session_path[i][0], session_path_tmp))
    {
      present = 1;

      position = i;

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_session_path[0][0], session_path_tmp, MAX_PATH_LENGTH);

  recent_session_menu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_session_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
    act->setData(QVariant(i));
    recent_session_menu->addAction(act);
  }
  recent_session_menu->addSeparator();
  act = new QAction("Clear list", recent_session_menu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_session_menu->addAction(act);

  err = read_session_file(session_path_tmp);

  if(err)
  {
    printf("load_session() returned error: %i\n", err);
  }
}


void UI_Mainwindow::save_montage()
{
  if(!files_open)  return;

  if(!signalcomps)  return;

  UI_SaveMontagewindow save_mtg(this);
}


void UI_Mainwindow::load_montage()
{
  if(!files_open)  return;

  UI_LoadMontagewindow load_mtg(this);
}


void UI_Mainwindow::view_montage()
{
  UI_ViewMontagewindow view_mtg(this);
}


void UI_Mainwindow::show_this_montage()
{
  UI_ShowActualMontagewindow show_mtg(this);
}


void UI_Mainwindow::view_session()
{
  UI_ViewSessionwindow view_ses(this);
}


void UI_Mainwindow::print_to_edf()
{
  print_screen_to_edf(this);
}


void UI_Mainwindow::print_to_bdf()
{
  print_screen_to_bdf(this);
}


void UI_Mainwindow::set_dc_offset_to_zero(int n)
{
  int i;

  if(!signalcomps)
  {
    return;
  }

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    signalcomp[i]->screen_offset_pix = 0.0;

    signalcomp[i]->screen_offset_unit = 0.0;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::show_help()
{
  open_manual("");
}


void UI_Mainwindow::show_kb_shortcuts()
{
  const char str1[]={
    "<table>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Next page</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>PgDn</td> </tr>\n"
    "<tr> <td>Former page</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>PgUp</td> </tr>\n"
    "<tr> <td>Shift right one tenth of page</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Right arrow</td> </tr>\n"
    "<tr> <td>Shift left one tenth of page</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Left arrow</td> </tr>\n"
    "<tr> <td>Shift left or right</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Mousewheel</td> </tr>\n"
    "<tr> <td>Zoom in or out horizontally</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Mousewheel</td> </tr>\n"
    "<tr> <td>Zoom in horizontally</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Plus</td> </tr>\n"
    "<tr> <td>Zoom out horizontally</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Minus</td> </tr>\n"
    "<tr> <td>Drag horizontally</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Keep middle mousebutton pressed</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Increase amplitude</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Plus</td> </tr>\n"
    "<tr> <td>Decrease amplitude</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Minus</td> </tr>\n"
    "<tr> <td>Shift traces up</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Up arrow</td> </tr>\n"
    "<tr> <td>Shift traces down</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Down arrow</td> </tr>\n"
    "<tr> <td>Move vertical scrollbar</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Shift + Up Arrow</td> </tr>\n"
    "<tr> <td>Move vertical scrollbar</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Shift + Down Arrow</td> </tr>\n"
    "<tr> <td>Go to start of file</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Home</td> </tr>\n"
    "<tr> <td>Go to end of file</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + End</td> </tr>\n"
    "<tr> <td>Load predefined montage</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>F1 - F8</td> </tr>\n"
    "<tr> <td>Crosshair (repeat for second crosshair)</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Alt + Shift + C</td> </tr>\n"
    "<tr> <td>Remove crosshairs and floating ruler</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Esc</td> </tr>\n"
    "<tr> <td>Toggle annotation markers</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + M</td> </tr>\n"
    "<tr> <td>Jump to next annotation</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + N</td> </tr>\n"
    "<tr> <td>Jump to previous annotation</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + B</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>After zooming in by drawing a rectangle:</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Zoom back</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Backspace</td> </tr>\n"
    "<tr> <td>Zoom in</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Insert</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Open a file</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + O</td> </tr>\n"
    "<tr> <td>Open a stream</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Shift + O</td> </tr>\n"
#ifdef Q_OS_WIN32
    "<tr> <td>Close all files</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + F4</td> </tr>\n"
    "<tr> <td>Exit program</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Alt + F4</td> </tr>\n"
#else
    "<tr> <td>Close all files</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl+W</td> </tr>\n"
    "<tr> <td>Exit program</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Q</td> </tr>\n"
#endif
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Toggle Playback or Pause</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Space</td> </tr>\n"
    "<tr> <td>Toggle Playback with video</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + Shift + V</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Annotation editor:</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Toggle annotation editor</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + E</td> </tr>\n"
    "<tr> <td>User buttons</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Press 1, 2, 3, etc.</td> </tr>\n"
    "<tr> <td>Create annotation</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Shift + draw rectangle</td> </tr>\n"
    "<tr> <td>Create signal linked annotation</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + draw rectangle</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Annotation editor + crosshair(s) active:</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>Create annotation</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>A</td> </tr>\n"
    "<tr> <td>Create annotation with amplitude difference</td> <td>&nbsp;&nbsp;:&nbsp;&nbsp;</td> <td>Ctrl + A</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "<tr> <td>&nbsp;</td> <td>&nbsp;</td> <td>&nbsp;</td> </tr>\n"
    "</table>\n"};

  tmessagebox(QMessageBox::Information, "Keyboard Shortcuts", "Hotkeys", str1, this);
}

#ifdef Q_OS_WIN32

QString UI_Mainwindow::specialFolder(int type)
{
    QString result;

    QLibrary library(QLatin1String("shell32"));
    QT_WA( {
        typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, LPTSTR, int, BOOL);
        GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve("SHGetSpecialFolderPathW");
        if (SHGetSpecialFolderPath) {
            TCHAR path_w[MAX_PATH];
            SHGetSpecialFolderPath(0, path_w, type, false);
            result = QString::fromUtf16((ushort*)path_w);
        }
    } , {
        typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, char*, int, BOOL);
        GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve("SHGetSpecialFolderPathA");
        if (SHGetSpecialFolderPath) {
            char path_w[MAX_PATH];
            SHGetSpecialFolderPath(0, path_w, type, false);
            result = QString::fromLocal8Bit(path_w);
        }
    } );

    if (result.isEmpty()) {
         result = QLatin1String("C:\\temp");
    }

    return result;
}

#endif


int UI_Mainwindow::file_is_opened(const char *o_path)
{
  int i;


  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, o_path))
    {
      return 1;
    }
  }

  return 0;
}


void UI_Mainwindow::organize_signals()
{
  UI_SignalChooser signalchooser(this, 3);
}


void UI_Mainwindow::edfplus_remove_duplicate_annotations()
{
  int i, dup_cnt=0, cnt;

  char str1_256[256];

  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  QProgressDialog progress("Checking for duplicates...", "Abort", 0, 10, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  for(i=0; i<files_open; i++)
  {
    cnt = edfplus_annotation_remove_duplicates(&edfheaderlist[i]->annot_list);

    dup_cnt += cnt;
  }

  progress.reset();

  if(dup_cnt)
  {
    for(i=0; i<files_open; i++)
    {
      if(annotations_dock[i] != NULL)
      {
        annotations_dock[i]->updateList(0);
      }
    }

    annotations_edited = 1;

    save_act->setEnabled(true);
  }

  snprintf(str1_256, 256, "Removed %i duplicates from the annotationlist(s)", dup_cnt);
  QMessageBox::information(this, "Ready", str1_256);
}


sigcompblck_t * UI_Mainwindow::create_signalcomp_copy(sigcompblck_t *original_signalcomp)
{
  int i;

  char spec_str_256[256],
       *filter_spec=NULL,
       *err=NULL;

  sigcompblck_t *newsignalcomp;

  if(signalcomps >= MAXSIGNALS)
  {
    return NULL;
  }

  if(original_signalcomp->ecg_filter != NULL)
  {
    return NULL;
  }

  if(original_signalcomp->zratio_filter != NULL)
  {
    return NULL;
  }

  newsignalcomp = (sigcompblck_t *)calloc(1, sizeof(sigcompblck_t));
  if(newsignalcomp == NULL)
  {
    QMessageBox::critical(this, "Error", "malloc() error");
    return NULL;
  }

  *newsignalcomp = *original_signalcomp;

  if(newsignalcomp->spike_filter)
  {
    newsignalcomp->spike_filter = create_spike_filter_copy(original_signalcomp->spike_filter);
    if(newsignalcomp->spike_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  if(newsignalcomp->plif_ecg_filter)
  {
    newsignalcomp->plif_ecg_filter = plif_ecg_subtract_filter_create_copy(original_signalcomp->plif_ecg_filter);
    if(newsignalcomp->plif_ecg_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  newsignalcomp->plif_ecg_filter_sav = NULL;

  if(newsignalcomp->plif_eeg_filter)
  {
    newsignalcomp->plif_eeg_filter = plif_eeg_subtract_filter_create_copy(original_signalcomp->plif_eeg_filter);
    if(newsignalcomp->plif_eeg_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  newsignalcomp->plif_eeg_filter_sav = NULL;

  for(i=0; i<newsignalcomp->filter_cnt; i++)
  {
    newsignalcomp->filter[i] = create_filter_copy(original_signalcomp->filter[i]);
    if(newsignalcomp->filter[i] == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  for(i=0; i<newsignalcomp->ravg_filter_cnt; i++)
  {
    newsignalcomp->ravg_filter[i] = create_ravg_filter_copy(original_signalcomp->ravg_filter[i]);
    if(newsignalcomp->ravg_filter[i] == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  for(i=0; i<newsignalcomp->fir_filter_cnt; i++)
  {
    newsignalcomp->fir_filter[i] = create_fir_filter_copy(original_signalcomp->fir_filter[i]);
    if(newsignalcomp->fir_filter[i] == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }
  }

  for(i=0; i<newsignalcomp->fidfilter_cnt; i++)
  {
    if(newsignalcomp->fidfilter_type[i] == 0)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str_256, 256, "HpBu%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str_256, 256, "HpCh%i/%f/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str_256, 256, "HpBe%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 1)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str_256, 256, "LpBu%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str_256, 256, "LpCh%i/%f/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str_256, 256, "LpBe%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 2)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str_256, 256, "BsRe/%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 3)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str_256, 256, "BpBu%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str_256, 256, "BpCh%i/%f/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str_256, 256, "BpBe%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 4)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str_256, 256, "BsBu%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str_256, 256, "BsCh%i/%f/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str_256, 256, "BsBe%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }
    }

    filter_spec = spec_str_256;

    err = fid_parse(newsignalcomp->edfparam_0->sf_f, &filter_spec, &newsignalcomp->fidfilter[i]);

    if(err != NULL)
    {
      QMessageBox::critical(this, "Error", err);
      free(err);
      err = NULL;
      free(newsignalcomp);
      newsignalcomp = NULL;
      return NULL;
    }

    newsignalcomp->fid_run[i] = fid_run_new(newsignalcomp->fidfilter[i], &newsignalcomp->fidfuncp[i]);

    newsignalcomp->fidbuf[i] = fid_run_newbuf(newsignalcomp->fid_run[i]);
    newsignalcomp->fidbuf2[i] = fid_run_newbuf(newsignalcomp->fid_run[i]);

//    memcpy(newsignalcomp->fidbuf[i], original_signalcomp->fidbuf[i], fid_run_bufsize(original_signalcomp->fid_run[i]));
//    memcpy(newsignalcomp->fidbuf2[i], original_signalcomp->fidbuf2[i], fid_run_bufsize(original_signalcomp->fid_run[i]));
  }

  signalcomp[signalcomps] = newsignalcomp;

  signalcomps++;

  return newsignalcomp;
}


void UI_Mainwindow::annot_dock_updated(void)
{
  emit annot_docklist_changed();
}


int UI_Mainwindow::get_filenum(edfhdrblck_t *ptr)
{
  int i;

  for(i=0; i<files_open; i++)
  {
    if(ptr == edfheaderlist[i])
    {
      return i;
    }
  }

  return -1;
}


void UI_Mainwindow::enable_hrv_stats_toolbar(const char *annotation, annotlist_t *annot_list, sigcompblck_t *sigcomp)
{
  char annot_descr[MAX_ANNOTATION_LEN]="";

  if(annotation == NULL)  return;

  strlcpy(annot_descr, annotation, MAX_ANNOTATION_LEN);

  if(sigcomp != NULL)
  {
    if(use_signallabel_in_annot_descr)
    {
      strlcat(annot_descr, " ", MAX_ANNOTATION_LEN);
      if(strlen(sigcomp->alias))
      {
        strlcat(annot_descr, sigcomp->alias, MAX_ANNOTATION_LEN);
      }
      else
      {
        strlcat(annot_descr, sigcomp->signallabel, MAX_ANNOTATION_LEN);
      }
    }
  }

  strlcpy(toolbar_stats.annot_label, annot_descr, MAX_ANNOTATION_LEN);

  toolbar_stats.annot_list = annot_list;

  toolbar_stats.sz = 0;

  toolbar_stats.active = 1;
}


void UI_Mainwindow::get_unique_annotations(edfhdrblck_t *hdr)
{
  int i, j, listsz;

  annotblck_t *annot;

  if(hdr == NULL)  return;

  hdr->unique_annotations_list[0][0] = 0;

  listsz = edfplus_annotation_size(&hdr->annot_list);

  for(i=0; i<listsz; i++)
  {
    annot = edfplus_annotation_get_item(&hdr->annot_list, i);

    for(j=0; j<MAX_UNIQUE_ANNOTATIONS; j++)
    {
      if(hdr->unique_annotations_list[j][0] == 0)
      {
        strlcpy(hdr->unique_annotations_list[j], annot->description, MAX_ANNOTATION_LEN);

        if(j < (MAX_UNIQUE_ANNOTATIONS - 1))
        {
          hdr->unique_annotations_list[j+1][0] = 0;
        }

        break;
      }

      if(!strcmp(annot->description, hdr->unique_annotations_list[j]))  break;
    }
  }
}


void UI_Mainwindow::create_annot_by_rect_draw()
{
  int i;

  if((!annot_editor_active) || (annotationEditDock == NULL))
  {
    return;
  }

  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    if(strlen(annot_by_rect_draw_description[i]))
    {
      break;
    }
  }
  if(i == MAX_ANNOTEDIT_SIDE_MENU_ANNOTS)
  {
    return;
  }

  annotationEditDock->process_annot_by_rect_draw();
}


void UI_Mainwindow::dig_min_max_overflow_timer_func()
{
  if(dig_min_max_overflow && (!dig_min_max_overflow_warning_showed))
  {
    dig_min_max_overflow_warning_showed = 1;

    QMessageBox::warning(this, "Warning",
                        "Detected some samples that have a value higher than the maximum specified in the EDF header and/or samples that have a value lower than the minimum specified in the EDF header.\n"
                        "Please check your EDF file using: Tools -> Check EDF/BDF compatibility\n"
                        "You can continue browsing this EDF file but all samples outside the maximum - minimum range will be clipped."
    );
  }
}


void UI_Mainwindow::remove_signalcomp(int signal_nr)
{
  int i, j, p;

  if((signal_nr < 0) || (signal_nr >= signalcomps))
  {
    return;
  }

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(spectrumdock[i]->signalcomp == signalcomp[signal_nr])
    {
      spectrumdock[i]->clear();
      spectrumdock[i]->dock->hide();
    }
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->spectr_dialog[i];

    if(p != 0)
    {
      delete spectrumdialog[p - 1];

      spectrumdialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    p = signalcomp[signal_nr]->cdsa_idx[i];

    if(p != 0)
    {
      delete cdsa_dock[p - 1];

      cdsa_dock[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    p = signalcomp[signal_nr]->aeeg_idx[i];

    if(p != 0)
    {
      delete aeeg_dock[p - 1];

      aeeg_dock[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->avg_dialog[i];

    if(p != 0)
    {
      delete averagecurvedialog[p - 1];

      averagecurvedialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->zscoredialog[i];

    if(p != 0)
    {
      delete zscoredialog[p - 1];

      zscoredialog[p - 1] = NULL;
    }
  }

  if(signalcomp[signal_nr]->hascursor2)
  {
    maincurve->crosshair_2.active = 0;
    maincurve->crosshair_2.moving = 0;
  }

  if(signalcomp[signal_nr]->hascursor1)
  {
    maincurve->crosshair_1.active = 0;
    maincurve->crosshair_2.active = 0;
    maincurve->crosshair_1.moving = 0;
    maincurve->crosshair_2.moving = 0;

    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->hascursor2 = 0;
    }
  }

  if(signalcomp[signal_nr]->hasruler)
  {
    maincurve->ruler_active = 0;
    maincurve->ruler_moving = 0;
  }

  for(j=0; j<signalcomp[signal_nr]->math_func_cnt_before; j++)
  {
    free_math_func(signalcomp[signal_nr]->math_func_before[j]);
  }

  signalcomp[signal_nr]->math_func_cnt_before = 0;

  for(j=0; j<signalcomp[signal_nr]->math_func_cnt_after; j++)
  {
    free_math_func(signalcomp[signal_nr]->math_func_after[j]);
  }

  signalcomp[signal_nr]->math_func_cnt_after = 0;

  for(j=0; j<signalcomp[signal_nr]->filter_cnt; j++)
  {
    free(signalcomp[signal_nr]->filter[j]);
  }

  signalcomp[signal_nr]->filter_cnt = 0;

  if(signalcomp[signal_nr]->plif_ecg_filter)
  {
    plif_ecg_free_subtract_filter(signalcomp[signal_nr]->plif_ecg_filter);

    signalcomp[signal_nr]->plif_ecg_filter = NULL;
  }

  if(signalcomp[signal_nr]->plif_ecg_filter_sav)
  {
    plif_ecg_free_subtract_filter(signalcomp[signal_nr]->plif_ecg_filter_sav);

    signalcomp[signal_nr]->plif_ecg_filter_sav = NULL;
  }

  if(signalcomp[signal_nr]->plif_eeg_filter)
  {
    plif_eeg_free_subtract_filter(signalcomp[signal_nr]->plif_eeg_filter);

    signalcomp[signal_nr]->plif_eeg_filter = NULL;
  }

  if(signalcomp[signal_nr]->plif_eeg_filter_sav)
  {
    plif_eeg_free_subtract_filter(signalcomp[signal_nr]->plif_eeg_filter_sav);

    signalcomp[signal_nr]->plif_eeg_filter_sav = NULL;
  }

  if(signalcomp[signal_nr]->spike_filter)
  {
    free_spike_filter(signalcomp[signal_nr]->spike_filter);

    signalcomp[signal_nr]->spike_filter = NULL;
  }

  for(j=0; j<signalcomp[signal_nr]->ravg_filter_cnt; j++)
  {
    free_ravg_filter(signalcomp[signal_nr]->ravg_filter[j]);
  }

  signalcomp[signal_nr]->ravg_filter_cnt = 0;

  for(j=0; j<signalcomp[signal_nr]->fir_filter_cnt; j++)
  {
    free_fir_filter(signalcomp[signal_nr]->fir_filter[j]);

    signalcomp[signal_nr]->fir_filter[j] = NULL;
  }

  signalcomp[signal_nr]->fir_filter_cnt = 0;

  if(signalcomp[signal_nr]->ecg_filter != NULL)
  {
    free_ecg_filter(signalcomp[signal_nr]->ecg_filter);

    signalcomp[signal_nr]->ecg_filter = NULL;

    strlcpy(signalcomp[signal_nr]->signallabel, signalcomp[signal_nr]->signallabel_bu, 512);
    signalcomp[signal_nr]->signallabellen = signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(signalcomp[signal_nr]->physdimension, signalcomp[signal_nr]->physdimension_bu, 9);
  }

  if(signalcomp[signal_nr]->zratio_filter != NULL)
  {
    free_zratio_filter(signalcomp[signal_nr]->zratio_filter);

    signalcomp[signal_nr]->zratio_filter = NULL;

    strlcpy(signalcomp[signal_nr]->signallabel, signalcomp[signal_nr]->signallabel_bu, 512);
    signalcomp[signal_nr]->signallabellen = signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(signalcomp[signal_nr]->physdimension, signalcomp[signal_nr]->physdimension_bu, 9);
  }

  for(j=0; j<signalcomp[signal_nr]->fidfilter_cnt; j++)
  {
    free(signalcomp[signal_nr]->fidfilter[j]);
    fid_run_free(signalcomp[signal_nr]->fid_run[j]);
    fid_run_freebuf(signalcomp[signal_nr]->fidbuf[j]);
    fid_run_freebuf(signalcomp[signal_nr]->fidbuf2[j]);
  }

  signalcomp[signal_nr]->fidfilter_cnt = 0;

  free(signalcomp[signal_nr]);

  signalcomp[signal_nr] = NULL;

  for(i=signal_nr; i<signalcomps - 1; i++)
  {
    signalcomp[i] = signalcomp[i + 1];
  }

  signalcomps--;

  if(!signalcomps)
  {
    free(viewbuf);
    viewbuf = NULL;

    slidertoolbar->setEnabled(false);
    positionslider->blockSignals(true);
  }
}


int UI_Mainwindow::get_signalcomp_number(const char *label)
{
  int i;

  char str1[512]="",
       str2[512]="";

  strlcpy(str1, label, 512);
  if(strip_label_types)
  {
    strip_types_from_label(str1);
  }
  trim_spaces(str1);

  for(i=0; i<signalcomps; i++)
  {
    strlcpy(str2, signalcomp[i]->signallabel, 512);
    trim_spaces(str2);
    if(!strcmp(str2, str1))
    {
      return i;
    }
  }

  return -1;
}

/* inv: 0 - not inverted
 * inv: 1 - inverted
 * inv: 2 - toggle
 *
 * n: default: -1: apply to all signals
 * n: >= 0: apply one signal only
 */
void UI_Mainwindow::signalcomp_invert(int inv, int n)
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))  continue;

    if((inv == 0) && (signalcomp[i]->polarity == -1))  /* not inverted */
    {
      signalcomp[i]->polarity *= -1;

      signalcomp[i]->screen_offset_pix *= -1;

      signalcomp[i]->screen_offset_unit *= -1;
    }
    else if((inv == 1) && (signalcomp[i]->polarity == 1))  /* inverted */
      {
        signalcomp[i]->polarity *= -1;

        signalcomp[i]->screen_offset_pix *= -1;

        signalcomp[i]->screen_offset_unit *= -1;
      }
      else if(inv == 2)  /* toggle */
      {
        signalcomp[i]->polarity *= -1;

        signalcomp[i]->screen_offset_pix *= -1;

        signalcomp[i]->screen_offset_unit *= -1;
      }
  }

  setup_viewbuf();
}


void UI_Mainwindow::desktop_resized()
{
  set_font_metrics(0);

  setup_viewbuf();
}


void UI_Mainwindow::set_font_metrics(int starting_up)
{
  int i, dpiy_old;

  double dpi_ratio;

  QFontMetrics fm(*normfont);

  norm_font_pixel_height = fm.height();

  h_scaling = fm.height() / 18.0;

  w_scaling = fm.tightBoundingRect("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").width() / 260.0;

  norm_font_pixel_width = w_scaling * 7.03;

//   printf("\nw_scaling: %.2f    h_scaling: %.2f\n", w_scaling, h_scaling);
//
//   printf("\nnorm_font_pixel_height: %.2f    norm_font_pixel_width: %.2f\n", norm_font_pixel_height, norm_font_pixel_width);

//////////////////////////////// START MONO-FONT /////////////////////////////////////////////

  QFontMetrics fm_mono(*monofont);

  mono_font_pixel_height = fm_mono.height();

  mono_font_pixel_width = fm_mono.tightBoundingRect("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").width() / 35.0;

//  printf("\nmono_font_pixel_height: %.2f    mono_font_pixel_width: %.2f\n", mono_font_pixel_height, mono_font_pixel_width);

//////////////////////////////// END MONO-FONT /////////////////////////////////////////////

  if(!starting_up)
  {
    dpiy_old = dpiy;

    dpix = maincurve->physicalDpiX();

    dpiy = maincurve->physicalDpiY();

//    printf("dpix: %i    dpiy: %i\n", dpix, dpiy);

    if(auto_dpi)
    {
      y_pixelsizefactor = 2.54 / dpiy;

      x_pixelsizefactor = 2.54 / dpix;
    }

    if(dpiy != dpiy_old)
    {
      dpi_ratio = (double)dpiy / (double)dpiy_old;

      for(i=0; i<signalcomps; i++)
      {
        signalcomp[i]->screen_offset_pix = dpi_ratio * signalcomp[i]->screen_offset_pix;

        signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * y_pixelsizefactor * signalcomp[i]->voltpercm;
      }
    }
  }
}


void UI_Mainwindow::mc_v_scrollbar_changed(int val)
{
  maincurve->set_vertical_scroll_value(val);

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_view_markers(bool checked)
{
  if(checked == true)
  {
    show_annot_markers = 1;
  }
  else
  {
    show_annot_markers = 0;
  }

  maincurve->update();
}


void UI_Mainwindow::set_vert_scrollbar(bool checked)
{
  if(checked == true)
  {
    mc_v_scrollarea_auto = 1;

    mc_v_scrollbar->setVisible(true);
  }
  else
  {
    mc_v_scrollarea_auto = 0;

    mc_v_scrollbar->setVisible(false);
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_linear_interpol(bool checked)
{
  if(checked == true)
  {
    linear_interpol = 1;
  }
  else
  {
    linear_interpol = 0;
  }

  setup_viewbuf();
}


void UI_Mainwindow::open_manual(const char *sect)
{
#ifdef Q_OS_LINUX
  char p_path[MAX_PATH_LENGTH]={""};
  snprintf(p_path, MAX_PATH_LENGTH, "file:///usr/share/doc/edfbrowser/manual.html%s", sect);
  QDesktopServices::openUrl(QUrl(p_path));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH]={""};
  strlcpy(p_path, "file:///", MAX_PATH_LENGTH);
  strlcat(p_path, specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(p_path, "\\EDFbrowser\\manual.html", MAX_PATH_LENGTH);
  strlcat(p_path, sect, MAX_PATH_LENGTH);
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}


void UI_Mainwindow::open_edf_file(const QString &file_path)
{
    if(file_path.isEmpty()) return;

    // Convert QString to char array
    QByteArray ba = file_path.toLocal8Bit();
    const char *file_path_str = ba.constData();

    // First check if we need to close any open files
    if(files_open)
    {
        if(annotations_edited)
        {
            QMessageBox messagewindow;
            messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
            messagewindow.setIcon(QMessageBox::Question);
            messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
            messagewindow.setDefaultButton(QMessageBox::Cancel);
            if(messagewindow.exec() == QMessageBox::Cancel) {
                return;
            }
        }
        close_all_files();
    }

    // Set path for opening
    strlcpy(path, file_path_str, MAX_PATH_LENGTH);

    // Set directory
    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

    // Open the file
    open_new_file();
}










