/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2024 Teunis van Beelen
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



#include "filtercurve.h"


FilterCurve::FilterCurve(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  SignalColor1 = Qt::blue;
  SignalColor2 = Qt::yellow;
  tracewidth = 0;
  BackgroundColor = Qt::gray;
  RasterColor = Qt::darkGray;
  MarkerColor = Qt::white;

  h_rastersize = 10;
  v_rastersize = 10;
  drawcurve_before_raster = 0;

  buf1 = NULL;
  buf2 = NULL;

  marker_1 = 0;
  marker_2 = 0;

  sz_hint_w = 150;
  sz_hint_h = 100;

  h_val_enabled = 0;
  v_val_enabled = 0;

  h_range_start = 1;
  h_range_end = 2;
  v_range_start = 1;
  v_range_end = 2;
}


FilterCurve::~FilterCurve()
{
}


void FilterCurve::paintEvent(QPaintEvent *)
{
  int i, w, h;

  double v_sens,
         offset,
         h_step,
         rasterstep,
         horizontal_marker_ratio;

  char str1_128[128]={""};

  w = width();
  h = height();

  horizontal_marker_ratio = w / 1000.0;

  QPainter painter(this);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
    painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  painter.fillRect(0, 0, w, h, BackgroundColor);

  if(w < 5)  return;

  if(h < 5)  return;

/////////////////////////////////// draw the curve ///////////////////////////////////////////

  if(drawcurve_before_raster)
  {
    if(buf1 == NULL)  return;

    if(buf2 != NULL)
    {
      if(max_value2 == min_value2)  return;

      if(bufsize2 < 2)  return;

      offset = (-(max_value2));

      v_sens = (-(h / (max_value2 - min_value2)));

      h_step = (double)w / (double)(bufsize2 - 1);

      painter.setPen(QPen(QBrush(SignalColor2, Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

      for(i = 0; i < (bufsize2 - 1); i++)
      {
        painter.drawLine(i * h_step, (buf2[i] + offset) * v_sens, (i + 1) * h_step, (buf2[i + 1] + offset) * v_sens);
      }
    }

    if(max_value1 == min_value1)  return;

    if(bufsize1 < 2)  return;

    offset = (-(max_value1));

    v_sens = (-(h / (max_value1 - min_value1)));

    h_step = (double)w / (double)(bufsize1 - 1);

    painter.setPen(QPen(QBrush(SignalColor1, Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    for(i = 0; i < (bufsize1 - 1); i++)
    {
      painter.drawLine(i * h_step, (buf1[i] + offset) * v_sens, (i + 1) * h_step, (buf1[i + 1] + offset) * v_sens);
    }
  }

/////////////////////////////////// draw the rasters ///////////////////////////////////////////

  painter.setPen(RasterColor);

  painter.drawRect (0, 0, w - 1, h - 1);

  if(h_rastersize > 1)
  {
    rasterstep = (double)w / h_rastersize;

    for(i = 0; i < h_rastersize; i++)
    {
      if(i)
      {
        painter.setPen(RasterColor);

        painter.drawLine(i * rasterstep, 0, i * rasterstep, h);
      }

      if(h_val_enabled & (!(i%2)))
      {
        snprintf(str1_128, 128, "%f", i * ((h_range_end - h_range_start) / h_rastersize));

        remove_trailing_zeros(str1_128);

        painter.setPen(SignalColor1);

        painter.drawText(i * rasterstep + 5, h - 5, str1_128);
      }
    }
  }

  if(v_rastersize > 1)
  {
    rasterstep = (double)h / v_rastersize;

    for(i = 0; i < v_rastersize; i++)
    {
      if(i)
      {
        painter.setPen(RasterColor);

        painter.drawLine(0, i * rasterstep, w, i * rasterstep);
      }

      if(v_val_enabled & (i%2))
      {
        snprintf(str1_128, 128, "%f", v_range_start + (i * ((v_range_end - v_range_start) / v_rastersize)));

        remove_trailing_zeros(str1_128);

        painter.setPen(SignalColor1);

        painter.drawText(5, (i * rasterstep) - 5, str1_128);
      }
    }
  }

/////////////////////////////////// draw the curve ///////////////////////////////////////////

  if(!drawcurve_before_raster)
  {
    if(buf1 == NULL)  return;

    if(buf2 != NULL)
    {
      if(max_value2 == min_value2)  return;

      if(bufsize2 < 2)  return;

      offset = (-(max_value2));

      v_sens = (-(h / (max_value2 - min_value2)));

      h_step = (double)w / (double)(bufsize2 - 1);

      painter.setPen(QPen(QBrush(SignalColor2, Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

      for(i = 0; i < (bufsize2 - 1); i++)
      {
        painter.drawLine(i * h_step, (buf2[i] + offset) * v_sens, (i + 1) * h_step, (buf2[i + 1] + offset) * v_sens);
      }
    }

    if(max_value1 == min_value1)  return;

    if(bufsize1 < 2)  return;

    offset = (-(max_value1));

    v_sens = (-(h / (max_value1 - min_value1)));

    h_step = (double)w / (double)(bufsize1 - 1);

    painter.setPen(QPen(QBrush(SignalColor1, Qt::SolidPattern), tracewidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    for(i = 0; i < (bufsize1 - 1); i++)
    {
      painter.drawLine(i * h_step, (buf1[i] + offset) * v_sens, (i + 1) * h_step, (buf1[i + 1] + offset) * v_sens);
    }
  }

/////////////////////////////////// draw the markers ///////////////////////////////////////////

  if((marker_1) || (marker_2))
  {
    painter.drawText(3, 18, "0 Hz");
  }

  if(marker_1)
  {
    painter.drawText(3 + (marker_1 * horizontal_marker_ratio), 18, "F1");
  }

  if(marker_2)
  {
    painter.drawText(1 + (marker_2 * horizontal_marker_ratio), 18, "F2");
  }

  if((marker_1) || (marker_2))
  {
    painter.setPen(SignalColor2);

    painter.drawText(w - 60, 18, "0 deg.");
    painter.drawText(w - 80, h - 3, "360 deg.");
  }

  if(marker_1)
  {
    painter.setPen(QPen(QBrush(MarkerColor, Qt::SolidPattern), 0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));

    painter.drawLine(marker_1 * horizontal_marker_ratio, 0, marker_1 * horizontal_marker_ratio, h);
  }

  if(marker_2)
  {
    painter.setPen(QPen(QBrush(MarkerColor, Qt::SolidPattern), 0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));

    painter.drawLine(marker_2 * horizontal_marker_ratio, 0, marker_2 * horizontal_marker_ratio, h);
  }
}


void FilterCurve::clear(void)
{
  drawCurve(NULL, 0, 0, 0);

  update();
}


void FilterCurve::drawCurve(double *samplebuf, int bsize, double max_val, double min_val)
{
  buf1 = samplebuf;

  bufsize1 = bsize;

  max_value1 = max_val;

  min_value1 = min_val;

  buf2 = NULL;

  bufsize2 = 0;

  marker_1 = 0;

  marker_2 = 0;

  update();
}


void FilterCurve::drawCurve(double *samplebuf, int bsize, double max_val, double min_val, double *samplebuf2, int size2, double max_val2, double min_val2, int mrkr1, int mrkr2)
{
  buf1 = samplebuf;

  bufsize1 = bsize;

  max_value1 = max_val;

  min_value1 = min_val;

  buf2 = samplebuf2;

  bufsize2 = size2;

  max_value2 = max_val2;

  min_value2 = min_val2;

  if(mrkr1 > 0)
  {
    marker_1 = mrkr1;
  }
  else
  {
    marker_1 = 0;
  }

  if(mrkr2 > 0)
  {
    marker_2 = mrkr2;
  }
  else
  {
    marker_2 = 0;
  }

  update();
}


void FilterCurve::setSignalColor1(QColor newColor)
{
  SignalColor1 = newColor;
  update();
}


void FilterCurve::setSignalColor2(QColor newColor)
{
  SignalColor2 = newColor;
  update();
}


void FilterCurve::setTraceWidth(int tr_width)
{
  tracewidth = tr_width;
  if(tracewidth < 0)  tracewidth = 0;
  update();
}


void FilterCurve::setBackgroundColor(QColor newColor)
{
  BackgroundColor = newColor;
  update();
}


void FilterCurve::setRasterColor(QColor newColor)
{
  RasterColor = newColor;
  update();
}


void FilterCurve::setMarkerColor(QColor newColor)
{
  MarkerColor = newColor;
  update();
}


void FilterCurve::setH_RasterSize(int newsize)
{
  h_rastersize = newsize;
  update();
}


void FilterCurve::setV_RasterSize(int newsize)
{
  v_rastersize = newsize;
  update();
}


QSize FilterCurve::minimumSizeHint() const
{
  return QSize(sz_hint_w, sz_hint_h);
}


int FilterCurve::heightForWidth(int w) const
{
  return w / 1.5;
}


void FilterCurve::setH_Range(double val1, double val2)
{
  h_range_start = val1;
  h_range_end = val2;
}


void FilterCurve::setV_Range(double val1, double val2)
{
  v_range_start = val1;
  v_range_end = val2;
}


void FilterCurve::setH_Range_Enabled(bool en)
{
  if(en == true)
  {
    h_val_enabled = 1;
  }
  else
  {
    h_val_enabled = 0;
  }
}


void FilterCurve::setV_Range_Enabled(bool en)
{
  if(en == true)
  {
    v_val_enabled = 1;
  }
  else
  {
    v_val_enabled = 0;
  }
}

















