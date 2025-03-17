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



#include "hrv_dock.h"


UI_hrv_dock::UI_hrv_dock(QWidget *w_parent, hrv_dock_param_t par)
{
  QLabel *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  trace_color = mainwindow->hrvdock_trace_color;

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  hrv_curve = new hrv_curve_widget;
  hrv_curve->setMinimumHeight(mainwindow->hrvdock_height);
  hrv_curve->setMaximumHeight(mainwindow->hrvdock_height);
  hrv_curve->setMinimumWidth(500);
  hrv_curve->setContentsMargins(0, 0, 0, 0);
  hrv_curve->set_params(&param);
  hrv_curve->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);
  hrv_curve->setToolTip(param.annot_name);
  hrv_curve->set_trace_color(trace_color);

  trck_indic = new simple_tracking_indicator3;
  trck_indic->set_scaling(w_scaling, h_scaling);
  trck_indic->set_maximum(param.edfhdr->recording_len_sec);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator3;
  srl_indic->set_scaling(w_scaling, h_scaling);
  srl_indic->set_params(&param);
  srl_indic->setContentsMargins(0, 0, 0, 0);
  srl_indic->setMinimumWidth(70 * w_scaling);
  srl_indic->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("bpm / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(hrv_curve, 0, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->setColumnStretch(1, 100);

  hrv_dock = new QToolBar("Heart Rate", mainwindow);
  hrv_dock->setOrientation(Qt::Horizontal);
  hrv_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  hrv_dock->addWidget(frame);
  hrv_dock->setAttribute(Qt::WA_DeleteOnClose, true);
  hrv_dock->setContextMenuPolicy(Qt::CustomContextMenu);

  context_menu = new QMenu(hrv_dock);
  QAction *settings_act = new QAction("Settings", this);
  QAction *close_act = new QAction("Close", this);
  context_menu->addAction(settings_act);
  context_menu->addAction(close_act);

  QObject::connect(mainwindow,   SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));
  QObject::connect(mainwindow,   SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));
  QObject::connect(hrv_dock,     SIGNAL(destroyed(QObject *)),               this, SLOT(hrv_dock_destroyed(QObject *)));
  QObject::connect(hrv_dock,     SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_context_menu(QPoint)));
  QObject::connect(settings_act, SIGNAL(triggered(bool)),                    this, SLOT(show_settings(bool)));
  QObject::connect(close_act,    SIGNAL(triggered(bool)),                    this, SLOT(close_dock(bool)));

  file_pos_changed(0);
}


UI_hrv_dock::~UI_hrv_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(hrv_dock);

    param.edfhdr->hrv_dock[param.instance_num] = 0;

    mainwindow->hrv_dock[param.instance_num] = NULL;
  }
}


void UI_hrv_dock::update_curve(void)
{
  hrv_curve->update();
}


void UI_hrv_dock::hrv_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.edfhdr->hrv_dock[param.instance_num] = 0;

    mainwindow->hrv_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_hrv_dock::file_pos_changed(long long)
{
  trck_indic->set_position((int)((param.edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


void UI_hrv_dock::show_context_menu(QPoint)
{
  context_menu->exec(QCursor::pos());
}


void UI_hrv_dock::show_settings(bool)
{
  QDialog *dialog = new QDialog;
  dialog->setMinimumSize(300 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  dialog->setWindowTitle("Heartrate");
  dialog->setModal(true);
  dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  dialog->setSizeGripEnabled(true);

  settings_close_button = new QPushButton("Close");

  max_bpm_spinbox = new QSpinBox;
  max_bpm_spinbox->setRange(70, 250);
  max_bpm_spinbox->setValue(mainwindow->hrvdock_max_bpm);

  min_bpm_spinbox = new QSpinBox;
  min_bpm_spinbox->setRange(0, 60);
  min_bpm_spinbox->setValue(mainwindow->hrvdock_min_bpm);

  height_spinbox = new QSpinBox;
  height_spinbox->setRange(80, 500);
  height_spinbox->setSingleStep(10);
  height_spinbox->setSuffix(" px");
  height_spinbox->setValue(mainwindow->hrvdock_height);

  color_button = new SpecialButton;
  color_button->setColor(trace_color);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(max_bpm_spinbox);
  hlayout1->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(min_bpm_spinbox);
  hlayout2->addStretch(1000);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(height_spinbox);
  hlayout3->addStretch(1000);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(color_button);
  hlayout4->addStretch(1000);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addStretch(1000);
  hlayout5->addWidget(settings_close_button);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Maximum BPM:", hlayout1);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Minimum BPM:", hlayout2);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Dock height:", hlayout3);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Trace color:", hlayout4);
  flayout->addRow(" ", (QWidget *)NULL);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addLayout(hlayout5);

  dialog->setLayout(vlayout1);

  QObject::connect(settings_close_button, SIGNAL(clicked()),         dialog,        SLOT(close()));
  QObject::connect(max_bpm_spinbox,       SIGNAL(valueChanged(int)), this,          SLOT(max_bpm_spinbox_changed(int)));
  QObject::connect(min_bpm_spinbox,       SIGNAL(valueChanged(int)), this,          SLOT(min_bpm_spinbox_changed(int)));
  QObject::connect(height_spinbox,        SIGNAL(valueChanged(int)), this,          SLOT(height_spinbox_changed(int)));
  QObject::connect(color_button,          SIGNAL(clicked(SpecialButton *)), this,   SLOT(color_button_clicked()));

  dialog->exec();
}


void UI_hrv_dock::height_spinbox_changed(int val)
{
  mainwindow->hrvdock_height = val;

  hrv_curve->setMinimumHeight(mainwindow->hrvdock_height);
  hrv_curve->setMaximumHeight(mainwindow->hrvdock_height);
}


void UI_hrv_dock::max_bpm_spinbox_changed(int val)
{
  mainwindow->hrvdock_max_bpm = val;

  srl_indic->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);
  srl_indic->update();

  hrv_curve->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);
  hrv_curve->update();
}


void UI_hrv_dock::min_bpm_spinbox_changed(int val)
{
  mainwindow->hrvdock_min_bpm = val;

  srl_indic->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);
  srl_indic->update();

  hrv_curve->set_range(mainwindow->hrvdock_min_bpm, mainwindow->hrvdock_max_bpm);
  hrv_curve->update();
}


void UI_hrv_dock::color_button_clicked()
{
  QColor c = QColorDialog::getColor(trace_color, hrv_curve, "Set trace color");
  if(c.isValid() == false)
  {
    return;
  }

  trace_color = c;

  mainwindow->hrvdock_trace_color = trace_color;

  color_button->setColor(trace_color);

  hrv_curve->set_trace_color(trace_color);

  hrv_curve->update();
}


void UI_hrv_dock::close_dock(bool)
{
  delete this;
}


simple_tracking_indicator3::simple_tracking_indicator3(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  w_scaling = 1;
  h_scaling = 1;

  setFixedHeight(16);

  pos = 0;
  max = 100;
}


void simple_tracking_indicator3::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedHeight(16 * h_scaling);
}


void simple_tracking_indicator3::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator3::set_maximum(long long max_)
{
  max = max_;
}


void simple_tracking_indicator3::paintEvent(QPaintEvent *)
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


void simple_tracking_indicator3::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
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


simple_ruler_indicator3::simple_ruler_indicator3(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);

  max_val = 180;

  min_val = 40;

  w_scaling = 1;
  h_scaling = 1;
}


void simple_ruler_indicator3::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * w_scaling);
}


void simple_ruler_indicator3::set_params(hrv_dock_param_t *parms)
{
  param = *parms;
}


void simple_ruler_indicator3::paintEvent(QPaintEvent *)
{
  int i, w, h;

  char str1_32[32]={""};

  double pixel_per_unit,
         offset;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  pixel_per_unit = (double)h / 6.0;

  offset = (double)h / 12.0;

  painter.drawLine(w - 4, offset, w - 4, h - offset);

  for(i=0; i<6; i++)
  {
    painter.drawLine(w - 4, (int)((pixel_per_unit * i) + 0.5 + offset), w - 13, (int)((pixel_per_unit * i) + 0.5 + offset));

    snprintf(str1_32, 32, "%i", (int)(min_val + ((5 - i) * ((max_val - min_val) / 5.0)) ));

    painter.drawText(QRectF(2, (int)((pixel_per_unit * i) + 0.5 + offset - (9 * h_scaling)), 40 * w_scaling, 25 * h_scaling), Qt::AlignRight | Qt::AlignHCenter, str1_32);
  }
}


void simple_ruler_indicator3::set_range(int min, int max)
{
  max_val = max;

  min_val = min;
}


hrv_curve_widget::hrv_curve_widget(QWidget *w_parent) : QWidget(w_parent)
{
  mainwindow = NULL;

  setAttribute(Qt::WA_OpaquePaintEvent);

  bpm_min = 50;

  bpm_max = 180;

  bpm_range = bpm_max - bpm_min;

  trace_color = Qt::red;
}


void hrv_curve_widget::set_params(hrv_dock_param_t *parms)
{
  param = *parms;

  mainwindow = param.mainwindow;
}


void hrv_curve_widget::set_range(int min, int max)
{
  bpm_max = max;

  bpm_min = min;

  bpm_range = bpm_max - bpm_min;
}


void hrv_curve_widget::set_trace_color(QColor color)
{
  trace_color = color;
}


void hrv_curve_widget::paintEvent(QPaintEvent *)
{
  int w, h, i, n,
      pos_x1=0,
      pos_x2=0,
      skip_first=1;

  double pixel_per_unit,
         pixel_per_sec,
         offset,
         bpm=0,
         old_bpm=0;

  long long hr_ival=0,
            former_onset=-1;

  char str1[MAX_ANNOTATION_LEN];

  annotlist_t *annot_list;

  annotblck_t *annot;

  w = width();
  h = height();

  QPainter painter(this);
#if (QT_VERSION >= 0x050000) && (QT_VERSION < 0x060000)
    painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif
  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.drawText(5 * mainwindow->w_scaling, 18 * mainwindow->h_scaling, param.annot_name);

  offset = ((double)h / 12.0);

  pixel_per_unit = ((double)(h - (2 * offset)) / (double)bpm_range);

  offset = h - offset;

  offset += bpm_min * pixel_per_unit;

  painter.setPen(trace_color);
//  painter.setPen(QPen(QBrush(Qt::red, Qt::SolidPattern), 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

  if(mainwindow == NULL)  return;

  pixel_per_sec = (double)w / (double)(param.edfhdr->recording_len_sec);

  annot_list = &param.edfhdr->annot_list;

  n = edfplus_annotation_size(annot_list);

  for(i=0; i<n; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);
    if(annot == NULL)
    {
      break;
    }

    strlcpy(str1, annot->description, MAX_ANNOTATION_LEN);
    trim_spaces(str1);

    if(!strcmp(str1, param.annot_name))
    {
      hr_ival = annot->onset - former_onset;

      bpm = (60.0 * TIME_FIXP_SCALING) / (double)hr_ival;

      pos_x2 = (((double)(annot->onset) * pixel_per_sec) / ((double)TIME_FIXP_SCALING)) + 0.5;

      if(skip_first || annot->hidden)
      {
        skip_first = 0;
      }
      else
      {
        painter.drawLine(pos_x1, (offset - (old_bpm * pixel_per_unit)) + 0.5,
                         pos_x2, (offset - (bpm * pixel_per_unit)) + 0.5);
      }

      pos_x1 = pos_x2;

      former_onset = annot->onset;

      old_bpm = bpm;
    }
  }
}































