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


#include "viewcurve.h"




ViewCurve::ViewCurve(QWidget *w_parent) : QWidget(w_parent)
{
  int i;

  setAttribute(Qt::WA_OpaquePaintEvent);

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  special_pen = new QPen(Qt::SolidPattern, 0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin);

  annot_marker_pen = new QPen(Qt::SolidPattern, 0, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin);

  signal_pen = new QPen(Qt::SolidPattern, 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

  ruler_pen = new QPen(Qt::SolidPattern, 0, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

  context_menu = new QMenu(w_parent);
  select_annot_act = new QAction("Select", this);
  context_menu->addAction(select_annot_act);
  hide_annot_act = new QAction("Hide", this);
  context_menu->addAction(hide_annot_act);
  average_annot_act = new QAction("Average", this);
  context_menu->addAction(average_annot_act);

  annot_sidemenu_act = new QAction("Select annotation", this);
//  annot_sidemenu_act->setShortcut(QKeySequence("Ctrl+Shift+A"));
  annot_sidemenu_act->setShortcut(QKeySequence("A"));
  addAction(annot_sidemenu_act);

  active_markers = (activemarkersblck_t *)calloc(1, sizeof(activemarkersblck_t));

  scroll_h_offset = 0;

  scrollbar_h_value = 0;

  annot_marker_moving = 0;

  active_marker_context_menu_request_idx = 0;

  use_move_events = 0;
  panning_moving = 0;
  for(i=0; i<MAXFILES; i++)
  {
    pan_mov_start_viewtime[i] = 0LL;
  }
  sidemenu_active = 0;
  draw_zoom_rectangle = 0;
  printing = 0;
  pressed_on_label = 0;

  original_sensitivity = 0;

  mouse_press_coordinate_x = 0;
  mouse_press_coordinate_y = 0;
  mouse_release_coordinate_x = 0;

  mouse_x = 0;
  mouse_y = 0;
  mouse_old_x = 0;
  mouse_old_y = 0;

////////////////////////////////////////////////////////

  backgroundcolor.setRed(64);
  backgroundcolor.setGreen(64);
  backgroundcolor.setBlue(64);

  small_ruler_color.setRed(255);
  small_ruler_color.setGreen(255);
  small_ruler_color.setBlue(255);

  big_ruler_color.setRed(128);
  big_ruler_color.setGreen(128);
  big_ruler_color.setBlue(128);

  mouse_rect_color.setRed(255);
  mouse_rect_color.setGreen(255);
  mouse_rect_color.setBlue(255);

  text_color.setRed(255);
  text_color.setGreen(255);
  text_color.setBlue(255);

  baseline_color.setRed(128);
  baseline_color.setGreen(128);
  baseline_color.setBlue(128);

  annot_marker_color.setRed(255);
  annot_marker_color.setGreen(255);
  annot_marker_color.setBlue(255);

  annot_marker_selected_color.setRed(255);
  annot_marker_selected_color.setGreen(228);
  annot_marker_selected_color.setBlue(0);

  annot_duration_color.setRed(0);
  annot_duration_color.setGreen(127);
  annot_duration_color.setBlue(127);
  annot_duration_color.setAlpha(32);

  annot_duration_color_selected.setRed(127);
  annot_duration_color_selected.setGreen(0);
  annot_duration_color_selected.setBlue(127);
  annot_duration_color_selected.setAlpha(32);

  signal_color = Qt::yellow;

  floating_ruler_color = Qt::cyan;

  memset(&crosshair_1, 0, sizeof(crosshair_t));
  memset(&crosshair_2, 0, sizeof(crosshair_t));

  crosshair_1.dot_sz = 4;
  crosshair_2.dot_sz = 4;

  crosshair_1.color = Qt::yellow;
  crosshair_2.color = Qt::cyan;

  blackwhite_printing = 1;

/////////////////////////////////////////////////////////

  signal_nr = 0;
  ruler_active = 0;
  ruler_moving = 0;

  printsize_x_factor = 1;
  printsize_y_factor = 1;

  printfont = new QFont(*mainwindow->monofont);

  screensamples = (int *)calloc(1, sizeof(int[MAXSIGNALS]));

  cpu_cnt = QThread::idealThreadCount();
  if(cpu_cnt < 1) cpu_cnt = 1;
  if(cpu_cnt > MAXSIGNALS) cpu_cnt = MAXSIGNALS;

  for(i=0; i<cpu_cnt; i++)
  {
    thr[i] = new drawCurve_stage_1_thread;
  }

  graphicBuf = NULL;
  printBuf = NULL;
  graphicBufWidth = 0;
  blackwhite_printing = 1;
  float_ruler_more = 0;
  floating_ruler_use_var_width = 0;
  linear_interpol = 0;

  shift_page_left_shortcut = NULL;
  shift_page_right_shortcut = NULL;
  shift_page_up_shortcut = NULL;
  shift_page_down_shortcut = NULL;

  arrowkeys_shortcuts_global_set_enabled(true);

  QObject::connect(select_annot_act,       SIGNAL(triggered(bool)), this, SLOT(select_annot_clicked(bool)));
  QObject::connect(hide_annot_act,         SIGNAL(triggered(bool)), this, SLOT(hide_annot_clicked(bool)));
  QObject::connect(average_annot_act,      SIGNAL(triggered(bool)), this, SLOT(average_annot_clicked(bool)));
  QObject::connect(annot_sidemenu_act,     SIGNAL(triggered(bool)), this, SLOT(annot_sidemenu_act_by_crosshair(bool)));
}


ViewCurve::~ViewCurve()
{
  int i;

  free(graphicBuf);

  free(active_markers);

  free(screensamples);

  delete printfont;
  delete annot_marker_pen;
  delete special_pen;
  delete signal_pen;
  delete ruler_pen;

  for(i=0; i<cpu_cnt; i++)
  {
    delete thr[i];
  }
}


void ViewCurve::arrowkeys_shortcuts_global_set_enabled(bool enabled)
{
  if(shift_page_left_shortcut != NULL)
  {
    QObject::disconnect(shift_page_left_shortcut, 0, 0, 0);
    delete shift_page_left_shortcut;
  }

  if(shift_page_right_shortcut != NULL)
  {
    QObject::disconnect(shift_page_right_shortcut, 0, 0, 0);
    delete shift_page_right_shortcut;
  }

  if(shift_page_up_shortcut != NULL)
  {
    QObject::disconnect(shift_page_up_shortcut, 0, 0, 0);
    delete shift_page_up_shortcut;
  }

  if(shift_page_down_shortcut != NULL)
  {
    QObject::disconnect(shift_page_down_shortcut, 0, 0, 0);
    delete shift_page_down_shortcut;
  }

  if(enabled == true)
  {
    shift_page_left_shortcut = new QShortcut(QKeySequence::MoveToPreviousChar, mainwindow);
    shift_page_right_shortcut = new QShortcut(QKeySequence::MoveToNextChar, mainwindow);
    shift_page_up_shortcut = new QShortcut(QKeySequence::MoveToPreviousLine, mainwindow);
    shift_page_down_shortcut = new QShortcut(QKeySequence::MoveToNextLine, mainwindow);
  }
  else
  {
    shift_page_left_shortcut = new QShortcut(QKeySequence::MoveToPreviousChar, this, 0, 0, Qt::WidgetShortcut);
    shift_page_right_shortcut = new QShortcut(QKeySequence::MoveToNextChar, this, 0, 0, Qt::WidgetShortcut);
    shift_page_up_shortcut = new QShortcut(QKeySequence::MoveToPreviousLine, this, 0, 0, Qt::WidgetShortcut);
    shift_page_down_shortcut = new QShortcut(QKeySequence::MoveToNextLine, this, 0, 0, Qt::WidgetShortcut);
  }

  QObject::connect(shift_page_left_shortcut,  SIGNAL(activated()), mainwindow, SLOT(shift_page_left()));
  QObject::connect(shift_page_right_shortcut, SIGNAL(activated()), mainwindow, SLOT(shift_page_right()));
  QObject::connect(shift_page_up_shortcut,    SIGNAL(activated()), mainwindow, SLOT(shift_page_up()));
  QObject::connect(shift_page_down_shortcut,  SIGNAL(activated()), mainwindow, SLOT(shift_page_down()));
}


void ViewCurve::annot_sidemenu_act_by_crosshair(bool)
{
  int i;

  if((!mainwindow->annot_editor_active) ||
     (mainwindow->annotationEditDock == NULL) ||
     (!mainwindow->signalcomps) ||
     (!crosshair_1.active))  return;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->hascursor1)
    {
      break;
    }
  }
  if(i == mainwindow->signalcomps)  return;

  mainwindow->annot_created_by_rect_draw_numchan = i;

  mainwindow->annot_created_by_rect_draw_onset = crosshair_1.time_relative;

  mainwindow->annot_created_by_rect_draw_duration = -TIME_FIXP_SCALING;

  if(mainwindow->signalcomp[i]->hascursor2)
  {
    if(crosshair_2.time_relative > crosshair_1.time_relative)
    {
      mainwindow->annot_created_by_rect_draw_duration = crosshair_2.time_relative - crosshair_1.time_relative;

      mainwindow->annot_created_by_rect_draw_crosshair_delta = crosshair_2.value - crosshair_1.value;
    }
  }

  mainwindow->annot_created_by_rect_draw_active = 2;

  emit annot_created_by_rect_draw();
}


void ViewCurve::wheelEvent(QWheelEvent *wheel_event)
{
  int i;

  if(mainwindow->mousewheelsens < 1)  return;

  if(mainwindow->files_open == 0)  return;

  if(QApplication::keyboardModifiers() == Qt::ControlModifier)
  {
#if QT_VERSION >= 0x050C00
    if(wheel_event->angleDelta().y() > 0)
#else
    if(wheel_event->delta() > 0)
#endif
    {
      mainwindow->set_page_div2();
    }
    else
    {
      mainwindow->set_page_mult2();

    }
  }
  else
  {
    if((mainwindow->video_player->status == VIDEO_STATUS_PLAYING) || (mainwindow->video_player->status == VIDEO_STATUS_PAUSED))
    {
#if QT_VERSION >= 0x050C00
      if(wheel_event->angleDelta().y() > 0)
#else
      if(wheel_event->delta() > 0)
#endif
      {
        mainwindow->video_player_seek((int)((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - (mainwindow->pagetime / mainwindow->mousewheelsens)) / TIME_FIXP_SCALING));
      }
      else
      {
        mainwindow->video_player_seek((int)((mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime + (mainwindow->pagetime / mainwindow->mousewheelsens)) / TIME_FIXP_SCALING));
      }

      if(mainwindow->video_player->status == VIDEO_STATUS_PLAYING)  return;
    }

    if((mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
    {
      for(i=0; i<mainwindow->files_open; i++)
      {
#if QT_VERSION >= 0x050C00
        if(wheel_event->angleDelta().y() > 0)
#else
        if(wheel_event->delta() > 0)
#endif
        {
          mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->pagetime / mainwindow->mousewheelsens);
        }
        else
        {
          mainwindow->edfheaderlist[i]->viewtime += (mainwindow->pagetime / mainwindow->mousewheelsens);
        }
      }
    }

    if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
    {
#if QT_VERSION >= 0x050C00
      if(wheel_event->angleDelta().y() > 0)
#else
      if(wheel_event->delta() > 0)
#endif
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime -= (mainwindow->pagetime / mainwindow->mousewheelsens);
      }
      else
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime += (mainwindow->pagetime / mainwindow->mousewheelsens);
      }
    }
  }

  mainwindow->setup_viewbuf();
}


void ViewCurve::mousePressEvent(QMouseEvent *press_event)
{
  int i, j,
      baseline,
      signalcomps,
      m_x,
      m_y,
      h_size,
      signallabel_strlen=1;

  sigcompblck_t **signalcomp;

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
#if QT_VERSION < 0x060000
  m_x = press_event->x();
  m_y = press_event->y();
#else
  m_x = press_event->position().x();
  m_y = press_event->position().y();
#endif

  mouse_press_coordinate_x = m_x;
  mouse_press_coordinate_y = m_y;
  mouse_release_coordinate_x = m_x;

  pressed_on_label = 0;

  h_size = (int)(4.0 / mainwindow->y_pixelsizefactor);

  setFocus(Qt::MouseFocusReason);

  mainwindow->annot_created_by_rect_draw_active = 0;

  mainwindow->annot_created_by_rect_draw_numchan = -1;

  if(press_event->button()==Qt::LeftButton)
  {
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    ruler_moving = 0;

    use_move_events = 1;
    setMouseTracking(true);

    mouse_old_x = m_x;
    mouse_old_y = m_y;

    if(ruler_active)
    {
      if((m_y>(ruler_y_position + h_size + 10))&&(m_y<(ruler_y_position  + h_size + 30))&&(m_x>ruler_x_position)&&(m_x<(ruler_x_position + 60)))
      {
        if(float_ruler_more)
        {
          float_ruler_more = 0;
        }
        else
        {
          float_ruler_more = 1;
        }

        update();

        return;
      }
      else if((m_y>ruler_y_position)&&(m_y<(ruler_y_position + ((int)(4.0 / mainwindow->y_pixelsizefactor))))&&(m_x>ruler_x_position)&&(m_x<(ruler_x_position + ((int)(w / 2.8)))))
      {
        ruler_moving = 1;
      }
    }

    if(!ruler_moving)
    {
      if(crosshair_1.active)
      {
        if((m_y >= (crosshair_1.y_position - 10 - mainwindow->mono_font_pixel_height)) && (m_y < (crosshair_1.y_position - 10 + (mainwindow->mono_font_pixel_height * 2))) &&
           (m_x >= crosshair_1.x_position) && (m_x < (crosshair_1.x_position + (mainwindow->mono_font_pixel_width * 17))))
        {
          crosshair_1.moving = 1;
        }

        if((m_x > (crosshair_1.x_position - mainwindow->mono_font_pixel_width)) && (m_x < (crosshair_1.x_position + mainwindow->mono_font_pixel_width)))
        {
          crosshair_1.moving = 1;
        }
      }
    }

    if((!ruler_moving)&&(!crosshair_1.moving))
    {
      if(crosshair_2.active)
      {
        if((m_y >= (crosshair_2.y_position - 10 - mainwindow->mono_font_pixel_height)) && (m_y < (crosshair_2.y_position - 10 + (mainwindow->mono_font_pixel_height * 4))) &&
           (m_x >= crosshair_2.x_position) && (m_x < (crosshair_2.x_position + (mainwindow->mono_font_pixel_width * 17))))
        {
          crosshair_2.moving = 1;
        }

        if((m_x > (crosshair_2.x_position - mainwindow->mono_font_pixel_width)) && (m_x < (crosshair_2.x_position + mainwindow->mono_font_pixel_width)))
        {
          crosshair_2.moving = 1;
        }
      }
    }

    if(mainwindow->annot_editor_active)
    {
      if((!ruler_moving)&&(!crosshair_1.moving)&&(!crosshair_2.moving))
      {
        if(QApplication::keyboardModifiers() != Qt::ControlModifier)
        {
          for(i=0; i<active_markers->count; i++)
          {
            if(m_x>(active_markers->list[i]->x_pos-(5*w_scaling))&&(m_x<(active_markers->list[i]->x_pos+(5*w_scaling))))
            {
              active_markers->selected_idx = i;

              active_markers->list[i]->grabbed = 1;

              if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
              {
                for(j=0; j<mainwindow->files_open; j++)
                {
                  if(mainwindow->annotations_dock[j] != NULL)
                  {
                    edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[j]->annot_list);
                  }
                }

                active_markers->list[i]->selected_in_dock = 1;

                mainwindow->annotationEditDock->set_selected_annotation(active_markers->list[i]);

                for(j=0; j<mainwindow->files_open; j++)
                {
                  if(mainwindow->annotations_dock[j] != NULL)
                  {
                    mainwindow->annotations_dock[j]->updateList(1);
                  }
                }
              }
              else
              {
                annot_marker_moving = 1;
              }

              break;
            }
          }
        }
      }
    }

    if((!ruler_moving)&&(!crosshair_1.moving)&&(!crosshair_2.moving)&&(!annot_marker_moving))
    {
      for(i=0; i<signalcomps; i++)
      {
        baseline = (((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0)) * (i + 1);
        baseline -= scroll_h_offset;

        if(signalcomp[i]->alias[0] != 0)
        {
          signallabel_strlen = strlen(signalcomp[i]->alias);
        }
        else
        {
          signallabel_strlen = strlen(signalcomp[i]->signallabel);
        }

        if((m_y<(baseline-(2*h_scaling)))&&(m_y>( baseline-(2*h_scaling)-mainwindow->mono_font_pixel_height))&&(m_x>(2*w_scaling))&&(m_x<((signallabel_strlen * mainwindow->mono_font_pixel_width) + (5 * w_scaling))))
        {
          original_screen_offset_pix = signalcomp[i]->screen_offset_pix;
          original_screen_offset_unit = signalcomp[i]->screen_offset_unit;
          signalcomp[i]->hasoffsettracking = 1;
          signal_nr = i;
          pressed_on_label = i + 1;
          label_press_y_position = m_y;

          break;
        }
      }
    }

    if((!pressed_on_label)&&(!ruler_moving)&&(!crosshair_1.moving)&&(!crosshair_2.moving)&&(!annot_marker_moving))
    {
      draw_zoom_rectangle = 1;

      mouse_x = m_x;
      mouse_y = m_y;
    }
  }

  if(press_event->button()==Qt::RightButton)
  {
    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->hascursor1 = 0;
      signalcomp[i]->hascursor2 = 0;
      signalcomp[i]->hasoffsettracking = 0;
    }
    crosshair_1.active = 0;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    use_move_events = 0;
    setMouseTracking(false);

    for(i=0; i<signalcomps; i++)
    {
      baseline = (((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0)) * (i + 1);
      baseline -= scroll_h_offset;

      if(signalcomp[i]->alias[0] != 0)
      {
        signallabel_strlen = strlen(signalcomp[i]->alias);
      }
      else
      {
        signallabel_strlen = strlen(signalcomp[i]->signallabel);
      }

      if((m_y<(baseline-(2*h_scaling)))&&(m_y>( baseline-(2*h_scaling)-mainwindow->mono_font_pixel_height))&&(m_x>(2*w_scaling))&&(m_x<((signallabel_strlen * mainwindow->mono_font_pixel_width) + (5 * w_scaling))))
      {
        original_sensitivity = signalcomp[i]->sensitivity;
        original_screen_offset_pix = signalcomp[i]->screen_offset_pix;
        original_screen_offset_unit = signalcomp[i]->screen_offset_unit;
        signalcomp[i]->hasgaintracking = 1;
        use_move_events = 1;
        setMouseTracking(true);
        signal_nr = i;

        break;
      }
    }

    if(!use_move_events)
    {
      for(i=0; i<signalcomps; i++)
      {
        signalcomp[i]->hasruler = 0;
      }
      ruler_active = 0;
      ruler_moving = 0;

      update();
    }

    if(!use_move_events)
    {
      if(!mainwindow->annot_editor_active)
      {
        if((!ruler_moving)&&(!crosshair_1.moving)&&(!crosshair_2.moving))
        {
          for(i=0; i<active_markers->count; i++)
          {
            if(!active_markers->list[i]->hidden)
            {
              if(m_x>(active_markers->list[i]->x_pos-(5*w_scaling))&&(m_x<(active_markers->list[i]->x_pos+(5*w_scaling))))
              {
                active_marker_context_menu_request_idx = i;

                context_menu->exec(QCursor::pos());

                break;
              }
            }
          }
        }
      }
    }
  }

#if QT_VERSION >= 0x050F00
  if(press_event->button()==Qt::MiddleButton)
#else
  if(press_event->button()==Qt::MidButton)
#endif
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      pan_mov_start_viewtime[i] = mainwindow->edfheaderlist[i]->viewtime;
    }

    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->hasoffsettracking = 0;
    }
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    use_move_events = 1;
    setMouseTracking(true);

    panning_moving = 1;
  }
}


void ViewCurve::mouseReleaseEvent(QMouseEvent *release_event)
{
  int i, j, dist1, dist2,
      baseline,
      signalcomps,
      m_x,
      m_y,
      signallabel_strlen=1;

  double v_zoomfactor=1;

  sigcompblck_t **signalcomp;

  struct
  {
    int x1;
    int x2;
    int y1;
    int y2;
    int w;
    int h;
  }
  sig_rect_select=
  {
    .x1=0,
    .x2=0,
    .y1=0,
    .y2=0,
    .w=0,
    .h=0
  };

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
#if QT_VERSION < 0x060000
  m_x = release_event->x();
  m_y = release_event->y();
#else
  m_x = release_event->position().x();
  m_y = release_event->position().y();
#endif

  mouse_release_coordinate_x = m_x;

  if(release_event->button()==Qt::LeftButton)
  {
    if(mainwindow->annotationEditDock != NULL)
    {
      if(crosshair_1.moving)
      {
        mainwindow->annotationEditDock->annotEditSetOnset(crosshair_1.time_relative);

        if(crosshair_2.active)
        {
          mainwindow->annotationEditDock->annotEditSetDuration(crosshair_2.time_relative - mainwindow->annotationEditDock->annotEditGetOnset());
        }
      }

      if(crosshair_2.moving)
      {
          // Don't update the onset time when changing file position
//      mainwindow->annotationEditDock->annotEditSetDuration(crosshair_2.time_relative - crosshair_1.time_relative);
        mainwindow->annotationEditDock->annotEditSetDuration(crosshair_2.time_relative - mainwindow->annotationEditDock->annotEditGetOnset());
      }
    }

    if((annot_marker_moving) && (active_markers->edf_hdr != NULL))
    {
      active_markers->list[active_markers->selected_idx]->x_pos = m_x;

      active_markers->list[active_markers->selected_idx]->onset = ((long long)((((double)m_x) / w) * mainwindow->pagetime))
                                                               + active_markers->edf_hdr->viewtime;

      active_markers->list[active_markers->selected_idx]->modified = 1;

      active_markers->list[active_markers->selected_idx]->grabbed = 1;

      for(j=0; j<mainwindow->files_open; j++)
      {
        if(mainwindow->annotations_dock[j] != NULL)
        {
          edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[j]->annot_list);
        }
      }

      active_markers->list[active_markers->selected_idx]->selected_in_dock = 1;

      if(mainwindow->annotationEditDock != NULL)
      {
        mainwindow->annotationEditDock->set_selected_annotation(active_markers->list[active_markers->selected_idx]);
      }

      for(j=0; j<mainwindow->files_open; j++)
      {
        if(mainwindow->annotations_dock[j] != NULL)
        {
          mainwindow->annotations_dock[j]->updateList(1);
        }
      }

      mainwindow->annotations_edited = 1;

      update();
    }

    ruler_moving = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    annot_marker_moving = 0;
    use_move_events = 0;
    setMouseTracking(false);

    if(draw_zoom_rectangle)
    {
      draw_zoom_rectangle = 0;

      if((abs(m_x - mouse_press_coordinate_x) >= 15) && (abs(m_y - mouse_press_coordinate_y) >= 15))
      {
        if(m_x > mouse_press_coordinate_x)
        {
          sig_rect_select.x1 = mouse_press_coordinate_x;
          sig_rect_select.x2 = m_x;
        }
        else
        {
          sig_rect_select.x1 = m_x;
          sig_rect_select.x2 = mouse_press_coordinate_x;
        }

        if(m_y > mouse_press_coordinate_y)
        {
          sig_rect_select.y1 = mouse_press_coordinate_y;
          sig_rect_select.y2 = m_y;
        }
        else
        {
          sig_rect_select.y1 = m_y;
          sig_rect_select.y2 = mouse_press_coordinate_y;
        }

        sig_rect_select.w = sig_rect_select.x2 - sig_rect_select.x1;
        sig_rect_select.h = sig_rect_select.y2 - sig_rect_select.y1;

        if((QApplication::keyboardModifiers() == Qt::ControlModifier) ||
           (QApplication::keyboardModifiers() == Qt::ShiftModifier))
        {
          if((mainwindow->annot_editor_active) && (mainwindow->annotationEditDock != NULL) && signalcomps)
          {
            if(QApplication::keyboardModifiers() == Qt::ControlModifier)
            {
              mainwindow->annot_created_by_rect_draw_numchan = 0;

              if(signalcomps > 1)
              {
                dist1 = 0x7fffffff;

                for(i=0; i<signalcomps; i++)
                {
  //                 printf("draw_rectangle_sum_cnt: %i   draw_rectangle_sum_y: %i\n",
  //                        signalcomp[i]->draw_rectangle_sum_cnt, signalcomp[i]->draw_rectangle_sum_y / signalcomp[i]->draw_rectangle_sum_cnt);
  //
  //                 baseline = (h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1);
  //                 baseline *= (i + 1);
  //                 baseline += signalcomp[i]->screen_offset_pix;

                  if(signalcomp[i]->draw_rectangle_sum_cnt < 1)  continue;

                  baseline = signalcomp[i]->draw_rectangle_sum_y / signalcomp[i]->draw_rectangle_sum_cnt;

                  dist2 = baseline - ((sig_rect_select.y1 + sig_rect_select.y2) / 2) - scroll_h_offset;
                  if(dist2 < 0)
                  {
                    dist2 *= -1;
                  }

                  if(dist2 < dist1)
                  {
                    dist1 = dist2;

                    mainwindow->annot_created_by_rect_draw_numchan = i;
                  }
                }
              }
            }
            else if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
              {
                mainwindow->annot_created_by_rect_draw_numchan = -1;
              }

            mainwindow->annot_created_by_rect_draw_onset = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)sig_rect_select.x1);

            mainwindow->annot_created_by_rect_draw_duration = (long long)((double)mainwindow->pagetime / ((double)w / (double)sig_rect_select.w));

            mainwindow->annot_created_by_rect_draw_active = 1;

            emit annot_created_by_rect_draw();
          }

          update();
        }
        else if((sig_rect_select.w >= 25) && (sig_rect_select.h >= 25))
          {
            for(i=0; i<mainwindow->files_open; i++)
            {
              mainwindow->zoomhistory->viewtime[mainwindow->zoomhistory->idx][i] = mainwindow->edfheaderlist[i]->viewtime;
            }
            mainwindow->zoomhistory->pagetime[mainwindow->zoomhistory->idx] = mainwindow->pagetime;
            for(i=0; i<signalcomps; i++)
            {
              mainwindow->zoomhistory->voltpercm[mainwindow->zoomhistory->idx][i] = signalcomp[i]->voltpercm;
              mainwindow->zoomhistory->sensitivity[mainwindow->zoomhistory->idx][i] = signalcomp[i]->sensitivity;
              mainwindow->zoomhistory->screen_offset_unit[mainwindow->zoomhistory->idx][i] = signalcomp[i]->screen_offset_unit;
            }
            mainwindow->zoomhistory->idx++;
            mainwindow->zoomhistory->idx %= MAXZOOMHISTORY;

            for(i=0; i<mainwindow->files_open; i++)
            {
              mainwindow->edfheaderlist[i]->viewtime += (long long)(((double)mainwindow->pagetime / (double)w) * (double)sig_rect_select.x1);
            }
            mainwindow->pagetime = (long long)((double)mainwindow->pagetime / ((double)w / (double)sig_rect_select.w));
            if(mainwindow->pagetime<1)  mainwindow->pagetime = 1;

            v_zoomfactor = h / ((double)sig_rect_select.h);

//             printf("m_y: %i\n"
//                    "mouse_press_coordinate_y: %i\n"
//                    "v_zoomfactor: %.1f\n"
//                    "mc_v_scrollarea_ratio: %i\n"
//                    "h: %i\n",
//                    m_y,
//                    mouse_press_coordinate_y,
//                    v_zoomfactor,
//                    mainwindow->mc_v_scrollarea_ratio,
//                    h);

            for(i=0; i<signalcomps; i++)
            {
              mainwindow->signalcomp[i]->screen_offset_pix *= v_zoomfactor;
              mainwindow->signalcomp[i]->screen_offset_pix += ((h * mainwindow->mc_v_scrollarea_ratio * (v_zoomfactor - 1) * (double)(i + 1)) / (signalcomps + 1));
              mainwindow->signalcomp[i]->screen_offset_pix -= (sig_rect_select.y1 * v_zoomfactor) + (scroll_h_offset * (v_zoomfactor - 1));
              mainwindow->signalcomp[i]->voltpercm /= v_zoomfactor;
              mainwindow->signalcomp[i]->sensitivity *= v_zoomfactor;
              mainwindow->signalcomp[i]->screen_offset_unit = -mainwindow->signalcomp[i]->screen_offset_pix * mainwindow->y_pixelsizefactor * mainwindow->signalcomp[i]->voltpercm;
            }

            mainwindow->zoomhistory->history_size_tail++;
            mainwindow->zoomhistory->history_size_head = 0;

            mainwindow->setup_viewbuf();
            signalcomps = mainwindow->signalcomps;
          }
          else
          {
            update();
          }
      }
      else
      {
        update();
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasoffsettracking)
      {
        signalcomp[i]->hasoffsettracking = 0;
        use_move_events = 0;
        setMouseTracking(false);
        update();
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      baseline = (((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0)) * (i + 1);
      baseline -= scroll_h_offset;

      if(signalcomp[i]->alias[0] != 0)
      {
        signallabel_strlen = strlen(signalcomp[i]->alias);
      }
      else
      {
        signallabel_strlen = strlen(signalcomp[i]->signallabel);
      }

      if((m_y<(baseline-(2*h_scaling)))&&(m_y>( baseline-(2*h_scaling)-mainwindow->mono_font_pixel_height))&&(m_x>(2*w_scaling))&&(m_x<((signallabel_strlen * mainwindow->mono_font_pixel_width) + (5 * w_scaling))))
      {
        if(pressed_on_label == (i + 1))
        {
          use_move_events = 0;
          setMouseTracking(false);
          update();

          signal_nr = i;
          exec_sidemenu(i);

          break;
        }
      }
    }
  }

  if(release_event->button()==Qt::RightButton)
  {
    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasgaintracking)
      {
        signalcomp[i]->hasgaintracking = 0;
        use_move_events = 0;
        setMouseTracking(false);
        update();
      }
    }
  }

#if QT_VERSION >= 0x050F00
  if(release_event->button()==Qt::MiddleButton)
#else
  if(release_event->button()==Qt::MidButton)
#endif
  {
    use_move_events = 0;
    setMouseTracking(false);

    panning_moving = 0;
  }

  pressed_on_label = 0;
}


void ViewCurve::mouseMoveEvent(QMouseEvent *move_event)
{
  int i, signalcomps, delta_x, delta_y;

  double d_temp;

  sigcompblck_t **signalcomp;

  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  if(use_move_events)
  {
    signalcomps = mainwindow->signalcomps;
    signalcomp = mainwindow->signalcomp;
#if QT_VERSION < 0x060000
    mouse_x = move_event->x();
    mouse_y = move_event->y();
#else
    mouse_x = move_event->position().x();
    mouse_y = move_event->position().y();
#endif
    if(pressed_on_label)
    {
      if((mouse_y > (label_press_y_position + 10)) || (mouse_y < (label_press_y_position - 10)))
      {
        pressed_on_label = 0;
      }
    }

    if(crosshair_1.moving)
    {
      if(mouse_x<(w-100))
      {
        crosshair_1.x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(crosshair_1.x_position<0)
        {
          crosshair_1.x_position = 0;
        }
      }

      if((mouse_y<(h-30))&&(mouse_y>30))
      {
        crosshair_1.y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(crosshair_1.y_position<1)
        {
          crosshair_1.y_position = 1;
        }
        else if(crosshair_1.y_position>(h-30))
          {
            crosshair_1.y_position = h - 30;
          }
      }
    }

    if(crosshair_2.moving)
    {
      if(mouse_x<(w-100))
      {
        crosshair_2.x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(crosshair_2.x_position<0)
        {
          crosshair_2.x_position = 0;
        }
      }

      if((mouse_y<(h-30))&&(mouse_y>30))
      {
        crosshair_2.y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(crosshair_2.y_position<1)
        {
          crosshair_2.y_position = 1;
        }
        else if(crosshair_2.y_position>(h-30))
          {
            crosshair_2.y_position = h - 30;
          }
      }
    }

    if(ruler_moving)
    {
      if(mouse_x<(w-100))
      {
        ruler_x_position += (mouse_x - mouse_old_x);
        mouse_old_x = mouse_x;
        if(ruler_x_position<1)
        {
          ruler_x_position = 1;
        }
      }

      if(mouse_y<(h-100))
      {
        ruler_y_position += (mouse_y - mouse_old_y);
        mouse_old_y = mouse_y;
        if(ruler_y_position<1)
        {
          ruler_y_position = 1;
        }
      }
    }

    if((annot_marker_moving) && (active_markers->edf_hdr != NULL))
    {
      active_markers->list[active_markers->selected_idx]->x_pos = mouse_x;

      active_markers->list[active_markers->selected_idx]->onset = ((long long)((((double)mouse_x) / w) * mainwindow->pagetime))
                                                               + active_markers->edf_hdr->viewtime;
    }

    delta_y = mouse_y - mouse_press_coordinate_y;

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->hasoffsettracking)
      {
        signalcomp[i]->screen_offset_pix = original_screen_offset_pix + delta_y;

        signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * mainwindow->y_pixelsizefactor * signalcomp[i]->voltpercm;
      }

      if(signalcomp[i]->hasgaintracking)
      {
        d_temp = original_sensitivity * (1.0 + ((double)(-delta_y) / 50.0));

        if(d_temp>0.000001)
        {
          signalcomp[i]->sensitivity = d_temp;

          signalcomp[i]->voltpercm = signalcomp[i]->edfparam_0->bitvalue / (signalcomp[i]->sensitivity * mainwindow->y_pixelsizefactor);
        }

        signalcomp[i]->screen_offset_pix = original_screen_offset_pix * (signalcomp[i]->sensitivity / original_sensitivity);

        signalcomp[i]->screen_offset_unit = -signalcomp[i]->screen_offset_pix * mainwindow->y_pixelsizefactor * signalcomp[i]->voltpercm;
      }
    }

    if(panning_moving)
    {
      delta_x = mouse_x - mouse_press_coordinate_x;

      d_temp = (double)delta_x / (double)w;

      if((mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET) ||
         (mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) ||
         (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
      {
        for(i=0; i<mainwindow->files_open; i++)
        {
          mainwindow->edfheaderlist[i]->viewtime = pan_mov_start_viewtime[i] - (mainwindow->pagetime * d_temp);
        }
      }

      if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
      {
        mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime = pan_mov_start_viewtime[mainwindow->sel_viewtime] - (mainwindow->pagetime * d_temp);
      }
    }

    if(draw_zoom_rectangle||annot_marker_moving)
    {
      update();
    }
    else if(panning_moving)
      {
        mainwindow->setup_viewbuf();
      }
      else
      {
        drawCurve_stage_1();
      }
  }
}


void ViewCurve::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
  paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  drawCurve_stage_2(&paint);

  if(mainwindow->dig_min_max_overflow && (!mainwindow->dig_min_max_overflow_warning_showed))
  {
    mainwindow->dig_min_max_overflow_timer->start(20);
  }
}


void ViewCurve::print_to_printer()
{
  int border_sz=500;


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  QPrinter print(QPrinter::HighResolution);

  print.setOutputFormat(QPrinter::NativeFormat);
#if QT_VERSION >= 0x050D00
  print.setPageSize(QPageSize(QPageSize::A4));
  print.setPageOrientation(QPageLayout::Landscape);
#else
  print.setPageSize(QPrinter::A4);
  print.setOrientation(QPrinter::Landscape);
#endif
  print.setCreator(PROGRAM_NAME);

  QPrintDialog printerdialog(&print, this);
  printerdialog.setWindowTitle("Print");

  if(printerdialog.exec()!=QDialog::Accepted)
  {
    return;
  }

  if(blackwhite_printing)
  {
    backup_colors_for_printing();
  }

  QPainter paint(&print);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
  paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  paint.translate(border_sz, border_sz);
#if QT_VERSION >= 0x050D00
  drawCurve_stage_1(&paint,
                    print.pageLayout().paintRectPixels(print.resolution()).width() - (border_sz * 2),
                    print.pageLayout().paintRectPixels(print.resolution()).height() - (border_sz * 2),
                    PRINT_LINE_WIDTH, PRINT_FORMAT_PRINTER);
#else
  drawCurve_stage_1(&paint,
                    print.pageRect().width() - (border_sz * 2),
                    print.pageRect().height() - (border_sz * 2),
                    PRINT_LINE_WIDTH, PRINT_FORMAT_PRINTER);
#endif
  if(blackwhite_printing)
  {
    restore_colors_after_printing();
  }

  mainwindow->setup_viewbuf();
}


void ViewCurve::print_to_pdf_A3()
{
  print_to_pdf(PRINT_FORMAT_PDF_A3);
}

void ViewCurve::print_to_pdf_A4()
{
  print_to_pdf(PRINT_FORMAT_PDF_A4);
}

void ViewCurve::print_to_pdf_Letter()
{
  print_to_pdf(PRINT_FORMAT_PDF_LETTER);
}

void ViewCurve::print_to_pdf(int paper_sz)
{
  int border_sz=500;

  char path[MAX_PATH_LENGTH]={""};


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  get_filename_from_path(path + strlen(path), mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - strlen(path));
  remove_extension_from_filename(path);
  strlcat(path, ".pdf", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Print to PDF", QString::fromLocal8Bit(path), "PDF files (*.pdf *.PDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  QPrinter print(QPrinter::HighResolution);

  print.setOutputFormat(QPrinter::PdfFormat);
  print.setOutputFileName(QString::fromLocal8Bit(path));
#if QT_VERSION >= 0x050D00
  switch(paper_sz)
  {
    case     PRINT_FORMAT_PDF_A3: print.setPageSize(QPageSize(QPageSize::A3));
             break;
    case     PRINT_FORMAT_PDF_A4: print.setPageSize(QPageSize(QPageSize::A4));
             break;
    case PRINT_FORMAT_PDF_LETTER: print.setPageSize(QPageSize(QPageSize::Letter));
             break;
    default: print.setPageSize(QPageSize(QPageSize::A4));
  }
  print.setPageOrientation(QPageLayout::Landscape);
#else
  switch(paper_sz)
  {
    case     PRINT_FORMAT_PDF_A3: print.setPageSize(QPrinter::A3);
             break;
    case     PRINT_FORMAT_PDF_A4: print.setPageSize(QPrinter::A4);
             break;
    case PRINT_FORMAT_PDF_LETTER: print.setPageSize(QPrinter::Letter);
             break;
    default: print.setPageSize(QPrinter::A4);
  }
  print.setOrientation(QPrinter::Landscape);
#endif
  print.setCreator(PROGRAM_NAME);

  if(blackwhite_printing)
  {
    backup_colors_for_printing();
  }

  QPainter paint(&print);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
    paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  paint.translate(border_sz, border_sz);
#if QT_VERSION >= 0x050D00
  drawCurve_stage_1(&paint,
                    print.pageLayout().paintRectPixels(print.resolution()).width() - (border_sz * 2),
                    print.pageLayout().paintRectPixels(print.resolution()).height() - (border_sz * 2),
                    PRINT_LINE_WIDTH, paper_sz);
#else
  drawCurve_stage_1(&paint,
                    print.pageRect().width() - (border_sz * 2),
                    print.pageRect().height() - (border_sz * 2),
                    PRINT_LINE_WIDTH, paper_sz);
#endif

  if(blackwhite_printing)
  {
    restore_colors_after_printing();
  }

  mainwindow->setup_viewbuf();
}


void ViewCurve::print_to_image(int w_img, int h_img, int img_format)
{
  char path[MAX_PATH_LENGTH]={""};


  if(!mainwindow->files_open)  return;
  if(!mainwindow->signalcomps)  return;

  if(mainwindow->recent_savedir[0]!=0)
  {
    strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  get_filename_from_path(path + strlen(path), mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - strlen(path));
  remove_extension_from_filename(path);
  strlcat(path, ".png", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Print to Image", QString::fromLocal8Bit(path), "PNG (*.png *.PNG)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  QPixmap pixmap(w_img, h_img);

  QPainter paint(&pixmap);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
    paint.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  drawCurve_stage_1(&paint, w_img, h_img, 0, img_format);

  pixmap.save(QString::fromLocal8Bit(path), "PNG", 90);

  mainwindow->setup_viewbuf();
}


void ViewCurve::drawCurve_stage_1(QPainter *painter, int w_width, int w_height, int print_linewidth, int print_format)
{
  int i, j, n, x1, y1, x2, y2,
      tmp,
      signalcomps,
      baseline,
      value,
      minimum,
      maximum,
      stat_zero_crossing=0;

  char *viewbuf=NULL;

  unsigned long long s, s2, total_samples_processed;

  double dig_value=0,
         f_tmp=0;

  sigcompblck_t **signalcomp=NULL;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;


  if(mainwindow->exit_in_progress)
  {
    free(graphicBuf);
    graphicBuf = NULL;
    return;
  }

  memset(screensamples, 0, sizeof(int[MAXSIGNALS]));

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;
  linear_interpol = mainwindow->linear_interpol;

  painter_pixelsizefactor = 1 / mainwindow->y_pixelsizefactor;

  if(mainwindow->mc_v_scrollarea_auto)
  {
    mainwindow->mc_v_scrollarea_ratio = signalcomps / mainwindow->mc_v_scrollarea_max_signals;

    if(signalcomps % mainwindow->mc_v_scrollarea_max_signals)
    {
      mainwindow->mc_v_scrollarea_ratio++;
    }

    if(!mainwindow->mc_v_scrollarea_ratio)
    {
      mainwindow->mc_v_scrollarea_ratio++;
    }

    if(mainwindow->mc_v_scrollarea_ratio > 1)
    {
      mainwindow->mc_v_scrollbar->setPageStep(100000 / (mainwindow->mc_v_scrollarea_ratio - 1));

      mainwindow->mc_v_scrollbar->setEnabled(true);
    }
    else
    {
      mainwindow->mc_v_scrollbar->setEnabled(false);
    }
  }
  else
  {
    mainwindow->mc_v_scrollarea_ratio = 1;

    mainwindow->mc_v_scrollbar->setEnabled(false);
  }

  if(w_width && w_height && (print_format != PRINT_FORMAT_NONE))  /* printing */
  {
    w = w_width;
    h = w_height * mainwindow->mc_v_scrollarea_ratio;

    scroll_h_offset = w_height * (mainwindow->mc_v_scrollarea_ratio - 1) * (scrollbar_h_value / 100000.0);

    printsize_x_factor = ((double)w_width) / ((double)width());
    printsize_y_factor = ((double)w_height) / ((double)height());

    painter_pixelsizefactor *= printsize_y_factor;

    printing = 1;
  }
  else  /* not printing */
  {
    w = width();
    h = height() * mainwindow->mc_v_scrollarea_ratio;

    scroll_h_offset = height() * (mainwindow->mc_v_scrollarea_ratio - 1) * (scrollbar_h_value / 100000.0);

    printsize_x_factor = 1;
    printsize_y_factor = 1;

    printing = 0;
  }

  if((w <8)||(h<8))  /* sanity check */
  {
    free(graphicBuf);
    graphicBuf = NULL;
    update();
    return;
  }

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->sample_pixel_ratio = (signalcomp[i]->edfparam_0->sf_f * ((double)mainwindow->pagetime / (double)TIME_FIXP_SCALING)) / (double)w;
  }

  if((viewbuf==NULL)||(screensamples==NULL)||(!signalcomps))
  {
    free(graphicBuf);
    graphicBuf = NULL;
    update();
    return;
  }

  if((graphicBuf==NULL)||(graphicBufWidth!=w))
  {
    free(graphicBuf);

    graphicBuf = (graphbuf_t *)malloc(sizeof(graphbuf_t) * w * 2 + 4);

    graphicBufWidth = w;
  }

  if(graphicBuf==NULL)
  {
    QMessageBox::critical(this, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    return;
  }

  if(mainwindow->use_threads)
  {
    n = cpu_cnt;

    if(signalcomps < n)
    {
      n = signalcomps;
    }

// for(i=0; i<n; i++)
// {
//   printf("start:  signalcomp is %08X      screensamples is %08X\n---------------------\n", (int)(signalcomp[i]), (int)(&screensamples[i]));
// }

    for(i=0, total_samples_processed=0LL; i<signalcomps; i++)
    {
      if(signalcomp[i]->sample_stop > signalcomp[i]->sample_start)
      {
        total_samples_processed += (signalcomp[i]->sample_stop - signalcomp[i]->sample_start);
      }
    }

//    printf("total_samples_processed: %llu\n", total_samples_processed);

    for(i=0; i<n; i++)
    {
      thr[i]->init_vars(mainwindow, &signalcomp[0], i,
                        signalcomps, viewbuf, w, h, &screensamples[0], printing,
                        graphicBuf, printsize_x_factor,
                        printsize_y_factor, &crosshair_1, &crosshair_2, cpu_cnt, linear_interpol);

      thr[i]->start();
    }

    for(i=0; i<n; i++)
    {
      thr[i]->wait();
    }
  }
  else
  {
    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->max_dig_value = 0x80000000;
      signalcomp[i]->min_dig_value = 0x7fffffff;
      signalcomp[i]->stat_cnt = 0;
      signalcomp[i]->stat_zero_crossing_cnt = 0;
      signalcomp[i]->stat_sum = 0;
      signalcomp[i]->stat_sum_sqr = 0;
      signalcomp[i]->stat_sum_rectified = 0;

      baseline = ((double)h / (signalcomps + 1.0)) * (i + 1);

      signalcomp[i]->pixels_shift = signalcomp[i]->sample_timeoffset_part / signalcomp[i]->sample_pixel_ratio;

      for(s=signalcomp[i]->sample_start; s<signalcomp[i]->samples_on_screen; s++)
      {
        if(s>=signalcomp[i]->sample_stop)  break;

        dig_value = 0;
        s2 = s + signalcomp[i]->sample_timeoffset - signalcomp[i]->sample_start;

        for(j=0; j<signalcomp[i]->num_of_signals; j++)
        {
          if(signalcomp[i]->edfhdr->bdf)
          {
            var.two[0] = *((unsigned short *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset
              + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)));

            var.four[2] = *((unsigned char *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset
              + ((s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record) * 3)
              + 2));

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            if(var.one_signed > signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max)
            {
              var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max;

              mainwindow->dig_min_max_overflow = 1;
            }
            else if(var.one_signed < signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min)
              {
                var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min;

                mainwindow->dig_min_max_overflow = 1;
              }

            f_tmp = var.one_signed;
          }

          if(signalcomp[i]->edfhdr->edf)
          {
            tmp = *(((short *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].datrec_offset))
              + (s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record));

            if(tmp > signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max)
            {
              tmp = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_max;

              mainwindow->dig_min_max_overflow = 1;
            }
            else if(tmp < signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min)
              {
                tmp = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].dig_min;

                mainwindow->dig_min_max_overflow = 1;
              }

            f_tmp = tmp;
          }

          f_tmp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
          f_tmp *= signalcomp[i]->factor[j];

          dig_value += f_tmp;
        }

        run_filter_list(signalcomp[i], s, &dig_value);

        if(printing)
        {
          value = (int)(dig_value * signalcomp[i]->sensitivity * printsize_y_factor) * signalcomp[i]->polarity;
        }
        else
        {
          value = (int)(dig_value * signalcomp[i]->sensitivity) * signalcomp[i]->polarity;

          signalcomp[i]->stat_cnt++;
          signalcomp[i]->stat_sum += dig_value;
          signalcomp[i]->stat_sum_sqr += ((dig_value) * (dig_value));
          if(dig_value < 0)
          {
            signalcomp[i]->stat_sum_rectified += (dig_value * -1.0);
          }
          else
          {
            signalcomp[i]->stat_sum_rectified += dig_value;
          }

          if(s==signalcomp[i]->sample_start)
          {
            if(dig_value < 0.0)
            {
              stat_zero_crossing = 0;
            }
            else
            {
              stat_zero_crossing = 1;
            }
          }
          else
          {
            if(dig_value < 0.0)
            {
              if(stat_zero_crossing)
              {
                stat_zero_crossing = 0;

                signalcomp[i]->stat_zero_crossing_cnt++;
              }
            }
            else
            {
              if(!stat_zero_crossing)
              {
                stat_zero_crossing = 1;

                signalcomp[i]->stat_zero_crossing_cnt++;
              }
            }
          }
        }

        if(((int)dig_value)>signalcomp[i]->max_dig_value)  signalcomp[i]->max_dig_value = dig_value;
        if(((int)dig_value)<signalcomp[i]->min_dig_value)  signalcomp[i]->min_dig_value = dig_value;

        value = baseline - value + (signalcomp[i]->screen_offset_pix * printsize_y_factor);

        if(s>=signalcomp[i]->sample_start)
        {
          x1 = (int)((double)s / signalcomp[i]->sample_pixel_ratio);
          y1 = signalcomp[i]->oldvalue;
          x2 = (int)(((double)(s + 1)) / signalcomp[i]->sample_pixel_ratio);
          y2 = value;

          if(signalcomp[i]->samples_on_screen < (unsigned long long)w)
          {
            if(linear_interpol)
            {
              if(s>signalcomp[i]->sample_start)
              {
                x1 = (int)(((double)(s - 1)) / signalcomp[i]->sample_pixel_ratio);
                x2 = (int)((double)s / signalcomp[i]->sample_pixel_ratio);

                graphicBuf[screensamples[i]].graphic_line[i].x1 = x1 - signalcomp[i]->pixels_shift;
                graphicBuf[screensamples[i]].graphic_line[i].y1 = y1;
                graphicBuf[screensamples[i]].graphic_line[i].x2 = x2 - signalcomp[i]->pixels_shift;
                graphicBuf[screensamples[i]].graphic_line[i].y2 = y2;

                screensamples[i]++;
              }
            }
            else
            {
              graphicBuf[screensamples[i]].graphic_line[i].x1 = x1 - signalcomp[i]->pixels_shift;
              graphicBuf[screensamples[i]].graphic_line[i].y1 = y2;
              graphicBuf[screensamples[i]].graphic_line[i].x2 = x2 - signalcomp[i]->pixels_shift;
              graphicBuf[screensamples[i]].graphic_line[i].y2 = y2;

              if(screensamples[i])
              {
                screensamples[i]++;

                graphicBuf[screensamples[i]].graphic_line[i].x1 = x1 - signalcomp[i]->pixels_shift;
                graphicBuf[screensamples[i]].graphic_line[i].y1 = y1;
                graphicBuf[screensamples[i]].graphic_line[i].x2 = x1 - signalcomp[i]->pixels_shift;
                graphicBuf[screensamples[i]].graphic_line[i].y2 = y2;
              }

              screensamples[i]++;
            }
          }
          else
          {
            if(!screensamples[i])
            {
              graphicBuf[screensamples[i]].graphic_line[i].x1 = x1;
              graphicBuf[screensamples[i]].graphic_line[i].y1 = y1;
              graphicBuf[screensamples[i]].graphic_line[i].x2 = x2;
              graphicBuf[screensamples[i]].graphic_line[i].y2 = y2;

              screensamples[i]++;
            }
            else
            {
              if((x1==x2)&&(x1==graphicBuf[screensamples[i]-1].graphic_line[i].x1)&&
                (graphicBuf[screensamples[i]-1].graphic_line[i].x1==graphicBuf[screensamples[i]-1].graphic_line[i].x2))
              {
                maximum = y1;
                if(y2>maximum)  maximum = y2;
                if(graphicBuf[screensamples[i]-1].graphic_line[i].y1>maximum)  maximum = graphicBuf[screensamples[i]-1].graphic_line[i].y1;
                if(graphicBuf[screensamples[i]-1].graphic_line[i].y2>maximum)  maximum = graphicBuf[screensamples[i]-1].graphic_line[i].y2;

                minimum = y1;
                if(y2<minimum)  minimum = y2;
                if(graphicBuf[screensamples[i]-1].graphic_line[i].y1<minimum)  minimum = graphicBuf[screensamples[i]-1].graphic_line[i].y1;
                if(graphicBuf[screensamples[i]-1].graphic_line[i].y2<minimum)  minimum = graphicBuf[screensamples[i]-1].graphic_line[i].y2;

                graphicBuf[screensamples[i]-1].graphic_line[i].y1 = maximum;
                graphicBuf[screensamples[i]-1].graphic_line[i].y2 = minimum;
              }
              else
              {
                graphicBuf[screensamples[i]].graphic_line[i].x1 = x1;
                graphicBuf[screensamples[i]].graphic_line[i].y1 = y1;
                graphicBuf[screensamples[i]].graphic_line[i].x2 = x2;
                graphicBuf[screensamples[i]].graphic_line[i].y2 = y2;

                if(screensamples[i]<(w * 2))  screensamples[i]++;
              }
            }
          }
        }

        signalcomp[i]->oldvalue = value;

        if(signalcomp[i]->hascursor1)
        {
          if(printing)
          {
            if(s == (unsigned long long)((double)(crosshair_1.x_position * printsize_x_factor + (double)signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio))
            {
              crosshair_1.y_value = value;
              crosshair_1.value = dig_value * signalcomp[i]->edfparam_0->bitvalue;
              crosshair_1.time_clock = signalcomp[i]->edfhdr->starttime_hr + signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1.x_position);
              crosshair_1.time_relative = signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1.x_position);
            }
          }
          else
          {
            if(s == (unsigned long long)((double)(crosshair_1.x_position + signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio))
            {
              crosshair_1.y_value = value;
              crosshair_1.value = dig_value * signalcomp[i]->edfparam_0->bitvalue;
              crosshair_1.time_clock = signalcomp[i]->edfhdr->starttime_hr + signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1.x_position);
              crosshair_1.time_relative = signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1.x_position);
            }
          }
        }

        if(signalcomp[i]->hascursor2)
        {
          if(printing)
          {
            if(s == (unsigned long long)((double)(crosshair_2.x_position * printsize_x_factor + (double)signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio))
            {
              crosshair_2.y_value = value;
              crosshair_2.value = dig_value * signalcomp[i]->edfparam_0->bitvalue;
              crosshair_2.time_clock = signalcomp[i]->edfhdr->starttime_hr + signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2.x_position);
              crosshair_2.time_relative = signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2.x_position);
            }
          }
          else
          {
            if(s == (unsigned long long)((double)(crosshair_2.x_position + signalcomp[i]->pixels_shift) * signalcomp[i]->sample_pixel_ratio))
            {
              crosshair_2.y_value = value;
              crosshair_2.value = dig_value * signalcomp[i]->edfparam_0->bitvalue;
              crosshair_2.time_clock = signalcomp[i]->edfhdr->starttime_hr + signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2.x_position);
              crosshair_2.time_relative = signalcomp[i]->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2.x_position);
            }
          }
        }
      }
    }
  }

  if(printing)
  {
    drawCurve_stage_2(painter, w_width, w_height, print_linewidth, print_format);
  }
  else
  {
    update();
  }
}


void drawCurve_stage_1_thread::init_vars(UI_Mainwindow *mainwindow_a, sigcompblck_t **signalcomp_a, int i_a,
                                         int signalcomps_a, char *viewbuf_a, int w_a, int h_a, int *screensamples_a, int printing_a,
                                         graphbuf_t *graphicBuf_a, double printsize_x_factor_a,
                                         double printsize_y_factor_a, crosshair_t *crosshair_1_a,
                                         crosshair_t *crosshair_2_a, int cpu_cnt_a, int linear_interpol_a)
{
  mainwindow = mainwindow_a;
  signalcomp_b = signalcomp_a;
  i = i_a;
  signalcomps = signalcomps_a;
  viewbuf = viewbuf_a;
  w = w_a;
  h = h_a;
  screensamples_b = screensamples_a;
  printing = printing_a;
  graphicBuf = graphicBuf_a;
  printsize_x_factor = printsize_x_factor_a;
  printsize_y_factor = printsize_y_factor_a;
  crosshair_1 = crosshair_1_a;
  crosshair_2 = crosshair_2_a;
  cpu_cnt = cpu_cnt_a;
  linear_interpol = linear_interpol_a;

// printf("init_vars(): i is %i   signalcomp_b is %08X      screensamples_b is %08X\n------------------------\n",
//        i, (int)signalcomp_b[i], (int)screensamples_b);

}


void drawCurve_stage_1_thread::run()
{
  int j, x1, y1, x2, y2,
      tmp,
      baseline,
      value,
      minimum,
      maximum,
      stat_zero_crossing;

  unsigned long long s, s2;

  double dig_value,
         f_tmp;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;


  for(; i < signalcomps; i += cpu_cnt)
  {

// printf("i is %i   cpu_cnt is %i\n", i, cpu_cnt);

    screensamples = &screensamples_b[i];

    signalcomp = signalcomp_b[i];


// printf("i is %i   signalcomp is %08X      screensamples is %08X\n",
//        i, (int)signalcomp, (int)screensamples);

    f_tmp = 0;
    dig_value=0;
    stat_zero_crossing=0;

    signalcomp->max_dig_value = 0x80000000;
    signalcomp->min_dig_value = 0x7fffffff;
    signalcomp->stat_cnt = 0;
    signalcomp->stat_zero_crossing_cnt = 0;
    signalcomp->stat_sum = 0;
    signalcomp->stat_sum_sqr = 0;
    signalcomp->stat_sum_rectified = 0;

    baseline = ((double)h / (signalcomps + 1.0)) * (i + 1);

    signalcomp->pixels_shift = signalcomp->sample_timeoffset_part / signalcomp->sample_pixel_ratio;

    for(s=signalcomp->sample_start; s < signalcomp->samples_on_screen; s++)
    {
      if(s>=signalcomp->sample_stop)  break;

      dig_value = 0;
      s2 = s + signalcomp->sample_timeoffset - signalcomp->sample_start;

      for(j=0; j<signalcomp->num_of_signals; j++)
      {
        if(signalcomp->edfhdr->bdf)
        {
          var.two[0] = *((unsigned short *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset
            + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)));

          var.four[2] = *((unsigned char *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset
            + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)
            + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          if(var.one_signed > signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max)
          {
            var.one_signed = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max;

            mainwindow->dig_min_max_overflow = 1;
          }
          else if(var.one_signed < signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min)
            {
              var.one_signed = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min;

              mainwindow->dig_min_max_overflow = 1;
            }

          f_tmp = var.one_signed;
        }

        if(signalcomp->edfhdr->edf)
        {
          tmp = *(((short *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset))
            + (s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record));

          if(tmp > signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max)
          {
            tmp = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max;

            mainwindow->dig_min_max_overflow = 1;
          }
          else if(tmp < signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min)
            {
              tmp = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min;

              mainwindow->dig_min_max_overflow = 1;
            }

          f_tmp = tmp;
        }

        f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
        f_tmp *= signalcomp->factor[j];

        dig_value += f_tmp;
      }

      run_filter_list(signalcomp, s, &dig_value);

      if(printing)
      {
        value = (int)(dig_value * signalcomp->sensitivity * printsize_y_factor) * signalcomp->polarity;
      }
      else
      {
        value = (int)(dig_value * signalcomp->sensitivity * signalcomp->polarity);

        signalcomp->stat_cnt++;
        signalcomp->stat_sum += dig_value;
        signalcomp->stat_sum_sqr += ((dig_value) * (dig_value));
        if(dig_value < 0)
        {
          signalcomp->stat_sum_rectified += (dig_value * -1.0);
        }
        else
        {
          signalcomp->stat_sum_rectified += dig_value;
        }

        if(s==signalcomp->sample_start)
        {
          if(dig_value < 0.0)
          {
            stat_zero_crossing = 0;
          }
          else
          {
            stat_zero_crossing = 1;
          }
        }
        else
        {
          if(dig_value < 0.0)
          {
            if(stat_zero_crossing)
            {
              stat_zero_crossing = 0;

              signalcomp->stat_zero_crossing_cnt++;
            }
          }
          else
          {
            if(!stat_zero_crossing)
            {
              stat_zero_crossing = 1;

              signalcomp->stat_zero_crossing_cnt++;
            }
          }
        }
      }

      if(dig_value > signalcomp->max_dig_value)  signalcomp->max_dig_value = dig_value;
      if(dig_value < signalcomp->min_dig_value)  signalcomp->min_dig_value = dig_value;

      if(printing)
      {
        value = baseline - value + (int)(signalcomp->screen_offset_pix * printsize_y_factor);
      }
      else
      {
        value = baseline - value + signalcomp->screen_offset_pix;
      }

      if(s>=signalcomp->sample_start)
      {
        x1 = (int)((double)s / signalcomp->sample_pixel_ratio);
        y1 = signalcomp->oldvalue;
        x2 = (int)(((double)(s + 1)) / signalcomp->sample_pixel_ratio);
        y2 = value;

        if(signalcomp->samples_on_screen < (unsigned long long)w)
        {
          if(linear_interpol)
          {
            if(s>signalcomp->sample_start)
            {
              x1 = (int)(((double)(s - 1)) / signalcomp->sample_pixel_ratio);
              x2 = (int)((double)s / signalcomp->sample_pixel_ratio);

              graphicBuf[*screensamples].graphic_line[i].x1 = x1 - signalcomp->pixels_shift;
              graphicBuf[*screensamples].graphic_line[i].y1 = y1;
              graphicBuf[*screensamples].graphic_line[i].x2 = x2 - signalcomp->pixels_shift;
              graphicBuf[*screensamples].graphic_line[i].y2 = y2;

              (*screensamples)++;
            }
          }
          else
          {
            graphicBuf[*screensamples].graphic_line[i].x1 = x1 - signalcomp->pixels_shift;
            graphicBuf[*screensamples].graphic_line[i].y1 = y2;
            graphicBuf[*screensamples].graphic_line[i].x2 = x2 - signalcomp->pixels_shift;
            graphicBuf[*screensamples].graphic_line[i].y2 = y2;

            if(*screensamples)
            {
              (*screensamples)++;

              graphicBuf[*screensamples].graphic_line[i].x1 = x1 - signalcomp->pixels_shift;
              graphicBuf[*screensamples].graphic_line[i].y1 = y1;
              graphicBuf[*screensamples].graphic_line[i].x2 = x1 - signalcomp->pixels_shift;
              graphicBuf[*screensamples].graphic_line[i].y2 = y2;
            }

            (*screensamples)++;
          }
        }
        else
        {
          if(!(*screensamples))
          {
            graphicBuf[*screensamples].graphic_line[i].x1 = x1;
            graphicBuf[*screensamples].graphic_line[i].y1 = y1;
            graphicBuf[*screensamples].graphic_line[i].x2 = x2;
            graphicBuf[*screensamples].graphic_line[i].y2 = y2;

            (*screensamples)++;
          }
          else
          {
            if((x1==x2)&&(x1==graphicBuf[(*screensamples)-1].graphic_line[i].x1)&&
              (graphicBuf[(*screensamples)-1].graphic_line[i].x1==graphicBuf[(*screensamples)-1].graphic_line[i].x2))
            {
              maximum = y1;
              if(y2>maximum)  maximum = y2;
              if(graphicBuf[(*screensamples)-1].graphic_line[i].y1>maximum)  maximum = graphicBuf[(*screensamples)-1].graphic_line[i].y1;
              if(graphicBuf[(*screensamples)-1].graphic_line[i].y2>maximum)  maximum = graphicBuf[(*screensamples)-1].graphic_line[i].y2;

              minimum = y1;
              if(y2<minimum)  minimum = y2;
              if(graphicBuf[(*screensamples)-1].graphic_line[i].y1<minimum)  minimum = graphicBuf[(*screensamples)-1].graphic_line[i].y1;
              if(graphicBuf[(*screensamples)-1].graphic_line[i].y2<minimum)  minimum = graphicBuf[(*screensamples)-1].graphic_line[i].y2;

              graphicBuf[(*screensamples)-1].graphic_line[i].y1 = maximum;
              graphicBuf[(*screensamples)-1].graphic_line[i].y2 = minimum;
            }
            else
            {
              graphicBuf[*screensamples].graphic_line[i].x1 = x1;
              graphicBuf[*screensamples].graphic_line[i].y1 = y1;
              graphicBuf[*screensamples].graphic_line[i].x2 = x2;
              graphicBuf[*screensamples].graphic_line[i].y2 = y2;

              if((*screensamples)<(w * 2))  (*screensamples)++;
            }
          }
        }
      }

      signalcomp->oldvalue = value;

      if(signalcomp->hascursor1)
      {
        if(printing)
        {
          if(s == (unsigned long long)((double)(crosshair_1->x_position * printsize_x_factor + (double)signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio))
          {
            crosshair_1->y_value = value;
            crosshair_1->value = dig_value * signalcomp->edfparam_0->bitvalue;
            crosshair_1->time_clock = signalcomp->edfhdr->starttime_hr + signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1->x_position);
            crosshair_1->time_relative = signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_1->x_position);
          }
        }
        else
        {
          if(s == (unsigned long long)((double)(crosshair_1->x_position + signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio))
          {
            crosshair_1->y_value = value;
            crosshair_1->value = dig_value * signalcomp->edfparam_0->bitvalue;
            crosshair_1->time_clock = signalcomp->edfhdr->starttime_hr + signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1->x_position);
            crosshair_1->time_relative = signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_1->x_position);
          }
        }
      }

      if(signalcomp->hascursor2)
      {
        if(printing)
        {
          if(s == (unsigned long long)((double)(crosshair_2->x_position * printsize_x_factor + (double)signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio))
          {
            crosshair_2->y_value = value;
            crosshair_2->value = dig_value * signalcomp->edfparam_0->bitvalue;
            crosshair_2->time_clock = signalcomp->edfhdr->starttime_hr + signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2->x_position);
            crosshair_2->time_relative = signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / ((double)w / printsize_x_factor)) * (double)crosshair_2->x_position);
          }
        }
        else
        {
          if(s == (unsigned long long)((double)(crosshair_2->x_position + signalcomp->pixels_shift) * signalcomp->sample_pixel_ratio))
          {
            crosshair_2->y_value = value;
            crosshair_2->value = dig_value * signalcomp->edfparam_0->bitvalue;
            crosshair_2->time_clock = signalcomp->edfhdr->starttime_hr + signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2->x_position);
            crosshair_2->time_relative = signalcomp->edfhdr->viewtime + (long long)(((double)mainwindow->pagetime / (double)w) * (double)crosshair_2->x_position);
          }
        }
      }
    }
  }
}


void ViewCurve::resizeEvent(QResizeEvent *rs_event)
{
  drawCurve_stage_1();

  QWidget::resizeEvent(rs_event);
}


void ViewCurve::backup_colors_for_printing()
{
  int i;

  backup_color_1 = backgroundcolor;
  backgroundcolor = Qt::white;
  backup_color_2 = small_ruler_color;
  small_ruler_color = Qt::black;
  backup_color_3 = big_ruler_color;
  big_ruler_color = Qt::gray;
  backup_color_4 = mouse_rect_color;
  mouse_rect_color = Qt::black;
  backup_color_5 = text_color;
  text_color = Qt::black;
  backup_color_14 = floating_ruler_color;
  floating_ruler_color = Qt::black;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    backup_color_10[i] = mainwindow->signalcomp[i]->color;
    mainwindow->signalcomp[i]->color = Qt::black;
  }
  backup_color_11 = crosshair_1.color;
  crosshair_1.color = Qt::black;
  backup_color_12 = crosshair_2.color;
  crosshair_2.color = Qt::black;
  backup_color_13 = baseline_color;
  baseline_color = Qt::gray;
  backup_color_15 = annot_marker_color;
  annot_marker_color = Qt::black;
  backup_color_16 = annot_marker_selected_color;
  annot_marker_selected_color = Qt::black;
  backup_color_17 = annot_duration_color;
  annot_duration_color = QColor(192, 192, 192, 32);
  backup_color_18 = annot_duration_color_selected;
  annot_duration_color_selected = QColor(192, 192, 192, 32);

  for(i=0; i<8; i++)
  {
    mc_annot_ov_color_predefined_bu[i] = mainwindow->mc_annot_ov_color_predefined[i];
    mainwindow->mc_annot_ov_color_predefined[i] = QColor(192, 192, 192, 32);
  }
}


void ViewCurve::restore_colors_after_printing()
{
  int i;

  backgroundcolor = backup_color_1;
  small_ruler_color = backup_color_2;
  big_ruler_color = backup_color_3;
  mouse_rect_color = backup_color_4;
  text_color = backup_color_5;
  floating_ruler_color = backup_color_14;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = backup_color_10[i];
  }
  crosshair_1.color = backup_color_11;
  crosshair_2.color = backup_color_12;
  baseline_color = backup_color_13;
  annot_marker_color = backup_color_15;
  annot_marker_selected_color = backup_color_16;
  annot_duration_color = backup_color_17;
  annot_duration_color_selected = backup_color_18;

  for(i=0; i<8; i++)
  {
    mainwindow->mc_annot_ov_color_predefined[i] = mc_annot_ov_color_predefined_bu[i];
  }
}


void ViewCurve::setCrosshair_1_center(void)
{
  crosshair_1.x_position = width() / 2;
}


void ViewCurve::dragEnterEvent(QDragEnterEvent *e)
{
  if(e->mimeData()->hasUrls())
  {
    e->acceptProposedAction();
  }
}


void ViewCurve::dropEvent(QDropEvent *e)
{
  if(e->mimeData()->urls().count() < 1)  return;

  strlcpy(mainwindow->drop_path, e->mimeData()->urls().first().toLocalFile().toLocal8Bit().data(), MAX_PATH_LENGTH);

  emit file_dropped();
}


void ViewCurve::select_annot_clicked(bool)
{
  int i, idx;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  idx = active_marker_context_menu_request_idx;
  if((idx < 0) || (idx >= active_markers->count))
  {
    return;
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i] != NULL)
    {
      edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[i]->annot_list);
    }
  }

  for(i=0; i<active_markers->count; i++)
  {
    if(i == idx)
    {
      active_markers->list[i]->selected_in_dock = 1;
    }
    else
    {
      active_markers->list[i]->selected_in_dock = 0;
    }
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i] != NULL)
    {
      mainwindow->annotations_dock[i]->updateList(1);
    }
  }
}


void ViewCurve::hide_annot_clicked(bool)
{
  int i, idx;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  idx = active_marker_context_menu_request_idx;
  if((idx < 0) || (idx >= active_markers->count))
  {
    return;
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i] != NULL)
    {
      edfplus_annotation_cancel_all_selected_in_dock(&mainwindow->edfheaderlist[i]->annot_list);
    }
  }

  for(i=0; i<active_markers->count; i++)
  {
    if(i == idx)
    {
      active_markers->list[i]->hidden = 1;

      break;
    }

    active_markers->list[i]->selected_in_dock = 0;
    active_markers->selected_idx = 0;
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i] != NULL)
    {
      mainwindow->annotations_dock[i]->updateList(1);
    }
  }
}


void ViewCurve::average_annot_clicked(bool)
{
  int idx;

  if(mainwindow->files_open < 1)
  {
    return;
  }

  if(!mainwindow->signalcomps)
  {
    QMessageBox::critical(this, "Error", "First add a signal to the screen.");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(this, "Error", "Close the annotation editor and try again.");
    return;
  }

  idx = active_marker_context_menu_request_idx;
  if((idx < 0) || (idx >= active_markers->count))
  {
    return;
  }

  UI_AveragerWindow average_wndw(mainwindow, active_markers->list[idx]);
}


void ViewCurve::set_vertical_scroll_value(int val)
{
  scrollbar_h_value = val;

  scroll_h_offset = height() * (mainwindow->mc_v_scrollarea_ratio - 1) * (scrollbar_h_value / 100000.0);
}
















