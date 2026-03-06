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



// The ECG waves are recorded on special graph paper that is divided into 1 mm2 grid-like boxes. The ECG paper speed is ordinarily 25 mm/sec.
// As a result, each 1 mm (small) horizontal box corresponds to 0.04 sec (40 ms), with heavier lines forming larger boxes that include five
// small boxes and hence represent 0.20 sec (200 ms) intervals. On occasion, the paper speed is increased to 50 mm/sec to better define waveforms.
// In this situation, there are only six leads per sheet of paper. Each large box is therefore only 0.10 sec and each small box is only 0.02 sec.
// In addition, the heart rate appears to be one-half of what is recorded at 25 mm/sec paper speed, and all of the ECG intervals are twice as
// long as normal. Other paper speeds are occasionally used.
//
// Vertically, the ECG graph measures the height (amplitude) of a given wave or deflection. The standard calibration is 10 mm (10 small boxes),
// equal to 1 mV. On occasion, particularly when the waveforms are small, double standard is used (20 mm equals 1 mv). When the wave forms are
// very large, half standard may be used (5 mm equals 1 mv). Paper speed and voltage are usually printed on the bottom of the ECG for reference.

void ViewCurve::draw_ecg_horizontal_graticule(QPainter *painter, int print_linewidth)
{
  int x_pix=0,
      m_pagetime;

  long long time_ppixel,
            ll_elapsed_time;

  char str4_1024[1024]="";

  m_pagetime = (int)(mainwindow->pagetime / TIME_FIXP_SCALING);

  time_ppixel = mainwindow->pagetime / w;

  if(mainwindow->files_open&&mainwindow->signalcomps)
  {
    ll_elapsed_time = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime;
  }
  else
  {
    ll_elapsed_time = 0;
  }

  ruler_pen->setWidth(print_linewidth);
  ruler_pen->setColor(small_ruler_color);
  painter->setPen(*ruler_pen);

  if((mainwindow->pagetime / (TIME_FIXP_SCALING / 100000)) <=1)  // timescale less than or equal to 10 microsecond
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 100000, 10);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 10000)) <=1)  // timescale less than or equal to 100 microsecond
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 10000, 100);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 1000)) <=1)  // timescale less than or equal to 1 millisecond
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 1000, 1000);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 100)) <=1)  // timescale less than or equal to 10 millisecond
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 100, 10);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 10)) <=1)  // timescale less than or equal to 100 millisecond
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 10, 100);
  }
  else if(m_pagetime<=1)  // timescale less than or equal to 1 second
  {
    draw_ecg_horizontal_hr_graticule(painter, ll_elapsed_time, 1, 1000);
  }
  else if(m_pagetime<60)  // timescale less than 100 seconds (00:01:00)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if(m_pagetime <= 33)
      {
        if((ll_elapsed_time / (TIME_FIXP_SCALING / 25))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING / 25)))
        {
          painter->drawLine(x_pix, 0, x_pix, h);
        }
      }

      if(((ll_elapsed_time / (TIME_FIXP_SCALING / 5))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING / 5))) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);
      }

      if(((ll_elapsed_time / TIME_FIXP_SCALING)!=((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        if(ll_elapsed_time < 0LL)
        {
          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 60);
        }
        else
        {
          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 60);
        }
        painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else
  {
    draw_generic_horizontal_graticule(painter, m_pagetime);
  }
}

void ViewCurve::draw_default_horizontal_graticule(QPainter *painter, int print_linewidth)
{
  int m_pagetime;

  long long ll_elapsed_time;

  m_pagetime = (int)(mainwindow->pagetime / TIME_FIXP_SCALING);

  if(mainwindow->files_open&&mainwindow->signalcomps)
  {
    ll_elapsed_time = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime;
  }
  else
  {
    ll_elapsed_time = 0;
  }

  ruler_pen->setWidth(print_linewidth);
  ruler_pen->setColor(small_ruler_color);
  painter->setPen(*ruler_pen);

  if((mainwindow->pagetime / (TIME_FIXP_SCALING / 100000)) <=1)  // timescale less than or equal to 10 microsecond
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 100000, 10);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 10000)) <=1)  // timescale less than or equal to 100 microsecond
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 10000, 100);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 1000)) <=1)  // timescale less than or equal to 1 millisecond
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 1000, 1000);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 100)) <=1)  // timescale less than or equal to 10 millisecond
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 100, 10);
  }
  else if((mainwindow->pagetime / (TIME_FIXP_SCALING / 10)) <=1)  // timescale less than or equal to 100 millisecond
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 10, 100);
  }
  else if(m_pagetime<=1)  // timescale less than or equal to 1 second
  {
    draw_default_horizontal_hr_graticule(painter, ll_elapsed_time, 1, 1000);
  }
  else
  {
    draw_generic_horizontal_graticule(painter, m_pagetime);
  }
}

void ViewCurve::draw_generic_horizontal_graticule(QPainter *painter, int m_pagetime)
{
  int x_pix=0;

  long long time_ppixel,
            ll_elapsed_time;

  char str4_1024[1024]="";

  time_ppixel = mainwindow->pagetime / w;

  if(mainwindow->files_open&&mainwindow->signalcomps)
  {
    ll_elapsed_time = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime;
  }
  else
  {
    ll_elapsed_time = 0;
  }

  if(m_pagetime<=20)  // timescale less than or equal to 20 seconds
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_FIXP_SCALING / 10))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING / 10)))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / TIME_FIXP_SCALING)!=((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        if(ll_elapsed_time < 0LL)
        {
          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 60);
        }
        else
        {
          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 60);
        }
        painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else if(m_pagetime<100)  // timescale less than 100 seconds (00:01:40)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_FIXP_SCALING / 5))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING / 5)))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / TIME_FIXP_SCALING)!=((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        if(ll_elapsed_time < 0LL)
        {
          if(!(((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 5))
          {
            snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 60);

            painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
          }
        }
        else
        {
          if(!(((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 5))
          {
            snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 60);

            painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
          }
        }
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else if(m_pagetime<1000)  // timescale less than 1000 seconds (00:16:40)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / 33333333LL)!=((ll_elapsed_time + time_ppixel) / 33333333LL))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / (TIME_FIXP_SCALING * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 10))) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        if(ll_elapsed_time < 0LL)
        {
          if(!(((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 10))
          {
            snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 60);

            painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
          }
        }
        else
        {
          if(!(((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 10))
          {
            snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 60);

            painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
          }
        }
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else if(m_pagetime<5000)  // timescale less than 5000 seconds (01:23:20)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_FIXP_SCALING * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 10)))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / (TIME_FIXP_SCALING * 60))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 60)))  ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        if(ll_elapsed_time < 0LL)
        {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 60);

          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_FIXP_SCALING * 60))) % 60);
        }
        else
        {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 60);

          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 60))) % 60);
        }
        painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else if(m_pagetime<173000)  // timescale less than 173000 seconds (48:03:20)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_FIXP_SCALING * 600))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 600)))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / (TIME_FIXP_SCALING * 3600))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 3600))) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
      {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        if(ll_elapsed_time < 0LL)
        {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time - time_ppixel) / TIME_FIXP_SCALING)) % 3600);

          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_FIXP_SCALING * 3600))) % 24);
        }
        else
        {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time + time_ppixel) / TIME_FIXP_SCALING)) % 3600);

          snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 3600))) % 24);
        }
        painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
  else if(m_pagetime<3000000)  // timescale less than 3000000 seconds (34 days and 17:20:00)
  {
    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_FIXP_SCALING * 3600))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 3600)))
      {
        painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
      }

      if(((ll_elapsed_time / (TIME_FIXP_SCALING * 86400))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING * 86400))) ||
         ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
     {
        ruler_pen->setColor(big_ruler_color);
        painter->setPen(*ruler_pen);
        painter->drawLine(x_pix, 0, x_pix, h);
        ruler_pen->setColor(small_ruler_color);
        painter->setPen(*ruler_pen);

        painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
      }

      ll_elapsed_time += time_ppixel;
    }
  }
}


void ViewCurve::draw_ecg_horizontal_hr_graticule(QPainter *painter, long long ll_elapsed_time, int scale, int range)
{
  int x_pix;

  long long time_ppixel;

  char str4_1024[1024]="";

  if(scale < 1)  return;

  time_ppixel = (mainwindow->pagetime * ((TIME_FIXP_SCALING_HR * scale) / TIME_FIXP_SCALING)) / w;

  ll_elapsed_time %= TIME_FIXP_SCALING;

  ll_elapsed_time *= ((TIME_FIXP_SCALING_HR * scale) / TIME_FIXP_SCALING);

  ruler_pen->setColor(small_ruler_color);
  painter->setPen(*ruler_pen);

  for(x_pix=0; x_pix<w; x_pix++)
  {
    if((ll_elapsed_time / (TIME_FIXP_SCALING_HR / 25))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / 25)))
    {
      painter->drawLine(x_pix, 0, x_pix, h);
    }

    if(((ll_elapsed_time / (TIME_FIXP_SCALING_HR / 5))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / 5))) ||
      ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
    {
      ruler_pen->setColor(big_ruler_color);
      painter->setPen(*ruler_pen);
      painter->drawLine(x_pix, 0, x_pix, h);

      if(ll_elapsed_time < 0LL)
      {
        snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_FIXP_SCALING_HR / range))) % 1000);
      }
      else
      {
        snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / range))) % 1000);
      }
      painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);

      ruler_pen->setColor(small_ruler_color);
      painter->setPen(*ruler_pen);
    }

    ll_elapsed_time += time_ppixel;
  }
}


void ViewCurve::draw_default_horizontal_hr_graticule(QPainter *painter, long long ll_elapsed_time, int scale, int range)
{
  int x_pix;

  long long time_ppixel;

  char str4_1024[1024]="";

  if(scale < 1)  return;

  time_ppixel = (mainwindow->pagetime * ((TIME_FIXP_SCALING_HR * scale) / TIME_FIXP_SCALING)) / w;

  ll_elapsed_time %= TIME_FIXP_SCALING;

  ll_elapsed_time *= ((TIME_FIXP_SCALING_HR * scale) / TIME_FIXP_SCALING);

  ruler_pen->setColor(small_ruler_color);
  painter->setPen(*ruler_pen);

  for(x_pix=0; x_pix<w; x_pix++)
  {
    if((ll_elapsed_time / (TIME_FIXP_SCALING_HR / 100))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / 100)))
    {
      painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling * printsize_y_factor);
    }

    if(((ll_elapsed_time / (TIME_FIXP_SCALING_HR / 10))!=((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / 10))) ||
      ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time >= 0)))
    {
      ruler_pen->setColor(big_ruler_color);
      painter->setPen(*ruler_pen);
      painter->drawLine(x_pix, 0, x_pix, h);
      if(ll_elapsed_time < 0LL)
      {
        snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_FIXP_SCALING_HR / range))) % 1000);
      }
      else
      {
        snprintf(str4_1024, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_FIXP_SCALING_HR / range))) % 1000);
      }
      painter->drawText(x_pix + (2 * printsize_y_factor), 20 * h_scaling * printsize_x_factor, str4_1024);
      ruler_pen->setColor(small_ruler_color);
      painter->setPen(*ruler_pen);

      painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling * printsize_y_factor);
    }

    ll_elapsed_time += time_ppixel;
  }
}

void ViewCurve::drawCurve_stage_2(QPainter *painter, int w_width, int w_height, int print_linewidth, int print_format)
{
  int i, j, k, n, p, x_tmp,
      signalcomps=0,
      baseline=0,
      p_decimals=0,
      vert_ruler_offset,
      marker_x,
      marker_x2,
      former_marker_x,
      annot_list_sz=0,
      annot_text_stack_idx=0,
      viewtimestr_end_x=0,
      pagetimestr_start_x=0,
      precision=3,
      prec_scale=10000,
      file_idx=0,
      annot_idx=0;

  static int old_w=100;

  double mono_font_pixel_height=20,
         mono_font_pixel_width=12,
         vertical_distance=1,
         dtmp=0,
         sense=0.5;

  char *viewbuf=NULL,
       str1_600[600]="",
       str4_1024[1024]="",
       str5_128[128]="",
       annot_description_str_256[256]={""},
       annot_onset_str_256[256]={""},
       annot_duration_str_256[256]={""},
       *chp=NULL;

  long long l_time,
            l_tmp,
            l_tmp2=0LL;

  sigcompblck_t **signalcomp=NULL;

  annotlist_t *annot_list=NULL;

  annotblck_t *annot=NULL;

  if(mainwindow->exit_in_progress || (painter == NULL))
  {
    return;
  }

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

/* painter_pixelsizefactor: number of pixels per cm vertical */
  painter_pixelsizefactor = 1.0 / mainwindow->y_pixelsizefactor;

  if(w_width && w_height && (print_format != PRINT_FORMAT_NONE))  /* printing */
  {
    w = w_width;
    h = w_height;

    printsize_x_factor = ((double)w_width) / ((double)width());
    printsize_y_factor = ((double)w_height) / ((double)height());

    painter_pixelsizefactor *= printsize_y_factor;

    mono_font_pixel_width = mainwindow->mono_font_pixel_width * printsize_x_factor;
    mono_font_pixel_height = mainwindow->mono_font_pixel_height * printsize_y_factor;

    *printfont = *mainwindow->monofont;
    printfont->setPixelSize(mono_font_pixel_width * 1.6 + 0.5);
    painter->setFont(*printfont);

    scroll_h_offset = h * (mainwindow->mc_v_scrollarea_ratio - 1) * (scrollbar_h_value / 100000.0);

    if((print_format == PRINT_FORMAT_PDF_A3) || (print_format == PRINT_FORMAT_PDF_A4) || (print_format == PRINT_FORMAT_PDF_LETTER) || (print_format == PRINT_FORMAT_PRINTER))
    {
      painter->setPen(QPen(QBrush(Qt::black), PRINT_LINE_WIDTH));
      painter->drawRect(-PRINT_LINE_WIDTH, -PRINT_LINE_WIDTH, w + PRINT_LINE_WIDTH, h + PRINT_LINE_WIDTH);
    }

    painter->setClipping(true);
    painter->setClipRect(0, 0, w, h);

    printing = 1;
  }
  else  /* not printing */
  {
    w = width();
    h = height();

    printsize_x_factor = 1;
    printsize_y_factor = 1;

    scroll_h_offset = h * (mainwindow->mc_v_scrollarea_ratio - 1) * (scrollbar_h_value / 100000.0);

    mono_font_pixel_width = mainwindow->mono_font_pixel_width;
    mono_font_pixel_height = mainwindow->mono_font_pixel_height;

    painter->setFont(*mainwindow->monofont);

    painter->setClipping(false);

    printing = 0;
  }

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->sample_pixel_ratio = (double)signalcomp[i]->samples_on_screen / (double)w;

    signalcomp[i]->draw_rectangle_sum_y = 0;

    signalcomp[i]->draw_rectangle_sum_cnt = 0;
  }

  vertical_distance = ((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0);

  painter->fillRect(0, 0, w, h, backgroundcolor);

  if(mainwindow->annot_editor_highres)
  {
    precision = 6;
    prec_scale = 10;
  }
  else
  {
    precision = 3;
    prec_scale = 10000;
  }

  if(mainwindow->show_annot_markers)
  {
    for(file_idx=0; file_idx<mainwindow->files_open; file_idx++)
    {
      annot_list = &mainwindow->edfheaderlist[file_idx]->annot_list;

      annot_list_sz = edfplus_annotation_size(annot_list);

      for(annot_idx=0; annot_idx<annot_list_sz; annot_idx++)
      {
        annot = edfplus_annotation_get_item(annot_list, annot_idx);

        if(annot->long_duration > 0LL)
        {
          l_tmp = annot->onset;

          if(l_tmp > (long long)(mainwindow->edfheaderlist[file_idx]->viewtime + mainwindow->pagetime))
          {
            break;
          }

          if(((l_tmp + annot->long_duration) > (mainwindow->edfheaderlist[file_idx]->viewtime - TIME_FIXP_SCALING)) && (!annot->hidden || mainwindow->annot_filter->hide_in_list_only))
          {
            l_tmp -= mainwindow->edfheaderlist[file_idx]->viewtime;

            marker_x = (int)((((double)w) / mainwindow->pagetime) * l_tmp);

            marker_x2 = (int)((((double)w) / mainwindow->pagetime) * annot->long_duration);

            if(marker_x < 0)
            {
              marker_x2 += marker_x;

              marker_x = 0;
            }

            if((marker_x + marker_x2) > w)
            {
              x_tmp = marker_x + marker_x2 - w;

              marker_x2 -= x_tmp;
            }

            if((marker_x < w) && (marker_x2 > 0))
            {
              chp = NULL;

              if((signalcomps > 1) && (mainwindow->channel_linked_annotations))
              {
                strlcpy(str4_1024, annot->description, 1024);
                chp = strstr(str4_1024, "@@");
                if(chp != NULL)
                {
                  if(strlen(chp) > 2)
                  {
                    chp += 2;
                    if(mainwindow->strip_label_types)
                    {
                      strip_types_from_label(chp);
                    }
                    trim_spaces(chp);

                    for(k=0; k<signalcomps; k++)
                    {
                      if(!strcmp(chp, signalcomp[k]->signallabel))
                      {
                        baseline = (vertical_distance * (k + 1)) + signalcomp[k]->screen_offset_pix;

                        if(annot->selected_in_dock)
                        {
                          painter->fillRect(marker_x, baseline - (vertical_distance / 2) - scroll_h_offset, marker_x2, vertical_distance, annot_duration_color_selected);
                        }
                        else
                        {
                          painter->fillRect(marker_x, baseline - (vertical_distance / 2) - scroll_h_offset, marker_x2, vertical_distance, annot_duration_color);
                        }

                        break;
                      }
                    }
                    if(k == signalcomps)
                    {
                      chp = NULL;
                    }
                  }
                  else
                  {
                    chp = NULL;
                  }
                }
              }

              if(chp == NULL)
              {
                if(annot->selected_in_dock)
                {
                  if(mainwindow->annotations_duration_background_type == 0)
                  {
                    painter->fillRect(marker_x, 0, marker_x2, h, annot_duration_color_selected);
                  }
                  else
                  {
                    if(file_idx % 2)
                    {
                      painter->fillRect(marker_x, 0, marker_x2, 4 * mono_font_pixel_height, annot_duration_color_selected);
                    }
                    else
                    {
                      painter->fillRect(marker_x, h - (4 * mono_font_pixel_height), marker_x2, 4 * mono_font_pixel_height, annot_duration_color_selected);
                    }
                  }
                }
                else
                {
                  for(p=0; p<MAX_MC_ANNOT_OV_COLORS; p++)
                  {
                    if(!strcmp(annot->description, mainwindow->mc_annot_ov_name_predefined[p]))
                    {
                      if(mainwindow->annotations_duration_background_type == 0)
                      {
                        painter->fillRect(marker_x, 0, marker_x2, h, mainwindow->mc_annot_ov_color_predefined[p]);
                      }
                      else
                      {
                        if(file_idx % 2)
                        {
                          painter->fillRect(marker_x, 0, marker_x2, 4 * mono_font_pixel_height, mainwindow->mc_annot_ov_color_predefined[p]);
                        }
                        else
                        {
                          painter->fillRect(marker_x, h - (4 * mono_font_pixel_height), marker_x2, 4 * mono_font_pixel_height, mainwindow->mc_annot_ov_color_predefined[p]);
                        }
                      }
                      break;
                    }
                  }
                  if(p == MAX_MC_ANNOT_OV_COLORS)
                  {
                    if(mainwindow->annotations_duration_background_type == 0)
                    {
                      painter->fillRect(marker_x, 0, marker_x2, h, annot_duration_color);
                    }
                    else
                    {
                      if(file_idx % 2)
                      {
                        painter->fillRect(marker_x, 0, marker_x2, 4 * mono_font_pixel_height, annot_duration_color);
                      }
                      else
                      {
                        painter->fillRect(marker_x, h - (4 * mono_font_pixel_height), marker_x2, 4 * mono_font_pixel_height, annot_duration_color);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  if(mainwindow->show_baselines)
  {
    vertical_distance = ((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0);

    for(i=0; i<signalcomps; i++)
    {
      baseline = vertical_distance * (i + 1);

      painter->setPen(baseline_color);

      painter->drawLine(0, baseline - scroll_h_offset, w, baseline - scroll_h_offset);

      dtmp = fabs(signalcomp[i]->voltpercm);

      if(dtmp < 0.001)
      {
        p_decimals = 5;
      }
      else if(dtmp < 0.01)
        {
          p_decimals = 4;
        }
        else if(dtmp < 0.1)
          {
            p_decimals = 3;
          }
          else if(dtmp < 1.0)
            {
              p_decimals = 2;
            }
            else if(dtmp < 10.0)
              {
                p_decimals = 1;
              }
              else
              {
                p_decimals = 0;
              }


      if(mainwindow->ecg_view_mode)
      {
        strlcpy(str1_600, signalcomp[i]->physdimension, 32);
        trim_spaces(str1_600);
        if(!strcmp(str1_600, "uV"))
        {
          sense = 500.0;
        }
        else if(!strcmp(str1_600, "mV"))
          {
            sense = 0.5;
          }
          else if(!strcmp(str1_600, "V"))
            {
              sense = 0.0005;
            }
            else
            {
              sense = 0.5;
            }

        for(j=1; j<120; j++)
        {
          vert_ruler_offset = j * 0.2 * painter_pixelsizefactor * (sense / signalcomp[i]->voltpercm);

          if(j==1)
          {
            if(vert_ruler_offset < 3)  break;
          }

          if(signalcomps!=1)
          {
            if(vert_ruler_offset>((vertical_distance / 2) - 8))
            {
              break;
            }
          }

          if((baseline + vert_ruler_offset) > ((h * mainwindow->mc_v_scrollarea_ratio) - (15 * printsize_y_factor)))
          {
            break;
          }

          if((baseline - vert_ruler_offset) < (15 * printsize_y_factor))
          {
            break;
          }

          if(j%5)
          {
            painter->setPen(small_ruler_color);

            painter->drawLine(0, baseline - vert_ruler_offset - scroll_h_offset, w, baseline - vert_ruler_offset - scroll_h_offset);

            painter->drawLine(0, baseline + vert_ruler_offset - scroll_h_offset, w, baseline + vert_ruler_offset - scroll_h_offset);
          }
        }

        for(j=1; j<120; j++)
        {
          vert_ruler_offset = j * 0.2 * painter_pixelsizefactor * (sense / signalcomp[i]->voltpercm);

          if(j==1)
          {
            if(vert_ruler_offset < 3)  break;
          }

          if(signalcomps!=1)
          {
            if(vert_ruler_offset>((vertical_distance / 2) - 8))
            {
              break;
            }
          }

          if((baseline + vert_ruler_offset) > ((h * mainwindow->mc_v_scrollarea_ratio) - (15 * printsize_y_factor)))
          {
            break;
          }

          if((baseline - vert_ruler_offset) < (15 * printsize_y_factor))
          {
            break;
          }

          if(!(j%5))
          {
            painter->setPen(baseline_color);

            painter->drawLine(0, baseline - vert_ruler_offset - scroll_h_offset, w, baseline - vert_ruler_offset - scroll_h_offset);

            painter->drawLine(0, baseline + vert_ruler_offset - scroll_h_offset, w, baseline + vert_ruler_offset - scroll_h_offset);

            if(!(j%10))
            {
              snprintf(str1_600, 128, "%+.*f %s", p_decimals,
                ((sense * j / 5) + ((signalcomp[i]->screen_offset_pix * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor))) * (double)signalcomp[i]->polarity,
                signalcomp[i]->physdimension);

              painter->drawText(5 * printsize_x_factor, baseline - vert_ruler_offset - (4 * printsize_y_factor) - scroll_h_offset, str1_600);

              snprintf(str1_600, 128, "%+.*f %s", p_decimals,
                (((signalcomp[i]->screen_offset_pix * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor)) - (sense * j / 5)) * (double)signalcomp[i]->polarity,
                signalcomp[i]->physdimension);

              painter->drawText(5 * printsize_x_factor, baseline + vert_ruler_offset - (4 * printsize_y_factor) - scroll_h_offset, str1_600);
            }
          }
        }
      }
      else  /* default viewmode (non ECG) */
      {
        for(j=1; j<24; j++)
        {
          vert_ruler_offset = j * painter_pixelsizefactor;

          if(signalcomps!=1)
          {
            if(vert_ruler_offset>((vertical_distance / 2) - 8))
            {
              break;
            }
          }

          if((baseline + vert_ruler_offset) > ((h * mainwindow->mc_v_scrollarea_ratio) - (15 * printsize_y_factor)))
          {
            break;
          }

          if((baseline - vert_ruler_offset) < (15 * printsize_y_factor))
          {
            break;
          }

          painter->drawLine(0, baseline - vert_ruler_offset - scroll_h_offset, w, baseline - vert_ruler_offset - scroll_h_offset);

          snprintf(str1_600, 128, "%+.*f %s", p_decimals,
            ((signalcomp[i]->voltpercm * j) + ((signalcomp[i]->screen_offset_pix * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor))) * (double)signalcomp[i]->polarity,
            signalcomp[i]->physdimension);

          painter->drawText(5 * printsize_x_factor, baseline - vert_ruler_offset - (4 * printsize_y_factor) - scroll_h_offset, str1_600);

          painter->drawLine(0, baseline + vert_ruler_offset - scroll_h_offset, w, baseline + vert_ruler_offset - scroll_h_offset);

          snprintf(str1_600, 128, "%+.*f %s", p_decimals,
            (((signalcomp[i]->screen_offset_pix * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor)) - (signalcomp[i]->voltpercm * j)) * (double)signalcomp[i]->polarity,
            signalcomp[i]->physdimension);

          painter->drawText(5 * printsize_x_factor, baseline + vert_ruler_offset - (4 * printsize_y_factor) - scroll_h_offset, str1_600);
        }
      }
    }
  }

  if(mainwindow->ecg_view_mode)
  {
    draw_ecg_horizontal_graticule(painter, print_linewidth);
  }
  else
  {
    draw_default_horizontal_graticule(painter, print_linewidth);
  }

  if(mainwindow->show_annot_markers || mainwindow->toolbar_stats.active)
  {
    annot_marker_pen->setColor(annot_marker_color);

    annot_marker_pen->setWidth(print_linewidth);

    painter->setPen(*annot_marker_pen);

    if(!annot_marker_moving)
    {
      active_markers->count = 0;
    }

    for(file_idx=0; file_idx<mainwindow->files_open; file_idx++)
    {
      former_marker_x = -10000;

      annot_list = &mainwindow->edfheaderlist[file_idx]->annot_list;

      annot_list_sz = edfplus_annotation_size(annot_list);

      if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.annot_list == annot_list))
      {
        mainwindow->toolbar_stats.sz = 0;
      }

      for(annot_idx=0; annot_idx<annot_list_sz; annot_idx++)
      {
        annot = edfplus_annotation_get_item(annot_list, annot_idx);

        l_tmp = annot->onset;

        if(l_tmp > (long long)(mainwindow->edfheaderlist[file_idx]->viewtime + mainwindow->pagetime))
        {
          break;
        }

        if((l_tmp > (mainwindow->edfheaderlist[file_idx]->viewtime - TIME_FIXP_SCALING)) && (!annot->hidden || mainwindow->annot_filter->hide_in_list_only))
        {
          if(mainwindow->show_annot_markers)
          {
            if(annot->selected_in_dock)
            {
              annot_marker_pen->setColor(annot_marker_selected_color);

              painter->setPen(*annot_marker_pen);
            }

            l_tmp -= mainwindow->edfheaderlist[file_idx]->viewtime;

            marker_x = (int)((((double)w) / mainwindow->pagetime) * l_tmp);

            chp = NULL;

            if((signalcomps > 1) && (mainwindow->channel_linked_annotations))
            {
              strlcpy(str4_1024, annot->description, 1024);
              chp = strstr(str4_1024, "@@");
              if(chp != NULL)
              {
                if(strlen(chp) > 2)
                {
                  chp += 2;
                  if(mainwindow->strip_label_types)
                  {
                    strip_types_from_label(chp);
                  }
                  trim_spaces(chp);

                  for(k=0; k<signalcomps; k++)
                  {
                    if(!strcmp(chp, signalcomp[k]->signallabel))
                    {
                      baseline = (vertical_distance * (k + 1)) + signalcomp[k]->screen_offset_pix;

                      painter->drawLine(marker_x, baseline + (vertical_distance / 2) - scroll_h_offset, marker_x, baseline - (vertical_distance / 2) - scroll_h_offset);

                      break;
                    }
                  }
                  if(k == signalcomps)
                  {
                    chp = NULL;
                  }
                }
                else
                {
                  chp = NULL;
                }
              }
            }

            if(chp == NULL)
            {
              painter->drawLine(marker_x, 0, marker_x, h);
            }

            viewtimestr_end_x = 4 + ((strlen(mainwindow->viewtime_string_128) + 1) * mono_font_pixel_width);

            if(mainwindow->display_pagetime_mmsec)
            {
              pagetimestr_start_x = w - (35 * mono_font_pixel_width);
            }
            else
            {
              pagetimestr_start_x = w - (22 * mono_font_pixel_width);
            }

            if(marker_x > (former_marker_x + (mono_font_pixel_width * 22)))
            {
              if(((marker_x > viewtimestr_end_x) && (marker_x < pagetimestr_start_x)) && (!(file_idx % 2)))
              {
                annot_text_stack_idx = 0;
              }
              else
              {
                annot_text_stack_idx = 1;
              }

              former_marker_x = marker_x;
            }

            if((annot->long_duration > 0LL) && (mainwindow->annotations_show_duration))
            {
              snprintf(annot_duration_str_256, 128, "%i.%0*i",
                      (int)(annot->long_duration / TIME_FIXP_SCALING),
                      precision,
                      (int)(((annot->long_duration % TIME_FIXP_SCALING) / prec_scale)));

              remove_trailing_zeros(annot_duration_str_256);

              strlcat(annot_duration_str_256, " sec", 256);
            }

            l_tmp = annot->onset;

            if(mainwindow->annotations_onset_relative)
            {
              if(l_tmp < 0LL)
              {
                snprintf(annot_onset_str_256, 128, "-%i:%02i:%02i.%0*i",
                        (int)(((-(l_tmp)) / TIME_FIXP_SCALING)/ 3600LL),
                        (int)((((-(l_tmp)) / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)(((-(l_tmp)) / TIME_FIXP_SCALING) % 60LL),
                        precision,
                        (int)((((-(l_tmp)) % TIME_FIXP_SCALING) / prec_scale)));
              }
              else
              {
                snprintf(annot_onset_str_256, 128, "%i:%02i:%02i.%0*i",
                        (int)((l_tmp / TIME_FIXP_SCALING)/ 3600LL),
                        (int)(((l_tmp / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)((l_tmp / TIME_FIXP_SCALING) % 60LL),
                        precision,
                        (int)(((l_tmp % TIME_FIXP_SCALING) / prec_scale)));
              }
            }
            else
            {
              snprintf(annot_onset_str_256, 128, "%i:%02i:%02i.%0*i",
                      (int)((((annot->onset + mainwindow->edfheaderlist[file_idx]->starttime_hr) / TIME_FIXP_SCALING)/ 3600) % 24),
                      (int)((((annot->onset + mainwindow->edfheaderlist[file_idx]->starttime_hr) / TIME_FIXP_SCALING) % 3600) / 60),
                      (int)(((annot->onset + mainwindow->edfheaderlist[file_idx]->starttime_hr) / TIME_FIXP_SCALING) % 60),
                      precision,
                      (int)(((annot->onset + mainwindow->edfheaderlist[file_idx]->starttime_hr) % TIME_FIXP_SCALING) / prec_scale));
            }

            remove_trailing_zeros(annot_onset_str_256);

            strlcpy(annot_description_str_256, annot->description, 128);

            utf8_set_char_len(annot_description_str_256, 20);

            if(file_idx % 2)
            {
              painter->drawText(marker_x + 5, 5 + (annot_text_stack_idx * mono_font_pixel_height), QString::fromUtf8(annot_description_str_256));    //FIXME  (printing)

              annot_text_stack_idx++;
            }
            else
            {
              if((annot->long_duration > 0LL) && (mainwindow->annotations_show_duration))
              {
                painter->drawText(marker_x + 5, (h - 5) - (annot_text_stack_idx * mono_font_pixel_height), annot_duration_str_256);    //FIXME  (printing)

                annot_text_stack_idx++;
              }
            }

            if(file_idx % 2)
            {
              painter->drawText(marker_x + (5 * printsize_x_factor), (5 * printsize_y_factor) + (annot_text_stack_idx * mono_font_pixel_height), annot_onset_str_256);

              annot_text_stack_idx++;
            }
            else
            {
              painter->drawText(marker_x + (5 * printsize_x_factor), (h - (5 * printsize_y_factor)) - (annot_text_stack_idx * mono_font_pixel_height), annot_onset_str_256);

              annot_text_stack_idx++;
            }

            if(file_idx % 2)
            {
              if((annot->long_duration > 0LL) && (mainwindow->annotations_show_duration))
              {
                painter->drawText(marker_x + (5 * printsize_x_factor), (5 * printsize_y_factor) + (annot_text_stack_idx * mono_font_pixel_height), annot_duration_str_256);

                annot_text_stack_idx++;
              }
            }
            else
            {
              painter->drawText(marker_x + (5 * printsize_x_factor), (h - (5 * printsize_y_factor)) - (annot_text_stack_idx * mono_font_pixel_height), QString::fromUtf8(annot_description_str_256));

              annot_text_stack_idx++;
            }

            if(!annot_marker_moving)
            {
              if(active_markers->count<MAX_ACTIVE_ANNOT_MARKERS)
              {
                annot->x_pos = marker_x;

                active_markers->list[active_markers->count] = annot;

                active_markers->count++;
              }
            }

            if(annot->selected_in_dock)
            {
              annot_marker_pen->setColor(annot_marker_color);  /* set it back to the normal (not selected) color */

              painter->setPen(*annot_marker_pen);
            }
          }

          if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.annot_list == annot_list))
          {
            if((!strcmp(annot->description, mainwindow->toolbar_stats.annot_label)) && (mainwindow->toolbar_stats.sz < STATISTICS_IVAL_LIST_SZ))
            {
              if(mainwindow->toolbar_stats.sz > 0)
              {
                mainwindow->toolbar_stats.ival[mainwindow->toolbar_stats.sz - 1] = ((double)(annot->onset - l_tmp2)) / (double)TIME_FIXP_SCALING;
              }

              l_tmp2 = annot->onset;

              mainwindow->toolbar_stats.sz++;
            }
          }
        }
      }
    }

    if(mainwindow->toolbar_stats.sz)  mainwindow->toolbar_stats.sz--;
  }

  ecg_hr_statistics_t hr_stats;

  if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.sz > 2))
  {
    if(ecg_get_hr_statistics(mainwindow->toolbar_stats.ival, mainwindow->toolbar_stats.sz, &hr_stats))
    {
      mainwindow->nav_toolbar_label->setText("Error");
    }
    else
    {
      snprintf(str4_1024, 1024, "  "
              "Beats: %i  "
              "Mean RR: %.1fms  "
              "SDNN RR: %.1fms  "
              "RMSSD RR: %.1fms  "
              "Mean HR: %.1fbpm  "
              "SDNN HR: %.1fbpm  "
              "NN20: %i  "
              "pNN20: %.1f%%  "
              "NN50:  %i  "
              "pNN50: %.1f%%  ",
              hr_stats.beat_cnt,
              hr_stats.mean_rr,
              hr_stats.sdnn_rr,
              hr_stats.rmssd_rr,
              hr_stats.mean_hr,
              hr_stats.sdnn_hr,
              hr_stats.NN20,
              hr_stats.pNN20,
              hr_stats.NN50,
              hr_stats.pNN50);

      mainwindow->nav_toolbar_label->setText(str4_1024);
    }
  }
  else
  {
    mainwindow->nav_toolbar_label->setText("");
  }

  if((viewbuf==NULL)||(graphicBuf==NULL)||(screensamples==NULL))
  {
    if(!printing)
    {
      painter->setPen(text_color);

      painter->drawText(8, h - 4, mainwindow->viewtime_string_128);
      painter->drawText(w - strlen(mainwindow->pagetime_string_128) * mono_font_pixel_width, h - 4, mainwindow->pagetime_string_128);

      if(!mainwindow->files_open)
      {
        painter->drawText(w / 2, h / 2, "NO FILE OPENED");
      }
      else if(!signalcomps)
        {
          if(mainwindow->files_open == 1)
          {
            snprintf(str4_1024, 1024, "A FILE IS OPENED BUT NO SIGNALS ARE ADDED TO THE SCREEN");
          }
          else
          {
            snprintf(str4_1024, 1024, "%i FILES ARE OPENED BUT NO SIGNALS ARE ADDED TO THE SCREEN", mainwindow->files_open);
          }

          painter->drawText(w * 0.4, h / 2, str4_1024);
        }
    }

    return;
  }

  if(mainwindow->clip_to_pane)
  {
    painter->setClipping(true);

    vertical_distance = ((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0);
  }

  for(k=0; k<signalcomps; k++)
  {
    if(mainwindow->signal_plotorder)
    {
      i = k;
    }
    else
    {
      i = signalcomps - 1 - k;
    }

    if(mainwindow->clip_to_pane)
    {
      painter->setClipRect(0, vertical_distance * i + (vertical_distance / 2) - scroll_h_offset, w, vertical_distance);
    }

    if(printing)
    {
      signal_pen->setColor((Qt::GlobalColor)signalcomp[i]->color);
      signal_pen->setWidth(print_linewidth);
      painter->setPen(*signal_pen);
    }
    else
    {
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
    }

    for(j=0; j<screensamples[i]; j++)
    {
      painter->drawLine(graphicBuf[j].graphic_line[i].x1,
                        graphicBuf[j].graphic_line[i].y1 - scroll_h_offset,
                        graphicBuf[j].graphic_line[i].x2,
                        graphicBuf[j].graphic_line[i].y2 - scroll_h_offset);

      if(draw_zoom_rectangle && use_move_events)
      {
        if(((mouse_x >= mouse_press_coordinate_x) &&
            (graphicBuf[j].graphic_line[i].x1 >= mouse_press_coordinate_x) &&
            (graphicBuf[j].graphic_line[i].x1 <= mouse_x))
           ||
           ((mouse_x < mouse_press_coordinate_x) &&
            (graphicBuf[j].graphic_line[i].x1 <= mouse_press_coordinate_x) &&
            (graphicBuf[j].graphic_line[i].x1 >= mouse_x)))
        {
          signalcomp[i]->draw_rectangle_sum_y += graphicBuf[j].graphic_line[i].y1;

          signalcomp[i]->draw_rectangle_sum_cnt++;
        }
      }
    }
  }

  if(!printing)  painter->setClipping(false);

  for(i=0; i<signalcomps; i++)
  {
    baseline = (h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1);
    baseline *= (i + 1);

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->hascursor1)
    {
      painter->setPen((Qt::GlobalColor)crosshair_1.color);
      if(crosshair_1.has_hor_line)
      {
        painter->drawLine(0, crosshair_1.y_value - scroll_h_offset, w, crosshair_1.y_value - scroll_h_offset);
      }
      if(crosshair_1.dot_sz)
      {
        painter->setBrush(QBrush((Qt::GlobalColor)crosshair_1.color));
        painter->drawEllipse((crosshair_1.x_position - crosshair_1.dot_sz) * printsize_x_factor,
                             crosshair_1.y_value - scroll_h_offset - (crosshair_1.dot_sz * printsize_y_factor),
                             (crosshair_1.dot_sz * 2 + 1) * printsize_x_factor, (crosshair_1.dot_sz * 2 + 1) * printsize_y_factor);
      }
      painter->drawLine(crosshair_1.x_position * printsize_x_factor, 0, crosshair_1.x_position * printsize_x_factor, h);
      snprintf(str1_600, 128, "%+f %s",
                      crosshair_1.value,
                      signalcomp[i]->physdimension);
      painter->drawText((crosshair_1.x_position + 5) * printsize_x_factor, ((crosshair_1.y_position - 10) * printsize_y_factor) + (mono_font_pixel_height * 2), str1_600);
      if(mainwindow->annot_editor_highres)
      {
        snprintf(str1_600, 128, "%2i:%02i:%02i.%06i",
                        (int)(((crosshair_1.time_clock / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                        (int)(((crosshair_1.time_clock / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)((crosshair_1.time_clock / TIME_FIXP_SCALING) % 60LL),
                        (int)((crosshair_1.time_clock % TIME_FIXP_SCALING) / 10LL));
      }
      else
      {
        snprintf(str1_600, 128, "%2i:%02i:%02i.%03i",
                        (int)(((crosshair_1.time_clock / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                        (int)(((crosshair_1.time_clock / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)((crosshair_1.time_clock / TIME_FIXP_SCALING) % 60LL),
                        (int)((crosshair_1.time_clock % TIME_FIXP_SCALING) / 10000LL));
      }
      if(crosshair_1.time_relative >= TIME_FIXP_SCALING)
      {
        if(mainwindow->annot_editor_highres)
        {
          snprintf(str1_600 + strlen(str1_600), 256 - strlen(str1_600), " (%i:%02i:%02i.%06i)",
                  (int)((crosshair_1.time_relative / TIME_FIXP_SCALING)/ 3600LL),
                  (int)(((crosshair_1.time_relative / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                  (int)((crosshair_1.time_relative / TIME_FIXP_SCALING) % 60LL),
                  (int)((crosshair_1.time_relative % TIME_FIXP_SCALING) / 10LL));
        }
        else
        {
          snprintf(str1_600 + strlen(str1_600), 256 - strlen(str1_600), " (%i:%02i:%02i.%03i)",
                  (int)((crosshair_1.time_relative / TIME_FIXP_SCALING)/ 3600LL),
                  (int)(((crosshair_1.time_relative / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                  (int)((crosshair_1.time_relative / TIME_FIXP_SCALING) % 60LL),
                  (int)((crosshair_1.time_relative % TIME_FIXP_SCALING) / 10000LL));
        }
      }
      else
      {
        convert_to_metric_suffix(str5_128, (double)crosshair_1.time_relative / TIME_FIXP_SCALING, 3, 128);

        snprintf(str1_600 + strlen(str1_600), 32, " (%.26sS)", str5_128);
      }

      painter->drawText((crosshair_1.x_position + 5) * printsize_x_factor, ((crosshair_1.y_position - 10) * printsize_y_factor) + mono_font_pixel_height, str1_600);
      if(signalcomp[i]->alias[0] != 0)
      {
        painter->drawText((crosshair_1.x_position + 5) * printsize_x_factor, ((crosshair_1.y_position - 10) * printsize_y_factor), signalcomp[i]->alias);
      }
      else
      {
        painter->drawText((crosshair_1.x_position + 5) * printsize_x_factor, ((crosshair_1.y_position - 10) * printsize_y_factor), signalcomp[i]->signallabel);
      }
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

      if(mainwindow->auto_update_annot_onset && (!crosshair_1.moving) && (mainwindow->annotationEditDock != NULL))
      {
        mainwindow->annotationEditDock->annotEditSetOnset(crosshair_1.time_relative);
      }
    }

    if(signalcomp[i]->hascursor2)
    {
      painter->setPen((Qt::GlobalColor)crosshair_2.color);
      if(crosshair_2.has_hor_line)
      {
        painter->drawLine(0, crosshair_2.y_value - scroll_h_offset, w, crosshair_2.y_value - scroll_h_offset);
      }
      if(crosshair_2.dot_sz)
      {
        painter->setBrush(QBrush((Qt::GlobalColor)crosshair_2.color));
        painter->drawEllipse((crosshair_2.x_position - crosshair_2.dot_sz) * printsize_x_factor,
                             crosshair_2.y_value - scroll_h_offset - (crosshair_2.dot_sz * printsize_y_factor),
                             (crosshair_2.dot_sz * 2 + 1) * printsize_x_factor, (crosshair_2.dot_sz * 2 + 1) * printsize_y_factor);
      }
      painter->drawLine(crosshair_2.x_position * printsize_x_factor, 0, crosshair_2.x_position * printsize_x_factor, h);
      snprintf(str1_600, 128, "%+f %s",
                      crosshair_2.value,
                      signalcomp[i]->physdimension);
      painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, ((crosshair_2.y_position - 10) * printsize_y_factor) + (mono_font_pixel_height * 4), str1_600);
      if(mainwindow->annot_editor_highres)
      {
        snprintf(str1_600, 128, "%2i:%02i:%02i.%06i",
                        (int)(((crosshair_2.time_clock / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                        (int)(((crosshair_2.time_clock / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)((crosshair_2.time_clock / TIME_FIXP_SCALING) % 60LL),
                        (int)((crosshair_2.time_clock % TIME_FIXP_SCALING) / 10LL));
      }
      else
      {
        snprintf(str1_600, 128, "%2i:%02i:%02i.%03i",
                        (int)(((crosshair_2.time_clock / TIME_FIXP_SCALING)/ 3600LL) % 24LL),
                        (int)(((crosshair_2.time_clock / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                        (int)((crosshair_2.time_clock / TIME_FIXP_SCALING) % 60LL),
                        (int)((crosshair_2.time_clock % TIME_FIXP_SCALING) / 10000LL));
      }
      if(crosshair_2.time_relative >= TIME_FIXP_SCALING)
      {
        if(mainwindow->annot_editor_highres)
        {
          snprintf(str1_600 + strlen(str1_600), 256 - strlen(str1_600), " (%i:%02i:%02i.%06i)",
                  (int)((crosshair_2.time_relative / TIME_FIXP_SCALING)/ 3600LL),
                  (int)(((crosshair_2.time_relative / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                  (int)((crosshair_2.time_relative / TIME_FIXP_SCALING) % 60LL),
                  (int)((crosshair_2.time_relative % TIME_FIXP_SCALING) / 10LL));
        }
        else
        {
          snprintf(str1_600 + strlen(str1_600), 256 - strlen(str1_600), " (%i:%02i:%02i.%03i)",
                  (int)((crosshair_2.time_relative / TIME_FIXP_SCALING)/ 3600LL),
                  (int)(((crosshair_2.time_relative / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                  (int)((crosshair_2.time_relative / TIME_FIXP_SCALING) % 60LL),
                  (int)((crosshair_2.time_relative % TIME_FIXP_SCALING) / 10000LL));
        }
      }
      else
      {
        convert_to_metric_suffix(str5_128, (double)crosshair_2.time_relative / TIME_FIXP_SCALING, 3, 128);

        snprintf(str1_600 + strlen(str1_600), 32, " (%.26sS)", str5_128);
      }

      painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, ((crosshair_2.y_position - 10) * printsize_y_factor) + (mono_font_pixel_height * 3), str1_600);
      snprintf(str1_600, 128, "delta %+f %s",
                      crosshair_2.value - crosshair_1.value,
                      signalcomp[i]->physdimension);
      painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, ((crosshair_2.y_position - 10) * printsize_y_factor) + (mono_font_pixel_height * 2), str1_600);
      l_time = crosshair_2.time_clock - crosshair_1.time_clock;
      if(l_time<0) l_time = -l_time;
      if(l_time >= TIME_FIXP_SCALING)
      {
        if(mainwindow->annot_editor_highres)
        {
          snprintf(str1_600, 128, "delta %i:%02i:%02i.%06i",
                          (int)((l_time / TIME_FIXP_SCALING)/ 3600LL),
                          (int)(((l_time / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                          (int)((l_time / TIME_FIXP_SCALING) % 60LL),
                          (int)((l_time % TIME_FIXP_SCALING) / 10LL));
        }
        else
        {
          snprintf(str1_600, 128, "delta %i:%02i:%02i.%03i",
                          (int)((l_time / TIME_FIXP_SCALING)/ 3600LL),
                          (int)(((l_time / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                          (int)((l_time / TIME_FIXP_SCALING) % 60LL),
                          (int)((l_time % TIME_FIXP_SCALING) / 10000LL));
        }
      }
      else
      {
        convert_to_metric_suffix(str5_128, (double)l_time / TIME_FIXP_SCALING, 3, 128);

        snprintf(str1_600, 32, "delta %.24sS", str5_128);
      }
      painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, ((crosshair_2.y_position - 10) * printsize_y_factor) + mono_font_pixel_height, str1_600);
      if(signalcomp[i]->alias[0] != 0)
      {
        painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, (crosshair_2.y_position - 10) * printsize_y_factor, signalcomp[i]->alias);
      }
      else
      {
        painter->drawText((crosshair_2.x_position + 5) * printsize_x_factor, (crosshair_2.y_position - 10) * printsize_y_factor, signalcomp[i]->signallabel);
      }
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
    }

    if(signalcomp[i]->hasoffsettracking)
    {
      n = snprintf(str1_600, 128, "offset: %f %s",
        -signalcomp[i]->screen_offset_pix * mainwindow->y_pixelsizefactor * signalcomp[i]->voltpercm,
        signalcomp[i]->physdimension);
      painter->fillRect((135 * w_scaling) - (mono_font_pixel_width / 2), baseline - (2 * h_scaling) - scroll_h_offset, (n + 1) * mono_font_pixel_width, -15 * h_scaling, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(135 * w_scaling, baseline - (5 * h_scaling) - scroll_h_offset, str1_600);
    }

    if(signalcomp[i]->hasgaintracking)
    {
      n = snprintf(str1_600, 128, "amplitude: %f %s/cm",
        signalcomp[i]->voltpercm,
        signalcomp[i]->physdimension);
      painter->fillRect((135 * w_scaling) - (mono_font_pixel_width / 2), baseline - (2 * h_scaling) - scroll_h_offset, (n + 1) * mono_font_pixel_width, -15 * h_scaling, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(135 * w_scaling, baseline - (5 * h_scaling) - scroll_h_offset, str1_600);
    }
  }

  vertical_distance = ((double)h * mainwindow->mc_v_scrollarea_ratio) / (signalcomps + 1.0);

  for(i=0; i<signalcomps; i++)
  {
    baseline = (vertical_distance * (i + 1)) - scroll_h_offset;

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->fillRect(2 * w_scaling, baseline - (2 * h_scaling), (strlen(signalcomp[i]->alias) * mono_font_pixel_width) + (5 * w_scaling), (-2 * h_scaling) - mono_font_pixel_height, backgroundcolor);
    }
    else
    {
      painter->fillRect(2 * w_scaling, baseline - (2 * h_scaling), (strlen(signalcomp[i]->signallabel) * mono_font_pixel_width) + (5 * w_scaling), (-2 * h_scaling) - mono_font_pixel_height, backgroundcolor);
    }

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->drawText(5 * w_scaling, baseline - (5 * h_scaling), signalcomp[i]->alias);
    }
    else
    {
      painter->drawText(5 * w_scaling, baseline - (5 * h_scaling), signalcomp[i]->signallabel);
    }

    if(signalcomp[i]->hasruler)
    {
      floating_ruler(painter, ruler_x_position, ruler_y_position, signalcomp[i], print_linewidth, mono_font_pixel_width);
    }
  }

  if(draw_zoom_rectangle)
  {
    special_pen->setColor(mouse_rect_color);

    painter->setPen(*special_pen);

    painter->drawLine(mouse_press_coordinate_x, mouse_press_coordinate_y, mouse_press_coordinate_x, mouse_y);
    painter->drawLine(mouse_press_coordinate_x, mouse_press_coordinate_y, mouse_x, mouse_press_coordinate_y);
    painter->drawLine(mouse_x, mouse_y, mouse_press_coordinate_x, mouse_y);
    painter->drawLine(mouse_x, mouse_y, mouse_x, mouse_press_coordinate_y);
  }

  painter->setPen(text_color);

  painter->fillRect(6 * printsize_x_factor, h - mono_font_pixel_height - 1,
                    strlen(mainwindow->viewtime_string_128) * mono_font_pixel_width, (4 * printsize_y_factor) + mono_font_pixel_height, backgroundcolor);
  painter->drawText(mono_font_pixel_width / 2, h - (4 * printsize_y_factor), mainwindow->viewtime_string_128);
  if((old_w != w) && mainwindow->display_pagetime_mmsec)
  {
    if((mainwindow->pagetime >= (TIME_FIXP_SCALING / 10)) && (mainwindow->pagetime < (3600LL * TIME_FIXP_SCALING)))
    {
      for(int r=0; r<64; r++)
      {
        if(mainwindow->pagetime_string_128[r] == 0)  break;

        if(mainwindow->pagetime_string_128[r] == '(')
        {
          if(printing)
          {
            mainwindow->pagetime_string_128[r] = 0;
          }
          else
          {
            if(mainwindow->pagetime > (600LL * TIME_FIXP_SCALING))
            {
              snprintf(mainwindow->pagetime_string_128 + r, 64, "(%i mm/min)",
                      (int)((w * mainwindow->x_pixelsizefactor * 600) / (((double)mainwindow->pagetime) / TIME_FIXP_SCALING) + 0.5));
            }
            else if(mainwindow->pagetime >= (60LL * TIME_FIXP_SCALING))
              {
                snprintf(mainwindow->pagetime_string_128 + r, 64, "(%.1f mm/sec)",
                        (w * mainwindow->x_pixelsizefactor * 10) / (((double)mainwindow->pagetime) / TIME_FIXP_SCALING));
              }
              else
              {
                snprintf(mainwindow->pagetime_string_128 + r, 64, "(%i mm/sec)",
                        (int)((w * mainwindow->x_pixelsizefactor * 10) / (((double)mainwindow->pagetime) / TIME_FIXP_SCALING) + 0.5));
              }
          }

          remove_trailing_zeros(mainwindow->pagetime_string_128);

          break;
        }
      }
    }
  }

  painter->fillRect(w - mono_font_pixel_width - (strlen(mainwindow->pagetime_string_128) * mono_font_pixel_width), h - mono_font_pixel_height - 1,
                    (strlen(mainwindow->pagetime_string_128) + 2) * mono_font_pixel_width, (4 * printsize_y_factor) + mono_font_pixel_height,
                    backgroundcolor);
  painter->drawText(w - (mono_font_pixel_width / 2) - (strlen(mainwindow->pagetime_string_128) * mono_font_pixel_width), h - (4 * printsize_y_factor), mainwindow->pagetime_string_128);

  old_w = w;
}


inline void ViewCurve::floating_ruler(QPainter *painter, int x_pos, int y_pos, sigcompblck_t *signalcomp, int print_linewidth, double mono_font_px_width)
{
  int i, j, v_use_exp=0, h_use_exp=0;

  double d_tmp,
         d_tmp2,
         h_size,
         w_size,
         pixels_per_second;

  char str_hz[7][16]={" 9", "10", "11", "12", "13", "14", "15"};

  char str_uv[5][16]={"200", "150", "100", "50", "0"};

  char str_s[4][16]={"1.00", "0.75", "0.50", "0.25"};


  if(w < 400)
  {
    return;
  }

  if(floating_ruler_use_var_width)
  {
    if((mainwindow->pagetime / (TIME_FIXP_SCALING / 10LL)) < 2LL)
    {
      h_use_exp = 1;
    }
  }
  else if((mainwindow->pagetime / TIME_FIXP_SCALING) < 2LL)
    {
      h_use_exp = 1;
    }

  if((mainwindow->pagetime / TIME_FIXP_SCALING) > 60LL)
  {
    h_use_exp = 1;
  }

  if((signalcomp->voltpercm < 1.0) && (signalcomp->voltpercm > -1.0))
  {
    v_use_exp = 1;
  }

  if((signalcomp->voltpercm > 2000.0) || (signalcomp->voltpercm < -2000.0))
  {
    v_use_exp = 1;
  }

  x_pos *= printsize_x_factor;
  y_pos *= printsize_y_factor;

  pixels_per_second = ((double)w) / (((double)mainwindow->pagetime) / ((double)TIME_FIXP_SCALING));

  h_size = 4.0 * painter_pixelsizefactor;

  if(floating_ruler_use_var_width)
  {
    w_size = pixels_per_second;

    while(w_size > (w / 6.0))
    {
      w_size /= 2;
    }

    while(w_size < (w / 12.1))
    {
      w_size *= 2;
    }
  }
  else
  {
    w_size = w / 10.0;
  }

  d_tmp = h_size / 7.0;

  d_tmp2 = h_size / 14.0;

  if(float_ruler_more)
  {
    for(i=0; i<7; i++)
    {
      if(h_use_exp)
      {
        snprintf(str_hz[i], 15, "%.1e",  (pixels_per_second / w_size) * (2.0 + i));
      }
      else
      {
        if(floating_ruler_use_var_width)
        {
          snprintf(str_hz[i], 15, "%.0f",  (pixels_per_second / w_size) * (2.0 + i));
        }
        else
        {
          snprintf(str_hz[i], 15, "%.1f",  (pixels_per_second / w_size) * (2.0 + i));
        }
      }

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.1e", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }
        else
        {
          snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.0e", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }
        else
        {
          snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1)));
        }

        str_uv[i][15] = 0;
      }
    }
  }
  else
  {
    for(i=0; i<7; i++)
    {
      if(h_use_exp)
      {
        snprintf(str_hz[i], 15, "%.1e",  (pixels_per_second / w_size) * (9.0 + i));
      }
      else
      {
        if(floating_ruler_use_var_width)
        {
          snprintf(str_hz[i], 15, "%.0f",  (pixels_per_second / w_size) * (9.0 + i));
        }
        else
        {
          snprintf(str_hz[i], 15, "%.1f",  (pixels_per_second / w_size) * (9.0 + i));
        }
      }

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.1e", signalcomp->voltpercm * (4 - i) * (double)signalcomp->polarity);
        }
        else
        {
          snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * (4 - i) * (double)signalcomp->polarity);
        }

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.0e", signalcomp->voltpercm * (4 - i) * signalcomp->polarity);
        }
        else
        {
          snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * (4 - i)) * signalcomp->polarity);
        }

        str_uv[i][15] = 0;
      }
    }
  }

  for(i=0; i<4; i++)
  {
    if(h_use_exp)
    {
      if(floating_ruler_use_var_width)
      {
        snprintf(str_s[i], 15, "%.1e",  ((w_size / 4.0) / pixels_per_second) * (4 - i));
      }
      else
      {
        snprintf(str_s[i], 15, "%.1e",  ((w / 40.0) / pixels_per_second) * (4 - i));
      }
    }
    else
    {
      if(floating_ruler_use_var_width)
      {
        snprintf(str_s[i], 15, "%.2f",  ((w_size / 4.0) / pixels_per_second) * (4 - i));
      }
      else
      {
        snprintf(str_s[i], 15, "%.2f",  ((w / 40.0) / pixels_per_second) * (4 - i));
      }
    }

    str_s[i][15] = 0;
  }

  painter->setPen((Qt::GlobalColor)floating_ruler_color);

  painter->drawText(x_pos, y_pos - (h / 80.0), "Hz");

  for(i=0; i<7; i++)
  {
    painter->drawText(x_pos - (mono_font_px_width/2) - (strlen(str_hz[i]) * mono_font_px_width), y_pos + (h / 204.8) + d_tmp2 + (d_tmp * i) + (h_scaling * 3.0 - 3.0), str_hz[i]);
  }

  for(i=0; i<8; i++)
  {
    painter->drawLine(x_pos, y_pos + (d_tmp * i), x_pos + w_size, y_pos + (d_tmp * i));
  }
  painter->drawLine(x_pos + w_size, y_pos, x_pos + w_size, y_pos + h_size);
  painter->drawLine(x_pos, y_pos, x_pos, y_pos + h_size);

  if(float_ruler_more)
  {
    for(j=2; j<9; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 2)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 1)));
      }
    }
  }
  else
  {
    for(j=9; j<16; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 9)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 8)));
      }
    }
  }

  painter->drawText(x_pos + (3.0 * printsize_x_factor), y_pos + h_size + (25.0 * printsize_y_factor), "more...");

  x_pos += (w_size + ((double)w / 18.0));

  d_tmp = h_size / 4.0;

  painter->drawText(x_pos, y_pos - ((double)h / 80.0), signalcomp->physdimension);

  for(i=0; i<5; i++)
  {
    painter->drawText(x_pos - (mono_font_px_width/2) - (strlen(str_uv[i]) * mono_font_px_width), y_pos + ((double)h / 204.8) + (d_tmp * i) + (h_scaling * 3.0 - 3.0), str_uv[i]);

    painter->drawLine(x_pos + ((double)w / 160.0), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  painter->drawLine(x_pos + ((double)w / 45.7), y_pos, x_pos + ((double)w / 45.7), y_pos + h_size);

  d_tmp = h_size / 20.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + ((double)w / 71.1), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  x_pos += ((double)w / 10.0);

  painter->drawLine(x_pos, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);
  painter->drawLine(x_pos + w_size, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);

  d_tmp = h_size / 4.0;
  d_tmp2 = w_size / 8.0;

  for(i=0; i<4; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }

  for(i=0; i<4; i++)
  {
    painter->drawText(x_pos - (mono_font_px_width/2) - (strlen(str_s[i]) * mono_font_px_width) + (d_tmp2 * i), y_pos + ((double)h / 204.8) + (d_tmp * i), str_s[i]);
  }

  painter->drawText(x_pos + ((double)w / 125.0) + w_size, y_pos + ((double)h / 204.8), "Sec");

  special_pen->setColor((Qt::GlobalColor)floating_ruler_color);

  special_pen->setWidth(print_linewidth);

  painter->setPen(*special_pen);

  d_tmp = h_size / 20.0;
  d_tmp2 = w_size / 40.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }
}














