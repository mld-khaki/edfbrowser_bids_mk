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



#include "aeeg_dock.h"


UI_aeeg_dock::UI_aeeg_dock(QWidget *w_parent, aeeg_dock_param_t par)
{
  char str_1024[1024]={""};

  QFrame *frame;

  QGridLayout *grid_layout;

  is_deleted = 0;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  param.mainwindow = mainwindow;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str_1024, 1024, " aEEG   %s", param.signalcomp->signallabel);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  aeeg_curve = new aeeg_curve_widget;
  aeeg_curve->set_params(&param);
  aeeg_curve->setMinimumHeight(mainwindow->aeegdock_height);
  aeeg_curve->setMaximumHeight(mainwindow->aeegdock_height);
  aeeg_curve->setMinimumWidth(100);
  aeeg_curve->setContentsMargins(0, 0, 0, 0);
  aeeg_curve->set_trace_color(Qt::darkBlue);
  aeeg_curve->set_range(0, param.scale_max_amp);
  aeeg_curve->make_logarithmic();

  srl_indic1 = new log_vruler_indicator;
  srl_indic1->set_params(&param);
  srl_indic1->set_scaling(w_scaling, h_scaling);
  srl_indic1->setContentsMargins(0, 0, 0, 0);
  srl_indic1->setMinimumWidth(40 * w_scaling);
  srl_indic1->set_range(0, param.scale_max_amp);

  srl_indic2 = new log_vruler_indicator;
  srl_indic2->set_params(&param);
  srl_indic2->set_scaling(w_scaling, h_scaling);
  srl_indic2->setContentsMargins(0, 0, 0, 0);
  srl_indic2->setMinimumWidth(40 * w_scaling);
  srl_indic2->set_range(0, param.scale_max_amp);
  srl_indic2->set_mirrored(true);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic1, 0, 0);
  grid_layout->addWidget(aeeg_curve, 0, 1);
  grid_layout->addWidget(srl_indic2, 0, 2);
  grid_layout->setColumnStretch(1, 100);

  aeeg_dock = new QToolBar(str_1024, mainwindow);
  aeeg_dock->setOrientation(Qt::Horizontal);
  aeeg_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  aeeg_dock->setAttribute(Qt::WA_DeleteOnClose);
  aeeg_dock->setContextMenuPolicy(Qt::CustomContextMenu);
  aeeg_dock->addWidget(frame);

  context_menu = new QMenu(aeeg_dock);
  QAction *settings_act = new QAction("Settings", this);
  QAction *close_act = new QAction("Close", this);
  context_menu->addAction(settings_act);
  context_menu->addAction(close_act);

  QObject::connect(aeeg_dock,    SIGNAL(destroyed(QObject *)),               this, SLOT(aeeg_dock_destroyed(QObject *)));
  QObject::connect(settings_act, SIGNAL(triggered(bool)),                    this, SLOT(show_settings(bool)));
  QObject::connect(aeeg_dock,    SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_context_menu(QPoint)));
  QObject::connect(mainwindow,   SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));
  QObject::connect(close_act,    SIGNAL(triggered(bool)),                    this, SLOT(close_dock(bool)));

  file_pos_changed(0);
}


UI_aeeg_dock::~UI_aeeg_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    aeeg_dock->close();

    param.signalcomp->aeeg_idx[param.instance_num] = 0;

    mainwindow->aeeg_dock[param.instance_num] = NULL;
  }

  free(param.max_seg_val);
  free(param.min_seg_val);
  free(param.max_median_val);
  free(param.min_median_val);
}


void UI_aeeg_dock::aeeg_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.signalcomp->aeeg_idx[param.instance_num] = 0;

    mainwindow->aeeg_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_aeeg_dock::close_dock(bool)
{
  delete this;
}


void UI_aeeg_dock::file_pos_changed(long long)
{
  int i;

  long long f_pos=0;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
    {
      if(param.signalcomp == mainwindow->signalcomp[i])
      {
        f_pos = mainwindow->signalcomp[i]->edfhdr->viewtime;

        break;
      }
    }
  }

  if(i == mainwindow->signalcomps)  return;

  aeeg_curve->set_marker_position((int)(f_pos / 10000000LL));
}


void UI_aeeg_dock::show_context_menu(QPoint)
{
  context_menu->exec(QCursor::pos());
}


void UI_aeeg_dock::show_settings(bool)
{
  char str_4096[4096]={"aEEG "};

  strlcat(str_4096, param.signalcomp->signallabel, 4096);

  QDialog *settings_dialog = new QDialog;
  settings_dialog->setMinimumSize(300, 350);
  settings_dialog->setWindowTitle(str_4096);
  settings_dialog->setModal(true);
  settings_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  QLabel *label = new QLabel;

  height_spinbox = new QSpinBox;
  height_spinbox->setRange(80, 500);
  height_spinbox->setSingleStep(10);
  height_spinbox->setSuffix(" px");
  height_spinbox->setValue(mainwindow->aeegdock_height);

  plot_margins_checkbox = new QCheckBox;
  plot_margins_checkbox->setTristate(false);
  if(param.plot_margins)
  {
    plot_margins_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    plot_margins_checkbox->setCheckState(Qt::Unchecked);
  }

  QPushButton *pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  snprintf(str_4096, 4096,
         " \n"
         "Segment length: %i sec\n"
         " \n"
         "Bandpass filter: %.1f - %.1f Hz\n"
         " \n"
         "Envelope Lowpass filter: %.2f Hz\n"
         " \n",
         param.segment_len,
         param.bp_min_hz,
         param.bp_max_hz,
         param.lp_hz);

  label->setText(str_4096);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(height_spinbox);
  hlayout2->addStretch(1000);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(plot_margins_checkbox);
  hlayout3->addStretch(1000);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Dock height: ", hlayout2);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Plot margins: ", hlayout3);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label);
  vlayout1->addStretch(1000);
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  settings_dialog->setLayout(vlayout1);

  QObject::connect(pushButton1,           SIGNAL(clicked()),         settings_dialog, SLOT(close()));
  QObject::connect(height_spinbox,        SIGNAL(valueChanged(int)), this,            SLOT(height_spinbox_changed(int)));
  QObject::connect(plot_margins_checkbox, SIGNAL(stateChanged(int)), this,            SLOT(plot_margins_checkbox_changed(int)));

  settings_dialog->exec();
}


void UI_aeeg_dock::plot_margins_checkbox_changed(int)
{
  if(plot_margins_checkbox->checkState() == Qt::Checked)
  {
    param.plot_margins = 1;

    mainwindow->aeeg_plot_margins = 1;
  }
  else
  {
    param.plot_margins = 0;

    mainwindow->aeeg_plot_margins = 0;
  }

  aeeg_curve->set_params(&param);
  aeeg_curve->update();
}


void UI_aeeg_dock::height_spinbox_changed(int val)
{
  mainwindow->aeegdock_height = val;

  aeeg_curve->setMinimumHeight(mainwindow->aeegdock_height);
  aeeg_curve->setMaximumHeight(mainwindow->aeegdock_height);
}


log_vruler_indicator::log_vruler_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(50);

  max_val = 100;

  min_val = 0;

  mirrored = 0;

  w_scaling = 1;
  h_scaling = 1;
}


void log_vruler_indicator::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(30 * w_scaling);
}


void log_vruler_indicator::set_params(aeeg_dock_param_t *parms)
{
  param = *parms;
}


void log_vruler_indicator::set_mirrored(bool mir)
{
  if(mir == true)
  {
    mirrored = 1;
  }
  else
  {
    mirrored = 0;
  }
}


void log_vruler_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, tmp;

  char str_128[128]={""};

  double pixel_per_unit;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  // 100  50  25  10   5   4   3   2   1  (log10 conversion)
  // 100  85  70  50  35  30  24  15   0

  pixel_per_unit = h / 105.0;

  if(!mirrored)
  {
    painter.drawLine(w - 4, 0, w - 4, h);
  }
  else
  {
    painter.drawLine(4, 0, 4, h);
  }

  for(i=1; i<5; i++)  // linear region
  {
    tmp = (i * 5 * pixel_per_unit) + 0.5;

    if(!mirrored)
    {
      painter.drawLine(w - 4, h - tmp, w - 13, h -  tmp);
    }
    else
    {
      painter.drawLine(4, h - tmp, 13, h -  tmp);
    }
  }

  for(i=1; i<6; i++)
  {
    switch(i)
    {
      case 0: tmp = 0;
              snprintf(str_128, 128, "%i", 0);
              break;
//          case 1: tmp = 35;
      case 1: tmp = 25;  // linear/log threshold hack
              snprintf(str_128, 128, "%i", (int)(max_val * 0.05 + 0.5));
              break;
      case 2: tmp = 50;
              snprintf(str_128, 128, "%i", (int)(max_val * 0.1 + 0.5));
              break;
      case 3: tmp = 70;
              snprintf(str_128, 128, "%i", (int)(max_val * 0.25 + 0.5));
              break;
      case 4: tmp = 85;
              snprintf(str_128, 128, "%i", (int)(max_val * 0.5 + 0.5));
              break;
      case 5: tmp = 100;
              snprintf(str_128, 128, "%i", (int)(max_val + 0.5));
              break;
    }

    tmp *= pixel_per_unit;

    if(!mirrored)
    {
      painter.drawLine(w - 4, h - tmp, w - 13, h - tmp);

      painter.drawText(QRectF(w - (35 * w_scaling), h - (int)(tmp + 0.5 + (9 * h_scaling)), 20 * w_scaling, 20 * h_scaling), Qt::AlignRight | Qt::AlignVCenter, str_128);
    }
    else
    {
      painter.drawLine(4, h - tmp, 13, h - tmp);

      painter.drawText(QRectF(13 * w_scaling, h - (int)(tmp + 0.5 + (9 * h_scaling)), 20 * w_scaling, 20 * h_scaling), Qt::AlignLeft | Qt::AlignVCenter, str_128);
    }
  }

  /* draw the physical dimension (unit) */
  if(!mirrored)
  {
    painter.drawText(2, h - 3, param.unit);
  }
  else
  {
    painter.drawText(13 * w_scaling, h - 3, param.unit);
  }
}


void log_vruler_indicator::set_range(int min, int max)
{
  max_val = max;

  min_val = min;
}


aeeg_curve_widget::aeeg_curve_widget(QWidget *w_parent) : QWidget(w_parent)
{
  mainwindow = NULL;

  memset(&param, 0, sizeof(aeeg_dock_param_t));

  setAttribute(Qt::WA_OpaquePaintEvent);

  aeeg_min = 0;

  aeeg_max = 100;

  aeeg_range = aeeg_max - aeeg_min;

  trace_color = Qt::red;

  marker_pos = 0;
}


void aeeg_curve_widget::set_params(aeeg_dock_param_t *parms)
{
  param = *parms;

  mainwindow = param.mainwindow;
}


void aeeg_curve_widget::set_range(int min, int max)
{
  aeeg_max = max;

  aeeg_min = min;

  aeeg_range = aeeg_max - aeeg_min;
}


void aeeg_curve_widget::set_trace_color(QColor color)
{
  trace_color = color;
}


void aeeg_curve_widget::make_logarithmic(void)
{
  int i;

  double dtmp,
         log_threshold_1,
         log_threshold_2;

  dtmp = aeeg_max / log10(aeeg_max);

  log_threshold_1 = aeeg_max / 10;

  log_threshold_2 = aeeg_max / 20;

  for(i=0; i<param.segments_in_recording; i++)
  {
    if(param.max_seg_val[i] > log_threshold_1)  // linear/log threshold hack
    {
      param.max_seg_val[i] = log10(param.max_seg_val[i]) * dtmp;
    }
    else
    {
      param.max_seg_val[i] *= log_threshold_2;
    }

    if(param.min_seg_val[i] > log_threshold_1)  // linear/log threshold hack
    {
      param.min_seg_val[i] = log10(param.min_seg_val[i]) * dtmp;
    }
    else
    {
      param.min_seg_val[i] *= log_threshold_2;
    }
  }

  for(i=0; i<param.medians_in_recording; i++)
  {
    if(param.max_median_val[i] > log_threshold_1)  // linear/log threshold hack
    {
      param.max_median_val[i] = log10(param.max_median_val[i]) * dtmp;
    }
    else
    {
      param.max_median_val[i] *= log_threshold_2;
    }

    if(param.min_median_val[i] > log_threshold_1)  // linear/log threshold hack
    {
      param.min_median_val[i] = log10(param.min_median_val[i]) * dtmp;
    }
    else
    {
      param.min_median_val[i] *= log_threshold_2;
    }
  }
}


void aeeg_curve_widget::set_marker_position(int pos_s)
{
  marker_pos = pos_s;

  update();
}


void aeeg_curve_widget::paintEvent(QPaintEvent *)
{
  int w, h, i, tmp=0,
      pos_x1,
      pos_x2,
      x_marker_pos=-1,
      trace_px_len,
      x_offset=0;

  double pixel_per_unit,
         h_step_segment,
         h_step_second,
         v_sense;

  char str_64[64]={""};

  w = width();
  h = height();

  QPainter painter(this);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
    painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  painter.fillRect(0, 0, w, h, Qt::white);

  if(param.mainwindow == NULL) return;

  // 100  50  25  10   5   4   3   2   1  (log10 conversion)
  // 100  85  70  50  35  30  24  15   0

  pixel_per_unit = h / 105.0;

  h_step_second = (6.0 / mainwindow->x_pixelsizefactor) / 3600.0;

  trace_px_len = ((param.signalcomp->file_duration / 10000000.0) / 3600.0) * (6.0 / mainwindow->x_pixelsizefactor);

  if((marker_pos >= -(int)(mainwindow->pagetime / 10000000LL)) && (marker_pos <= (param.signalcomp->file_duration / 10000000LL)))
  {
    x_marker_pos = marker_pos * h_step_second;
  }

  /* auto-adjust the horizontal centering around the file position indicator */
  if(trace_px_len > w)
  {
    x_offset = (w - trace_px_len) / 2;

    tmp = (w / 2) - x_marker_pos - x_offset;

    if(tmp > 0)
    {
      x_offset += tmp;

      if(x_offset > 0)
      {
        x_offset = 0;
      }
    }
    else if(tmp < 0)
      {
        x_offset += tmp;

        if(x_offset < (w - trace_px_len))
        {
          x_offset = w - trace_px_len;
        }
      }
  }

  /* set horizontal scale to 6 cm per hour -> approx. 1 pixel per segment */
  h_step_segment = (6.0 / mainwindow->x_pixelsizefactor) / (3600.0 / param.segment_len);

  v_sense = (h / aeeg_max) / 1.05;

  /* draw the rulers */
  painter.setPen(Qt::lightGray);

  for(i=1; i<5; i++)  // linear region
  {
    tmp = (i * 5 * pixel_per_unit) + 0.5;

    painter.drawLine(0, h - tmp, w, h -  tmp);
  }

  for(i=1; i<6; i++)
  {
    switch(i)
    {
      case 0: tmp = 0;
              break;
//          case 1: tmp = 35;
      case 1: tmp = 25;  // linear/log threshold hack
              break;
      case 2: tmp = 50;
              break;
      case 3: tmp = 70;
              break;
      case 4: tmp = 85;
              break;
      case 5: tmp = 100;
              break;
    }

    tmp *= pixel_per_unit;

    painter.drawLine(0, h - tmp, w, h - tmp);
  }

  /* horizontal ruler, draw a vertical line (graticule) for every hour */
  for(i=1; 128; i++)
  {
    tmp = (i * (6.0 / mainwindow->x_pixelsizefactor)) + x_offset + 0.5;

    if(tmp > (w - x_offset))  break;

    painter.drawLine(tmp, 0, tmp, h);
  }

  /* draw the signal label */
  strlcpy(str_64, "aEEG ", 64);
  strlcat(str_64, param.signalcomp->signallabel, 64);
  painter.drawText(30 * mainwindow->w_scaling, h * 0.7, str_64);
  painter.drawText(w - (120 * mainwindow->w_scaling), h * 0.7, str_64);

  if(x_offset)
  {
    painter.translate(x_offset, 0);
  }

  /* draw the marker */
  if(x_marker_pos >= 0)
  {
    painter.setPen(Qt::red);
    painter.drawLine(x_marker_pos, 0, x_marker_pos, h);
  }

  /* draw the traces */
  painter.setPen(trace_color);

  for(i=0; i<param.segments_in_recording; i++)
  {
    pos_x1 = i * h_step_segment;

    painter.drawLine(pos_x1, h - (param.max_seg_val[i] * v_sense), pos_x1, h - (param.min_seg_val[i] * v_sense));

    pos_x2 = (i + 1) * h_step_segment;

    for(++pos_x1; pos_x1 < pos_x2; pos_x1++)
    {
      painter.drawLine(pos_x1, h - (param.max_seg_val[i] * v_sense), pos_x1, h - (param.min_seg_val[i] * v_sense));
    }
  }

  if(param.plot_margins)
  {  /* draw the margins */
    painter.setPen(Qt::green);
    h_step_segment *= 20;

    for(i=0; i<param.medians_in_recording; i++)
    {
      painter.drawLine(i * h_step_segment, h - (param.max_median_val[i] * v_sense), (i + 1) * h_step_segment, h - (param.max_median_val[i] * v_sense));

      painter.drawLine(i * h_step_segment, h - (param.min_median_val[i] * v_sense), (i + 1) * h_step_segment, h - (param.min_median_val[i] * v_sense));
    }
  }
}































