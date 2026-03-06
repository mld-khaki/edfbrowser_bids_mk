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


void UI_Mainwindow::get_rgbcolor_settings(xml_hdl_t *xml_hdl, const char *id, int cnt, QColor *rgb_color)
{
  char result[XML_STRBUFLEN];

  QColor tmp_color;


  if(xml_goto_nth_element_inside(xml_hdl, id, cnt))
  {
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "red", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setRed(atoi(result));

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "green", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setGreen(atoi(result));

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "blue", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setBlue(atoi(result));

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "alpha", 0))
  {
    if(!xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      tmp_color.setAlpha(atoi(result));
    }

    xml_go_up(xml_hdl);
  }

  *rgb_color = tmp_color;

  xml_go_up(xml_hdl);
}


void UI_Mainwindow::read_color_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH]={""},
       result[XML_STRBUFLEN]={""};

  xml_hdl_t *xml_hdl=NULL;

#ifdef Q_OS_WIN32
  strlcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\settings.xml", MAX_PATH_LENGTH);
#else
  strlcpy(cfg_path, getenv("HOME"), MAX_PATH_LENGTH);
  strlcat(cfg_path, "/.", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "/settings.xml", MAX_PATH_LENGTH);
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(!xml_hdl)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "colors", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  get_rgbcolor_settings(xml_hdl, "backgroundcolor", 0, &maincurve->backgroundcolor);

  get_rgbcolor_settings(xml_hdl, "small_ruler_color", 0, &maincurve->small_ruler_color);

  get_rgbcolor_settings(xml_hdl, "big_ruler_color", 0, &maincurve->big_ruler_color);

  get_rgbcolor_settings(xml_hdl, "mouse_rect_color", 0, &maincurve->mouse_rect_color);

  get_rgbcolor_settings(xml_hdl, "text_color", 0, &maincurve->text_color);

  get_rgbcolor_settings(xml_hdl, "baseline_color", 0, &maincurve->baseline_color);

  get_rgbcolor_settings(xml_hdl, "annot_marker_color", 0, &maincurve->annot_marker_color);

  get_rgbcolor_settings(xml_hdl, "annot_marker_selected_color", 0, &maincurve->annot_marker_selected_color);

  get_rgbcolor_settings(xml_hdl, "annot_duration_color", 0, &maincurve->annot_duration_color);

  get_rgbcolor_settings(xml_hdl, "annot_duration_color_selected", 0, &maincurve->annot_duration_color_selected);

  if(!xml_goto_nth_element_inside(xml_hdl, "annot_ov_predefined_block", 0))
  {
    for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
    {
      get_rgbcolor_settings(xml_hdl, "mc_annot_ov_color_predefined", i, &mc_annot_ov_color_predefined[i]);
    }

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "signal_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->signal_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "floating_ruler_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->floating_ruler_color = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "floating_ruler_use_var_width", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    maincurve->floating_ruler_use_var_width = atoi(result);
    if((maincurve->floating_ruler_use_var_width < 0) || (maincurve->floating_ruler_use_var_width > 1))
    {
      maincurve->floating_ruler_use_var_width = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "blackwhite_printing", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->blackwhite_printing = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_annot_markers", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  show_annot_markers = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "show_annot_duration", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    annotations_show_duration = atoi(result);

    xml_go_up(xml_hdl);
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "annotations_duration_background_type", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    annotations_duration_background_type = atoi(result);

    xml_go_up(xml_hdl);
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "channel_linked_annotations", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    channel_linked_annotations = atoi(result);

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "show_baselines", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  show_baselines = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_1_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->crosshair_1.color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_2_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->crosshair_2.color = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_1_has_hor_line", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    maincurve->crosshair_1.has_hor_line = atoi(result);
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_2_has_hor_line", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    maincurve->crosshair_2.has_hor_line = atoi(result);
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_1_dot_sz", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    maincurve->crosshair_1.dot_sz = atoi(result);
    if(maincurve->crosshair_1.dot_sz < 0)
    {
      maincurve->crosshair_1.dot_sz = 0;
    }
    if(maincurve->crosshair_1.dot_sz > 32)
    {
      maincurve->crosshair_1.dot_sz = 32;
    }
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_2_dot_sz", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    maincurve->crosshair_2.dot_sz = atoi(result);
    if(maincurve->crosshair_2.dot_sz < 0)
    {
      maincurve->crosshair_2.dot_sz = 0;
    }
    if(maincurve->crosshair_2.dot_sz > 32)
    {
      maincurve->crosshair_2.dot_sz = 32;
    }
  }

  xml_go_up(xml_hdl);

  xml_close(xml_hdl);

//  printf("Read all color settings.\n");
}


void UI_Mainwindow::read_recent_file_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH]={""},
       result[XML_STRBUFLEN]={""};

  xml_hdl_t *xml_hdl=NULL;

  QAction *act=NULL;

#ifdef Q_OS_WIN32
  strlcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\settings.xml", MAX_PATH_LENGTH);
#else
  strlcpy(cfg_path, getenv("HOME"), MAX_PATH_LENGTH);
  strlcat(cfg_path, "/.", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "/settings.xml", MAX_PATH_LENGTH);
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(xml_hdl==NULL)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_montagedir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(recent_montagedir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_sessiondir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(recent_sessiondir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_savedir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(recent_savedir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_opendir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(recent_opendir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_video_opendir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(recent_video_opendir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_colordir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    strlcpy(recent_colordir, result, MAX_PATH_LENGTH);
    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_file", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    if(result[0] != 0)
    {
      strlcpy(&recent_file_path[0][0], result, MAX_PATH_LENGTH);
      act = new QAction(QString::fromLocal8Bit(&recent_file_path[0][0]), recent_filesmenu);
      act->setData(QVariant(0));
      recent_filesmenu->addAction(act);

      for(i=1; i<MAX_RECENTFILES; i++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          break;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }
        if(result[0] == 0)
        {
          break;
        }
        strlcpy(&recent_file_path[i][0], result, MAX_PATH_LENGTH);
        act = new QAction(QString::fromLocal8Bit(&recent_file_path[i][0]), recent_filesmenu);
        act->setData(QVariant(i));
        recent_filesmenu->addAction(act);
      }

      recent_filesmenu->addSeparator();
      act = new QAction("Clear list", recent_filesmenu);
      act->setData(QVariant(MAX_RECENTFILES));
      recent_filesmenu->addAction(act);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_file_mtg", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    strlcpy(&recent_file_mtg_path[0][0], result, MAX_PATH_LENGTH);
    for(i=1; i<MAX_RECENTFILES; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        break;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }
      strlcpy(&recent_file_mtg_path[i][0], result, MAX_PATH_LENGTH);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_session_file", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    if(result[0] != 0)
    {
      strlcpy(&recent_session_path[0][0], result, MAX_PATH_LENGTH);
      act = new QAction(QString::fromLocal8Bit(&recent_session_path[0][0]), recent_session_menu);
      act->setData(QVariant(0));
      recent_session_menu->addAction(act);

      for(i=1; i<MAX_RECENTFILES; i++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          break;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }
        if(result[0] == 0)
        {
          break;
        }
        strlcpy(&recent_session_path[i][0], result, MAX_PATH_LENGTH);
        act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
        act->setData(QVariant(i));
        recent_session_menu->addAction(act);
      }

      recent_session_menu->addSeparator();
      act = new QAction("Clear list", recent_session_menu);
      act->setData(QVariant(MAX_RECENTFILES));
      recent_session_menu->addAction(act);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "predefined_mtg_path", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    strlcpy(&predefined_mtg_path[0][0], result, MAX_PATH_LENGTH);

    for(i=1; i < MAXPREDEFINEDMONTAGES; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        break;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        break;
      }
      strlcpy(&predefined_mtg_path[i][0], result, MAX_PATH_LENGTH);
    }

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

//  printf("Read all recent file settings.\n");
}


void UI_Mainwindow::read_general_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH]={""},
       result[XML_STRBUFLEN]={""};

  xml_hdl_t *xml_hdl=NULL;

#ifdef Q_OS_WIN32
  strlcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\settings.xml", MAX_PATH_LENGTH);
#else
  strlcpy(cfg_path, getenv("HOME"), MAX_PATH_LENGTH);
  strlcat(cfg_path, "/.", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  strlcat(cfg_path, "/settings.xml", MAX_PATH_LENGTH);
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(xml_hdl==NULL)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "cfg_app_version", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strlcpy(cfg_app_version_str_17, result, 17);

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "font_size", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    font_size = atoi(result);
    if(font_size < 8)  font_size = 8;
    if(font_size > 24)  font_size = 24;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "monofont_size", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    monofont_size = atoi(result);
    if(monofont_size < 8)  monofont_size = 8;
    if(monofont_size > 24)  monofont_size = 24;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "window_geometry", 0)))
  {
    if(xml_goto_nth_element_inside(xml_hdl, "window_width_sav_rest", 0))
    {
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    window_width_sav_rest = atoi(result);
    if((window_width_sav_rest < 640) || (window_width_sav_rest > 100000))
    {
      window_width_sav_rest = 0;
    }

    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "window_height_sav_rest", 0))
    {
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    window_height_sav_rest = atoi(result);
    if((window_height_sav_rest < 480) || (window_height_sav_rest > 100000))
    {
      window_height_sav_rest = 0;
      window_width_sav_rest = 0;
    }

    xml_go_up(xml_hdl);
  }
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "pixelsizefactor", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }

  y_pixelsizefactor = atof(result);
  if(y_pixelsizefactor < 0.00001)
  {
    y_pixelsizefactor = 0.0294382;
  }

  xml_go_up(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "auto_dpi", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    auto_dpi = atoi(result);
    if((auto_dpi < 0) || (auto_dpi > 1))
    {
      auto_dpi = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "x_pixelsizefactor", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    x_pixelsizefactor = atof(result);
    if(x_pixelsizefactor < 0.00001)
    {
      x_pixelsizefactor = 0.0294382;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "ecg_view_mode", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    ecg_view_mode = atoi(result);
    if(ecg_view_mode != 1)
    {
      ecg_view_mode = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "clip_to_pane", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    clip_to_pane = atoi(result);
    if(clip_to_pane != 1)
    {
      clip_to_pane = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "signal_plotorder", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    signal_plotorder = atoi(result);
    if(signal_plotorder != 1)
    {
      signal_plotorder = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "auto_reload_mtg", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    auto_reload_mtg = atoi(result);
    if(auto_reload_mtg)
    {
      auto_reload_mtg = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "read_biosemi_status_signal", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    read_biosemi_status_signal = atoi(result);
    if(read_biosemi_status_signal)
    {
      read_biosemi_status_signal = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "read_nk_trigger_signal", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    read_nk_trigger_signal = atoi(result);
    if(read_nk_trigger_signal)
    {
      read_nk_trigger_signal = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "maxfilesize_to_readin_annotations", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    maxfilesize_to_readin_annotations = atoll(result);
    if((maxfilesize_to_readin_annotations < 104857600LL) || (maxfilesize_to_readin_annotations > 10485760000LL))
    {
      maxfilesize_to_readin_annotations = 10485760000LL;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "use_threads", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    use_threads = atoi(result);
    if(use_threads)
    {
      use_threads = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "session_relative_paths", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    session_relative_paths = atoi(result);
    if(session_relative_paths != 1)
    {
      session_relative_paths = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "edf_debug", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    edf_debug = atoi(result);
    if(edf_debug != 1)
    {
      edf_debug = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "strip_label_types", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strip_label_types = atoi(result);
    if(strip_label_types)
    {
      strip_label_types = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_invert_eeg_signals", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_invert_eeg_signals = atoi(result);
    if(default_invert_eeg_signals != 1)
    {
      default_invert_eeg_signals = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_time_scale", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_time_scale = atoi(result);
    if((default_time_scale < 0) || (default_time_scale > 86400))
    {
      default_time_scale = 10;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_fit_signals_to_pane", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_fit_signals_to_pane = atoi(result);
    if(default_fit_signals_to_pane != 1)
    {
      default_fit_signals_to_pane = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrummarkerblock", 0)))
  {
    if(xml_goto_nth_element_inside(xml_hdl, "items", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > MAXSPECTRUMMARKERS) || (atoi(result) < 0))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->items = atoi(result);

    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "method", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > 2) || (atoi(result) < 0))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->method = atoi(result);

    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "auto_adjust", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->auto_adjust = atoi(result);

      if((spectrum_colorbar->auto_adjust > 1) || (spectrum_colorbar->auto_adjust < 0))
      {
        spectrum_colorbar->auto_adjust = 1;
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "max_colorbar_value", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->max_colorbar_value = atof(result);

      if((spectrum_colorbar->max_colorbar_value > 100000.0) || (spectrum_colorbar->max_colorbar_value < 0.0001))
      {
        spectrum_colorbar->max_colorbar_value = 1.0;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atof(result) > 1000000.0) || (atof(result) < 0.00001))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->freq[0] = atof(result);

    for(i=1; i < spectrum_colorbar->items; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        xml_close(xml_hdl);
        return;
      }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(i < spectrum_colorbar->items)
      {
        if((atof(result) > 1000000.0) || (atof(result) <= spectrum_colorbar->freq[i-1]))
        {
          xml_close(xml_hdl);
          return;
        }
      }

      spectrum_colorbar->freq[i] = atof(result);
    }

    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > 18) || (atoi(result) < 2))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->color[0] = atoi(result);

    for(i=1; i < spectrum_colorbar->items; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        xml_close(xml_hdl);
        return;
      }

      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if((atoi(result) > 18) || (atoi(result) < 2))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->color[i] = atoi(result);
    }

    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "label", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strlcpy(spectrum_colorbar->label[0], result, 17);

      for(i=1; i < spectrum_colorbar->items; i++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          xml_close(xml_hdl);
          return;
        }

        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(spectrum_colorbar->label[i], result, 17);
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "colors", 0))
  {
    if(!xml_goto_nth_element_inside(xml_hdl, "annot_ov_predefined_block", 0))
    {
      for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
      {
        if(!xml_goto_nth_element_inside(xml_hdl, "mc_annot_ov_name_predefined", i))
        {
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            xml_close(xml_hdl);
            return;
          }

          strlcpy(mc_annot_ov_name_predefined[i], result, 32);

          xml_go_up(xml_hdl);
        }
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotations_import_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "format", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->format = atoi(result);

      if((import_annotations_var->format < 0) || (import_annotations_var->format > 4))
      {
        import_annotations_var->format = 1;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "onsettimeformat", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->onsettimeformat = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "onsetcolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->onsetcolumn = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "descriptioncolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->descriptioncolumn = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "useduration", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->useduration = atoi(result);

      if(import_annotations_var->useduration != 1)
      {
        import_annotations_var->useduration = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "usestoptime", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->usestoptime = atoi(result);

      if(import_annotations_var->usestoptime != 1)
      {
        import_annotations_var->usestoptime = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "durationcolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->durationcolumn = atoi(result);

      if(import_annotations_var->durationcolumn < 0)
      {
        import_annotations_var->durationcolumn = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "stopcolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->stopcolumn = atoi(result);

      if(import_annotations_var->stopcolumn < 0)
      {
        import_annotations_var->stopcolumn = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "datastartline", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->datastartline = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "separator", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }
      strlcpy(import_annotations_var->separator, result, 4);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ascii_txt_encoding", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }
      import_annotations_var->ascii_txt_encoding = atoi(result);
      if((import_annotations_var->ascii_txt_encoding < 0) || (import_annotations_var->ascii_txt_encoding > 1))
      {
        import_annotations_var->ascii_txt_encoding = 0;
      }
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "csv_equal_filename", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }
      import_annotations_var->csv_equal_filename = atoi(result);
      if((import_annotations_var->csv_equal_filename < 0) || (import_annotations_var->csv_equal_filename > 1))
      {
        import_annotations_var->csv_equal_filename = 0;
      }
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "dceventbittime", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->dceventbittime = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "triggerlevel", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->triggerlevel = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "manualdescription", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->manualdescription = atoi(result);

      if((import_annotations_var->manualdescription < 0) || (import_annotations_var->manualdescription > 1))
      {
        import_annotations_var->manualdescription = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "description", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strlcpy(import_annotations_var->description, result, 21);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ignoreconsecutive", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->ignoreconsecutive = atoi(result);

      if(import_annotations_var->ignoreconsecutive != 1)
      {
        import_annotations_var->ignoreconsecutive = 0;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotations_export_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "separator", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->separator = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "format", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->format = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "duration", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->duration = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "txt_encoding", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->txt_encoding = atoi(result);
      if((export_annotations_var->txt_encoding < 0) || (export_annotations_var->txt_encoding > 1))
      {
        export_annotations_var->txt_encoding = 0;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_segmentlen", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_segmentlen = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_blocklen", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_blocklen = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_overlap", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_overlap = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_window_func", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_window_func = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_min_hz", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_min_hz = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_max_hz", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_max_hz = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_max_pwr", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_max_pwr = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_min_pwr", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_min_pwr = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_log", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_log = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_pwr_voltage", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_pwr_voltage = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "cdsa_max_voltage", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      cdsa_max_voltage = atof(result);

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_segmentlen", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_segmentlen = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_bp_min_hz", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_bp_min_hz = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_bp_max_hz", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_bp_max_hz = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_lp_hz", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_lp_hz = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_scale_max_amp", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_scale_max_amp = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeegdock_height", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeegdock_height = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "aeeg_plot_margins", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      aeeg_plot_margins = atoi(result);
      if(aeeg_plot_margins != 0)
      {
        aeeg_plot_margins = 1;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotation_editor", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_highres", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_highres = 1;
      }
      else
      {
        annot_editor_highres = 0;
      }

      xml_go_up(xml_hdl);
    }

    for(i=0; i<8; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_edit_user_button_enabled", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        if(atoi(result) == 1)
        {
          annot_edit_user_button_enabled[i] = 1;
        }
        else
        {
          annot_edit_user_button_enabled[i] = 0;
        }

        xml_go_up(xml_hdl);
      }

      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_onset_on_page_middle", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        if(atoi(result) == 1)
        {
          annot_editor_user_button_onset_on_page_middle[i] = 1;
        }
        else
        {
          annot_editor_user_button_onset_on_page_middle[i] = 0;
        }

        xml_go_up(xml_hdl);
      }
    }

    for(i=0; i<8; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_edit_user_button_name", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(annot_edit_user_button_name[i], result, 32);

        xml_go_up(xml_hdl);
      }
    }

    for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_by_rect_draw_description", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(annot_by_rect_draw_description[i], result, 32);

        xml_go_up(xml_hdl);
      }
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_update_annot_description", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_user_button_update_annot_description = 1;
      }
      else
      {
        annot_editor_user_button_update_annot_description = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_update_annot_onset", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_user_button_update_annot_onset = 1;
      }
      else
      {
        annot_editor_user_button_update_annot_onset = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_update_annot_duration", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_user_button_update_annot_duration = 1;
      }
      else
      {
        annot_editor_user_button_update_annot_duration = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_jump_to_next_page", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_user_button_jump_to_next_page = 1;
      }
      else
      {
        annot_editor_user_button_jump_to_next_page = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_stay_on_epoch_boundary", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        annot_editor_user_button_stay_on_epoch_boundary = 1;
      }
      else
      {
        annot_editor_user_button_stay_on_epoch_boundary = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_epoch_len", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_editor_user_button_epoch_len = atoll(result);
      if((annot_editor_user_button_epoch_len < TIME_FIXP_SCALING) || (annot_editor_user_button_epoch_len > (300LL * TIME_FIXP_SCALING)))
      {
        annot_editor_user_button_epoch_len = 30LL * TIME_FIXP_SCALING;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "annot_editor_user_button_page_len", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_editor_user_button_page_len = atoll(result);
      if((annot_editor_user_button_page_len < TIME_FIXP_SCALING) || (annot_editor_user_button_page_len > (300LL * TIME_FIXP_SCALING)))
      {
        annot_editor_user_button_page_len = 30LL * TIME_FIXP_SCALING;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "hypnogram", 0)))
  {
    for(i=0; i<6; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "stage_name", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(hypnogram_stage_name[i], result, 32);

        xml_go_up(xml_hdl);
      }
    }

    for(i=0; i<HYPNOGRAM_STAGENUM; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_name", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(hypnogram_annot_name[i], result, 32);

        xml_go_up(xml_hdl);
      }
    }

    for(i=0; i<HYPNOGRAM_STAGENUM; i++)
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "annot_ov_name", i)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strlcpy(hypnogram_annot_ov_name[i], result, 32);

        xml_go_up(xml_hdl);
      }
    }

    for(i=0; i<HYPNOGRAM_OVNUM; i++)
    {
      get_rgbcolor_settings(xml_hdl, "annot_ov_color", i, &hypnogram_annot_ov_color[i]);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hypnogram_use_epoch_len", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hypnogram_use_epoch_len = atoi(result);
      if(hypnogram_use_epoch_len != 1)
      {
        hypnogram_use_epoch_len = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "use_overlays", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hypnogram_use_overlays = atoi(result);
      if(hypnogram_use_overlays != 1)
      {
        hypnogram_use_overlays = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hypnogramdock_height", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hypnogramdock_height = atoi(result);
      if((hypnogramdock_height < 80) || (hypnogramdock_height > 500))
      {
        hypnogramdock_height = 150;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "ecg_qrs", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "r_peak_description", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      trim_spaces(result);
      if(strlen(result))
      {
        strlcpy(ecg_qrs_rpeak_descr, result, 32);
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "use_signallabel_in_annot_descr", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      use_signallabel_in_annot_descr = atoi(result);
      if(use_signallabel_in_annot_descr != 1)
      {
        use_signallabel_in_annot_descr = 0;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "hrvdock", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "hrvdock_max_bpm", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hrvdock_max_bpm = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hrvdock_min_bpm", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hrvdock_min_bpm = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hrvdock_height", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      hrvdock_height = atoi(result);

      xml_go_up(xml_hdl);
    }

    get_rgbcolor_settings(xml_hdl, "hrvdock_trace_color", 0, &hrvdock_trace_color);

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotlistdock", 0)))
  {
    get_rgbcolor_settings(xml_hdl, "annot_list_edited_txt_color", 0, &annot_list_edited_txt_color);

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "live_stream_update_interval", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    live_stream_update_interval = atoi(result);
    if((live_stream_update_interval < 100) || (live_stream_update_interval > 3000))
    {
      live_stream_update_interval = 500;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "powerlinefreq", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    powerlinefreq = atoi(result);
    if((powerlinefreq != 50) && (powerlinefreq != 60))
    {
      powerlinefreq = 50;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mousewheelsens", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mousewheelsens = atoi(result);
    if((mousewheelsens < 0) || (mousewheelsens > 100))
    {
      mousewheelsens = 10;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_period", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    average_period = atoi(result);

    if(average_period < 1)  average_period = 300;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_ratio", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    average_ratio = atoi(result);
    if((average_ratio < 0) || (average_ratio > 2))
    {
      average_ratio = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_upsidedown", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      average_upsidedown = 1;
    }
    else
    {
      average_upsidedown = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_bw", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      average_bw = 1;
    }
    else
    {
      average_bw = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_bw", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_bw = 1;
    }
    else
    {
      spectrum_bw = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_sqrt", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_sqrt = 1;
    }
    else
    {
      spectrum_sqrt = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_vlog", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_vlog = 1;
    }
    else
    {
      spectrum_vlog = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_window", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_window = atoi(result);
    if((spectrum_window < 0) || (spectrum_window > 12))
    {
      spectrum_window = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_blocksize_predefined", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_blocksize_predefined = atoi(result);
    if((spectrum_blocksize_predefined < 0) || (spectrum_blocksize_predefined > 23))
    {
      spectrum_blocksize_predefined = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_blocksize_userdefined", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_blocksize_userdefined = atoi(result);
    if((spectrum_blocksize_userdefined < 10) || (spectrum_blocksize_userdefined > 1000))
    {
      spectrum_blocksize_userdefined = 200;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_overlap", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_overlap = atoi(result);
    if((spectrum_overlap < 0) || (spectrum_overlap > 4))
    {
      spectrum_overlap = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_sqrt", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_sqrt = 1;
    }
    else
    {
      spectrumdock_sqrt = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_vlog", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_vlog = 1;
    }
    else
    {
      spectrumdock_vlog = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_window", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrumdock_window = atoi(result);
    if((spectrumdock_window < 0) || (spectrumdock_window > 12))
    {
      spectrumdock_window = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_blocksize_predefined", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrumdock_blocksize_predefined = atoi(result);
    if((spectrumdock_blocksize_predefined < 0) || (spectrumdock_blocksize_predefined > 23))
    {
      spectrumdock_blocksize_predefined = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_blocksize_userdefined", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrumdock_blocksize_userdefined = atoi(result);
    if((spectrumdock_blocksize_userdefined < 10) || (spectrumdock_blocksize_userdefined > 1000))
    {
      spectrumdock_blocksize_userdefined = 200;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_overlap", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrumdock_overlap = atoi(result);
    if((spectrumdock_overlap < 0) || (spectrumdock_overlap > 4))
    {
      spectrumdock_overlap = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_colorbars", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_colorbars = 1;
    }
    else
    {
      spectrumdock_colorbars = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.zscore_page_len", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.zscore_page_len = atoi(result);

    if((z_score_var.zscore_page_len < 10) || (z_score_var.zscore_page_len > 60))
    {
      z_score_var.zscore_page_len = 30;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.z_threshold", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.z_threshold = atof(result);

    if((z_score_var.z_threshold < -0.5) || (z_score_var.z_threshold > 0.5))
    {
      z_score_var.z_threshold = 0.0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.crossoverfreq", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.crossoverfreq = atof(result);

    if((z_score_var.crossoverfreq < 5.0) || (z_score_var.crossoverfreq > 9.5))
    {
      z_score_var.crossoverfreq = 7.5;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.z_hysteresis", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.z_hysteresis = atof(result);

    if((z_score_var.z_hysteresis < 0.0) || (z_score_var.z_hysteresis > 0.25))
    {
      z_score_var.z_hysteresis = 0.0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.zscore_error_detection", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.zscore_error_detection = atoi(result);

    if((z_score_var.zscore_error_detection < 50) || (z_score_var.zscore_error_detection > 100))
    {
      z_score_var.zscore_error_detection = 50;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "raw2edf_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "sf", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.sf = atoi(result);
      if(raw2edf_var.sf < 1)  raw2edf_var.sf = 1;
      if(raw2edf_var.sf > 1000000)  raw2edf_var.sf = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "chns", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.chns = atoi(result);
      if(raw2edf_var.chns < 1)  raw2edf_var.chns = 1;
      if(raw2edf_var.chns > 256)  raw2edf_var.chns = 256;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "phys_max", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.phys_max = atoi(result);
      if(raw2edf_var.phys_max < 1)  raw2edf_var.phys_max = 1;
      if(raw2edf_var.phys_max > 10000000)  raw2edf_var.phys_max = 10000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "straightbinary", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.straightbinary = atoi(result);
      if(raw2edf_var.straightbinary < 0)  raw2edf_var.straightbinary = 0;
      if(raw2edf_var.straightbinary > 1)  raw2edf_var.straightbinary = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "endianness", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.endianness = atoi(result);
      if(raw2edf_var.endianness < 0)  raw2edf_var.endianness = 0;
      if(raw2edf_var.endianness > 1)  raw2edf_var.endianness = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "samplesize", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.samplesize = atoi(result);
      if(raw2edf_var.samplesize < 0)  raw2edf_var.samplesize = 0;
      if(raw2edf_var.samplesize > 2)  raw2edf_var.samplesize = 2;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "offset", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.offset = atoi(result);
      if(raw2edf_var.offset < 0)  raw2edf_var.offset = 0;
      if(raw2edf_var.offset > 1000000)  raw2edf_var.offset = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "skipblocksize", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.skipblocksize = atoi(result);
      if(raw2edf_var.skipblocksize < 0)  raw2edf_var.skipblocksize = 0;
      if(raw2edf_var.skipblocksize > 1000000)  raw2edf_var.skipblocksize = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "skipbytes", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.skipbytes = atoi(result);
      if(raw2edf_var.skipbytes < 1)  raw2edf_var.skipbytes = 1;
      if(raw2edf_var.skipbytes > 1000000)  raw2edf_var.skipbytes = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "phys_dim", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strlcpy(raw2edf_var.phys_dim, result, 16);
      latin1_to_ascii(raw2edf_var.phys_dim, strlen(raw2edf_var.phys_dim));
      trim_spaces(raw2edf_var.phys_dim);

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotfilter_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "tmin", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->tmin = atoi(result);
      if(annot_filter->tmin < 1)  annot_filter->tmin = 1;
      if(annot_filter->tmin > 500000)  annot_filter->tmin = 500000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "tmax", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->tmax = atoi(result);
      if(annot_filter->tmax < 1)  annot_filter->tmax = 1;
      if(annot_filter->tmax > 500000)  annot_filter->tmax = 500000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "invert", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->invert = atoi(result);
      if(annot_filter->invert < 0)  annot_filter->invert = 0;
      if(annot_filter->invert > 1)  annot_filter->invert = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hide_other", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->hide_other = atoi(result);
      if(annot_filter->hide_other < 0)  annot_filter->hide_other = 0;
      if(annot_filter->hide_other > 1)  annot_filter->hide_other = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hide_in_list_only", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->hide_in_list_only = atoi(result);
      if(annot_filter->hide_in_list_only < 0)  annot_filter->hide_in_list_only = 0;
      if(annot_filter->hide_in_list_only > 1)  annot_filter->hide_in_list_only = 1;

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "check_for_updates", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    check_for_updates = atoi(result);
    if((check_for_updates < 0) || (check_for_updates > 1))  check_for_updates = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "last_check_for_updates_time", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    last_check_for_updates_time = atoll(result);
    if(last_check_for_updates_time < 0LL)  last_check_for_updates_time = 0LL;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "viewtime_indicator_type", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    viewtime_indicator_type = atoi(result);
    if((viewtime_indicator_type < 0) || (viewtime_indicator_type > 2))  viewtime_indicator_type = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "display_pagetime_mmsec", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    display_pagetime_mmsec = atoi(result);
    if(display_pagetime_mmsec != 0)  display_pagetime_mmsec = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mc_v_scrollarea_auto", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mc_v_scrollarea_auto = atoi(result);
    if(mc_v_scrollarea_auto != 1)  mc_v_scrollarea_auto = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mc_v_scrollarea_max_signals", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mc_v_scrollarea_max_signals = atoi(result);
    if(mc_v_scrollarea_max_signals > 64)  mc_v_scrollarea_max_signals = 64;
    if(mc_v_scrollarea_max_signals < 1)  mc_v_scrollarea_max_signals = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mainwindow_title_type", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mainwindow_title_type = atoi(result);
    if((mainwindow_title_type < 0) || (mainwindow_title_type > 2))  mainwindow_title_type = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_amplitude", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_amplitude = atof(result);
    if((default_amplitude < 0.001) || (default_amplitude > 10000000))  default_amplitude = 100;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_amplitude_physmax_div", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_amplitude_physmax_div = atoi(result);
    if((default_amplitude_physmax_div < 1) || (default_amplitude_physmax_div > 100))  default_amplitude_physmax_div = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_amplitude_use_physmax_div", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_amplitude_use_physmax_div = atoi(result);
    if(default_amplitude_use_physmax_div != 1)  default_amplitude_use_physmax_div = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "linear_interpolation", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    linear_interpol = atoi(result);
    if(linear_interpol != 1)
    {
      linear_interpol = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annot_onset_at_start_of_page_on_jump", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    annot_onset_at_start_of_page_on_jump = atoi(result);
    if(annot_onset_at_start_of_page_on_jump != 1)
    {
      annot_onset_at_start_of_page_on_jump = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotlist_scrolltoitem_while_browsing", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    annotlist_scrolltoitem_while_browsing = atoi(result);
    if(annotlist_scrolltoitem_while_browsing != 1)
    {
      annotlist_scrolltoitem_while_browsing = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "auto_update_annot_onset", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    auto_update_annot_onset = atoi(result);
    if(auto_update_annot_onset != 1)
    {
      auto_update_annot_onset = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "use_diverse_signal_colors", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    use_diverse_signal_colors = atoi(result);
    if(use_diverse_signal_colors != 1)
    {
      use_diverse_signal_colors = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "rc_host_server_public", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    rc_host_server_public = atoi(result);
    if(rc_host_server_public != 1)
    {
      rc_host_server_public = 0;
    }

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

//  printf("Read all general settings.\n");
}


void UI_Mainwindow::write_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH]={""},
       str1_1024[1024]={""};

  FILE *cfgfile=NULL;

#ifdef Q_OS_WIN32
  strlcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(cfg_path, "\\", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  mkdir(cfg_path);
  strlcat(cfg_path, "\\settings.xml", MAX_PATH_LENGTH);
#else
  strlcpy(cfg_path, getenv("HOME"), MAX_PATH_LENGTH);
  strlcat(cfg_path, "/.", MAX_PATH_LENGTH);
  strlcat(cfg_path, PROGRAM_NAME, MAX_PATH_LENGTH);
  mkdir(cfg_path,  S_IRWXU);
  strlcat(cfg_path, "/settings.xml", MAX_PATH_LENGTH);
#endif

  cfgfile = fopeno(cfg_path, "wb");
  if(cfgfile)
  {
    fprintf(cfgfile, "<?xml version=\"1.0\"?>\n"
                     "<config>\n"
                     "  <cfg_app_version>" PROGRAM_NAME " " PROGRAM_VERSION "</cfg_app_version>\n"
                     "  <UI>\n"
                     "    <font_size>%i</font_size>\n"
                     "    <monofont_size>%i</monofont_size>\n", font_size, monofont_size);

    fprintf(cfgfile, "    <window_geometry>\n"
                     "      <window_width_sav_rest>%i</window_width_sav_rest>\n"
                     "      <window_height_sav_rest>%i</window_height_sav_rest>\n"
                     "    </window_geometry>\n", window_width_sav_rest, window_height_sav_rest);

    fprintf(cfgfile, "    <colors>\n");

    fprintf(cfgfile, "      <backgroundcolor>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </backgroundcolor>\n",
                    maincurve->backgroundcolor.red(),
                    maincurve->backgroundcolor.green(),
                    maincurve->backgroundcolor.blue());

    fprintf(cfgfile, "      <small_ruler_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </small_ruler_color>\n",
                    maincurve->small_ruler_color.red(),
                    maincurve->small_ruler_color.green(),
                    maincurve->small_ruler_color.blue());

    fprintf(cfgfile, "      <big_ruler_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </big_ruler_color>\n",
                    maincurve->big_ruler_color.red(),
                    maincurve->big_ruler_color.green(),
                    maincurve->big_ruler_color.blue());

    fprintf(cfgfile, "      <mouse_rect_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </mouse_rect_color>\n",
                    maincurve->mouse_rect_color.red(),
                    maincurve->mouse_rect_color.green(),
                    maincurve->mouse_rect_color.blue());

    fprintf(cfgfile, "      <text_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </text_color>\n",
                    maincurve->text_color.red(),
                    maincurve->text_color.green(),
                    maincurve->text_color.blue());

    fprintf(cfgfile, "      <baseline_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </baseline_color>\n",
                    maincurve->baseline_color.red(),
                    maincurve->baseline_color.green(),
                    maincurve->baseline_color.blue());

    fprintf(cfgfile, "      <annot_marker_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </annot_marker_color>\n",
                    maincurve->annot_marker_color.red(),
                    maincurve->annot_marker_color.green(),
                    maincurve->annot_marker_color.blue());

    fprintf(cfgfile, "      <annot_marker_selected_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </annot_marker_selected_color>\n",
                    maincurve->annot_marker_selected_color.red(),
                    maincurve->annot_marker_selected_color.green(),
                    maincurve->annot_marker_selected_color.blue());

    fprintf(cfgfile, "      <annot_duration_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "        <alpha>%i</alpha>\n"
                    "      </annot_duration_color>\n",
                    maincurve->annot_duration_color.red(),
                    maincurve->annot_duration_color.green(),
                    maincurve->annot_duration_color.blue(),
                    maincurve->annot_duration_color.alpha());

    fprintf(cfgfile, "      <annot_duration_color_selected>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "        <alpha>%i</alpha>\n"
                    "      </annot_duration_color_selected>\n",
                    maincurve->annot_duration_color_selected.red(),
                    maincurve->annot_duration_color_selected.green(),
                    maincurve->annot_duration_color_selected.blue(),
                    maincurve->annot_duration_color_selected.alpha());

    fprintf(cfgfile, "      <annot_ov_predefined_block>\n");

    for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
    {
      fprintf(cfgfile, "        <mc_annot_ov_color_predefined>\n"
                       "          <red>%i</red>\n"
                       "          <green>%i</green>\n"
                       "          <blue>%i</blue>\n"
                       "          <alpha>%i</alpha>\n"
                       "        </mc_annot_ov_color_predefined>\n",
                       mc_annot_ov_color_predefined[i].red(),
                       mc_annot_ov_color_predefined[i].green(),
                       mc_annot_ov_color_predefined[i].blue(),
                       mc_annot_ov_color_predefined[i].alpha());
    }

    for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, mc_annot_ov_name_predefined[i], 1024);

      fprintf(cfgfile, "        <mc_annot_ov_name_predefined>%s</mc_annot_ov_name_predefined>\n", str1_1024);
    }

    fprintf(cfgfile, "      </annot_ov_predefined_block>\n");

    fprintf(cfgfile, "      <signal_color>%i</signal_color>\n",
                    maincurve->signal_color);

    fprintf(cfgfile, "      <crosshair_1_color>%i</crosshair_1_color>\n",
                    maincurve->crosshair_1.color);

    fprintf(cfgfile, "      <crosshair_2_color>%i</crosshair_2_color>\n",
                    maincurve->crosshair_2.color);

    fprintf(cfgfile, "      <crosshair_1_has_hor_line>%i</crosshair_1_has_hor_line>\n",
                    maincurve->crosshair_1.has_hor_line);

    fprintf(cfgfile, "      <crosshair_2_has_hor_line>%i</crosshair_2_has_hor_line>\n",
                    maincurve->crosshair_2.has_hor_line);

    fprintf(cfgfile, "      <crosshair_1_dot_sz>%i</crosshair_1_dot_sz>\n",
                    maincurve->crosshair_1.dot_sz);

    fprintf(cfgfile, "      <crosshair_2_dot_sz>%i</crosshair_2_dot_sz>\n",
                    maincurve->crosshair_2.dot_sz);

    fprintf(cfgfile, "      <floating_ruler_color>%i</floating_ruler_color>\n",
                    maincurve->floating_ruler_color);

    fprintf(cfgfile, "      <floating_ruler_use_var_width>%i</floating_ruler_use_var_width>\n",
                    maincurve->floating_ruler_use_var_width);

    fprintf(cfgfile, "      <blackwhite_printing>%i</blackwhite_printing>\n",
                    maincurve->blackwhite_printing);

    fprintf(cfgfile, "      <show_annot_markers>%i</show_annot_markers>\n",
                    show_annot_markers);

    fprintf(cfgfile, "      <channel_linked_annotations>%i</channel_linked_annotations>\n",
                    channel_linked_annotations);

    fprintf(cfgfile, "      <show_annot_duration>%i</show_annot_duration>\n",
                    annotations_show_duration);

    fprintf(cfgfile, "      <annotations_duration_background_type>%i</annotations_duration_background_type>\n",
                    annotations_duration_background_type);

    fprintf(cfgfile, "      <show_baselines>%i</show_baselines>\n",
                    show_baselines);

    fprintf(cfgfile, "    </colors>\n");

    fprintf(cfgfile, "    <ecg_view_mode>%i</ecg_view_mode>\n",
                    ecg_view_mode);

    fprintf(cfgfile, "    <clip_to_pane>%i</clip_to_pane>\n",
                    clip_to_pane);

    fprintf(cfgfile, "    <signal_plotorder>%i</signal_plotorder>\n",
                    signal_plotorder);

    fprintf(cfgfile, "    <auto_reload_mtg>%i</auto_reload_mtg>\n",
                    auto_reload_mtg);

    fprintf(cfgfile, "    <read_biosemi_status_signal>%i</read_biosemi_status_signal>\n",
                    read_biosemi_status_signal);

    fprintf(cfgfile, "    <read_nk_trigger_signal>%i</read_nk_trigger_signal>\n",
                    read_nk_trigger_signal);

    fprintf(cfgfile, "    <use_threads>%i</use_threads>\n",
                    use_threads);

    fprintf(cfgfile, "    <session_relative_paths>%i</session_relative_paths>\n",
                    session_relative_paths);

    fprintf(cfgfile, "    <edf_debug>%i</edf_debug>\n",
                    edf_debug);

    fprintf(cfgfile, "    <strip_label_types>%i</strip_label_types>\n",
                    strip_label_types);

    fprintf(cfgfile, "    <default_invert_eeg_signals>%i</default_invert_eeg_signals>\n",
                    default_invert_eeg_signals);

    fprintf(cfgfile, "    <default_time_scale>%i</default_time_scale>\n",
                    default_time_scale);

    fprintf(cfgfile, "    <default_fit_signals_to_pane>%i</default_fit_signals_to_pane>\n",
                    default_fit_signals_to_pane);

#ifdef Q_OS_WIN32
    __mingw_fprintf(cfgfile, "    <maxfilesize_to_readin_annotations>%lli</maxfilesize_to_readin_annotations>\n",
                    maxfilesize_to_readin_annotations);
#else
    fprintf(cfgfile, "    <maxfilesize_to_readin_annotations>%lli</maxfilesize_to_readin_annotations>\n",
                    maxfilesize_to_readin_annotations);
#endif

    fprintf(cfgfile, "    <pixelsizefactor>%.10e</pixelsizefactor>\n    <auto_dpi>%i</auto_dpi>\n    <x_pixelsizefactor>%.10e</x_pixelsizefactor>\n",
                     y_pixelsizefactor, auto_dpi, x_pixelsizefactor);

    fprintf(cfgfile, "    <recent_dir>");

    xml_fwrite_encode_entity(cfgfile, recent_opendir);

    fprintf(cfgfile, "</recent_dir>\n");

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      fprintf(cfgfile, "    <recent_file>");

      xml_fwrite_encode_entity(cfgfile, &recent_file_path[i][0]);

      fprintf(cfgfile, "</recent_file>\n");
    }

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      fprintf(cfgfile, "    <recent_file_mtg>");

      xml_fwrite_encode_entity(cfgfile, &recent_file_mtg_path[i][0]);

      fprintf(cfgfile, "</recent_file_mtg>\n");
    }

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      fprintf(cfgfile, "    <recent_session_file>");

      xml_fwrite_encode_entity(cfgfile, &recent_session_path[i][0]);

      fprintf(cfgfile, "</recent_session_file>\n");
    }

    fprintf(cfgfile, "    <recent_montagedir>");

    xml_fwrite_encode_entity(cfgfile, recent_montagedir);

    fprintf(cfgfile, "</recent_montagedir>\n");

    fprintf(cfgfile, "    <recent_sessiondir>");

    xml_fwrite_encode_entity(cfgfile, recent_sessiondir);

    fprintf(cfgfile, "</recent_sessiondir>\n");

    fprintf(cfgfile, "    <recent_savedir>");

    xml_fwrite_encode_entity(cfgfile, recent_savedir);

    fprintf(cfgfile, "</recent_savedir>\n");

    fprintf(cfgfile, "    <recent_opendir>");

    xml_fwrite_encode_entity(cfgfile, recent_opendir);

    fprintf(cfgfile, "</recent_opendir>\n");

    fprintf(cfgfile, "    <recent_video_opendir>");

    xml_fwrite_encode_entity(cfgfile, recent_video_opendir);

    fprintf(cfgfile, "</recent_video_opendir>\n");

    fprintf(cfgfile, "    <recent_colordir>");

    xml_fwrite_encode_entity(cfgfile, recent_colordir);

    fprintf(cfgfile, "</recent_colordir>\n");

    for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
    {
      fprintf(cfgfile, "    <predefined_mtg_path>");

      xml_fwrite_encode_entity(cfgfile, &predefined_mtg_path[i][0]);

      fprintf(cfgfile, "</predefined_mtg_path>\n");
    }

    fprintf(cfgfile, "    <spectrummarkerblock>\n");

    fprintf(cfgfile, "      <items>%i</items>\n", spectrum_colorbar->items);

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <frequency>%.10e</frequency>\n", spectrum_colorbar->freq[i]);
    }

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <color>%i</color>\n", spectrum_colorbar->color[i]);
    }

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <label>");

      xml_fwrite_encode_entity(cfgfile, spectrum_colorbar->label[i]);

      fprintf(cfgfile, "</label>\n");
    }

    fprintf(cfgfile, "      <method>%i</method>\n", spectrum_colorbar->method);

    fprintf(cfgfile, "      <auto_adjust>%i</auto_adjust>\n", spectrum_colorbar->auto_adjust);

    fprintf(cfgfile, "      <max_colorbar_value>%.8e</max_colorbar_value>\n", spectrum_colorbar->max_colorbar_value);

    fprintf(cfgfile, "    </spectrummarkerblock>\n");

    fprintf(cfgfile, "    <annotations_import_var>\n");

    fprintf(cfgfile, "      <format>%i</format>\n", import_annotations_var->format);

    fprintf(cfgfile, "      <onsettimeformat>%i</onsettimeformat>\n", import_annotations_var->onsettimeformat);

    fprintf(cfgfile, "      <onsetcolumn>%i</onsetcolumn>\n", import_annotations_var->onsetcolumn);

    fprintf(cfgfile, "      <descriptioncolumn>%i</descriptioncolumn>\n", import_annotations_var->descriptioncolumn);

    fprintf(cfgfile, "      <useduration>%i</useduration>\n", import_annotations_var->useduration);

    fprintf(cfgfile, "      <usestoptime>%i</usestoptime>\n", import_annotations_var->usestoptime);

    fprintf(cfgfile, "      <durationcolumn>%i</durationcolumn>\n", import_annotations_var->durationcolumn);

    fprintf(cfgfile, "      <stopcolumn>%i</stopcolumn>\n", import_annotations_var->stopcolumn);

    fprintf(cfgfile, "      <datastartline>%i</datastartline>\n", import_annotations_var->datastartline);

    xml_strlcpy_encode_entity(str1_1024, import_annotations_var->separator, 1024);

    fprintf(cfgfile, "      <separator>%s</separator>\n", str1_1024);

    fprintf(cfgfile, "      <ascii_txt_encoding>%i</ascii_txt_encoding>\n", import_annotations_var->ascii_txt_encoding);

    fprintf(cfgfile, "      <csv_equal_filename>%i</csv_equal_filename>\n", import_annotations_var->csv_equal_filename);

    fprintf(cfgfile, "      <dceventbittime>%i</dceventbittime>\n", import_annotations_var->dceventbittime);

    fprintf(cfgfile, "      <triggerlevel>%.12e</triggerlevel>\n", import_annotations_var->triggerlevel);

    fprintf(cfgfile, "      <manualdescription>%i</manualdescription>\n", import_annotations_var->manualdescription);

    xml_strlcpy_encode_entity(str1_1024, import_annotations_var->description, 1024);

    fprintf(cfgfile, "      <description>%s</description>\n", str1_1024);

    fprintf(cfgfile, "      <ignoreconsecutive>%i</ignoreconsecutive>\n", import_annotations_var->ignoreconsecutive);

    fprintf(cfgfile, "    </annotations_import_var>\n");

    fprintf(cfgfile, "    <annotations_export_var>\n");

    fprintf(cfgfile, "      <separator>%i</separator>\n", export_annotations_var->separator);

    fprintf(cfgfile, "      <format>%i</format>\n", export_annotations_var->format);

    fprintf(cfgfile, "      <duration>%i</duration>\n", export_annotations_var->duration);

    fprintf(cfgfile, "      <txt_encoding>%i</txt_encoding>\n", export_annotations_var->txt_encoding);

    fprintf(cfgfile, "    </annotations_export_var>\n");

    fprintf(cfgfile, "    <cdsa>\n");
    fprintf(cfgfile, "      <cdsa_segmentlen>%i</cdsa_segmentlen>\n", cdsa_segmentlen);
    fprintf(cfgfile, "      <cdsa_blocklen>%i</cdsa_blocklen>\n", cdsa_blocklen);
    fprintf(cfgfile, "      <cdsa_overlap>%i</cdsa_overlap>\n", cdsa_overlap);
    fprintf(cfgfile, "      <cdsa_window_func>%i</cdsa_window_func>\n", cdsa_window_func);
    fprintf(cfgfile, "      <cdsa_min_hz>%i</cdsa_min_hz>\n", cdsa_min_hz);
    fprintf(cfgfile, "      <cdsa_max_hz>%i</cdsa_max_hz>\n", cdsa_max_hz);
    fprintf(cfgfile, "      <cdsa_max_pwr>%i</cdsa_max_pwr>\n", cdsa_max_pwr);
    fprintf(cfgfile, "      <cdsa_min_pwr>%i</cdsa_min_pwr>\n", cdsa_min_pwr);
    fprintf(cfgfile, "      <cdsa_log>%i</cdsa_log>\n", cdsa_log);
    fprintf(cfgfile, "      <cdsa_pwr_voltage>%i</cdsa_pwr_voltage>\n", cdsa_pwr_voltage);
    fprintf(cfgfile, "      <cdsa_max_voltage>%.10e</cdsa_max_voltage>\n", cdsa_max_voltage);
    fprintf(cfgfile, "    </cdsa>\n");

    fprintf(cfgfile, "    <aeeg>\n");
    fprintf(cfgfile, "      <aeeg_segmentlen>%i</aeeg_segmentlen>\n", aeeg_segmentlen);
    fprintf(cfgfile, "      <aeeg_bp_min_hz>%e</aeeg_bp_min_hz>\n", aeeg_bp_min_hz);
    fprintf(cfgfile, "      <aeeg_bp_max_hz>%e</aeeg_bp_max_hz>\n", aeeg_bp_max_hz);
    fprintf(cfgfile, "      <aeeg_lp_hz>%e</aeeg_lp_hz>\n", aeeg_lp_hz);
    fprintf(cfgfile, "      <aeeg_scale_max_amp>%e</aeeg_scale_max_amp>\n", aeeg_scale_max_amp);
    fprintf(cfgfile, "      <aeegdock_height>%i</aeegdock_height>\n", aeegdock_height);
    fprintf(cfgfile, "      <aeeg_plot_margins>%i</aeeg_plot_margins>\n", aeeg_plot_margins);
    fprintf(cfgfile, "    </aeeg>\n");

    fprintf(cfgfile, "    <hypnogram>\n");
    for(i=0; i<HYPNOGRAM_STAGENUM; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, hypnogram_stage_name[i], 1024);

      fprintf(cfgfile, "      <stage_name>%s</stage_name>\n", str1_1024);
    }
    for(i=0; i<HYPNOGRAM_STAGENUM; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, hypnogram_annot_name[i], 1024);

      fprintf(cfgfile, "      <annot_name>%s</annot_name>\n", str1_1024);
    }
    for(i=0; i<HYPNOGRAM_OVNUM; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, hypnogram_annot_ov_name[i], 1024);

      fprintf(cfgfile, "      <annot_ov_name>%s</annot_ov_name>\n", str1_1024);
    }
    for(i=0; i<HYPNOGRAM_OVNUM; i++)
    {
      fprintf(cfgfile, "      <annot_ov_color>\n"
                      "        <red>%i</red>\n"
                      "        <green>%i</green>\n"
                      "        <blue>%i</blue>\n"
                      "        <alpha>%i</alpha>\n"
                      "      </annot_ov_color>\n",
                      hypnogram_annot_ov_color[i].red(),
                      hypnogram_annot_ov_color[i].green(),
                      hypnogram_annot_ov_color[i].blue(),
                      hypnogram_annot_ov_color[i].alpha());
    }
    fprintf(cfgfile, "      <hypnogram_use_epoch_len>%i</hypnogram_use_epoch_len>\n", hypnogram_use_epoch_len);
    fprintf(cfgfile, "      <use_overlays>%i</use_overlays>\n", hypnogram_use_overlays);
    fprintf(cfgfile, "      <hypnogramdock_height>%i</hypnogramdock_height>\n", hypnogramdock_height);
    fprintf(cfgfile, "    </hypnogram>\n");

    fprintf(cfgfile, "    <ecg_qrs>\n");
    xml_strlcpy_encode_entity(str1_1024, ecg_qrs_rpeak_descr, 1024);
    fprintf(cfgfile, "      <r_peak_description>%s</r_peak_description>\n", str1_1024);
    fprintf(cfgfile, "      <use_signallabel_in_annot_descr>%i</use_signallabel_in_annot_descr>\n", use_signallabel_in_annot_descr);
    fprintf(cfgfile, "    </ecg_qrs>\n");

    fprintf(cfgfile, "    <hrvdock>\n");
    fprintf(cfgfile, "      <hrvdock_max_bpm>%i</hrvdock_max_bpm>\n", hrvdock_max_bpm);
    fprintf(cfgfile, "      <hrvdock_min_bpm>%i</hrvdock_min_bpm>\n", hrvdock_min_bpm);
    fprintf(cfgfile, "      <hrvdock_height>%i</hrvdock_height>\n", hrvdock_height);
    fprintf(cfgfile, "      <hrvdock_trace_color>\n"
                     "        <red>%i</red>\n"
                     "        <green>%i</green>\n"
                     "        <blue>%i</blue>\n"
                     "      </hrvdock_trace_color>\n",
                            hrvdock_trace_color.red(),
                            hrvdock_trace_color.green(),
                            hrvdock_trace_color.blue());
    fprintf(cfgfile, "    </hrvdock>\n");

    fprintf(cfgfile, "    <annotlistdock>\n");
    fprintf(cfgfile, "      <annot_list_edited_txt_color>\n"
                     "        <red>%i</red>\n"
                     "        <green>%i</green>\n"
                     "        <blue>%i</blue>\n"
                     "      </annot_list_edited_txt_color>\n",
                            annot_list_edited_txt_color.red(),
                            annot_list_edited_txt_color.green(),
                            annot_list_edited_txt_color.blue());
    fprintf(cfgfile, "    </annotlistdock>\n");

    fprintf(cfgfile, "    <annotation_editor>\n");
    fprintf(cfgfile, "      <annot_editor_highres>%i</annot_editor_highres>\n", annot_editor_highres);
    for(i=0; i<8; i++)
    {
      fprintf(cfgfile, "      <annot_edit_user_button_enabled>%i</annot_edit_user_button_enabled>\n", annot_edit_user_button_enabled[i]);
    }
    for(i=0; i<8; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, annot_edit_user_button_name[i], 1024);
      fprintf(cfgfile, "      <annot_edit_user_button_name>%s</annot_edit_user_button_name>\n", str1_1024);
    }
    for(i=0; i<8; i++)
    {
      fprintf(cfgfile, "      <annot_editor_user_button_onset_on_page_middle>%i</annot_editor_user_button_onset_on_page_middle>\n", annot_editor_user_button_onset_on_page_middle[i]);
    }
    fprintf(cfgfile, "      <annot_editor_user_button_update_annot_description>%i</annot_editor_user_button_update_annot_description>\n", annot_editor_user_button_update_annot_description);
    fprintf(cfgfile, "      <annot_editor_user_button_update_annot_onset>%i</annot_editor_user_button_update_annot_onset>\n", annot_editor_user_button_update_annot_onset);
    fprintf(cfgfile, "      <annot_editor_user_button_update_annot_duration>%i</annot_editor_user_button_update_annot_duration>\n", annot_editor_user_button_update_annot_duration);
    fprintf(cfgfile, "      <annot_editor_user_button_jump_to_next_page>%i</annot_editor_user_button_jump_to_next_page>\n", annot_editor_user_button_jump_to_next_page);
    fprintf(cfgfile, "      <annot_editor_user_button_stay_on_epoch_boundary>%i</annot_editor_user_button_stay_on_epoch_boundary>\n", annot_editor_user_button_stay_on_epoch_boundary);
#ifdef Q_OS_WIN32
    __mingw_fprintf(cfgfile, "      <annot_editor_user_button_epoch_len>%lli</annot_editor_user_button_epoch_len>\n", annot_editor_user_button_epoch_len);
    __mingw_fprintf(cfgfile, "      <annot_editor_user_button_page_len>%lli</annot_editor_user_button_page_len>\n", annot_editor_user_button_page_len);
#else
    fprintf(cfgfile, "      <annot_editor_user_button_epoch_len>%lli</annot_editor_user_button_epoch_len>\n", annot_editor_user_button_epoch_len);
    fprintf(cfgfile, "      <annot_editor_user_button_page_len>%lli</annot_editor_user_button_page_len>\n", annot_editor_user_button_page_len);
#endif
    for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
    {
      xml_strlcpy_encode_entity(str1_1024, annot_by_rect_draw_description[i], 1024);
      fprintf(cfgfile, "      <annot_by_rect_draw_description>%s</annot_by_rect_draw_description>\n", str1_1024);
    }
    fprintf(cfgfile, "    </annotation_editor>\n");

    fprintf(cfgfile, "    <live_stream_update_interval>%i</live_stream_update_interval>\n", live_stream_update_interval);

    fprintf(cfgfile, "    <powerlinefreq>%i</powerlinefreq>\n", powerlinefreq);

    fprintf(cfgfile, "    <mousewheelsens>%i</mousewheelsens>\n", mousewheelsens);

    fprintf(cfgfile, "    <average_period>%i</average_period>\n", average_period);

    fprintf(cfgfile, "    <average_ratio>%i</average_ratio>\n", average_ratio);

    fprintf(cfgfile, "    <average_upsidedown>%i</average_upsidedown>\n", average_upsidedown);

    fprintf(cfgfile, "    <average_bw>%i</average_bw>\n", average_bw);

    fprintf(cfgfile, "    <spectrum_bw>%i</spectrum_bw>\n", spectrum_bw);

    fprintf(cfgfile, "    <spectrum_sqrt>%i</spectrum_sqrt>\n", spectrum_sqrt);

    fprintf(cfgfile, "    <spectrum_vlog>%i</spectrum_vlog>\n", spectrum_vlog);

    fprintf(cfgfile, "    <spectrum_window>%i</spectrum_window>\n", spectrum_window);

    fprintf(cfgfile, "    <spectrum_blocksize_predefined>%i</spectrum_blocksize_predefined>\n", spectrum_blocksize_predefined);

    fprintf(cfgfile, "    <spectrum_blocksize_userdefined>%i</spectrum_blocksize_userdefined>\n", spectrum_blocksize_userdefined);

    fprintf(cfgfile, "    <spectrum_overlap>%i</spectrum_overlap>\n", spectrum_overlap);

    fprintf(cfgfile, "    <spectrumdock_sqrt>%i</spectrumdock_sqrt>\n", spectrumdock_sqrt);

    fprintf(cfgfile, "    <spectrumdock_vlog>%i</spectrumdock_vlog>\n", spectrumdock_vlog);

    fprintf(cfgfile, "    <spectrumdock_window>%i</spectrumdock_window>\n", spectrumdock_window);

    fprintf(cfgfile, "    <spectrumdock_blocksize_predefined>%i</spectrumdock_blocksize_predefined>\n", spectrumdock_blocksize_predefined);

    fprintf(cfgfile, "    <spectrumdock_blocksize_userdefined>%i</spectrumdock_blocksize_userdefined>\n", spectrumdock_blocksize_userdefined);

    fprintf(cfgfile, "    <spectrumdock_overlap>%i</spectrumdock_overlap>\n", spectrumdock_overlap);

    fprintf(cfgfile, "    <spectrumdock_colorbars>%i</spectrumdock_colorbars>\n", spectrumdock_colorbars);

    fprintf(cfgfile, "    <z_score_var.crossoverfreq>%.10e</z_score_var.crossoverfreq>\n", z_score_var.crossoverfreq);

    fprintf(cfgfile, "    <z_score_var.z_threshold>%.10e</z_score_var.z_threshold>\n", z_score_var.z_threshold);

    fprintf(cfgfile, "    <z_score_var.zscore_page_len>%i</z_score_var.zscore_page_len>\n", z_score_var.zscore_page_len);

    fprintf(cfgfile, "    <z_score_var.zscore_error_detection>%i</z_score_var.zscore_error_detection>\n", z_score_var.zscore_error_detection);

    fprintf(cfgfile, "    <z_score_var.z_hysteresis>%.10e</z_score_var.z_hysteresis>\n", z_score_var.z_hysteresis);

    fprintf(cfgfile, "    <raw2edf_var>\n");

    fprintf(cfgfile, "      <sf>%i</sf>\n", raw2edf_var.sf);

    fprintf(cfgfile, "      <chns>%i</chns>\n", raw2edf_var.chns);

    fprintf(cfgfile, "      <phys_max>%i</phys_max>\n", raw2edf_var.phys_max);

    fprintf(cfgfile, "      <straightbinary>%i</straightbinary>\n", raw2edf_var.straightbinary);

    fprintf(cfgfile, "      <endianness>%i</endianness>\n", raw2edf_var.endianness);

    fprintf(cfgfile, "      <samplesize>%i</samplesize>\n", raw2edf_var.samplesize);

    fprintf(cfgfile, "      <offset>%i</offset>\n", raw2edf_var.offset);

    fprintf(cfgfile, "      <skipblocksize>%i</skipblocksize>\n", raw2edf_var.skipblocksize);

    fprintf(cfgfile, "      <skipbytes>%i</skipbytes>\n", raw2edf_var.skipbytes);

    xml_strlcpy_encode_entity(str1_1024, raw2edf_var.phys_dim, 1024);

    fprintf(cfgfile, "      <phys_dim>%s</phys_dim>\n", str1_1024);

    fprintf(cfgfile, "    </raw2edf_var>\n");

    fprintf(cfgfile, "    <annotfilter_var>\n");

    fprintf(cfgfile, "      <tmin>%i</tmin>\n", annot_filter->tmin);

    fprintf(cfgfile, "      <tmax>%i</tmax>\n", annot_filter->tmax);

    fprintf(cfgfile, "      <invert>%i</invert>\n", annot_filter->invert);

    fprintf(cfgfile, "      <hide_other>%i</hide_other>\n", annot_filter->hide_other);

    fprintf(cfgfile, "      <hide_in_list_only>%i</hide_in_list_only>\n", annot_filter->hide_in_list_only);

    fprintf(cfgfile, "    </annotfilter_var>\n");

    fprintf(cfgfile, "    <check_for_updates>%i</check_for_updates>\n", check_for_updates);
#ifdef Q_OS_WIN32
    __mingw_fprintf(cfgfile, "    <last_check_for_updates_time>%lli</last_check_for_updates_time>\n", last_check_for_updates_time);
#else
    fprintf(cfgfile, "    <last_check_for_updates_time>%lli</last_check_for_updates_time>\n", last_check_for_updates_time);
#endif
    fprintf(cfgfile, "    <viewtime_indicator_type>%i</viewtime_indicator_type>\n", viewtime_indicator_type);

    fprintf(cfgfile, "    <display_pagetime_mmsec>%i</display_pagetime_mmsec>\n", display_pagetime_mmsec);

    fprintf(cfgfile, "    <mc_v_scrollarea_auto>%i</mc_v_scrollarea_auto>\n", mc_v_scrollarea_auto);

    fprintf(cfgfile, "    <mc_v_scrollarea_max_signals>%i</mc_v_scrollarea_max_signals>\n", mc_v_scrollarea_max_signals);

    fprintf(cfgfile, "    <mainwindow_title_type>%i</mainwindow_title_type>\n", mainwindow_title_type);

    fprintf(cfgfile, "    <default_amplitude>%.10e</default_amplitude>\n", default_amplitude);

    fprintf(cfgfile, "    <default_amplitude_physmax_div>%i</default_amplitude_physmax_div>\n", default_amplitude_physmax_div);

    fprintf(cfgfile, "    <default_amplitude_use_physmax_div>%i</default_amplitude_use_physmax_div>\n", default_amplitude_use_physmax_div);

    fprintf(cfgfile, "    <linear_interpolation>%i</linear_interpolation>\n", linear_interpol);

    fprintf(cfgfile, "    <annot_onset_at_start_of_page_on_jump>%i</annot_onset_at_start_of_page_on_jump>\n", annot_onset_at_start_of_page_on_jump);

    fprintf(cfgfile, "    <auto_update_annot_onset>%i</auto_update_annot_onset>\n", auto_update_annot_onset);

    fprintf(cfgfile, "    <annotlist_scrolltoitem_while_browsing>%i</annotlist_scrolltoitem_while_browsing>\n", annotlist_scrolltoitem_while_browsing);

    fprintf(cfgfile, "    <use_diverse_signal_colors>%i</use_diverse_signal_colors>\n", use_diverse_signal_colors);

    fprintf(cfgfile, "    <rc_host_server_public>%i</rc_host_server_public>\n", rc_host_server_public);

    fprintf(cfgfile, "  </UI>\n</config>\n");

    fclose(cfgfile);
  }
}


















