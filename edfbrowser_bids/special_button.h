/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2024 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
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



#ifndef SPECIALBUTTON_H
#define SPECIALBUTTON_H

#include <string.h>

#include <QtGlobal>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QSize>
#include <QFontMetrics>


class SpecialButton: public QWidget
{
  Q_OBJECT

public:
  SpecialButton(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const;
  int heightForWidth(int) const;

public slots:
  void setColor(QColor);
  QColor color();
  void setGlobalColor(int);
  int globalColor();
  void setText(const char *);

protected:
  void paintEvent(QPaintEvent *);
  QColor ButtonColor;
  void mouseReleaseEvent(QMouseEvent *);

private:
  char buttonText[2048];

  int global_Color,
      sz_hint_w,
      sz_hint_h;

signals:
     void clicked(SpecialButton *);
};


#endif


