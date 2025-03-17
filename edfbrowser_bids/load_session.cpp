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


int UI_Mainwindow::read_session_file(const char *path_session)
{
  int i, j, n=0, r,
      tmp,
      skip,
      found,
      signalcomps_read=0,
      signals_read,
      signal_cnt,
      filters_read,
      spike_filter_cnt=0,
      filter_cnt=0,
      math_funcs_before_read,
      math_funcs_after_read,
      math_func,
      math_func_cnt_before=0,
      math_func_cnt_after=0,
      math_func_pk_hold_smpls=0,
      ravg_filter_cnt=0,
      fidfilter_cnt=0,
      fir_filter_cnt=0,
      order=1,
      type=0,
      model=0,
      size=0,
      amp_cat[3],
      f_ruler_cnt=0,
      holdoff=100,
      plif_powerlinefrequency,
      not_compatibel,
      sf,
      n_taps,
      sense,
      use_relative_path=0,
      hdr_idx=0,
      sigcomp_idx=0;

  char result[XML_STRBUFLEN],
       scratchpad_2048[2048]="",
       str1_128[128]="",
       str2_512[512]="",
       *err_ptr=NULL,
       *filter_spec_ptr=NULL,
       spec_str_256[256]="";

  double frequency=1.0,
         frequency2=2.0,
         ripple=1.0,
         velocity=1.0,
         dthreshold,
         *fir_vars=NULL;

  xml_hdl_t *xml_hdl=NULL;

  sigcompblck_t *newsignalcomp=NULL;

  struct spectrumdocksettings settings[MAXSPECTRUMDOCKS];

  memset(settings, 0, sizeof(struct spectrumdocksettings[MAXSPECTRUMDOCKS]));

  hypnogram_dock_param_t hypnogram_param;

  memset(&hypnogram_param, 0, sizeof(hypnogram_dock_param_t));

  cdsa_dock_param_t cdsa_param;

  memset(&cdsa_param, 0, sizeof(cdsa_dock_param_t));

  aeeg_dock_param_t aeeg_param;

  memset(&aeeg_param, 0, sizeof(aeeg_dock_param_t));

  if(path_session == NULL) return -999;

  xml_hdl = xml_get_handle(path_session);
  if(xml_hdl==NULL)
  {
    if(rc_cmd_in_progress)
    {
      return 105;
    }
    else
    {
      snprintf(scratchpad_2048, 2048, "Cannot open session file:\n%s", path_session);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad_2048));
      messagewindow.exec();
      return -1;
    }
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_session"))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "relative_path", 0))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  use_relative_path = atoi(result);
  if((use_relative_path < 0) || (use_relative_path > 1))
  {
    use_relative_path = 0;
  }
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "edf_files", 0))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }

  for(i=0; i<MAXFILES; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "file", i))
    {
      break;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }

    cmdlineargument = 0;

    rc_file_open_err = 0;

    rc_file_open_requested = 1;

    if(use_relative_path)
    {
      get_directory_from_path(path, path_session, MAX_PATH_LENGTH);
      strlcat(path, "/", MAX_PATH_LENGTH);
      strlcat(path, result, MAX_PATH_LENGTH);
      sanitize_path(path);
    }
    else
    {
      strlcpy(path, result, MAX_PATH_LENGTH);
    }

    open_new_file();

    rc_file_open_requested = 0;

    if(rc_file_open_err)
    {
      return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  for(i=0; i<files_open; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "viewtime", i))
    {
      break;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }
    edfheaderlist[i]->viewtime = atoll(result);

    xml_go_up(xml_hdl);
  }

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "ref_file", 0))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  sel_viewtime = atoi(result);
  if((sel_viewtime < 0) || (sel_viewtime >= files_open))
  {
    sel_viewtime = 0;
  }
  setMainwindowTitle(edfheaderlist[sel_viewtime]);
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "timesync_mode", 0))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  viewtime_sync = atoi(result);
  if((viewtime_sync < 0) || (viewtime_sync > 3))
  {
    viewtime_sync = 1;
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
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "pagetime", 0))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }
  pagetime = atoll(result);
  if(pagetime < 10000LL)
  {
    pagetime = 10000LL;
  }
  xml_go_up(xml_hdl);


  if(!xml_goto_nth_element_inside(xml_hdl, "ecg_view_mode", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }
    ecg_view_mode = atoi(result);
    if(ecg_view_mode != 1)
    {
      ecg_view_mode = 0;
    }
    xml_go_up(xml_hdl);
  }

  while(1)
  {
    skip = 0;

    xml_goto_root(xml_hdl);

    signals_read = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalcomposition", signalcomps_read))
    {
      break;
    }

    if(signalcomps >= MAXSIGNALS)
    {
      snprintf(str2_512, 512, "Cannot display more than %i traces.", MAXSIGNALS);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
      messagewindow.exec();
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }

    newsignalcomp = (sigcompblck_t *)calloc(1, sizeof(sigcompblck_t));
    if(newsignalcomp==NULL)
    {
      if(!rc_cmd_in_progress)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
        messagewindow.exec();
      }
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }

    newsignalcomp->uid = uid_seq++;

    if(xml_goto_nth_element_inside(xml_hdl, "num_of_signals", 0))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    signal_cnt = atoi(result);
    if((signal_cnt<1)||(signal_cnt>MAXSIGNALS))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "file", 0))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    for(n=0; n<files_open; n++)
    {
      if(use_relative_path)
      {
        get_directory_from_path(path, path_session, MAX_PATH_LENGTH);
        strlcat(path, "/", MAX_PATH_LENGTH);
        strlcat(path, result, MAX_PATH_LENGTH);
        sanitize_path(path);
      }
      else
      {
        strlcpy(path, result, MAX_PATH_LENGTH);
      }

      if(!strcmp(edfheaderlist[n]->filename, path))
      {
        break;
      }
    }
    if(n >= files_open)
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    xml_go_up(xml_hdl);

    newsignalcomp->edfhdr = edfheaderlist[n];
    newsignalcomp->num_of_signals = signal_cnt;
    newsignalcomp->hascursor1 = 0;
    newsignalcomp->hascursor2 = 0;
    newsignalcomp->hasoffsettracking = 0;
    newsignalcomp->hasgaintracking = 0;
    newsignalcomp->screen_offset_pix = 0;
    newsignalcomp->screen_offset_unit = 0;
    newsignalcomp->filter_cnt = 0;
    newsignalcomp->ravg_filter_cnt = 0;
    newsignalcomp->fir_filter_cnt = 0;
    newsignalcomp->plif_ecg_filter = NULL;
    newsignalcomp->plif_ecg_filter_sav = NULL;
    newsignalcomp->plif_eeg_filter = NULL;
    newsignalcomp->plif_eeg_filter_sav = NULL;
    newsignalcomp->ecg_filter = NULL;
    newsignalcomp->fidfilter_cnt = 0;
    newsignalcomp->hasruler = 0;
    newsignalcomp->polarity = 1;

    if(!(xml_goto_nth_element_inside(xml_hdl, "alias", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(result[0] != 0)
      {
        strncpy(newsignalcomp->alias, result, 16);
        newsignalcomp->alias[16] = 0;
        latin1_to_ascii(newsignalcomp->alias, strlen(newsignalcomp->alias));
        trim_spaces(newsignalcomp->alias);
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "voltpercm", 0))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->voltpercm = atof(result);
    if(newsignalcomp->voltpercm==0.0)  newsignalcomp->voltpercm = 0.000000001;
    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "screen_offset_unit", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->screen_offset_unit = atof(result);
      newsignalcomp->screen_offset_pix = -newsignalcomp->screen_offset_unit / (y_pixelsizefactor * newsignalcomp->voltpercm);
      xml_go_up(xml_hdl);
    }
    else if(!xml_goto_nth_element_inside(xml_hdl, "screen_offset", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        newsignalcomp->screen_offset_pix = atof(result);
        newsignalcomp->screen_offset_unit = -newsignalcomp->screen_offset_pix * y_pixelsizefactor * newsignalcomp->voltpercm;
        xml_go_up(xml_hdl);
      }

    if(!(xml_goto_nth_element_inside(xml_hdl, "polarity", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->polarity = atoi(result);
      if(newsignalcomp->polarity != -1)
      {
        newsignalcomp->polarity = 1;
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->color = atoi(result);
    if((newsignalcomp->color < 2) || (newsignalcomp->color > 18))
    {
      newsignalcomp->color = 2;
    }
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_before", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_cnt_before = atoi(result);
      if(math_func_cnt_before < 0)  math_func_cnt_before = 0;
      if(math_func_cnt_before > MAXMATHFUNCS)  math_func_cnt_before = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_after", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_cnt_after = atoi(result);
      if(math_func_cnt_after < 0)  math_func_cnt_after = 0;
      if(math_func_cnt_after > MAXMATHFUNCS)  math_func_cnt_after = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      filter_cnt = atoi(result);
      if(filter_cnt < 0)  filter_cnt = 0;
      if(filter_cnt > MAXFILTERS)  filter_cnt = MAXFILTERS;
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "spike_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      spike_filter_cnt = atoi(result);
      if(spike_filter_cnt < 0)  spike_filter_cnt = 0;
      if(spike_filter_cnt > 1)  spike_filter_cnt = 1;
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ravg_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      ravg_filter_cnt = atoi(result);
      if(ravg_filter_cnt < 0)  ravg_filter_cnt = 0;
      if(ravg_filter_cnt > MAXFILTERS)  ravg_filter_cnt = MAXFILTERS;
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "fir_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      fir_filter_cnt = atoi(result);
      if(fir_filter_cnt < 0)  fir_filter_cnt = 0;
      if(fir_filter_cnt > MAXFILTERS)  fir_filter_cnt = MAXFILTERS;
      xml_go_up(xml_hdl);
    }

    if(filter_cnt)
    {
      fidfilter_cnt = 0;
    }
    else
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "fidfilter_cnt", 0)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        fidfilter_cnt = atoi(result);
        if(fidfilter_cnt < 0)  fidfilter_cnt = 0;
        if(fidfilter_cnt > MAXFILTERS)  fidfilter_cnt = MAXFILTERS;
        xml_go_up(xml_hdl);
      }
    }

    for(signals_read=0; signals_read<signal_cnt; signals_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "signal", signals_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "factor", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->factor[signals_read] = atof(result);
      if(newsignalcomp->factor[signals_read] < -128)
      {
        newsignalcomp->factor[signals_read] = -128;
      }
      if(newsignalcomp->factor[signals_read] > 128)
      {
        newsignalcomp->factor[signals_read] = 128;
      }
      if((newsignalcomp->factor[signals_read] < 0.001) && (newsignalcomp->factor[signals_read] > -0.001))
      {
        newsignalcomp->factor[signals_read] = 1;
      }

      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "edfindex", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "label", 0))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        remove_trailing_spaces(result);

        found = 0;

        for(i=0; i<newsignalcomp->edfhdr->edfsignals; i++)
        {
          strlcpy(scratchpad_2048, newsignalcomp->edfhdr->edfparam[i].label, 2048);

          remove_trailing_spaces(scratchpad_2048);

          if(!strcmp(scratchpad_2048, result))
          {
            newsignalcomp->edfsignal[signals_read] = i;

            if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].annotation)
            {
              found = 0;
            }
            else
            {
              found = 1;
            }
            break;
          }
        }
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->edfsignal[signals_read] = atoi(result);

        if((newsignalcomp->edfsignal[signals_read] < 0) || (newsignalcomp->edfsignal[signals_read] >= newsignalcomp->edfhdr->edfsignals))
        {
          found = 0;
        }
        else
        {
          if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].annotation)
          {
            found = 0;
          }
          else
          {
            found = 1;
          }
        }
      }

      if(!found)
      {
        free(newsignalcomp);
        newsignalcomp = NULL;
        skip = 1;
        signalcomps_read++;
        xml_go_up(xml_hdl);
        xml_go_up(xml_hdl);
        break;
      }

      if(!signals_read)
      {
        newsignalcomp->edfparam_0 = &newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]];

        newsignalcomp->sensitivity = newsignalcomp->edfparam_0->bitvalue / (newsignalcomp->voltpercm * y_pixelsizefactor);

        newsignalcomp->signallabel[0] = 0;
      }
      else
      {
        if((newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].smp_per_record != newsignalcomp->edfparam_0->smp_per_record) ||
           (dblcmp(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].bitvalue, newsignalcomp->edfparam_0->bitvalue)) ||
           (strcmp(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].physdimension, newsignalcomp->edfparam_0->physdimension)))
        {
          free(newsignalcomp);
          newsignalcomp = NULL;
          skip = 1;
          signalcomps_read++;
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          break;
        }
      }

      if(signal_cnt>1)
      {
        if(newsignalcomp->factor[signals_read] < 0)
        {
          strlcat(newsignalcomp->signallabel, "- ", 512);
        }
        else
        {
          if(signals_read)
          {
            strlcat(newsignalcomp->signallabel, "+ ", 512);
          }
        }
      }
      strlcpy(str1_128, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].label, 128);
      if(strip_label_types)
      {
        newsignalcomp->signallabel_type_stripped = strip_types_from_label(str1_128);
      }
      strlcat(newsignalcomp->signallabel, str1_128, 512);
      remove_trailing_spaces(newsignalcomp->signallabel);
      strlcat(newsignalcomp->signallabel, " ", 512);

      newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(skip || (!newsignalcomp))  continue;

    remove_trailing_spaces(newsignalcomp->signallabel);
    newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

    strlcpy(newsignalcomp->physdimension, newsignalcomp->edfparam_0->physdimension, 9);
    remove_trailing_spaces(newsignalcomp->physdimension);

    if(spike_filter_cnt)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "spike_filter", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "velocity", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      velocity = atof(result);
      if(velocity < 0.0001)  velocity = 0.0001;
      if(velocity > 10E9)  velocity = 10E9;

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "holdoff", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      holdoff = atoi(result);
      if(holdoff < 10)  holdoff = 10;
      if(holdoff > 1000)  holdoff = 1000;

      newsignalcomp->spike_filter = create_spike_filter(newsignalcomp->edfparam_0->sf_f,
        velocity / newsignalcomp->edfparam_0->bitvalue,
        holdoff, NULL);

      if(newsignalcomp->spike_filter == NULL)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->spike_filter_velocity = velocity;

      newsignalcomp->spike_filter_holdoff = holdoff;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(math_funcs_before_read=0; math_funcs_before_read<math_func_cnt_before; math_funcs_before_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "math_func_before", math_funcs_before_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func = atoi(result);
      if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "pk_hold_smpls", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_pk_hold_smpls = atoi(result);
      if(math_func_pk_hold_smpls < 0)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->math_func_before[newsignalcomp->math_func_cnt_before] = create_math_func(math_func, math_func_pk_hold_smpls);
      if(newsignalcomp->math_func_before[newsignalcomp->math_func_cnt_before] == NULL)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->math_func_cnt_before++;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(math_funcs_after_read=0; math_funcs_after_read<math_func_cnt_after; math_funcs_after_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "math_func_after", math_funcs_after_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func = atoi(result);
      if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "pk_hold_smpls", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_pk_hold_smpls = atoi(result);
      if(math_func_pk_hold_smpls < 0)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->math_func_after[newsignalcomp->math_func_cnt_after] = create_math_func(math_func, math_func_pk_hold_smpls);
      if(newsignalcomp->math_func_after[newsignalcomp->math_func_cnt_after] == NULL)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->math_func_cnt_after++;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "filter", filters_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "LPF", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency = atof(result);

      if((type   < 0) || (type   >   1) || (frequency < 0.0001))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(frequency >= newsignalcomp->edfparam_0->sf_f / 2.0)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(type == 0)
      {
        snprintf(spec_str_256, 256, "HpBu1/%f", frequency);
      }

      if(type == 1)
      {
        snprintf(spec_str_256, 256, "LpBu1/%f", frequency);
      }

      filter_spec_ptr = spec_str_256;

      err_ptr = fid_parse(newsignalcomp->edfparam_0->sf_f,
                      &filter_spec_ptr,
                      &newsignalcomp->fidfilter[filters_read]);

      if(err_ptr != NULL)
      {
        if(!rc_cmd_in_progress)
        {
          snprintf(str2_512, 512, "%s\nFile: %s line: %i", err_ptr, __FILE__, __LINE__);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
          messagewindow.exec();
        }
        free(err_ptr);
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->fid_run[filters_read] = fid_run_new(newsignalcomp->fidfilter[filters_read],
                                                                         &newsignalcomp->fidfuncp[filters_read]);

      newsignalcomp->fidbuf[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);
      newsignalcomp->fidbuf2[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);

      newsignalcomp->fidfilter_freq[filters_read] = frequency;

      newsignalcomp->fidfilter_freq2[filters_read] = frequency * 1.12;

      newsignalcomp->fidfilter_ripple[filters_read] = -1.0;

      newsignalcomp->fidfilter_order[filters_read] = 1;

      newsignalcomp->fidfilter_type[filters_read] = type;

      newsignalcomp->fidfilter_model[filters_read] = 0;

      newsignalcomp->fidfilter_setup[filters_read] = 1;

      newsignalcomp->fidfilter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<ravg_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "ravg_filter", filters_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      size = atoi(result);

      if((type   < 0) || (type   >   1) || (size < 2) || (size > 10000))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->ravg_filter[filters_read] = create_ravg_filter(type, size);
      if(newsignalcomp->ravg_filter[filters_read] == NULL)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->ravg_filter_size[filters_read] = size;

      newsignalcomp->ravg_filter_type[filters_read] = type;

      newsignalcomp->ravg_filter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fidfilter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fidfilter", filters_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "model", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      model = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency2", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency2 = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "ripple", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      ripple = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "order", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      order = atoi(result);

      if((model  < 0)         || (model  >   2)       ||
         (order  < 1)         || (order  > 100)       ||
         (type   < 0)         || (type   >   4)       ||
         (ripple < (-6.0))    || (ripple > (-0.1))    ||
         (frequency < 0.0001) || (frequency2 < 0.0001))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(frequency >= newsignalcomp->edfparam_0->sf_f / 2.0)
      {
        if(!rc_cmd_in_progress)
        {
          snprintf(str2_512, 512, "There seems to be an error in this montage file.\nThe frequency of the filter(s) must be less than: samplerate / 2\nFile: %s line: %i", __FILE__, __LINE__);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
          messagewindow.exec();
        }
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(type > 2)
      {
        if(frequency2 >= newsignalcomp->edfparam_0->sf_f / 2.0)
        {
          if(!rc_cmd_in_progress)
          {
            snprintf(str2_512, 512, "There seems to be an error in this montage file.\nThe frequency of the filter(s) must be less than: samplerate / 2\nFile: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
            messagewindow.exec();
          }
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
      }

      if((type == 0) || (type == 1))
      {
        if(order > 8)
        {
          order = 8;
        }
      }

      if(type == 0)
      {
        if(model == 0)
        {
          snprintf(spec_str_256, 256, "HpBu%i/%f", order, frequency);
        }

        if(model == 1)
        {
          snprintf(spec_str_256, 256, "HpCh%i/%f/%f", order, ripple, frequency);
        }

        if(model == 2)
        {
          snprintf(spec_str_256, 256, "HpBe%i/%f", order, frequency);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          snprintf(spec_str_256, 256, "LpBu%i/%f", order, frequency);
        }

        if(model == 1)
        {
          snprintf(spec_str_256, 256, "LpCh%i/%f/%f", order, ripple, frequency);
        }

        if(model == 2)
        {
          snprintf(spec_str_256, 256, "LpBe%i/%f", order, frequency);
        }
      }

      if(type == 2)
      {
        if(order > 100)
        {
          order = 100;
        }

        if(order < 3)
        {
          order = 3;
        }

        if(model == 0)
        {
          snprintf(spec_str_256, 256, "BsRe/%i/%f", order, frequency);
        }
      }

      if((type == 3) || (type == 4))
      {
        if(order < 2)
        {
          order = 2;
        }

        if(order % 2)
        {
          order++;
        }

        if(order > 16)
        {
          order = 16;
        }
      }

      if(type == 3)
      {
        if(model == 0)
        {
          snprintf(spec_str_256, 256, "BpBu%i/%f-%f", order, frequency, frequency2);
        }

        if(model == 1)
        {
          snprintf(spec_str_256, 256, "BpCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
        }

        if(model == 2)
        {
          snprintf(spec_str_256, 256, "BpBe%i/%f-%f", order, frequency, frequency2);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          snprintf(spec_str_256, 256, "BsBu%i/%f-%f", order, frequency, frequency2);
        }

        if(model == 1)
        {
          snprintf(spec_str_256, 256, "BsCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
        }

        if(model == 2)
        {
          snprintf(spec_str_256, 256, "BsBe%i/%f-%f", order, frequency, frequency2);
        }
      }

      filter_spec_ptr = spec_str_256;

      err_ptr = fid_parse(newsignalcomp->edfparam_0->sf_f,
                      &filter_spec_ptr,
                      &newsignalcomp->fidfilter[filters_read]);

      if(err_ptr != NULL)
      {
        if(!rc_cmd_in_progress)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", err_ptr);
          messagewindow.exec();
        }
        free(err_ptr);
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->fid_run[filters_read] = fid_run_new(newsignalcomp->fidfilter[filters_read],
                                                                         &newsignalcomp->fidfuncp[filters_read]);

      newsignalcomp->fidbuf[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);
      newsignalcomp->fidbuf2[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);

      newsignalcomp->fidfilter_freq[filters_read] = frequency;

      newsignalcomp->fidfilter_freq2[filters_read] = frequency2;

      newsignalcomp->fidfilter_ripple[filters_read] = ripple;

      newsignalcomp->fidfilter_order[filters_read] = order;

      newsignalcomp->fidfilter_type[filters_read] = type;

      newsignalcomp->fidfilter_model[filters_read] = model;

      newsignalcomp->fidfilter_setup[filters_read] = 1;

      newsignalcomp->fidfilter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_ecg_filter", 0))
    {
      not_compatibel = 0;

      sf = newsignalcomp->edfparam_0->sf_int;

      if(!sf)
      {
        not_compatibel = 1;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      if(sf % plif_powerlinefrequency)  not_compatibel = 1;

      strlcpy(str1_128, newsignalcomp->edfparam_0->physdimension, 128);

      remove_trailing_spaces(str1_128);

      if(!strcmp(str1_128, "uV"))
      {
        dthreshold = 10.0 / newsignalcomp->edfparam_0->bitvalue;
      }
      else if(!strcmp(str1_128, "mV"))
        {
          dthreshold = 1e-2 / newsignalcomp->edfparam_0->bitvalue;
        }
        else if(!strcmp(str1_128, "V"))
          {
            dthreshold = 1e-5 / newsignalcomp->edfparam_0->bitvalue;
          }
          else
          {
            not_compatibel = 1;
          }

      if(!not_compatibel)
      {
        newsignalcomp->plif_ecg_filter = plif_ecg_create_subtract_filter(sf, plif_powerlinefrequency, dthreshold);
        if(newsignalcomp->plif_ecg_filter == NULL)
        {
          if(!rc_cmd_in_progress)
          {
            snprintf(str2_512, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
            messagewindow.exec();
          }
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->plif_ecg_filter_sav = plif_ecg_create_subtract_filter(sf, plif_powerlinefrequency, dthreshold);
        if(newsignalcomp->plif_ecg_filter_sav == NULL)
        {
          if(!rc_cmd_in_progress)
          {
            snprintf(str2_512, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
            messagewindow.exec();
          }
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->plif_ecg_subtract_filter_plf = plif_powerlinefrequency / 60;
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_eeg_filter", 0))
    {
      not_compatibel = 0;

      sf = newsignalcomp->edfparam_0->sf_int;

      if(!sf)
      {
        not_compatibel = 1;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      if(sf % plif_powerlinefrequency)  not_compatibel = 1;

      if(!not_compatibel)
      {
        newsignalcomp->plif_eeg_filter = plif_eeg_create_subtract_filter(sf, plif_powerlinefrequency);
        if(newsignalcomp->plif_eeg_filter == NULL)
        {
          if(!rc_cmd_in_progress)
          {
            snprintf(str2_512, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
            messagewindow.exec();
          }
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->plif_eeg_filter_sav = plif_eeg_create_subtract_filter(sf, plif_powerlinefrequency);
        if(newsignalcomp->plif_eeg_filter_sav == NULL)
        {
          if(!rc_cmd_in_progress)
          {
            snprintf(str2_512, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
            messagewindow.exec();
          }
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->plif_eeg_subtract_filter_plf = plif_powerlinefrequency / 60;
      }

      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fir_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fir_filter", filters_read))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(!xml_goto_nth_element_inside(xml_hdl, "description", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        strlcpy(str2_512, result, 512);

        xml_go_up(xml_hdl);
      }
      else
      {
        str2_512[0] = 0;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      n_taps = atoi(result);
      if((n_taps < 2) || (n_taps > 4000))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      xml_go_up(xml_hdl);

      fir_vars = (double *)malloc(sizeof(double) * n_taps);
      if(fir_vars == NULL)
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "tap", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      fir_vars[0] = atof(result);

      for(r=1; r<n_taps; r++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        fir_vars[r] = atof(result);
      }

      xml_go_up(xml_hdl);

      newsignalcomp->fir_filter[filters_read] = create_fir_filter(fir_vars, n_taps, str2_512);
      if(newsignalcomp->fir_filter[filters_read] == NULL)
      {
        free(fir_vars);
        fir_vars = NULL;
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->fir_filter_setup[filters_read] = 1;

      newsignalcomp->fir_filter_cnt++;

      free(fir_vars);
      fir_vars = NULL;

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      if(type == 1)
      {
        if(newsignalcomp->edfparam_0->sf_f >= 199.999)
        {
          strlcpy(str2_512, newsignalcomp->edfparam_0->physdimension, 32);
          trim_spaces(str2_512);
          sense = 0;
          if(!strcmp(str2_512, "uV"))
          {
            sense = 1;
          }
          else if(!strcmp(str2_512, "mV"))
            {
              sense = 1000;
            }
            else if(!strcmp(str2_512, "V"))
              {
                sense = 1000000;
              }

          if(sense > 0)
          {
            newsignalcomp->ecg_filter = create_ecg_filter(newsignalcomp->edfparam_0->sf_f,
                                                          newsignalcomp->edfparam_0->bitvalue,
                                                          sense);
            if(newsignalcomp->ecg_filter == NULL)
            {
              if(!rc_cmd_in_progress)
              {
                snprintf(str2_512, 512, "Could not create an ECG filter.\nFile: %s line: %i", __FILE__, __LINE__);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
                messagewindow.exec();
              }
              return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
            }

            strlcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel, 512);
            newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
            strlcpy(newsignalcomp->signallabel, "HR", 512);
            newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
            strlcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension, 9);
            strlcpy(newsignalcomp->physdimension, "bpm", 9);
          }
        }
      }

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(newsignalcomp->ecg_filter == NULL)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "zratio_filter", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        type = atoi(result);

        xml_go_up(xml_hdl);

        if(type == 1)
        {
          if(xml_goto_nth_element_inside(xml_hdl, "crossoverfreq", 0))
          {
            return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
          newsignalcomp->zratio_crossoverfreq = atof(result);

          if((newsignalcomp->zratio_crossoverfreq < 5.0) || (newsignalcomp->zratio_crossoverfreq > 9.5))
          {
            newsignalcomp->zratio_crossoverfreq = 7.5;
          }

          newsignalcomp->zratio_filter = create_zratio_filter(newsignalcomp->edfparam_0->smp_per_record,
                                                              newsignalcomp->edfhdr->long_data_record_duration,
                                                              newsignalcomp->zratio_crossoverfreq,
                                                              newsignalcomp->edfparam_0->bitvalue);

          if(newsignalcomp->zratio_filter == NULL)
          {
            if(!rc_cmd_in_progress)
            {
              snprintf(str2_512, 512, "A memory allocation error occurred when creating a Z-ratio filter.\nFile: %s line: %i", __FILE__, __LINE__);
              QMessageBox messagewindow(QMessageBox::Critical, "Error", str2_512);
              messagewindow.exec();
            }
            return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }

          strlcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel, 512);
          newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
          strlcpy(newsignalcomp->signallabel, "Z-ratio ", 512);
          strlcat(newsignalcomp->signallabel, newsignalcomp->signallabel_bu, 512);
          newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
          strlcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension, 9);
          strlcpy(newsignalcomp->physdimension, "", 9);

          xml_go_up(xml_hdl);
        }

        xml_go_up(xml_hdl);
      }
    }

    if(f_ruler_cnt == 0)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "floating_ruler", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "hasruler", 0))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        tmp = atoi(result);

        xml_go_up(xml_hdl);

        if(tmp == 1)
        {
          f_ruler_cnt = 1;

          maincurve->ruler_x_position = 200;
          maincurve->ruler_y_position = 200;
          maincurve->float_ruler_more = 0;

          if(!xml_goto_nth_element_inside(xml_hdl, "ruler_x_position", 0))
          {
            if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
            {
              return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
            }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 5000))
            {
              maincurve->ruler_x_position = tmp;
            }

            xml_go_up(xml_hdl);
          }

          if(!xml_goto_nth_element_inside(xml_hdl, "ruler_y_position", 0))
          {
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 5000))
            {
              maincurve->ruler_y_position = tmp;
            }

            xml_go_up(xml_hdl);
          }

          if(!xml_goto_nth_element_inside(xml_hdl, "floating_ruler_value", 0))
          {
            if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
            {
              return session_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
            }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 2))
            {
              maincurve->float_ruler_more = tmp;
            }

            xml_go_up(xml_hdl);
          }

          newsignalcomp->hasruler = 1;
        }

        xml_go_up(xml_hdl);
      }
    }

    signalcomp[signalcomps] = newsignalcomp;

    signalcomps++;

    signalcomps_read++;
  }

  newsignalcomp = NULL;

  xml_goto_root(xml_hdl);

  videopath[0] = 0;

  session_video_seek = 0;

  session_video_starttime = 0;

  video_pause_requested = 0;

  if(!xml_goto_nth_element_inside(xml_hdl, "video", 0))
  {
    if(xml_goto_nth_element_inside(xml_hdl, "file", 0))
    {
      return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      if(use_relative_path)
      {
        get_directory_from_path(videopath, path_session, MAX_PATH_LENGTH);
        strlcat(videopath, "/", MAX_PATH_LENGTH);
        strlcat(videopath, result, MAX_PATH_LENGTH);
        sanitize_path(videopath);
      }
      else
      {
        strlcpy(videopath, result, MAX_PATH_LENGTH);
      }
      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "starttime", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }
        session_video_starttime = atoll(result);
        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "seek", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }
        session_video_seek = atoi(result);
        if(session_video_seek < 0)
        {
          session_video_seek = 0;
        }
        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "paused", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }
        video_pause_requested = atoi(result);
        if((video_pause_requested < 0) || (video_pause_requested > 1))
        {
          video_pause_requested = 0;
        }
        xml_go_up(xml_hdl);
      }
    }

    xml_go_up(xml_hdl);
  }

  xml_goto_root(xml_hdl);

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    settings[i].signalnr = -1;
  }

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "powerspectrumdock", i)))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "signalnum", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].signalnr = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "dashboard", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].dashboard = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "amp", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].amp = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "log_min_sl", 0))
      {
        settings[i].log_min_sl = 1000;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].log_min_sl = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "wheel", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].wheel = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "span", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].span = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "center", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].center = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "log", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].log = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "sqrt", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].sqrt = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "colorbar", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].colorbar = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "blocksize_predefined", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].blocksize_predefined = atoi(result);
        if((settings[i].blocksize_predefined < 0) || (settings[i].blocksize_predefined > 9))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "dftblocksize", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].dftblocksize = atoi(result);
        if(settings[i].dftblocksize < 2)
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "overlap", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].overlap = atoi(result);
        if((settings[i].overlap < 1) || (settings[i].overlap > 5))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "window_type", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].window_type = atoi(result);
        if((settings[i].window_type < 0) || (settings[i].window_type > 12))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_sqrt", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_sqrt = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_vlog", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_sqrt_vlog", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_sqrt_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "minvalue_vlog", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].minvalue_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "minvalue_sqrt_vlog", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].minvalue_sqrt_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      xml_go_up(xml_hdl);
    }
    else
    {
      break;
    }
  }

  xml_goto_root(xml_hdl);

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "hypnogram", i)))
    {
      memset(&hypnogram_param, 0, sizeof(hypnogram_dock_param_t));

      if(xml_goto_nth_element_inside(xml_hdl, "instance_num", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        hypnogram_param.instance_num = atoi(result);
        if((hypnogram_param.instance_num < 0) || (hypnogram_param.instance_num >= MAXHYPNOGRAMDOCKS))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "hdr_idx", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        hdr_idx = atoi(result);
        if((hdr_idx < 0) || (hdr_idx >= files_open))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        hypnogram_param.edfhdr = edfheaderlist[hdr_idx];

        xml_go_up(xml_hdl);
      }

      if(!xml_goto_nth_element_inside(xml_hdl, "hypnogram_use_epoch_len", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        hypnogram_param.use_epoch_len = atoi(result);
        if(hypnogram_param.use_epoch_len != 1)
        {
          hypnogram_param.use_epoch_len = 0;
        }

        hypnogram_use_epoch_len = hypnogram_param.use_epoch_len;

        xml_go_up(xml_hdl);
      }

      if(!xml_goto_nth_element_inside(xml_hdl, "use_overlays", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        hypnogram_param.use_overlays = atoi(result);
        if(hypnogram_param.use_overlays != 1)
        {
          hypnogram_param.use_overlays = 0;
        }

        hypnogram_use_overlays = hypnogram_param.use_overlays;

        xml_go_up(xml_hdl);
      }

      for(j=0; j<HYPNOGRAM_STAGENUM; j++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "stage_name", j))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        strlcpy(hypnogram_param.stage_name[j], result, 32);

        strlcpy(hypnogram_stage_name[j], hypnogram_param.stage_name[j], 32);

        xml_go_up(xml_hdl);
      }

      for(j=0; j<HYPNOGRAM_STAGENUM; j++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "annot_name", j))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        strlcpy(hypnogram_param.annot_name[j], result, 32);

        strlcpy(hypnogram_annot_name[j], hypnogram_param.annot_name[j], 32);

        xml_go_up(xml_hdl);
      }

      for(j=0; j<HYPNOGRAM_OVNUM; j++)
      {
        if(!xml_goto_nth_element_inside(xml_hdl, "annot_ov_name", j))
        {
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
          }

          strlcpy(hypnogram_param.annot_ov_name[j], result, 32);

          strlcpy(hypnogram_annot_ov_name[j], hypnogram_param.annot_ov_name[j], 32);

          xml_go_up(xml_hdl);
        }
      }

      for(j=0; j<HYPNOGRAM_OVNUM; j++)
      {
        get_rgbcolor_settings(xml_hdl, "annot_ov_color", j, &hypnogram_annot_ov_color[j]);

        hypnogram_param.annot_ov_color[j].r = hypnogram_annot_ov_color[j].red();
        hypnogram_param.annot_ov_color[j].g = hypnogram_annot_ov_color[j].green();
        hypnogram_param.annot_ov_color[j].b = hypnogram_annot_ov_color[j].blue();
        hypnogram_param.annot_ov_color[j].a = hypnogram_annot_ov_color[j].alpha();
      }

      hypnogram_param.mainwindow = this;

      hypnogram_dock[hypnogram_param.instance_num] = new UI_hypnogram_dock(this, hypnogram_param);

      addToolBar(Qt::BottomToolBarArea, hypnogram_dock[hypnogram_param.instance_num]->hypnogram_dock);

      insertToolBarBreak(hypnogram_dock[hypnogram_param.instance_num]->hypnogram_dock);

      hypnogram_param.edfhdr->hypnogram_idx[hypnogram_param.instance_num] = hypnogram_param.instance_num + 1;

      QObject::connect(this, SIGNAL(annot_docklist_changed()), hypnogram_dock[hypnogram_param.instance_num], SLOT(update_curve()));

      xml_go_up(xml_hdl);
    }
    else
    {
      break;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa", i)))
    {
      memset(&cdsa_param, 0, sizeof(cdsa_dock_param_t));

      if(xml_goto_nth_element_inside(xml_hdl, "instance_num", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.instance_num = atoi(result);
        if((cdsa_param.instance_num < 0) || (cdsa_param.instance_num >= MAXCDSADOCKS))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "sigcomp_idx", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        sigcomp_idx = atoi(result);
        if((sigcomp_idx < 0) || (sigcomp_idx >= signalcomps))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        cdsa_param.signalcomp = signalcomp[sigcomp_idx];

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "min_hz", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.min_hz = atoi(result);
        if(cdsa_param.min_hz < 0)
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "max_hz", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.max_hz = atoi(result);
        if(cdsa_param.max_hz < 0)
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "segment_len", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.segment_len = atoi(result);
        if((cdsa_param.segment_len < 5) || (cdsa_param.segment_len > 300))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "block_len", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.block_len = atoi(result);
        if((cdsa_param.block_len < 1) || (cdsa_param.block_len > 10))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "overlap", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.overlap = atoi(result);
        if((cdsa_param.overlap < 1) || (cdsa_param.overlap > 5))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "window_func", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.window_func = atoi(result);
        if((cdsa_param.window_func < 0) || (cdsa_param.window_func > 12))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "max_voltage", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.max_voltage = atof(result);
        if((cdsa_param.max_voltage < 1.0e-7) || (cdsa_param.max_voltage > 1.0e5))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "max_pwr", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.max_pwr = atoi(result);
        if((cdsa_param.max_pwr < -159) || (cdsa_param.max_pwr > 160))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "min_pwr", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.min_pwr = atoi(result);
        if((cdsa_param.min_pwr < -160) || (cdsa_param.min_pwr > 159))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "log", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.log = atoi(result);
        if((cdsa_param.log < 0) || (cdsa_param.log > 1))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "power_voltage", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        cdsa_param.power_voltage = atoi(result);
        if((cdsa_param.power_voltage < 0) || (cdsa_param.power_voltage > 1))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if((!files_open) || (!signalcomps) || (live_stream_active))
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(cdsa_param.signalcomp->ecg_filter != NULL)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(cdsa_param.signalcomp->edfparam_0->sf_int < 30)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(cdsa_param.signalcomp->edfhdr->recording_len_sec < 30)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      for(j=0; j<MAXCDSADOCKS; j++)
      {
        if(cdsa_dock[j] == NULL)
        {
          UI_cdsa_window wndw(this, cdsa_param.signalcomp, j, &cdsa_param);
          break;
        }
      }

      xml_go_up(xml_hdl);
    }
    else
    {
      break;
    }
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg", i)))
    {
      memset(&aeeg_param, 0, sizeof(aeeg_dock_param_t));

      if(xml_goto_nth_element_inside(xml_hdl, "instance_num", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.instance_num = atoi(result);
        if((aeeg_param.instance_num < 0) || (aeeg_param.instance_num >= MAXAEEGDOCKS))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "sigcomp_idx", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        sigcomp_idx = atoi(result);
        if((sigcomp_idx < 0) || (sigcomp_idx >= signalcomps))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        aeeg_param.signalcomp = signalcomp[sigcomp_idx];

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "bp_min_hz", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.bp_min_hz = atof(result);
        if(aeeg_param.bp_min_hz < 1)
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "bp_max_hz", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.bp_max_hz = atof(result);
        if(aeeg_param.bp_max_hz < 2)
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "segment_len", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.segment_len = atoi(result);
        if((aeeg_param.segment_len < 5) || (aeeg_param.segment_len > 300))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "lp_hz", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.lp_hz = atof(result);
        if((aeeg_param.lp_hz < 0.00999) || (aeeg_param.lp_hz > 5.001))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "scale_max_amp", 0))
      {
        return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.scale_max_amp = atof(result);
        if((aeeg_param.scale_max_amp < 9.999) || (aeeg_param.scale_max_amp > 500.001))
        {
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          continue;
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "plot_margins", 0))
      {
        aeeg_param.plot_margins = 1;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return session_format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        aeeg_param.plot_margins = atoi(result);
        if(aeeg_param.plot_margins != 0)
        {
          aeeg_param.plot_margins = 1;
        }

        xml_go_up(xml_hdl);
      }

      if((!files_open) || (!signalcomps) || (live_stream_active))
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(aeeg_param.signalcomp->ecg_filter != NULL)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(aeeg_param.signalcomp->edfparam_0->sf_f < 99.999)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      if(aeeg_param.signalcomp->edfhdr->recording_len_sec < 300)
      {
        xml_go_up(xml_hdl);
        continue;
      }

      for(j=0; j<MAXAEEGDOCKS; j++)
      {
        if(aeeg_dock[j] == NULL)
        {
          UI_aeeg_window wndw(this, aeeg_param.signalcomp, j, &aeeg_param);
          break;
        }
      }

      xml_go_up(xml_hdl);
    }
    else
    {
      break;
    }
  }

  xml_close(xml_hdl);

  setup_viewbuf();

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if((settings[i].signalnr >= 0) && (settings[i].signalnr < MAXSIGNALS) && (settings[i].signalnr < signalcomps))
    {
      spectrumdock[i]->init(settings[i].signalnr);

      spectrumdock[i]->setsettings(settings[i]);
    }
  }

  timescale_doubler = round_125_cat(pagetime);

  for(i=0; i<3; i++)
  {
    amp_cat[i] = 0;
  }

  for(i=0; i<signalcomps; i++)
  {
    tmp = round_125_cat(signalcomp[i]->voltpercm);

    switch(tmp)
    {
      case 10 : amp_cat[0]++;
                break;
      case 20 : amp_cat[1]++;
                break;
      case 50 : amp_cat[2]++;
                break;
    }
  }

  amplitude_doubler = 10;

  if((amp_cat[1] > amp_cat[0]) && (amp_cat[1] >= amp_cat[2]))
  {
    amplitude_doubler = 20;
  }

  if((amp_cat[2] > amp_cat[0]) && (amp_cat[2] > amp_cat[1]))
  {
    amplitude_doubler = 50;
  }

  if(f_ruler_cnt == 1)
  {
    maincurve->ruler_active = 1;
  }

  if(strlen(videopath) > 5)
  {
    session_start_video = 1;

    start_stop_video();
  }

  return 0;
}


int UI_Mainwindow::session_format_error(const char *file_name, int line_number, sigcompblck_t *sigcomp, xml_hdl_t *hdl)
{
  char str1_2048[2048]="";

  if(rc_file_open_err == 103)
  {
    strlcpy(str1_2048, "Attempt to open too many files", 2048);
  }
  else if(rc_file_open_err == 104)
    {
      strlcpy(str1_2048, "File has an unknown extension", 2048);
    }
    else if(rc_file_open_err == 105)
      {
        snprintf(str1_2048, 2048, "Cannot open file for reading:\n  \n%s", path);
      }
      else if(rc_file_open_err == 106)
        {
          strlcpy(str1_2048, "File is not EDF or BDF compliant", 2048);
        }
        else if(rc_file_open_err == 107)
          {
            strlcpy(str1_2048, "File is discontiguous (EDF+D or BDF+D)", 2048);
          }
          else if(rc_file_open_err == 108)
            {
              strlcpy(str1_2048, "File has a formatting error", 2048);
            }
            else
            {
              snprintf(str1_2048, 2048, "There seems to be an error in this session file.\nFile: %s\nline: %i", file_name, line_number);
            }

  if(!rc_cmd_in_progress)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
    messagewindow.exec();
  }
  else
  {
    printf("%s\n", str1_2048);
    rc_load_session_err = rc_file_open_err;
    if(!rc_load_session_err)
    {
      rc_load_session_err = 108;
    }
  }
  free(sigcomp);
  xml_close(hdl);
  return 0;
}















