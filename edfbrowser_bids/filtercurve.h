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



#ifndef FILTERCURVE_H
#define FILTERCURVE_H


#include <QtGlobal>
#include <QWidget>
#include <QPainter>
#include <QPen>

#include "utils.h"




class FilterCurve: public QWidget
{
  Q_OBJECT

public:
  FilterCurve(QWidget *parent=0);
  ~FilterCurve();

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const;
  int heightForWidth(int) const;

  void setSignalColor1(QColor);
  void setSignalColor2(QColor);
  void setTraceWidth(int);
  void setBackgroundColor(QColor);
  void setRasterColor(QColor);
  void setMarkerColor(QColor);
  void setH_RasterSize(int);
  void setH_Range(double, double);
  void setH_Range_Enabled(bool);
  void setV_RasterSize(int);
  void setV_Range(double, double);
  void setV_Range_Enabled(bool);
  void drawCurve(double *, int, double, double);
  void drawCurve(double *, int, double, double, double *, int, double, double, int mrkr1 = 0, int mrkr2 = 0);
  void clear(void);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  QColor SignalColor1,
         SignalColor2,
         BackgroundColor,
         RasterColor,
         MarkerColor;

  double max_value1,
         min_value1,
         *buf1,
         max_value2,
         min_value2,
         *buf2,
         h_range_start,
         h_range_end,
         v_range_start,
         v_range_end;

  int bufsize1,
      bufsize2,
      h_rastersize,
      v_rastersize,
      drawcurve_before_raster,
      tracewidth,
      marker_1,
      marker_2,
      sz_hint_w,
      sz_hint_h,
      h_val_enabled,
      v_val_enabled;
};


#endif


