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



#include "hypnogram_dock.h"


UI_hypnogram_dock::UI_hypnogram_dock(QWidget *w_parent, hypnogram_dock_param_t par)
{
  QLabel *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  hypnogram_curve = new hypnogram_curve_widget;
  hypnogram_curve->setMinimumHeight(mainwindow->hypnogramdock_height);
  hypnogram_curve->setMaximumHeight(mainwindow->hypnogramdock_height);
  hypnogram_curve->setMinimumWidth(100);
  hypnogram_curve->setContentsMargins(0, 0, 0, 0);
  hypnogram_curve->set_params(&param);

  trck_indic = new simple_tracking_indicator2;
  trck_indic->set_scaling(w_scaling, h_scaling);
  trck_indic->set_maximum(param.edfhdr->recording_len_sec);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator2;
  srl_indic->set_scaling(w_scaling, h_scaling);
  srl_indic->set_params(&param);
  srl_indic->setContentsMargins(0, 0, 0, 0);
  srl_indic->setMinimumWidth(80 * w_scaling);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Stage / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(hypnogram_curve, 0, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->setColumnStretch(1, 100);

  hypnogram_dock = new QToolBar("Hypnogram", mainwindow);
  hypnogram_dock->setOrientation(Qt::Horizontal);
  hypnogram_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  hypnogram_dock->setMinimumWidth(800);
  hypnogram_dock->addWidget(frame);
  hypnogram_dock->setAttribute(Qt::WA_DeleteOnClose, true);
  hypnogram_dock->setContextMenuPolicy(Qt::CustomContextMenu);

  context_menu = new QMenu(hypnogram_dock);
  QAction *settings_act = new QAction("Settings", this);
  QAction *close_act = new QAction("Close", this);
  context_menu->addAction(settings_act);
  context_menu->addAction(close_act);

  QObject::connect(mainwindow,     SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));
  QObject::connect(hypnogram_dock, SIGNAL(destroyed(QObject *)),               this, SLOT(hypnogram_dock_destroyed(QObject *)));
  QObject::connect(hypnogram_dock, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_context_menu(QPoint)));
  QObject::connect(settings_act,   SIGNAL(triggered(bool)),                    this, SLOT(show_settings(bool)));
  QObject::connect(close_act,      SIGNAL(triggered(bool)),                    this, SLOT(close_dock(bool)));

  file_pos_changed(0);

//   printf("mainwindow: %p\n"
//          "instance_num: %i\n"
//          "use_epoch_len: %i\n"
//          "use_overlay: %i\n"
//          "stage_name[0]: %s\n"
//          "stage_name[1]: %s\n"
//          "stage_name[2]: %s\n"
//          "annot_name[0]: %s\n"
//          "annot_name[1]: %s\n"
//          "annot_name[2]: %s\n"
//          "annot_ov_name[0]: %s\n"
//          "annot_ov_name[1]: %s\n"
//          "annot_ov_name[2]: %s\n",
//          mainwindow,
//          param.instance_num,
//          param.use_epoch_len,
//          param.use_overlays,
//          param.stage_name[0],
//          param.stage_name[1],
//          param.stage_name[2],
//          param.annot_name[0],
//          param.annot_name[1],
//          param.annot_name[2],
//          param.annot_ov_name[0],
//          param.annot_ov_name[1],
//          param.annot_ov_name[2]
//          );
}


UI_hypnogram_dock::~UI_hypnogram_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(hypnogram_dock);

    param.edfhdr->hypnogram_idx[param.instance_num] = 0;

    mainwindow->hypnogram_dock[param.instance_num] = NULL;
  }
}


void UI_hypnogram_dock::close_dock(bool)
{
  delete this;
}


void UI_hypnogram_dock::update_curve(void)
{
  hypnogram_curve->update();
}


void UI_hypnogram_dock::hypnogram_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.edfhdr->hypnogram_idx[param.instance_num] = 0;

    mainwindow->hypnogram_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_hypnogram_dock::show_context_menu(QPoint)
{
  context_menu->exec(QCursor::pos());
}


void UI_hypnogram_dock::file_pos_changed(long long)
{
  trck_indic->set_position((int)((param.edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


void UI_hypnogram_dock::show_settings(bool)
{
  QDialog *settings_dialog = new QDialog;
  settings_dialog->setMinimumSize(300, 200);
  settings_dialog->setWindowTitle("Hypnogram");
  settings_dialog->setModal(true);
  settings_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  height_spinbox = new QSpinBox;
  height_spinbox->setRange(80, 500);
  height_spinbox->setSingleStep(10);
  height_spinbox->setSuffix(" px");
  height_spinbox->setValue(mainwindow->hypnogramdock_height);

  QPushButton *pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(height_spinbox);
  hlayout2->addStretch(1000);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow("Dock height: ", hlayout2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addStretch(1000);
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  settings_dialog->setLayout(vlayout1);

  QObject::connect(pushButton1,    SIGNAL(clicked()),         settings_dialog, SLOT(close()));
  QObject::connect(height_spinbox, SIGNAL(valueChanged(int)), this,            SLOT(height_spinbox_changed(int)));

  settings_dialog->exec();
}


void UI_hypnogram_dock::height_spinbox_changed(int val)
{
  mainwindow->hypnogramdock_height = val;

  hypnogram_curve->setMinimumHeight(mainwindow->hypnogramdock_height);
  hypnogram_curve->setMaximumHeight(mainwindow->hypnogramdock_height);
}


simple_tracking_indicator2::simple_tracking_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  pos = 0;
  max = 100;
}


void simple_tracking_indicator2::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedHeight(16 * h_scaling);
}


void simple_tracking_indicator2::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator2::set_maximum(long long max_)
{
  max = max_;
}


void simple_tracking_indicator2::paintEvent(QPaintEvent *)
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


void simple_tracking_indicator2::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
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


simple_ruler_indicator2::simple_ruler_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);
}


void simple_ruler_indicator2::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * w_scaling);
}


void simple_ruler_indicator2::set_params(hypnogram_dock_param_t *parms)
{
  param = *parms;
}


void simple_ruler_indicator2::paintEvent(QPaintEvent *)
{
  int i, w, h;

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

  for(i=0; i<HYPNOGRAM_STAGENUM; i++)
  {
    painter.drawLine(w - 4, (int)((pixel_per_unit * i) + 0.5 + offset), w - 13, (int)((pixel_per_unit * i) + 0.5 + offset));

    painter.drawText(QRectF(2, (int)((pixel_per_unit * i) + 0.5 + offset) - (9 * h_scaling), 60 * w_scaling, 25 * h_scaling), Qt::AlignRight | Qt::AlignHCenter, param.stage_name[i]);
  }
}


hypnogram_curve_widget::hypnogram_curve_widget(QWidget *w_parent) : QWidget(w_parent)
{
  mainwindow = NULL;

  setAttribute(Qt::WA_OpaquePaintEvent);
}


void hypnogram_curve_widget::set_params(hypnogram_dock_param_t *parms)
{
  int i;

  param = *parms;

  mainwindow = param.mainwindow;

  for(i=0; i<HYPNOGRAM_OVNUM; i++)
  {
    annot_ov_q_color[i] = QColor(param.annot_ov_color[i].r, param.annot_ov_color[i].g, param.annot_ov_color[i].b, param.annot_ov_color[i].a);
  }
}


void hypnogram_curve_widget::paintEvent(QPaintEvent *)
{
  int w, h, i, j, k, n, pos_x1=0, pos_x2=0, stage=0, tmp1, tmp2;

  double pixel_per_unit,
         pixel_per_sec,
         offset;

  long long annot_end=0LL,
            prev_annot_end=0x7fffffffffffffffLL,
            tdiff=0LL;

  char str1_48[48]={""};

  annotlist_t *annot_list;

  annotblck_t *annot;

  w = width();
  h = height();

  QPainter painter(this);

  pixel_per_unit = ((double)h / 6.0);

  offset = ((double)h / 12.0);

  tmp1 = pixel_per_unit + 0.5;
  painter.fillRect(0,    0, w, tmp1, QColor(255, 255, 217));
  tmp2 = pixel_per_unit * 2.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(217, 255, 217));
  tmp1 = pixel_per_unit * 3.0 + 0.5;
  painter.fillRect(0, tmp2, w, tmp1 - tmp2, QColor(231, 248, 255));
  tmp2 = pixel_per_unit * 4.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(213, 237, 255));
  tmp1 = pixel_per_unit * 5.0 + 0.5;
  painter.fillRect(0, tmp2, w, tmp1 - tmp2, QColor(186, 225, 255));
  tmp2 = pixel_per_unit * 6.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(160, 210, 255));

  painter.setPen(Qt::blue);

  if(mainwindow == NULL)  return;

  pixel_per_sec = (double)w / (double)(param.edfhdr->recording_len_sec);

  annot_list = &param.edfhdr->annot_list;

  n = edfplus_annotation_size(annot_list);

  if(param.use_overlays)
  {
    for(i=0, k=0; i<n; i++)
    {
      annot = edfplus_annotation_get_item(annot_list, i);

      strlcpy(str1_48, annot->description, 48);
      trim_spaces(str1_48);

      for(j=0; j<HYPNOGRAM_OVNUM; j++)
      {
        if(!strcmp(str1_48, param.annot_ov_name[j]))
        {
          pos_x2 = ((double)(annot->onset) * pixel_per_sec) / TIME_FIXP_SCALING;

          if(param.use_epoch_len)
          {
            if(annot->long_duration > 0LL)
            {
              pos_x1 = ((double)(annot->long_duration) * pixel_per_sec) / TIME_FIXP_SCALING;

              painter.fillRect(pos_x2, 0, pos_x1, h, annot_ov_q_color[j]);
            }
          }
          else
          {
            if(k)
            {
              painter.fillRect(pos_x1, 0, pos_x2 - pos_x1, h, annot_ov_q_color[stage]);
            }

            pos_x1 = pos_x2;
          }

          stage = j;

          k++;

          break;
        }
      }
    }

    if(!param.use_epoch_len)
    {
      if(k)
      {
        painter.fillRect(pos_x1, 0, w - pos_x1, h, annot_ov_q_color[stage]);
      }
    }
  }

  pos_x1=0;
  pos_x2=0;
  stage=0;

  for(i=0, k=0; i<n; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str1_48, annot->description, 48);
    trim_spaces(str1_48);

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      if(!strcmp(str1_48, param.annot_name[j]))
      {
        pos_x2 = ((double)(annot->onset) * pixel_per_sec) / TIME_FIXP_SCALING;

        if(param.use_epoch_len)
        {
          if(annot->long_duration > 0LL)
          {
            annot_end = annot->onset + annot->long_duration;

            pos_x1 = ((double)(annot_end) * pixel_per_sec) / TIME_FIXP_SCALING;

            painter.drawLine(pos_x1, offset + (j * pixel_per_unit) + 0.5,
                             pos_x2, offset + (j * pixel_per_unit) + 0.5);
          }
        }
        else
        {
          if(k)
          {
            painter.drawLine(pos_x1, offset + (stage * pixel_per_unit) + 0.5,
                             pos_x2, offset + (stage * pixel_per_unit) + 0.5);
          }

          pos_x1 = pos_x2;
        }

        if(k)
        {
          painter.drawLine(pos_x2, offset + (stage * pixel_per_unit) + 0.5,
                           pos_x2, offset + (j * pixel_per_unit) + 0.5);
        }

        if(param.use_epoch_len)
        {
          if(prev_annot_end != 0x7fffffffffffffff)
          {
            tdiff = annot->onset - prev_annot_end;

            if(tdiff > mainwindow->hypnogram_epoch_len_threshold)
            {
              painter.fillRect((int)(((double)(prev_annot_end) * pixel_per_sec) / TIME_FIXP_SCALING),
                               0,
                               ((int)(((double)(tdiff) * pixel_per_sec) / TIME_FIXP_SCALING)) + 2,
                               h,
                               QColor(128, 0, 0, 48));
            }
            else if(tdiff < -mainwindow->hypnogram_epoch_len_threshold)
              {
                painter.fillRect((int)(((double)(prev_annot_end) * pixel_per_sec) / TIME_FIXP_SCALING),
                                 0,
                                 ((int)(((double)(tdiff) * pixel_per_sec) / TIME_FIXP_SCALING)) - 2,
                                 h,
                                 QColor(128, 0, 0, 48));
              }
          }

          prev_annot_end = annot_end;
        }

        stage = j;

        k++;

        break;
      }
    }
  }

  if(!param.use_epoch_len)
  {
    if(k)
    {
      painter.drawLine(pos_x1, offset + (stage * pixel_per_unit) + 0.5,
                       w, offset + (stage * pixel_per_unit) + 0.5);
    }
  }
}





























