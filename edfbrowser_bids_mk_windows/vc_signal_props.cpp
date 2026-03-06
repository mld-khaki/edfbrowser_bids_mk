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


void ViewCurve::exec_sidemenu(int signal_nr_intern)
{
  char str_32[32]={""};

  signal_nr = signal_nr_intern;

  sidemenu = new QDialog(this);
  sidemenu->setMinimumSize(250 * w_scaling, 640 * h_scaling);
  sidemenu->setWindowTitle("Signal properties");
  sidemenu->setModal(true);
  sidemenu->setAttribute(Qt::WA_DeleteOnClose, true);

  QFormLayout *flayout = new QFormLayout;

  QLineEdit *SidemenuLabelLineEdit = new QLineEdit;
  SidemenuLabelLineEdit->setText(mainwindow->signalcomp[signal_nr]->signallabel);
  SidemenuLabelLineEdit->setReadOnly(true);
  flayout->addRow("Label", SidemenuLabelLineEdit);

  AliasLineEdit = new QLineEdit;
  AliasLineEdit->setText(mainwindow->signalcomp[signal_nr]->alias);
  AliasLineEdit->setMaxLength(16);
  flayout->addRow("Alias", AliasLineEdit);

  ScaleBox = new QDoubleSpinBox;
  ScaleBox->setDecimals(8);
  ScaleBox->setMaximum(1e6);
  ScaleBox->setMinimum(1e-7);
  if(mainwindow->signalcomp[signal_nr]->edfparam_0->bitvalue < 0.0)
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm * -1.0);
  }
  else
  {
    ScaleBox->setValue(mainwindow->signalcomp[signal_nr]->voltpercm);
  }
  strlcpy(str_32, mainwindow->signalcomp[signal_nr]->physdimension, 32);
  trim_spaces(str_32);
  str_insert_substr(str_32, 0, 32, " ", 0, 1);
  strlcat(str_32, "/cm", 32);
  ScaleBox->setSuffix(str_32);
  flayout->addRow("Amplitude", ScaleBox);

  ScaleBox2 = new QDoubleSpinBox;
  ScaleBox2->setDecimals(8);
  ScaleBox2->setMaximum(1000000.0);
  ScaleBox2->setMinimum(-1000000.0);
  ScaleBox2->setValue(-mainwindow->signalcomp[signal_nr]->screen_offset_pix * mainwindow->y_pixelsizefactor * mainwindow->signalcomp[signal_nr]->voltpercm);
  ScaleBox2->setSuffix(mainwindow->signalcomp[signal_nr]->physdimension);
  strlcpy(str_32, mainwindow->signalcomp[signal_nr]->physdimension, 32);
  trim_spaces(str_32);
  str_insert_substr(str_32, 0, 32, " ", 0, 1);
  ScaleBox2->setSuffix(str_32);
  flayout->addRow("Offset", ScaleBox2);

  sidemenuButton1 = new QPushButton;
  sidemenuButton1->setText("Ruler");

  sidemenuButton2 = new QPushButton;
  sidemenuButton2->setText("Crosshair");

  sidemenuButton3 = new QPushButton;
  sidemenuButton3->setText("Fit to pane");

  sidemenuButton4 = new QPushButton;
  sidemenuButton4->setText("Color");

  sidemenuButton5 = new QPushButton;
  sidemenuButton5->setText("Invert");

  sidemenuButton6 = new QPushButton;
  sidemenuButton6->setText("Spectrum");

  sidemenuButton7 = new QPushButton;
  sidemenuButton7->setText("Z-EEG");

  sidemenuButton8 = new QPushButton;
  sidemenuButton8->setText("Remove filters");

  sidemenuButton9 = new QPushButton;
  sidemenuButton9->setText("Remove signal");

  sidemenuButton10 = new QPushButton;
  sidemenuButton10->setText("Adjust filter");

  sidemenuButton11 = new QPushButton;
  sidemenuButton11->setText("Statistics");

  sidemenuButton12 = new QPushButton;
  sidemenuButton12->setText("QRS detector");
  if(mainwindow->live_stream_active)
  {
    sidemenuButton12->setEnabled(false);
  }

  sidemenuButton13 = new QPushButton;
  sidemenuButton13->setText("Heart Rate");

  sidemenuButton14 = new QPushButton;
  sidemenuButton14->setText("CDSA");
  if(mainwindow->live_stream_active)
  {
    sidemenuButton14->setEnabled(false);
  }

  sidemenuButton15 = new QPushButton;
  sidemenuButton15->setText("aEEG");
  if(mainwindow->live_stream_active)
  {
    sidemenuButton15->setEnabled(false);
  }

  sidemenuButton16 = new QPushButton;
  sidemenuButton16->setText("Close");

  QGridLayout *gr = new QGridLayout;
  gr->addWidget(sidemenuButton1, 0, 1);
  gr->addWidget(sidemenuButton2, 1, 1);
  gr->addWidget(sidemenuButton3, 2, 1);
  gr->addWidget(sidemenuButton4, 3, 1);
  gr->addWidget(sidemenuButton5, 4, 1);
  gr->addWidget(sidemenuButton6, 5, 1);
  gr->addWidget(sidemenuButton7, 6, 1);
  gr->addWidget(sidemenuButton8, 7, 1);
  gr->addWidget(sidemenuButton9, 8, 1);
  gr->addWidget(sidemenuButton10, 9, 1);
  gr->addWidget(sidemenuButton11, 10, 1);
  gr->addWidget(sidemenuButton12, 11, 1);
  gr->addWidget(sidemenuButton13, 12, 1);
  gr->addWidget(sidemenuButton14, 13, 1);
  gr->addWidget(sidemenuButton15, 14, 1);
  gr->addWidget(sidemenuButton16, 15, 1);
  gr->setColumnStretch(0, 1000);
  gr->setColumnStretch(2, 1000);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addLayout(gr);
  vlayout1->addStretch(1000);
  sidemenu->setLayout(vlayout1);

  QObject::connect(ScaleBox,          SIGNAL(valueChanged(double)),     this,     SLOT(ScaleBoxChanged(double)));
  QObject::connect(ScaleBox2,         SIGNAL(valueChanged(double)),     this,     SLOT(ScaleBox2Changed(double)));
  QObject::connect(sidemenuButton1,   SIGNAL(clicked()),                this,     SLOT(RulerButton()));
  QObject::connect(sidemenuButton2,   SIGNAL(clicked()),                this,     SLOT(CrosshairButton()));
  QObject::connect(sidemenuButton3,   SIGNAL(clicked()),                this,     SLOT(FittopaneButton()));
  QObject::connect(sidemenuButton4,   SIGNAL(clicked()),                this,     SLOT(ColorButton()));
  QObject::connect(sidemenuButton5,   SIGNAL(clicked()),                this,     SLOT(signalInvert()));
  QObject::connect(sidemenuButton6,   SIGNAL(clicked()),                this,     SLOT(FreqSpecButton()));
  QObject::connect(sidemenuButton7,   SIGNAL(clicked()),                this,     SLOT(Z_scoringButton()));
  QObject::connect(sidemenuButton8,   SIGNAL(clicked()),                this,     SLOT(RemovefilterButton()));
  QObject::connect(sidemenuButton9,   SIGNAL(clicked()),                this,     SLOT(RemovesignalButton()));
  QObject::connect(sidemenuButton10,  SIGNAL(clicked()),                this,     SLOT(AdjustFilterButton()));
  QObject::connect(sidemenuButton11,  SIGNAL(clicked()),                this,     SLOT(StatisticsButton()));
  QObject::connect(sidemenuButton12,  SIGNAL(clicked()),                this,     SLOT(QRSdetectButton()));
  QObject::connect(sidemenuButton13,  SIGNAL(clicked()),                this,     SLOT(ECGdetectButton()));
  QObject::connect(sidemenuButton14,  SIGNAL(clicked()),                this,     SLOT(cdsa_button()));
  QObject::connect(sidemenuButton15,  SIGNAL(clicked()),                this,     SLOT(aeeg_button()));
  QObject::connect(sidemenuButton16,  SIGNAL(clicked()),                this,     SLOT(sidemenu_close()));
  QObject::connect(AliasLineEdit,     SIGNAL(returnPressed()),          this,     SLOT(sidemenu_close()));

  sidemenu->exec();
}


void ViewCurve::sidemenu_close()
{
  if(AliasLineEdit->isModified() == true)
  {
    strlcpy(mainwindow->signalcomp[signal_nr]->alias, AliasLineEdit->text().toLatin1().data(), 17);

    latin1_to_ascii(mainwindow->signalcomp[signal_nr]->alias, strlen(mainwindow->signalcomp[signal_nr]->alias));

    trim_spaces(mainwindow->signalcomp[signal_nr]->alias);
  }

  sidemenu->close();
}


void ViewCurve::signalInvert()
{
  sidemenu->close();

  if((signal_nr < 0) || (signal_nr >= mainwindow->signalcomps))
  {
    return;
  }

  mainwindow->signalcomp_invert(2, signal_nr);
}


void ViewCurve::ECGdetectButton()
{
  int sense=1;

  char str_32[32]={""};

  sigcompblck_t *newsignalcomp;

  sidemenu->close();

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->sf_f < 199.999)
  {
    QMessageBox::critical(this, "Error", "Sample rate of selected signal is less than 200 Hz!");
    return;
  }

  strlcpy(str_32, mainwindow->signalcomp[signal_nr]->edfparam_0->physdimension, 32);
  trim_spaces(str_32);
  if(!strcmp(str_32, "uV"))
  {
    sense = 1;
  }
  else if(!strcmp(str_32, "mV"))
    {
      sense = 1000;
    }
    else if(!strcmp(str_32, "V"))
      {
        sense = 1000000;
      }
      else
      {
        QMessageBox::critical(this, "Error", "Unknown unit (physical dimension), expected uV or mV or V");
        return;
      }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Z-ratio is active for this signal!");
  }

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Heartrate detection is already active for this signal!");
    return;
  }

  newsignalcomp = mainwindow->create_signalcomp_copy(mainwindow->signalcomp[signal_nr]);
  if(newsignalcomp == NULL)
  {
    return;
  }

  newsignalcomp->ecg_filter =
    create_ecg_filter(newsignalcomp->edfparam_0->sf_f,
                      newsignalcomp->edfparam_0->bitvalue,
                      sense);

  if(newsignalcomp->ecg_filter == NULL)
  {
    QMessageBox::critical(this, "Error", "Could not create the QRS detector.");
    return;
  }

  strlcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel, 512);
  newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
  strlcpy(newsignalcomp->signallabel, "HR", 512);
  newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
  strlcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension, 9);
  strlcpy(newsignalcomp->physdimension, "bpm", 9);
  newsignalcomp->alias[0] = 0;
  newsignalcomp->polarity = 1;

  if(newsignalcomp->edfparam_0->bitvalue < 0.0)
  {
    newsignalcomp->sensitivity = newsignalcomp->edfparam_0->bitvalue / -5.0 / mainwindow->y_pixelsizefactor;

    newsignalcomp->voltpercm = -5;
  }
  else
  {
    newsignalcomp->sensitivity = newsignalcomp->edfparam_0->bitvalue / 5.0 / mainwindow->y_pixelsizefactor;

    newsignalcomp->voltpercm = 5.0;
  }

  newsignalcomp->screen_offset_unit = -55.0;

  newsignalcomp->screen_offset_pix = -newsignalcomp->screen_offset_unit / (mainwindow->y_pixelsizefactor * newsignalcomp->voltpercm);

  mainwindow->setup_viewbuf();
}


void ViewCurve::QRSdetectButton()
{
  char str_32[32]={""};

  sidemenu->close();

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->sf_f < 199.999)
  {
    QMessageBox::critical(this, "Error", "Sample rate of selected signal is less than 200 Hz!");
    return;
  }

  strlcpy(str_32, mainwindow->signalcomp[signal_nr]->edfparam_0->physdimension, 32);
  trim_spaces(str_32);
  if((strcmp(str_32, "uV")) && (strcmp(str_32, "mV")) && (strcmp(str_32, "V")))
  {
    QMessageBox::critical(this, "Error", "Unknown physical dimension (unit), expected uV or mV or V");
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Z-ratio is active for this signal!");
    return;
  }

  UI_QRS_detector ui_qrs_det(mainwindow, mainwindow->signalcomp[signal_nr]);

  mainwindow->enable_hrv_stats_toolbar(mainwindow->ecg_qrs_rpeak_descr, &mainwindow->signalcomp[signal_nr]->edfhdr->annot_list, mainwindow->signalcomp[signal_nr]);
}


void ViewCurve::AdjustFilterButton()
{
  sidemenu->close();

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if((!mainwindow->signalcomp[signal_nr]->filter_cnt) &&
     (!mainwindow->signalcomp[signal_nr]->fidfilter_cnt) &&
     (!mainwindow->signalcomp[signal_nr]->ravg_filter_cnt))
  {
    return;
  }

  AdjustFilterSettings filtersettings(mainwindow->signalcomp[signal_nr], this);
}


void ViewCurve::StatisticsButton()
{
  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  sidemenu->close();

  UI_StatisticWindow show_stats_window(mainwindow->signalcomp[signal_nr], mainwindow->pagetime, mainwindow);
}


void ViewCurve::FreqSpecButton()
{
  int i, j;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(mainwindow->spectrumdialog[i] == NULL)
    {
      mainwindow->spectrumdialog[i] = new UI_FreqSpectrumWindow(mainwindow->signalcomp[signal_nr], mainwindow->viewbuf, mainwindow->spectrumdialog, i, mainwindow);

      break;
    }
  }

  sidemenu->close();

  if(i<MAXSPECTRUMDIALOGS)
  {
    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[signal_nr]->spectr_dialog[j] == 0)
      {
        mainwindow->signalcomp[signal_nr]->spectr_dialog[j] = i + 1;

        break;
      }
    }

    mainwindow->spectrumdialog[i]->SpectrumDialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);

    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->spectrumdialog[j] != NULL)
      {
        mainwindow->spectrumdialog[j]->SpectrumDialog->raise();
      }
    }
    mainwindow->spectrumdialog[i]->SpectrumDialog->activateWindow();
    mainwindow->spectrumdialog[i]->SpectrumDialog->raise();
  }
}


void ViewCurve::cdsa_button()
{
  int i;

  sidemenu->close();

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->sf_int < 30)
  {
    QMessageBox::critical(this, "Error", "Samplefrequency must be at least 30Hz and must be an integer value.");
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfhdr->recording_len_sec < 30)
  {
    QMessageBox::critical(this, "Error", "Recording length must be at least 30 seconds.");
    return;
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(mainwindow->cdsa_dock[i] == NULL)
    {
      UI_cdsa_window wndw(mainwindow, mainwindow->signalcomp[signal_nr], i);

      break;
    }
  }
}


void ViewCurve::aeeg_button()
{
  int i;

  char str_32[32]={""};

  sidemenu->close();

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->sf_int < 100)
  {
    QMessageBox::critical(this, "Error", "Samplefrequency must be at least 100Hz and must be an integer value.");
    return;
  }

  strlcpy(str_32, mainwindow->signalcomp[signal_nr]->edfparam_0->physdimension, 32);
  trim_spaces(str_32);
  if((strcmp(str_32, "uV")) && (strcmp(str_32, "mV")) && (strcmp(str_32, "V")))
  {
    QMessageBox::critical(this, "Error", "Unknown physical dimension (unit), expected uV or mV or V");
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfhdr->recording_len_sec < 30)
  {
    QMessageBox::critical(this, "Error", "Recording length must be at least 30 seconds.");
    return;
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(mainwindow->aeeg_dock[i] == NULL)
    {
      UI_aeeg_window wndw(mainwindow, mainwindow->signalcomp[signal_nr], i);

      break;
    }
  }
}


void ViewCurve::Z_scoringButton()
{
  int i, j;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Heartrate detection is active for this signal!");
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Z-ratio is already active for this signal!");
    return;
  }

  if(mainwindow->annot_editor_active)
  {
    QMessageBox::critical(this, "Error", "Close the annotation editor and try again.");
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->sf_int < 100)
  {
    QMessageBox::critical(this, "Error", "Samplerate must be an integer >= 100Hz");
    return;
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(mainwindow->zscoredialog[i] == NULL)
    {
      mainwindow->zscoredialog[i] = new UI_ZScoreWindow(mainwindow, mainwindow->zscoredialog, i, signal_nr);

      break;
    }
  }

  sidemenu->close();

  if(i<MAXZSCOREDIALOGS)
  {
    for(j=0; j<MAXSPECTRUMDIALOGS; j++)
    {
      if(mainwindow->signalcomp[signal_nr]->zscoredialog[j] == 0)
      {
        mainwindow->signalcomp[signal_nr]->zscoredialog[j] = i + 1;

        break;
      }
    }

    mainwindow->zscoredialog[i]->zscore_dialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);

    for(j=0; j<MAXZSCOREDIALOGS; j++)
    {
      if(mainwindow->zscoredialog[j] != NULL)
      {
        mainwindow->zscoredialog[j]->zscore_dialog->raise();
      }
    }
    mainwindow->zscoredialog[i]->zscore_dialog->activateWindow();
    mainwindow->zscoredialog[i]->zscore_dialog->raise();
  }
}


void ViewCurve::FittopaneButton()
{
  if((signal_nr < 0) || (signal_nr >= mainwindow->signalcomps))
  {
    return;
  }

  mainwindow->fit_signals_to_pane(signal_nr);

  sidemenu->close();
}


void ViewCurve::ColorButton()
{
  int color;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  sidemenu->hide();

  UI_ColorMenuDialog colormenudialog(&color, this);

  if(color < 0)
  {
    sidemenu->close();

    return;
  }

  mainwindow->signalcomp[signal_nr]->color = color;

  update();

  sidemenu->close();
}


void ViewCurve::ScaleBox2Changed(double value)
{
  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  mainwindow->signalcomp[signal_nr]->screen_offset_unit = value;

  mainwindow->signalcomp[signal_nr]->screen_offset_pix = -value / (mainwindow->y_pixelsizefactor * mainwindow->signalcomp[signal_nr]->voltpercm);

  drawCurve_stage_1();
}


void ViewCurve::ScaleBoxChanged(double value)
{
  double original_value;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(mainwindow->signalcomp[signal_nr]->edfparam_0->bitvalue < 0.0)
  {
    value *= -1.0;
  }

  mainwindow->signalcomp[signal_nr]->sensitivity = mainwindow->signalcomp[signal_nr]->edfparam_0->bitvalue / value / mainwindow->y_pixelsizefactor;

  original_value = mainwindow->signalcomp[signal_nr]->voltpercm;

  mainwindow->signalcomp[signal_nr]->voltpercm = value;

  mainwindow->signalcomp[signal_nr]->screen_offset_pix *= (original_value / value);

  mainwindow->signalcomp[signal_nr]->screen_offset_unit *= (original_value / value);

  drawCurve_stage_1();
}


void ViewCurve::RemovefilterButton()
{
  int j;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->filter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->filter_cnt = 0;

  for(j=0; j<mainwindow->signalcomp[signal_nr]->ravg_filter_cnt; j++)
  {
    free_ravg_filter(mainwindow->signalcomp[signal_nr]->ravg_filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->ravg_filter_cnt = 0;

  for(j=0; j<mainwindow->signalcomp[signal_nr]->fir_filter_cnt; j++)
  {
    free_fir_filter(mainwindow->signalcomp[signal_nr]->fir_filter[j]);
  }

  mainwindow->signalcomp[signal_nr]->fir_filter_cnt = 0;

  if(mainwindow->signalcomp[signal_nr]->ecg_filter != NULL)
  {
    free_ecg_filter(mainwindow->signalcomp[signal_nr]->ecg_filter);

    mainwindow->signalcomp[signal_nr]->ecg_filter = NULL;

    strlcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu, 512);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu, 9);
  }

  if(mainwindow->signalcomp[signal_nr]->zratio_filter != NULL)
  {
    free_zratio_filter(mainwindow->signalcomp[signal_nr]->zratio_filter);

    mainwindow->signalcomp[signal_nr]->zratio_filter = NULL;

    strlcpy(mainwindow->signalcomp[signal_nr]->signallabel, mainwindow->signalcomp[signal_nr]->signallabel_bu, 512);
    mainwindow->signalcomp[signal_nr]->signallabellen = mainwindow->signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(mainwindow->signalcomp[signal_nr]->physdimension, mainwindow->signalcomp[signal_nr]->physdimension_bu, 9);
  }

  for(j=0; j<mainwindow->signalcomp[signal_nr]->fidfilter_cnt; j++)
  {
    free(mainwindow->signalcomp[signal_nr]->fidfilter[j]);
    fid_run_free(mainwindow->signalcomp[signal_nr]->fid_run[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf[j]);
    fid_run_freebuf(mainwindow->signalcomp[signal_nr]->fidbuf2[j]);
  }

  mainwindow->signalcomp[signal_nr]->fidfilter_cnt = 0;

  mainwindow->signalcomp[signal_nr]->signallabel[mainwindow->signalcomp[signal_nr]->signallabellen] = 0;

  drawCurve_stage_1();

  sidemenu->close();
}


void ViewCurve::RemovesignalButton()
{
  mainwindow->remove_signalcomp(signal_nr);

  mainwindow->setup_viewbuf();

  sidemenu->close();
}


void ViewCurve::RulerButton()
{
  int i;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if((width() < 300) || (height() < 300))
  {
    return;
  }

  crosshair_1.moving = 0;
  crosshair_2.moving = 0;
  ruler_active = 0;
  ruler_moving = 0;
  use_move_events = 0;
  setMouseTracking(false);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->hasruler = 0;
  }

  ruler_x_position = 200;
  ruler_y_position = 200;
  mainwindow->signalcomp[signal_nr]->hasruler = 1;
  ruler_active = 1;

  update();

  sidemenu->close();
}


void ViewCurve::CrosshairButton()
{
  int i;

  if(signal_nr >= mainwindow->signalcomps)
  {
    return;
  }

  if(!crosshair_1.active)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->hascursor1 = 0;
      mainwindow->signalcomp[i]->hascursor2 = 0;
    }
    crosshair_1.value = 0.0;
    crosshair_2.value = 0.0;
    mainwindow->signalcomp[signal_nr]->hascursor1 = 1;
    use_move_events = 0;
    setMouseTracking(true);
    crosshair_1.active = 1;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    crosshair_1.edf_hdr = mainwindow->signalcomp[signal_nr]->edfhdr;

    crosshair_1.x_position = w * 0.3;
    crosshair_1.y_position = h * 0.7;

    drawCurve_stage_1();
  }
  else
  {
    if(!crosshair_2.active)
    {
      for(i=0; i<mainwindow->signalcomps; i++)
      {
        mainwindow->signalcomp[i]->hascursor2 = 0;
      }
      crosshair_2.value = 0.0;
      mainwindow->signalcomp[signal_nr]->hascursor2 = 1;
      use_move_events = 0;
      setMouseTracking(true);
      crosshair_2.active = 1;
      crosshair_1.moving = 0;
      crosshair_2.moving = 0;
      crosshair_2.edf_hdr = mainwindow->signalcomp[signal_nr]->edfhdr;

      crosshair_2.x_position = w * 0.6;
      crosshair_2.y_position = h * 0.7;

      drawCurve_stage_1();
    }
  }

  sidemenu->close();
}


void ViewCurve::next_crosshair_act_triggered()
{
  int i, n=0;

  if(!mainwindow->signalcomps)
  {
    return;
  }

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->hascursor1)
    {
      n = i;

      break;
    }
  }

  if((!crosshair_1.active) || crosshair_2.active)
  {
    if(!crosshair_1.active)
    {
      n = 0;
    }
    else if(crosshair_2.active)
    {
      n++;

      n %= mainwindow->signalcomps;
    }

    for(i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->hascursor1 = 0;
      mainwindow->signalcomp[i]->hascursor2 = 0;
    }
    crosshair_1.value = 0.0;
    crosshair_2.value = 0.0;
    mainwindow->signalcomp[n]->hascursor1 = 1;
    use_move_events = 0;
    setMouseTracking(true);
    crosshair_1.active = 1;
    crosshair_2.active = 0;
    crosshair_1.moving = 0;
    crosshair_2.moving = 0;
    crosshair_1.edf_hdr = mainwindow->signalcomp[n]->edfhdr;

    crosshair_1.x_position = w * 0.3;
    crosshair_1.y_position = h * 0.7;

    drawCurve_stage_1();
  }
  else
  {
    if(!crosshair_2.active)
    {
      for(i=0; i<mainwindow->signalcomps; i++)
      {
        mainwindow->signalcomp[i]->hascursor2 = 0;
      }
      crosshair_2.value = 0.0;
      mainwindow->signalcomp[n]->hascursor2 = 1;
      use_move_events = 0;
      setMouseTracking(true);
      crosshair_2.active = 1;
      crosshair_1.moving = 0;
      crosshair_2.moving = 0;
      crosshair_2.edf_hdr = mainwindow->signalcomp[n]->edfhdr;

      crosshair_2.x_position = w * 0.6;
      crosshair_2.y_position = h * 0.7;

      drawCurve_stage_1();
    }
  }
}
























