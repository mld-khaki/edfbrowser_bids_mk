/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2024 Teunis van Beelen
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




#include "statistics_dialog.h"


#define STAT_JOB_SRC_SIGNAL 0
#define STAT_JOB_SRC_ECG    1
#define STAT_JOB_SRC_ANNOT  2

/* 200 bpm x 60 min. x 24 hours x 2 days */
#define BEAT_IVAL_LIST_SZ   (576000)




UI_StatisticWindow::UI_StatisticWindow(sigcompblck_t *signalcomp,
                                       long long pagetime,
                                       QWidget *w_parent,
                                       annotlist_t *annot_list,
                                       annotblck_t *annot)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  int i,
      tmp,
//       NN20,
//       pNN20,
//       NN50,
//       pNN50,
      job_src=0,
      err;

  char stat_str_2048[2048]={""},
       label_64[64]={""};

  double *beat_interval_list=NULL;

//   double d_tmp,
//          average_bpm,
//          average_rr,
//          sdnn_bpm,
//          sdnn_rr,
//          *buf_bpm=NULL,
//          rmssd_rr;

  long long l_tmp=0;

  annotblck_t *tmp_annot=NULL;

  StatDialog = new QDialog;
  StatDialog->setWindowTitle("Statistics");
  StatDialog->setModal(true);
  StatDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  StatDialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  StatDialog->setWindowIcon(QIcon(":/images/edf.png"));

  if(signalcomp != NULL)
  {
    if(signalcomp->ecg_filter != NULL)
    {
      job_src = STAT_JOB_SRC_ECG;
    }
    else
    {
      job_src = STAT_JOB_SRC_SIGNAL;
    }
  }
  else
  {
    job_src = STAT_JOB_SRC_ANNOT;

    beat_interval_list = (double *)malloc(sizeof(double) * BEAT_IVAL_LIST_SZ);
    if(beat_interval_list == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
      messagewindow.exec();
      return;
    }
  }

  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    StatDialog->setMinimumSize(600 * mainwindow->w_scaling, 450 * mainwindow->h_scaling);
    StatDialog->setSizeGripEnabled(true);

    startSlider = new QSlider;
    startSlider->setOrientation(Qt::Horizontal);
    startSlider->setMinimum(0);
    startSlider->setMaximum(295);
    startSlider->setValue(0);

    stopSlider = new QSlider;
    stopSlider->setOrientation(Qt::Horizontal);
    stopSlider->setMinimum(5);
    stopSlider->setMaximum(300);
    stopSlider->setValue(300);

    curve1 = new SignalCurve(StatDialog);
    curve1->setSignalColor(Qt::darkGreen);
    curve1->setBackgroundColor(Qt::black);
    curve1->setRasterColor(Qt::gray);
    curve1->setTraceWidth(0);
    if(job_src == STAT_JOB_SRC_ECG)
    {
      curve1->setH_label(signalcomp->physdimension);
    }
    curve1->setLowerLabel("HR (beats/min)");
    curve1->setDashBoardEnabled(false);
    if(job_src == STAT_JOB_SRC_ANNOT)
    {
      strlcpy(stat_str_2048, "Distribution ", 2048);
      strlcat(stat_str_2048, annot->description, 2048);
      curve1->setUpperLabel1(stat_str_2048);
    }
    else
    {
      curve1->setUpperLabel1("Distribution");
    }
    curve1->setFillSurfaceEnabled(true);

    vlayout2_1 = new QVBoxLayout;
    vlayout2_1->setSpacing(20);
    vlayout2_1->addWidget(curve1);
    vlayout2_1->addWidget(startSlider);
    vlayout2_1->addWidget(stopSlider);
  }

  if(job_src == STAT_JOB_SRC_SIGNAL)
  {
    StatDialog->setMinimumSize(300 * mainwindow->w_scaling, 440 * mainwindow->h_scaling);
  }

  Label1 = new QLabel(StatDialog);
  Label1->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  pushButton1 = new QPushButton(StatDialog);
  pushButton1->setText("&Close");

  hlayout1_1_1 = new QHBoxLayout;
  hlayout1_1_1->addWidget(pushButton1);
  hlayout1_1_1->addStretch(100);

  vlayout1_1 = new QVBoxLayout;
  vlayout1_1->setSpacing(20);
  vlayout1_1->addWidget(Label1);
  vlayout1_1->addStretch(100);
  vlayout1_1->addLayout(hlayout1_1_1);

  hlayout1 = new QHBoxLayout;
  hlayout1->addLayout(vlayout1_1, 1);
  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    hlayout1->addLayout(vlayout2_1, 100);
  }

  StatDialog->setLayout(hlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), StatDialog, SLOT(close()));

  for(i=0; i<300; i++)
  {
    bpm_distribution[i] = 0;
  }

  if(job_src == STAT_JOB_SRC_SIGNAL)
  {
    if(signalcomp->alias[0] != 0)
    {
      strlcpy(label_64, signalcomp->alias, 64);
    }
    else
    {
      strlcpy(label_64, signalcomp->signallabel, 64);
    }

    if((signalcomp->stat_cnt < 1) || (pagetime < 10LL))
    {
      snprintf(stat_str_2048, 2048, "Signal:  %s\n\nSamples:   0\n\nSum:       0 %s\n\nMean:      0 %s\n\nRMS:       0 %s\n\nMRS:       0 %s\n\nZero crossings:  0\n\nFrequency:  0 Hz\n\n"
                               "Max. peak:  0 %s\n\nMin. peak:  0 %s\n\nPeak to peak:  0 %s",
              label_64,
              signalcomp->physdimension,
              signalcomp->physdimension,
              signalcomp->physdimension,
              signalcomp->physdimension,
              signalcomp->physdimension,
              signalcomp->physdimension,
              signalcomp->physdimension);
    }
    else
    {
      snprintf(stat_str_2048, 2048, "Signal:  %s\n\nSamples:   %i\n\nSum:       %f %s\n\nMean:      %f %s\n\nRMS:       %f %s\n\nMRS:       %f %s\n\nZero crossings:  %i\n\nFrequency:  %f Hz\n\n"
                               "Max. peak:  %f %s\n\nMin. peak:  %f %s\n\nPeak to peak:  %f %s",
              label_64,
              signalcomp->stat_cnt,
              signalcomp->stat_sum * signalcomp->edfparam_0->bitvalue,
              signalcomp->physdimension,
              (signalcomp->stat_sum * signalcomp->edfparam_0->bitvalue) / signalcomp->stat_cnt,
              signalcomp->physdimension,
              fabs(sqrt(signalcomp->stat_sum_sqr / signalcomp->stat_cnt) * signalcomp->edfparam_0->bitvalue),
              signalcomp->physdimension,
              fabs((signalcomp->stat_sum_rectified / signalcomp->stat_cnt) * signalcomp->edfparam_0->bitvalue),
              signalcomp->physdimension,
              signalcomp->stat_zero_crossing_cnt,
              (((double)signalcomp->stat_zero_crossing_cnt / 2.0)) / ((double)pagetime / (double)TIME_FIXP_SCALING),
              signalcomp->max_dig_value * signalcomp->edfparam_0->bitvalue,
              signalcomp->physdimension,
              signalcomp->min_dig_value * signalcomp->edfparam_0->bitvalue,
              signalcomp->physdimension,
              (signalcomp->max_dig_value - signalcomp->min_dig_value) * signalcomp->edfparam_0->bitvalue,
              signalcomp->physdimension
            );
    }
  }

  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    if(job_src == STAT_JOB_SRC_ECG)
    {
      beat_cnt = ecg_filter_get_beat_cnt(signalcomp->ecg_filter);

      beat_interval_list = ecg_filter_get_interval_beatlist(signalcomp->ecg_filter);
    }

    if(job_src == STAT_JOB_SRC_ANNOT)
    {
      QProgressDialog progress("Processing...", "Abort", 0, 1);
      progress.setWindowModality(Qt::WindowModal);
      progress.setMinimumDuration(200);
      progress.reset();

      progress.setRange(0, edfplus_annotation_size(annot_list));
      progress.setValue(0);

      int p_i=0, p_j=0;

      for(i=0, beat_cnt=0; beat_cnt<BEAT_IVAL_LIST_SZ; i++)
      {
        tmp_annot = edfplus_annotation_get_item_visible_only_cached(annot_list, i, &p_i, &p_j);

        if(tmp_annot == NULL)  break;

        if(!strcmp(tmp_annot->description, annot->description))
        {
          if(beat_cnt)
          {
            beat_interval_list[beat_cnt - 1] = ((double)(tmp_annot->onset - l_tmp)) / (double)TIME_FIXP_SCALING;
          }

          l_tmp = tmp_annot->onset;

          beat_cnt++;
        }

        if(!(i%1000))
        {
          progress.setValue(i);

          qApp->processEvents();

          if(progress.wasCanceled() == true)
          {
            break;
          }
        }
      }

      if(beat_cnt)  beat_cnt--;

      progress.reset();
    }

    if(beat_cnt < 3)
    {
      snprintf(stat_str_2048, 2048, "Not enough beats.");
    }
    else
    {
      err = ecg_get_hr_statistics(beat_interval_list, beat_cnt, &hr_stat);
      if(err)
      {
        snprintf(stat_str_2048, 2048, "Error %i occurred at line %i in file %s.", err, __LINE__, __FILE__);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", stat_str_2048);
        messagewindow.exec();
        return;
      }

//       average_bpm = 0.0;
//       average_rr = 0.0;
//       sdnn_bpm = 0.0;
//       sdnn_rr = 0.0;
//       rmssd_rr = 0.0;
//       NN20 = 0;
//       NN50 = 0;
//
//       buf_bpm = (double *)malloc(sizeof(double) * beat_cnt);
//       if(buf_bpm == NULL)
//       {
//         QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
//         messagewindow.exec();
//         return;
//       }
//
//       for(i=0; i<beat_cnt; i++)
//       {
//         buf_bpm[i] = 60.0 / beat_interval_list[i];
//
//         average_bpm += buf_bpm[i];
//         average_rr += beat_interval_list[i];
//
//         if(i < (beat_cnt - 1))
//         {
//           d_tmp = (beat_interval_list[i] - beat_interval_list[i + 1]) * 1000.0;
//
//           rmssd_rr += (d_tmp * d_tmp);
//
//           if(((beat_interval_list[i] - beat_interval_list[i + 1]) > 0.02 ) || ((beat_interval_list[i + 1] - beat_interval_list[i]) > 0.02 ))
//           {
//             NN20++;
//           }
//
//           if(((beat_interval_list[i] - beat_interval_list[i + 1]) > 0.05 ) || ((beat_interval_list[i + 1] - beat_interval_list[i]) > 0.05 ))
//           {
//             NN50++;
//           }
//         }
//       }
//
//       average_bpm /= beat_cnt;
//       average_rr /= beat_cnt;
//       rmssd_rr /= beat_cnt;
//       rmssd_rr = sqrt(rmssd_rr);
//
//       pNN20 = (NN20 * 100) / (beat_cnt - 1);
//       pNN50 = (NN50 * 100) / (beat_cnt - 1);
//
//       for(i=0; i<beat_cnt; i++)
//       {
//         sdnn_bpm += (buf_bpm[i] - average_bpm) * (buf_bpm[i] - average_bpm);
//         sdnn_rr += (beat_interval_list[i] - average_rr) * (beat_interval_list[i] - average_rr);
//       }
//
//       sdnn_bpm = sqrt(sdnn_bpm / beat_cnt);
//       sdnn_rr = sqrt(sdnn_rr / beat_cnt);
//
//       sprintf(stat_str_2048,
//               "Heart Rate\n\n"
//               "Beats:    %3i\n\n"
//               "Mean RR:  %3i ms\n\n"
//               "SDNN RR:  %3i ms\n\n"
//               "RMSSD RR: %3i ms\n\n"
//               "Mean HR:  %3.3f bpm\n\n"
//               "SDNN HR:  %3.3f bpm\n\n"
//               "NN20:     %3i\n\n"
//               "pNN20:    %3i %%\n\n"
//               "NN50:     %3i\n\n"
//               "pNN50:    %3i %%\n\n",
//               beat_cnt,
//               (int)(average_rr * 1000.0),
//               (int)(sdnn_rr * 1000.0),
//               (int)rmssd_rr,
//               average_bpm,
//               sdnn_bpm,
//               NN20,
//               pNN20,
//               NN50,
//               pNN50);
//
//       free(buf_bpm);

      snprintf(stat_str_2048, 2048,
              "Heart Rate\n\n"
              "Beats:    %3i\n\n"
              "Mean RR:  %3.1f ms\n\n"
              "SDNN RR:  %3.1f ms\n\n"
              "RMSSD RR: %3.1f ms\n\n"
              "Mean HR:  %3.1f bpm\n\n"
              "SDNN HR:  %3.1f bpm\n\n"
              "NN20:     %3i\n\n"
              "pNN20:    %3.1f %%\n\n"
              "NN50:     %3i\n\n"
              "pNN50:    %3.1f %%\n\n",
              hr_stat.beat_cnt,
              hr_stat.mean_rr,
              hr_stat.sdnn_rr,
              hr_stat.rmssd_rr,
              hr_stat.mean_hr,
              hr_stat.sdnn_hr,
              hr_stat.NN20,
              hr_stat.pNN20,
              hr_stat.NN50,
              hr_stat.pNN50);

      for(i=0; i<beat_cnt; i++)
      {
        tmp = 60.0 / beat_interval_list[i];

        if((tmp > 0) && (tmp < 301))
        {
          bpm_distribution[tmp-1]++;
        }
      }

      max_val = 1;

      for(i=0; i<300; i++)
      {
        if(bpm_distribution[i] > max_val)
        {
          max_val = bpm_distribution[i];
        }
      }

      for(i=0; i<300; i++)
      {
        if(bpm_distribution[i] > (max_val / 70))
        {
          start_ruler = i;

          break;
        }
      }

      for(i=299; i>=0; i--)
      {
        if(bpm_distribution[i] > (max_val / 70))
        {
          end_ruler = i + 1;

          if(end_ruler > 300)
          {
            end_ruler = 300;
          }

          break;
        }
      }

      if(start_ruler >= end_ruler)
      {
        start_ruler = 0;

        end_ruler = 300;
      }

      startSlider->setValue(start_ruler);
      stopSlider->setValue(end_ruler);

      curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

      curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

      QObject::connect(startSlider, SIGNAL(valueChanged(int)), this, SLOT(startSliderMoved(int)));
      QObject::connect(stopSlider,  SIGNAL(valueChanged(int)), this, SLOT(stopSliderMoved(int)));
    }
  }

  if(job_src == STAT_JOB_SRC_ANNOT)
  {
    free(beat_interval_list);

    mainwindow->enable_hrv_stats_toolbar(annot->description, annot_list, NULL);
  }

  Label1->setText(stat_str_2048);

  StatDialog->exec();
}


void UI_StatisticWindow::startSliderMoved(int)
{
  startSlider->blockSignals(true);
  stopSlider->blockSignals(true);

  start_ruler = startSlider->value();
  end_ruler = stopSlider->value();

  if(end_ruler < (start_ruler + 5))
  {
    end_ruler = start_ruler + 5;

    stopSlider->setValue(end_ruler);
  }

  curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

  curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

  startSlider->blockSignals(false);
  stopSlider->blockSignals(false);
}


void UI_StatisticWindow::stopSliderMoved(int)
{
  startSlider->blockSignals(true);
  stopSlider->blockSignals(true);

  start_ruler = startSlider->value();
  end_ruler = stopSlider->value();

  if(start_ruler > (end_ruler - 5))
  {
    start_ruler = end_ruler - 5;

    startSlider->setValue(start_ruler);
  }

  curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

  curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

  startSlider->blockSignals(false);
  stopSlider->blockSignals(false);
}




