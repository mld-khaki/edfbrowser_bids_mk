/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 - 2024 Teunis van Beelen
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


void UI_Mainwindow::save_session()
{
  int i, j, k, r, hdr_idx=0, sigcomp_idx=0, use_index=0, present=0, position=0, button_nr=0;

  QAction *act=NULL;

  if(!files_open)
  {
    save_session_act->setEnabled(false);
    return;
  }

  char session_path_tmp[MAX_PATH_LENGTH]="",
       path_relative[MAX_PATH_LENGTH]="";

  FILE *pro_file=NULL;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations which will not be stored in the session.\n"
                          "It's strongly recommended you save them first.");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  strlcpy(session_path_tmp, recent_sessiondir, MAX_PATH_LENGTH);
  strlcat(session_path_tmp, "/my_session.esf", MAX_PATH_LENGTH);

  strlcpy(session_path_tmp, QFileDialog::getSaveFileName(0, "Save session", QString::fromLocal8Bit(session_path_tmp), "Session files (*.esf *.ESF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(session_path_tmp, ""))
  {
    return;
  }

  if(strlen(session_path_tmp) > 4)
  {
    if(strcmp(session_path_tmp + strlen(session_path_tmp) - 4, ".esf"))
    {
      strlcat(session_path_tmp, ".esf", MAX_PATH_LENGTH);
    }
  }

  get_directory_from_path(recent_sessiondir, session_path_tmp, MAX_PATH_LENGTH);

  pro_file = fopeno(session_path_tmp, "wb");
  if(pro_file==NULL)
  {
    QMessageBox::critical(this, "Error", "Cannot create session file for writing.");
    return;
  }

  fprintf(pro_file, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_session>\n");

  fprintf(pro_file, "  <relative_path>%i</relative_path>\n", session_relative_paths);

  fprintf(pro_file, "  <edf_files>\n");
  for(i=0; i<files_open; i++)
  {
    fprintf(pro_file, "    <file>");
    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path_tmp, edfheaderlist[i]->filename, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
//      printf("src1: ->%s<-\nsrc2: ->%s<-\ndest: ->%s<-\n", session_path_tmp, edfheaderlist[i]->filename, path_relative);
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, edfheaderlist[i]->filename);
    }
    fprintf(pro_file, "</file>\n");
  }
  for(i=0; i<files_open; i++)
  {
#ifdef Q_OS_WIN32
    __mingw_fprintf(pro_file, "    <viewtime>%lli</viewtime>\n", edfheaderlist[i]->viewtime);
#else
    fprintf(pro_file, "    <viewtime>%lli</viewtime>\n", edfheaderlist[i]->viewtime);
#endif
  }
  fprintf(pro_file, "  </edf_files>\n");

  fprintf(pro_file, "  <ref_file>%i</ref_file>\n", sel_viewtime);

  fprintf(pro_file, "  <timesync_mode>%i</timesync_mode>\n", viewtime_sync);

#ifdef Q_OS_WIN32
  __mingw_fprintf(pro_file, "  <pagetime>%lli</pagetime>\n", pagetime);
#else
  fprintf(pro_file, "  <pagetime>%lli</pagetime>\n", pagetime);
#endif

  fprintf(pro_file, "  <ecg_view_mode>%i</ecg_view_mode>\n", ecg_view_mode);

  for(i=0; i<signalcomps; i++)
  {
    fprintf(pro_file, "  <signalcomposition>\n");

    fprintf(pro_file, "    <file>");

    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path_tmp, signalcomp[i]->edfhdr->filename, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
//      printf("src1: ->%s<-\nsrc2: ->%s<-\ndest: ->%s<-\n", session_path_tmp, signalcomp[i]->edfhdr->filename, path_relative);
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, signalcomp[i]->edfhdr->filename);
    }

    fprintf(pro_file, "</file>\n");

    fprintf(pro_file, "    <alias>");

    xml_fwrite_encode_entity(pro_file, signalcomp[i]->alias);

    fprintf(pro_file, "</alias>\n");

    fprintf(pro_file, "    <num_of_signals>%i</num_of_signals>\n", signalcomp[i]->num_of_signals);

    fprintf(pro_file, "    <voltpercm>%e</voltpercm>\n", signalcomp[i]->voltpercm);

    fprintf(pro_file, "    <screen_offset>%e</screen_offset>\n", signalcomp[i]->screen_offset_pix);

    fprintf(pro_file, "    <screen_offset_unit>%e</screen_offset_unit>\n", signalcomp[i]->screen_offset_unit);

    fprintf(pro_file, "    <polarity>%i</polarity>\n", signalcomp[i]->polarity);

    fprintf(pro_file, "    <color>%i</color>\n", signalcomp[i]->color);

    if(signalcomp[i]->spike_filter)
    {
      fprintf(pro_file, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 1);
    }
    else
    {
      fprintf(pro_file, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 0);
    }

    fprintf(pro_file, "    <math_func_cnt_before>%i</math_func_cnt_before>\n", signalcomp[i]->math_func_cnt_before);

    fprintf(pro_file, "    <math_func_cnt_after>%i</math_func_cnt_after>\n", signalcomp[i]->math_func_cnt_after);

    fprintf(pro_file, "    <filter_cnt>%i</filter_cnt>\n", signalcomp[i]->filter_cnt);

    fprintf(pro_file, "    <fidfilter_cnt>%i</fidfilter_cnt>\n", signalcomp[i]->fidfilter_cnt);

    fprintf(pro_file, "    <ravg_filter_cnt>%i</ravg_filter_cnt>\n", signalcomp[i]->ravg_filter_cnt);

    fprintf(pro_file, "    <fir_filter_cnt>%i</fir_filter_cnt>\n", signalcomp[i]->fir_filter_cnt);

    for(j=0; j<signalcomp[i]->num_of_signals; j++)
    {
      fprintf(pro_file, "    <signal>\n");

      if(use_index)
      {
        fprintf(pro_file, "      <edfindex>%i</edfindex>\n", signalcomp[i]->edfsignal[j]);
      }
      else
      {
        fprintf(pro_file, "      <label>");

        xml_fwrite_encode_entity(pro_file, signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].label);

        fprintf(pro_file, "</label>\n");
      }

      fprintf(pro_file, "      <factor>%e</factor>\n", signalcomp[i]->factor[j]);

      fprintf(pro_file, "    </signal>\n");
    }

    if(signalcomp[i]->spike_filter)
    {
      fprintf(pro_file, "    <spike_filter>\n");

      fprintf(pro_file, "      <velocity>%.16f</velocity>\n", signalcomp[i]->spike_filter_velocity);

      fprintf(pro_file, "      <holdoff>%i</holdoff>\n", signalcomp[i]->spike_filter_holdoff);

      fprintf(pro_file, "    </spike_filter>\n");
    }

    for(j=0; j<signalcomp[i]->math_func_cnt_before; j++)
    {
      fprintf(pro_file, "    <math_func_before>\n");

      fprintf(pro_file, "      <func>%i</func>\n", signalcomp[i]->math_func_before[j]->func);

      fprintf(pro_file, "      <pk_hold_smpls>%i</pk_hold_smpls>\n", signalcomp[i]->math_func_before[j]->pk_hold_smpls_set);

      fprintf(pro_file, "    </math_func_before>\n");
    }

    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      fprintf(pro_file, "    <filter>\n");

      fprintf(pro_file, "      <LPF>%i</LPF>\n", signalcomp[i]->filter[j]->is_LPF);

      fprintf(pro_file, "      <frequency>%.16f</frequency>\n", signalcomp[i]->filter[j]->cutoff_frequency);

      fprintf(pro_file, "    </filter>\n");
    }

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      fprintf(pro_file, "    <ravg_filter>\n");

      fprintf(pro_file, "      <type>%i</type>\n", signalcomp[i]->ravg_filter[j]->type);

      fprintf(pro_file, "      <size>%i</size>\n", signalcomp[i]->ravg_filter[j]->size);

      fprintf(pro_file, "    </ravg_filter>\n");
    }

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      fprintf(pro_file, "    <fidfilter>\n");

      fprintf(pro_file, "      <type>%i</type>\n", signalcomp[i]->fidfilter_type[j]);

      fprintf(pro_file, "      <frequency>%.16f</frequency>\n", signalcomp[i]->fidfilter_freq[j]);

      fprintf(pro_file, "      <frequency2>%.16f</frequency2>\n", signalcomp[i]->fidfilter_freq2[j]);

      fprintf(pro_file, "      <ripple>%.16f</ripple>\n", signalcomp[i]->fidfilter_ripple[j]);

      fprintf(pro_file, "      <order>%i</order>\n", signalcomp[i]->fidfilter_order[j]);

      fprintf(pro_file, "      <model>%i</model>\n", signalcomp[i]->fidfilter_model[j]);

      fprintf(pro_file, "    </fidfilter>\n");
    }

    for(j=0; j<signalcomp[i]->fir_filter_cnt; j++)
    {
      k = fir_filter_size(signalcomp[i]->fir_filter[j]);

      fprintf(pro_file, "    <fir_filter>\n");

      fprintf(pro_file, "      <description>");

      xml_fwrite_encode_entity(pro_file, signalcomp[i]->fir_filter[j]->label);

      fprintf(pro_file, "</description>\n");

      fprintf(pro_file, "      <size>%i</size>\n", k);

      for(r=0; r<k; r++)
      {
        fprintf(pro_file, "      <tap>%.24f</tap>\n", fir_filter_tap(r, signalcomp[i]->fir_filter[j]));
      }

      fprintf(pro_file, "    </fir_filter>\n");
    }

    for(j=0; j<signalcomp[i]->math_func_cnt_after; j++)
    {
      fprintf(pro_file, "    <math_func_after>\n");

      fprintf(pro_file, "      <func>%i</func>\n", signalcomp[i]->math_func_after[j]->func);

      fprintf(pro_file, "      <pk_hold_smpls>%i</pk_hold_smpls>\n", signalcomp[i]->math_func_after[j]->pk_hold_smpls_set);

      fprintf(pro_file, "    </math_func_after>\n");
    }

    if(signalcomp[i]->plif_ecg_filter != NULL)
    {
      fprintf(pro_file, "    <plif_ecg_filter>\n");

      fprintf(pro_file, "      <plf>%i</plf>\n", signalcomp[i]->plif_ecg_subtract_filter_plf);

      fprintf(pro_file, "    </plif_ecg_filter>\n");
    }

    if(signalcomp[i]->plif_eeg_filter != NULL)
    {
      fprintf(pro_file, "    <plif_eeg_filter>\n");

      fprintf(pro_file, "      <plf>%i</plf>\n", signalcomp[i]->plif_eeg_subtract_filter_plf);

      fprintf(pro_file, "    </plif_eeg_filter>\n");
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      fprintf(pro_file, "    <ecg_filter>\n");

      fprintf(pro_file, "      <type>1</type>\n");

      fprintf(pro_file, "    </ecg_filter>\n");
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      fprintf(pro_file, "    <zratio_filter>\n");

      fprintf(pro_file, "      <type>1</type>\n");

      fprintf(pro_file, "      <crossoverfreq>%e</crossoverfreq>\n", signalcomp[i]->zratio_crossoverfreq);

      fprintf(pro_file, "    </zratio_filter>\n");
    }

    if(signalcomp[i]->hasruler)
    {
      fprintf(pro_file, "    <floating_ruler>\n");

      fprintf(pro_file, "      <hasruler>1</hasruler>\n");

      fprintf(pro_file, "      <ruler_x_position>%i</ruler_x_position>\n", maincurve->ruler_x_position);

      fprintf(pro_file, "      <ruler_y_position>%i</ruler_y_position>\n", maincurve->ruler_y_position);

      fprintf(pro_file, "      <floating_ruler_value>%i</floating_ruler_value>\n", maincurve->float_ruler_more);

      fprintf(pro_file, "    </floating_ruler>\n");
    }

    fprintf(pro_file, "  </signalcomposition>\n");
  }

  if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
  {
    fprintf(pro_file, "  <video>\n");

    fprintf(pro_file, "    <file>");
    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path_tmp, videopath, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, videopath);
    }
    fprintf(pro_file, "</file>\n");

#ifdef Q_OS_WIN32
    __mingw_fprintf(pro_file, "    <starttime>%lli</starttime>\n", video_player->utc_starttime);
#else
    fprintf(pro_file, "    <starttime>%lli</starttime>\n", video_player->utc_starttime);
#endif
    fprintf(pro_file, "    <seek>%i</seek>\n", video_player->fpos);

    if(video_player->status == VIDEO_STATUS_PAUSED)
    {
      fprintf(pro_file, "    <paused>1</paused>\n");
    }
    else
    {
      fprintf(pro_file, "    <paused>0</paused>\n");
    }

    fprintf(pro_file, "  </video>\n");
  }

  struct spectrumdocksettings settings;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(spectrumdock[i]->dock->isVisible())
    {
      spectrumdock[i]->getsettings(&settings);

      if(settings.signalnr >= 0)
      {
        fprintf(pro_file, "  <powerspectrumdock>\n");

        fprintf(pro_file, "    <signalnum>%i</signalnum>\n", settings.signalnr);

        fprintf(pro_file, "    <dashboard>%i</dashboard>\n", settings.dashboard);

        fprintf(pro_file, "    <amp>%i</amp>\n", settings.amp);

        fprintf(pro_file, "    <log_min_sl>%i</log_min_sl>\n", settings.log_min_sl);

        fprintf(pro_file, "    <wheel>%i</wheel>\n", settings.wheel);

        fprintf(pro_file, "    <span>%i</span>\n", settings.span);

        fprintf(pro_file, "    <center>%i</center>\n", settings.center);

        fprintf(pro_file, "    <log>%i</log>\n", settings.log);

        fprintf(pro_file, "    <sqrt>%i</sqrt>\n", settings.sqrt);

        fprintf(pro_file, "    <colorbar>%i</colorbar>\n", settings.colorbar);

        fprintf(pro_file, "    <blocksize_predefined>%i</blocksize_predefined>\n", settings.blocksize_predefined);

        fprintf(pro_file, "    <dftblocksize>%i</dftblocksize>\n", settings.dftblocksize);

        fprintf(pro_file, "    <window_type>%i</window_type>\n", settings.window_type);

        fprintf(pro_file, "    <overlap>%i</overlap>\n", settings.overlap);

        fprintf(pro_file, "    <maxvalue>%.10e</maxvalue>\n", settings.maxvalue);

        fprintf(pro_file, "    <maxvalue_sqrt>%.10e</maxvalue_sqrt>\n", settings.maxvalue_sqrt);

        fprintf(pro_file, "    <maxvalue_vlog>%.10e</maxvalue_vlog>\n", settings.maxvalue_vlog);

        fprintf(pro_file, "    <maxvalue_sqrt_vlog>%.10e</maxvalue_sqrt_vlog>\n", settings.maxvalue_sqrt_vlog);

        fprintf(pro_file, "    <minvalue_vlog>%.10e</minvalue_vlog>\n", settings.minvalue_vlog);

        fprintf(pro_file, "    <minvalue_sqrt_vlog>%.10e</minvalue_sqrt_vlog>\n", settings.minvalue_sqrt_vlog);

        fprintf(pro_file, "  </powerspectrumdock>\n");
      }
    }
  }

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(hypnogram_dock[i] == NULL)  continue;

    for(hdr_idx=0; hdr_idx<files_open; hdr_idx++)
    {
      if(edfheaderlist[hdr_idx] == hypnogram_dock[i]->param.edfhdr)
      {
        break;
      }
    }
    if(hdr_idx == files_open)  continue;

    fprintf(pro_file, "  <hypnogram>\n");

    fprintf(pro_file, "    <instance_num>%i</instance_num>\n", hypnogram_dock[i]->param.instance_num);

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      fprintf(pro_file, "    <stage_name>");
      xml_fwrite_encode_entity(pro_file, hypnogram_dock[i]->param.stage_name[j]);
      fprintf(pro_file, "</stage_name>\n");
    }

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      fprintf(pro_file, "    <annot_name>");
      xml_fwrite_encode_entity(pro_file, hypnogram_dock[i]->param.annot_name[j]);
      fprintf(pro_file, "</annot_name>\n");
    }

    for(j=0; j<HYPNOGRAM_OVNUM; j++)
    {
      fprintf(pro_file, "    <annot_ov_name>");
      xml_fwrite_encode_entity(pro_file, hypnogram_dock[i]->param.annot_ov_name[j]);
      fprintf(pro_file, "</annot_ov_name>\n");
    }

    for(j=0; j<HYPNOGRAM_OVNUM; j++)
    {
      fprintf(pro_file, "    <annot_ov_color>\n"
                        "      <red>%i</red>\n"
                        "      <green>%i</green>\n"
                        "      <blue>%i</blue>\n"
                        "      <alpha>%i</alpha>\n"
                        "    </annot_ov_color>\n",
                        hypnogram_dock[i]->param.annot_ov_color[j].r,
                        hypnogram_dock[i]->param.annot_ov_color[j].g,
                        hypnogram_dock[i]->param.annot_ov_color[j].b,
                        hypnogram_dock[i]->param.annot_ov_color[j].a);
    }

    fprintf(pro_file, "    <hypnogram_use_epoch_len>%i</hypnogram_use_epoch_len>\n", hypnogram_dock[i]->param.use_epoch_len);

    fprintf(pro_file, "    <use_overlays>%i</use_overlays>\n", hypnogram_dock[i]->param.use_overlays);

    fprintf(pro_file, "    <hdr_idx>%i</hdr_idx>\n", hdr_idx);

    fprintf(pro_file, "  </hypnogram>\n");
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] == NULL)  continue;

    for(sigcomp_idx=0; sigcomp_idx<signalcomps; sigcomp_idx++)
    {
      if(signalcomp[sigcomp_idx] == cdsa_dock[i]->param.signalcomp)
      {
        break;
      }
    }
    if(sigcomp_idx == signalcomps)  continue;

    fprintf(pro_file, "  <cdsa>\n");

    fprintf(pro_file, "    <sigcomp_idx>%i</sigcomp_idx>\n", sigcomp_idx);

    fprintf(pro_file, "    <min_hz>%i</min_hz>\n", cdsa_dock[i]->param.min_hz);

    fprintf(pro_file, "    <max_hz>%i</max_hz>\n", cdsa_dock[i]->param.max_hz);

    fprintf(pro_file, "    <segment_len>%i</segment_len>\n", cdsa_dock[i]->param.segment_len);

    fprintf(pro_file, "    <block_len>%i</block_len>\n", cdsa_dock[i]->param.block_len);

    fprintf(pro_file, "    <overlap>%i</overlap>\n", cdsa_dock[i]->param.overlap);

    fprintf(pro_file, "    <window_func>%i</window_func>\n", cdsa_dock[i]->param.window_func);

    fprintf(pro_file, "    <max_voltage>%.10e</max_voltage>\n", cdsa_dock[i]->param.max_voltage);

    fprintf(pro_file, "    <max_pwr>%i</max_pwr>\n", cdsa_dock[i]->param.max_pwr);

    fprintf(pro_file, "    <min_pwr>%i</min_pwr>\n", cdsa_dock[i]->param.min_pwr);

    fprintf(pro_file, "    <log>%i</log>\n", cdsa_dock[i]->param.log);

    fprintf(pro_file, "    <power_voltage>%i</power_voltage>\n", cdsa_dock[i]->param.power_voltage);

    fprintf(pro_file, "    <instance_num>%i</instance_num>\n", cdsa_dock[i]->param.instance_num);

    fprintf(pro_file, "  </cdsa>\n");
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(aeeg_dock[i] == NULL)  continue;

    for(sigcomp_idx=0; sigcomp_idx<signalcomps; sigcomp_idx++)
    {
      if(signalcomp[sigcomp_idx] == aeeg_dock[i]->param.signalcomp)
      {
        break;
      }
    }
    if(sigcomp_idx == signalcomps)  continue;

    fprintf(pro_file, "  <aeeg>\n");

    fprintf(pro_file, "    <sigcomp_idx>%i</sigcomp_idx>\n", sigcomp_idx);

    fprintf(pro_file, "    <bp_min_hz>%e</bp_min_hz>\n", aeeg_dock[i]->param.bp_min_hz);

    fprintf(pro_file, "    <bp_max_hz>%e</bp_max_hz>\n", aeeg_dock[i]->param.bp_max_hz);

    fprintf(pro_file, "    <segment_len>%i</segment_len>\n", aeeg_dock[i]->param.segment_len);

    fprintf(pro_file, "    <lp_hz>%e</lp_hz>\n", aeeg_dock[i]->param.lp_hz);

    fprintf(pro_file, "    <scale_max_amp>%e</scale_max_amp>\n", aeeg_dock[i]->param.scale_max_amp);

    fprintf(pro_file, "    <plot_margins>%i</plot_margins>\n", aeeg_dock[i]->param.plot_margins);

    fprintf(pro_file, "    <instance_num>%i</instance_num>\n", aeeg_dock[i]->param.instance_num);

    fprintf(pro_file, "  </aeeg>\n");
  }

  fprintf(pro_file, "</" PROGRAM_NAME "_session>\n");

  fclose(pro_file);

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
}

















