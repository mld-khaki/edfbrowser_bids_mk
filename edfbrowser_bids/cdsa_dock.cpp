/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2024 Teunis van Beelen
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



#include "cdsa_dock.h"


UI_cdsa_dock::UI_cdsa_dock(QWidget *w_parent, cdsa_dock_param_t par)
{
  char str1_1024[1024]={""};

  QLabel *cdsa_label,
         *ruler_label,
         *color_indic_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str1_1024, 1024, "Color Density Spectral Array   %s", param.signalcomp->signallabel);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  cdsa_label = new QLabel;
  cdsa_label->setScaledContents(true);
  cdsa_label->setPixmap(*param.pxm);
  cdsa_label->setMinimumHeight(100 * h_scaling);
  cdsa_label->setMinimumWidth(100 * w_scaling);
  cdsa_label->setContentsMargins(0, 0, 0, 0);

  trck_indic = new simple_tracking_indicator;
  trck_indic->set_scaling(w_scaling, h_scaling);
  trck_indic->set_maximum(param.segments_in_recording * param.segment_len);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator;
  srl_indic->set_scaling(w_scaling, h_scaling);
  srl_indic->set_maximum(param.max_hz);
  srl_indic->set_minimum(param.min_hz);
  srl_indic->set_unit("Hz");
  srl_indic->setContentsMargins(0, 0, 0, 0);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Hz / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  color_indic = new simple_color_index;
  color_indic->set_scaling(w_scaling, h_scaling);
  color_indic->set_max_volt(param.max_voltage);
  color_indic->set_max_pwr(param.max_pwr);
  color_indic->set_min_pwr(param.min_pwr);
  color_indic->set_log_enabled(param.log);
  color_indic->set_pwr_enabled(param.power_voltage);
  color_indic->set_unit(param.unit);

  color_indic_label = new QLabel;
  color_indic_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  color_indic_label->setText(param.unit);
  color_indic_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(cdsa_label, 0, 1);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
  grid_layout->addWidget(color_indic, 0, 2);
  grid_layout->addWidget(color_indic_label, 1, 2);

  cdsa_dock = new QDockWidget(str1_1024, mainwindow);
  cdsa_dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  cdsa_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  cdsa_dock->setAttribute(Qt::WA_DeleteOnClose);
  cdsa_dock->setContextMenuPolicy(Qt::CustomContextMenu);
  cdsa_dock->setWidget(frame);

  mainwindow->addDockWidget(Qt::BottomDockWidgetArea, cdsa_dock, Qt::Horizontal);

  QObject::connect(cdsa_dock,  SIGNAL(destroyed(QObject *)),               this, SLOT(cdsa_dock_destroyed(QObject *)));
  QObject::connect(cdsa_dock,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextmenu_requested(QPoint)));
  QObject::connect(mainwindow, SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);

  delete param.pxm;
}


UI_cdsa_dock::~UI_cdsa_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    cdsa_dock->close();

    param.signalcomp->cdsa_idx[param.instance_num] = 0;

    mainwindow->cdsa_dock[param.instance_num] = NULL;
  }
}


void UI_cdsa_dock::cdsa_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.signalcomp->cdsa_idx[param.instance_num] = 0;

    mainwindow->cdsa_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_cdsa_dock::contextmenu_requested(QPoint)
{
  int ov_lap[5]={0, 50, 67, 75, 80};

  char str[4096]={"CDSA "},
       wnd_func[13][32]={"Rectangular",
                    "Hamming",
                    "4-term Blackman-Harris",
                    "7-term Blackman-Harris",
                    "Nuttall3b",
                    "Nuttall4c",
                    "Hann",
                    "HFT223D",
                    "HFT95",
                    "Kaiser2",
                    "Kaiser3",
                    "Kaiser4",
                    "Kaiser5"},
      yesno[2][32]={"no", "yes"};

  strlcat(str, param.signalcomp->signallabel, 4096);

  QDialog *myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(300, 215);
  myobjectDialog->setWindowTitle(str);
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  QLabel *label = new QLabel;

  QPushButton *pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  if(param.log)
  {
    snprintf(str, 4096,
           "Segment length: %i sec.\n"
           "Block length: %i sec.\n"
           "Overlap: %i %%\n"
           "Window function: %s\n"
           "Max. level: %i %s\n"
           "Min. level: %i %s\n"
           "Logarithmic: %s\n"
           "Power: %s",
           param.segment_len,
           param.block_len,
           ov_lap[param.overlap - 1],
           wnd_func[param.window_func],
           param.max_pwr, param.unit,
           param.min_pwr, param.unit,
           yesno[param.log],
           yesno[param.power_voltage]);
  }
  else
  {
    snprintf(str, 4096,
           "Segment length: %i sec.\n"
           "Block length: %i sec.\n"
           "Overlap: %i %%\n"
           "Window function: %s\n"
           "Max. level: %f %s\n"
           "Logarithmic: %s\n"
           "Power: %s",
           param.segment_len,
           param.block_len,
           ov_lap[param.overlap - 1],
           wnd_func[param.window_func],
           param.max_voltage, param.unit,
           yesno[param.log],
           yesno[param.power_voltage]);
  }

  label->setText(str);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_cdsa_dock::file_pos_changed(long long)
{
  int i;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
    {
      if(param.signalcomp == mainwindow->signalcomp[i])
      {
        break;
      }
    }
  }

  if(i == mainwindow->signalcomps)  return;

  trck_indic->set_position((int)((param.signalcomp->edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


simple_tracking_indicator::simple_tracking_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  w_scaling = 1;
  h_scaling = 1;

  pos = 0;
  max = 100;
}


void simple_tracking_indicator::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedHeight(16 * h_scaling);
}


void simple_tracking_indicator::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator::set_maximum(long long max_)
{
  max = max_;
}


void simple_tracking_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, pos_x, step=0;

  char str1_128[128]={""};

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  if(((double)w / (double)max) > 0.02)
  {
    step = 1;
  }
  else if(((double)w / (double)max) > 0.01)
    {
      step = 2;
    }
    else if(((double)w / (double)max) > 0.005)
      {
        step = 4;
      }

  if(step)
  {
    for(i=0; i<200; i+=step)
    {
      pos_x = (((double)(i * 3600) / (double)max) * (double)w) + 0.5;

      if(pos_x > w)
      {
        break;
      }

      painter.drawLine(pos_x, 0, pos_x, h);

      snprintf(str1_128, 128, "%ih", i);

      painter.drawText(pos_x + 4, h - 2, str1_128);
    }
  }

  draw_small_arrow(&painter, (int)((((double)pos / (double)max) * w) + 0.5), 0, 0, Qt::black);
}


void simple_tracking_indicator::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
{
  QPainterPath path;

  if(rot == 0)
  {
    path.moveTo(xpos,      ypos);
    path.lineTo(xpos - (8 * w_scaling), ypos + (15 * h_scaling));
    path.lineTo(xpos + (8 * w_scaling), ypos + (15 * h_scaling));
    path.lineTo(xpos,      ypos);

    painter->fillPath(path, color);
  }
}


simple_color_index::simple_color_index(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);

  w_scaling = 1;
  h_scaling = 1;

  max_volt = 100;
  min_pwr = 0;
  max_pwr = 100;
  log = 1;
  unit[0] = 0;
}


void simple_color_index::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * h_scaling);
}


void simple_color_index::set_max_volt(double val)
{
  max_volt = val;
}


void simple_color_index::set_max_pwr(int val)
{
  max_pwr = val;
}


void simple_color_index::set_min_pwr(int val)
{
  min_pwr = val;
}


void simple_color_index::set_log_enabled(int val)
{
  log = val;
}


void simple_color_index::set_pwr_enabled(int val)
{
  pwr = val;
}


void simple_color_index::set_unit(const char *str)
{
  strlcpy(unit, str, 32);
}


void simple_color_index::paintEvent(QPaintEvent *)
{
  int i, j, w, h,
      rgb_map[1786][3],
      rgb_idx,
      colorbar_width,
      colorbar_height,
      colorbar_h_pos;

  double color_idx_per_pixel;

  char str1_32[32]={""};

  w = width();
  h = height();

  QPainter painter(this);

  for(i=0; i<256; i++)
  {
    rgb_map[i][0] = i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = i;
  }

  for(i=256; i<511; i++)
  {
    rgb_map[i][0] = 510 - i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = 255;
  }

  for(i=511; i<766; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = i - 510;
    rgb_map[i][2] = 255;
  }

  for(i=766; i<1021; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 1020 - i;
  }

  for(i=1021; i<1276; i++)
  {
    rgb_map[i][0] = i - 1020;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 0;
  }

  for(i=1276; i<1531; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = 1530 - i;
    rgb_map[i][2] = 0;
  }

  for(i=1531; i<1786; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = i - 1530;
    rgb_map[i][2] = i - 1530;
  }

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  colorbar_width = 15 * w_scaling;

  colorbar_h_pos = 8 * h_scaling;

  colorbar_height = h - (16 * h_scaling);

  painter.drawRect(5, colorbar_h_pos, colorbar_width, colorbar_height);

  if(log)
  {
    snprintf(str1_32, 32, "%i", max_pwr);

    painter.drawText(10 + (15 * w_scaling), 15 * h_scaling, str1_32);

    for(i=1; i<7; i++)
    {
      snprintf(str1_32, 32, "%.1f", (((max_pwr - min_pwr) / 7.0) * i) + min_pwr);

      painter.drawText(10 + (15 * w_scaling), h - ((4.0 * h_scaling) + (i * ((h - (17.0 * h_scaling)) / 7.0))), str1_32);
    }

    snprintf(str1_32, 32, "%i", min_pwr);

    painter.drawText(10 + (15 * w_scaling), h - (5 * h_scaling), str1_32);
  }
  else
  {
    setFixedWidth(80 * w_scaling);

    if(pwr)
    {
      for(i=0; i<8; i++)
      {
        snprintf(str1_32, 32, "%.1e", ((max_volt * max_volt) / 7.0) * i);

        painter.drawText(10 + (15 * w_scaling), h - ((4.0 * h_scaling) + (i * ((h - (17.0 * h_scaling)) / 7.0))), str1_32);
      }
    }
    else
    {
      for(i=0; i<8; i++)
      {
        snprintf(str1_32, 32, "%.1e", (max_volt / 7.0) * i);

        painter.drawText(10 + (15 * w_scaling), h - ((4.0 * h_scaling) + (i * ((h - (17.0 * h_scaling)) / 7.0))), str1_32);
      }
    }
  }

  color_idx_per_pixel = 1785.0 / (colorbar_height - 1);

  for(i=0; i<(colorbar_height-1); i++)
  {
    rgb_idx = ((colorbar_height - 1) - i) * color_idx_per_pixel;

    if(rgb_idx > 1785)  rgb_idx = 1785;

    if(rgb_idx < 0)  rgb_idx = 0;

    painter.setPen(QColor(rgb_map[rgb_idx][0], rgb_map[rgb_idx][1], rgb_map[rgb_idx][2]));

    for(j=0; j<(colorbar_width-1); j++)
    {
      painter.drawPoint(6 + j, colorbar_h_pos + 1 + i);
    }
  }
}


simple_ruler_indicator::simple_ruler_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);

  w_scaling = 1;
  h_scaling = 1;

  min = 0;
  max = 100;
}


void simple_ruler_indicator::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * h_scaling);
}


void simple_ruler_indicator::set_minimum(int val)
{
  min = val;
}


void simple_ruler_indicator::set_maximum(int val)
{
  max = val;
}


void simple_ruler_indicator::set_unit(const char *str)
{
  strlcpy(unit, str, 32);
}


void simple_ruler_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, range, skip;

  double pixel_per_unit;

  char str1_64[64]={""};

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  painter.drawLine(w - 4, 0, w - 4, h);

  range = max - min;

  pixel_per_unit = (double)h / (double)range;

  for(skip=1; skip<100000; )
  {
    if((skip * pixel_per_unit) > 25)  break;

    switch(skip)
    {
      case   1 : skip = 2;
                 break;
      case   2 : skip = 5;
                 break;
      case   5 : skip = 10;
                 break;
      case  10 : skip = 20;
                 break;
      case  20 : skip = 50;
                 break;
      case  50 : skip = 100;
                 break;
      case 100 : skip = 200;
                 break;
      case 200 : skip = 500;
                 break;
      case 500 : skip = 1000;
                 break;
      case 1000 : skip = 2000;
                 break;
      case 2000 : skip = 5000;
                 break;
      case 5000 : skip = 10000;
                 break;
      case 10000 : skip = 20000;
                 break;
      case 20000 : skip = 50000;
                 break;
      case 50000 : skip = 100000;
                 break;
      case 100000 : skip = 200000;
                 break;
      case 200000 : skip = 500000;
                 break;
    }
  }

  for(i=0; i<=range; i++)
  {
    if(!((i + min) % skip))
    {
      painter.drawLine(w - 4, h - (int)((pixel_per_unit * i) + 0.5), w - 13, h - (int)((pixel_per_unit * i) + 0.5));

      snprintf(str1_64, 64, "%i", min + i);

      painter.drawText(QRectF(2, h - (int)((pixel_per_unit * i) + 0.5) - (9 * h_scaling), 40 * w_scaling, 25 * h_scaling), Qt::AlignRight | Qt::AlignHCenter, str1_64);
    }
  }
}





























