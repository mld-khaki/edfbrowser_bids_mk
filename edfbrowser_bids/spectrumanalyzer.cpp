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



#include "spectrumanalyzer.h"


#define SPECT_LOG_MINIMUM (1E-13)
#define SPECT_LOG_MINIMUM_LOG (-12)


static const int dftsz_range[24]={200,1000,1024,2048,4096,5000,8192,10000,16384,32768,50000,65536,100000,
                    131072,262144,500000,524288,1000000,1048576,2097152,4194304,5000000,8388608,10000000};



UI_FreqSpectrumWindow::UI_FreqSpectrumWindow(sigcompblck_t *signal_comp, char *view_buf, UI_FreqSpectrumWindow **spectrdialog, int number, QWidget *w_parent)
{
  int i;

  char str_2048[2048]={""};

  long long l_temp;

  fft_data = NULL;

  buf1_fft_input = NULL;
  buf2_psd = NULL;
  buf3_amp = NULL;
  buf4_psd_log = NULL;
  buf5_amp_log = NULL;

  busy = 0;

  first_run = 1;

  fft_inputbufsize = 0;

  spectrumdialog_is_destroyed = 0;

  class_is_deleted = 0;

  flywheel_value = 1050;

  spectrumdialog = spectrdialog;

  spectrumdialognumber = number;

  mainwindow = (UI_Mainwindow *)w_parent;

  viewbuf = view_buf;

  signalcomp = signal_comp;

  dftblocksize = 200;

  window_type = 0;

  overlap = 1;

  for(i=strlen(signalcomp->edfhdr->filename); i>0; i--)
  {
       if((signalcomp->edfhdr->filename[i-1]=='/')||(signalcomp->edfhdr->filename[i-1]=='\\'))  break;
  }

  if(signalcomp->edfhdr->viewtime>=0LL)
  {
    if(signalcomp->alias[0] != 0)
    {
      snprintf(signallabel, 1024, "   %s   %i:%02i:%02i.%04i   %s", signalcomp->alias,
                (int)((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING)/ 3600LL),
                (int)(((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                (int)((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING) % 60LL),
                (int)((signalcomp->edfhdr->viewtime % TIME_FIXP_SCALING) / 1000LL),
                signalcomp->edfhdr->filename + i);
    }
    else
    {
      snprintf(signallabel, 1024, "   %s   %i:%02i:%02i.%04i   %s", signalcomp->signallabel,
                (int)((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING)/ 3600LL),
                (int)(((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING) % 3600LL) / 60LL),
                (int)((signalcomp->edfhdr->viewtime / TIME_FIXP_SCALING) % 60LL),
                (int)((signalcomp->edfhdr->viewtime % TIME_FIXP_SCALING) / 1000LL),
                signalcomp->edfhdr->filename + i);
    }
  }
  else
  {
    l_temp = -signalcomp->edfhdr->viewtime;

    if(signalcomp->alias[0] != 0)
    {
      snprintf(signallabel, 1024, "   %s   -%i:%02i:%02i.%04i   %s", signalcomp->alias,
              (int)((l_temp / TIME_FIXP_SCALING)/ 3600LL),
              (int)(((l_temp / TIME_FIXP_SCALING) % 3600LL) / 60LL),
              (int)((l_temp / TIME_FIXP_SCALING) % 60LL),
              (int)((l_temp % TIME_FIXP_SCALING) / 1000LL),
              signalcomp->edfhdr->filename + i);
    }
    else
    {
      snprintf(signallabel, 1024, "   %s   -%i:%02i:%02i.%04i   %s", signalcomp->signallabel,
              (int)((l_temp / TIME_FIXP_SCALING)/ 3600LL),
              (int)(((l_temp / TIME_FIXP_SCALING) % 3600LL) / 60LL),
              (int)((l_temp / TIME_FIXP_SCALING) % 60LL),
              (int)((l_temp % TIME_FIXP_SCALING) / 1000LL),
              signalcomp->edfhdr->filename + i);
    }
  }

  remove_trailing_zeros(signallabel);

  strlcpy(physdimension, signalcomp->physdimension, 9);

  SpectrumDialog = new QDialog;
  SpectrumDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  SpectrumDialog->setMinimumSize(700 * mainwindow->w_scaling, 500 * mainwindow->h_scaling);
  SpectrumDialog->setSizeGripEnabled(true);
  SpectrumDialog->setModal(false);
  SpectrumDialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  if(mainwindow->spectrum_sqrt)
  {
    snprintf(str_2048, 2048, "Amplitude Spectrum %s", signallabel);
  }
  else
  {
    snprintf(str_2048, 2048, "Power Spectral Density %s", signallabel);
  }
  SpectrumDialog->setWindowTitle(str_2048);
  SpectrumDialog->setWindowIcon(QIcon(":/images/edf.png"));

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::green);
  curve1->setBackgroundColor(Qt::black);
  curve1->setRasterColor(Qt::gray);
  curve1->setTraceWidth(0);
  curve1->setH_label("Hz");
  curve1->setLowerLabel("Frequency");
  if(mainwindow->spectrum_sqrt)
  {
    if(mainwindow->spectrum_vlog)
    {
      snprintf(str_2048, 2048, "log10(%s)", physdimension);

      curve1->setV_label(str_2048);
    }
    else
    {
      curve1->setV_label(physdimension);
    }
  }
  else
  {
    if(mainwindow->spectrum_vlog)
    {
      snprintf(str_2048, 2048, "log10((%s)^2/Hz)", physdimension);
    }
    else
    {
      snprintf(str_2048, 2048, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str_2048);
  }
  curve1->create_button("to Text");
  curve1->setDashBoardEnabled(false);

  flywheel1 = new UI_Flywheel;

  amplitudeSlider = new QSlider;
  amplitudeSlider->setOrientation(Qt::Vertical);
  amplitudeSlider->setMinimum(1);
  amplitudeSlider->setMaximum(2000);
  amplitudeSlider->setValue(1000);
  amplitudeSlider->setInvertedAppearance(true);

  log_minslider = new QSlider;
  log_minslider->setOrientation(Qt::Vertical);
  log_minslider->setMinimum(1);
  log_minslider->setMaximum(2000);
  log_minslider->setValue(1000);
  log_minslider->setInvertedAppearance(false);

  amplitudeLabel = new QLabel;
  amplitudeLabel->setText("Amplitude");
  amplitudeLabel->setAlignment(Qt::AlignHCenter);

  sqrtCheckBox = new QCheckBox("Amplitude");
  sqrtCheckBox->setTristate(false);
  if(mainwindow->spectrum_sqrt)
  {
    sqrtCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    sqrtCheckBox->setCheckState(Qt::Unchecked);
  }

  VlogCheckBox = new QCheckBox("Log");
  VlogCheckBox->setTristate(false);
  if(mainwindow->spectrum_vlog)
  {
    VlogCheckBox->setCheckState(Qt::Checked);

    log_minslider->setVisible(true);
  }
  else
  {
    VlogCheckBox->setCheckState(Qt::Unchecked);

    log_minslider->setVisible(false);
  }

  BWCheckBox = new QCheckBox("B/W");
  BWCheckBox->setTristate(false);
  if(mainwindow->spectrum_bw == 1)
  {
    BWCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    BWCheckBox->setCheckState(Qt::Unchecked);
  }

  dftsz_box = new QComboBox;
  dftsz_box->addItem("Blocksize: user defined");
  dftsz_box->addItem("Blocksize: 1000 smpls");
  dftsz_box->addItem("Blocksize: 1024 smpls");
  dftsz_box->addItem("Blocksize: 2048 smpls");
  dftsz_box->addItem("Blocksize: 4096 smpls");
  dftsz_box->addItem("Blocksize: 5000 smpls");
  dftsz_box->addItem("Blocksize: 8192 smpls");
  dftsz_box->addItem("Blocksize: 10000 smpls");
  dftsz_box->addItem("Blocksize: 16384 smpls");
  dftsz_box->addItem("Blocksize: 32768 smpls");
  dftsz_box->addItem("Blocksize: 50000 smpls");
  dftsz_box->addItem("Blocksize: 65536 smpls");
  dftsz_box->addItem("Blocksize: 100000 smpls");
  dftsz_box->addItem("Blocksize: 131072 smpls");
  dftsz_box->addItem("Blocksize: 262144 smpls");
  dftsz_box->addItem("Blocksize: 500000 smpls");
  dftsz_box->addItem("Blocksize: 524288 smpls");
  dftsz_box->addItem("Blocksize: 1000000 smpls");
  dftsz_box->addItem("Blocksize: 1048576 smpls");
  dftsz_box->addItem("Blocksize: 2097152 smpls");
  dftsz_box->addItem("Blocksize: 4194304 smpls");
  dftsz_box->addItem("Blocksize: 5000000 smpls");
  dftsz_box->addItem("Blocksize: 8388608 smpls");
  dftsz_box->addItem("Blocksize: 10000000 smpls");
  dftsz_box->setCurrentIndex(mainwindow->spectrum_blocksize_predefined);

  windowBox = new QComboBox;
  windowBox->addItem("Window: Rectangular");
  windowBox->addItem("Window: Hamming");
  windowBox->addItem("Window: 4-term Blackman-Harris");
  windowBox->addItem("Window: 7-term Blackman-Harris");
  windowBox->addItem("Window: Nuttall3b");
  windowBox->addItem("Window: Nuttall4c");
  windowBox->addItem("Window: Hann");
  windowBox->addItem("Window: HFT223D");
  windowBox->addItem("Window: HFT95");
  windowBox->addItem("Window: Kaiser2");
  windowBox->addItem("Window: Kaiser3");
  windowBox->addItem("Window: Kaiser4");
  windowBox->addItem("Window: Kaiser5");
  windowBox->setCurrentIndex(mainwindow->spectrum_window);

  window_type = mainwindow->spectrum_window;

  dftsz_spinbox = new QSpinBox;
  dftsz_spinbox->setMinimum(10);
  dftsz_spinbox->setSingleStep(2);
  dftsz_spinbox->setPrefix("Blocksize: ");
  dftsz_spinbox->setSuffix(" smpls");
  if(mainwindow->spectrum_blocksize_predefined)
  {
    dftsz_spinbox->setMaximum(10000000);

    dftsz_spinbox->setValue(dftsz_range[mainwindow->spectrum_blocksize_predefined]);

    dftblocksize = dftsz_range[mainwindow->spectrum_blocksize_predefined];

    dftsz_spinbox->setEnabled(false);
  }
  else
  {
    dftsz_spinbox->setMaximum(1000);

    dftsz_spinbox->setValue(mainwindow->spectrum_blocksize_userdefined);

    dftblocksize = mainwindow->spectrum_blocksize_userdefined;
  }

  overlap_box = new QComboBox;
  overlap_box->addItem("Overlap: 0%");
  overlap_box->addItem("Overlap: 50%");
  overlap_box->addItem("Overlap: 67%");
  overlap_box->addItem("Overlap: 75%");
  overlap_box->addItem("Overlap: 80%");
  overlap_box->setCurrentIndex(mainwindow->spectrum_overlap);

  overlap = mainwindow->spectrum_overlap + 1;

  vlayout3 = new QVBoxLayout;
  vlayout3->addStretch(100);
  vlayout3->addWidget(flywheel1, 200);
  vlayout3->addStretch(100);

  hlayout4 = new QHBoxLayout;
  hlayout4->addStretch(200);
  hlayout4->addLayout(vlayout3, 100);
  hlayout4->addStretch(200);
  hlayout4->addWidget(amplitudeSlider, 300);
  hlayout4->addWidget(log_minslider, 300);

  vlayout2 = new QVBoxLayout;
  vlayout2->setSpacing(10);
  vlayout2->addWidget(amplitudeLabel, 0, Qt::AlignHCenter);
  vlayout2->addLayout(hlayout4, 200);
  vlayout2->addWidget(sqrtCheckBox);
  vlayout2->addWidget(VlogCheckBox);
  vlayout2->addWidget(BWCheckBox);
  vlayout2->addWidget(windowBox);
  vlayout2->addWidget(dftsz_box);
  vlayout2->addWidget(dftsz_spinbox);
  vlayout2->addWidget(overlap_box);

  spanSlider = new QSlider;
  spanSlider->setOrientation(Qt::Horizontal);
  spanSlider->setMinimum(1);
  spanSlider->setMaximum(1000);
  spanSlider->setValue(1000);

  spanLabel = new QLabel;
  spanLabel->setText("Span");
  spanLabel->setAlignment(Qt::AlignHCenter);

  centerSlider = new QSlider;
  centerSlider->setOrientation(Qt::Horizontal);
  centerSlider->setMinimum(0);
  centerSlider->setMaximum(1000);
  centerSlider->setValue(0);

  centerLabel = new QLabel;
  centerLabel->setText("Center");
  centerLabel->setAlignment(Qt::AlignHCenter);

  hlayout1 = new QHBoxLayout;
  hlayout1->setSpacing(20);
  hlayout1->addLayout(vlayout2);
  hlayout1->addWidget(curve1, 1000);

  hlayout2 = new QHBoxLayout;
  hlayout2->setSpacing(20);
  hlayout2->addWidget(spanLabel);
  hlayout2->addWidget(spanSlider, 1000);

  hlayout3 = new QHBoxLayout;
  hlayout3->setSpacing(20);
  hlayout3->addWidget(centerLabel);
  hlayout3->addWidget(centerSlider, 1000);

  vlayout1 = new QVBoxLayout;
  vlayout1->setSpacing(20);
  vlayout1->addLayout(hlayout1);
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(hlayout3);

  SpectrumDialog->setLayout(vlayout1);

  t1 = new QTimer(this);
  t1->setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1->setTimerType(Qt::PreciseTimer);
#endif
  t1->start(10);

  QObject::connect(t1,              SIGNAL(timeout()),                this, SLOT(update_curve()));
  QObject::connect(amplitudeSlider, SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(log_minslider,   SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(spanSlider,      SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(centerSlider,    SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(sqrtCheckBox,    SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(VlogCheckBox,    SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(BWCheckBox,      SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(SpectrumDialog,  SIGNAL(destroyed(QObject *)),     this, SLOT(SpectrumDialogDestroyed(QObject *)));
  QObject::connect(curve1,          SIGNAL(extra_button_clicked()),   this, SLOT(print_to_txt()));
  QObject::connect(flywheel1,       SIGNAL(dialMoved(int)),           this, SLOT(update_flywheel(int)));
  QObject::connect(this,            SIGNAL(finished()),               this, SLOT(thr_finished_func()));
  QObject::connect(dftsz_spinbox,   SIGNAL(valueChanged(int)),        this, SLOT(dftsz_value_changed(int)));
  QObject::connect(windowBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(windowBox_changed(int)));
  QObject::connect(dftsz_box,       SIGNAL(currentIndexChanged(int)), this, SLOT(dftsz_box_changed(int)));
  QObject::connect(overlap_box,     SIGNAL(currentIndexChanged(int)), this, SLOT(overlap_box_changed(int)));

  SpectrumDialog->show();
}


void UI_FreqSpectrumWindow::windowBox_changed(int idx)
{
  if(busy)  return;

  if(window_type == idx)  return;

  mainwindow->spectrum_window = idx;

  window_type = idx;

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::dftsz_value_changed(int new_val)
{
  if(busy)  return;

  if(dftblocksize == new_val)  return;

  mainwindow->spectrum_blocksize_userdefined = new_val;

  dftblocksize = new_val;

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::dftsz_box_changed(int idx)
{
  mainwindow->spectrum_blocksize_predefined = idx;

  if(idx)
  {
    if(dftsz_range[idx] > samples)
    {
      dftsz_box->setCurrentIndex(0);

      return;
    }

    dftsz_spinbox->setMaximum(10000000);

    dftsz_spinbox->setValue(dftsz_range[idx]);

    dftsz_spinbox->setEnabled(false);
  }
  else
  {
    dftsz_spinbox->setEnabled(true);

    dftsz_spinbox->setValue(dftsz_range[idx]);

    if(samples < 1000)
    {
      dftsz_spinbox->setMaximum(samples);
    }
    else
    {
      dftsz_spinbox->setMaximum(1000);
    }
  }
}


void UI_FreqSpectrumWindow::overlap_box_changed(int idx)
{
  if(busy)  return;

  if(overlap == (idx + 1))  return;

  mainwindow->spectrum_overlap = idx;

  overlap = idx + 1;

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::update_flywheel(int new_value)
{
  flywheel_value += new_value;

  if(flywheel_value < 10)
  {
    flywheel_value = 10;
  }

  if(flywheel_value > 100000)
  {
    flywheel_value = 100000;
  }

  sliderMoved(0);
}


void UI_FreqSpectrumWindow::print_to_txt()
{
  int i;

  char str_1024[1024]={""},
       path[MAX_PATH_LENGTH]={""};

  FILE *outputfile;

  if(fft_data == NULL)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
    strlcat(path, "/", MAX_PATH_LENGTH);
  }
  strlcat(path, "spectrum.txt", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Export to text (ASCII)", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopen(path, "wb");
  if(outputfile == NULL)
  {
    snprintf(str_1024, 1024, "Cannot open file %.990s for writing.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str_1024));
    messagewindow.exec();
    return;
  }

  fprintf(outputfile, "FFT Power Spectral Density (Power/%fHz)\n", freqstep);
  if(signalcomp->alias[0] != 0)
  {
    fprintf(outputfile, "Signal: %s\n", signalcomp->alias);
  }
  else
  {
    fprintf(outputfile, "Signal: %s\n", signalcomp->signallabel);
  }
  fprintf(outputfile, "FFT blocksize: %i\n", fft_data->dft_sz);
  switch(fft_data->wndw_type)
  {
    case FFT_WNDW_TYPE_RECT                  : fprintf(outputfile, "FFT window function: None\n");
            break;
    case FFT_WNDW_TYPE_HAMMING               : fprintf(outputfile, "FFT window function: Hamming\n");
            break;
    case FFT_WNDW_TYPE_4TERM_BLACKMANHARRIS  : fprintf(outputfile, "FFT window function: 4-term Blackman-Harris\n");
            break;
    case FFT_WNDW_TYPE_7TERM_BLACKMANHARRIS  : fprintf(outputfile, "FFT window function: 7-term Blackman-Harris\n");
            break;
    case FFT_WNDW_TYPE_NUTTALL3B             : fprintf(outputfile, "FFT window function: Nuttall3b\n");
            break;
    case FFT_WNDW_TYPE_NUTTALL4C             : fprintf(outputfile, "FFT window function: Nuttall4c\n");
            break;
    case FFT_WNDW_TYPE_HANN                  : fprintf(outputfile, "FFT window function: Hann\n");
            break;
    case FFT_WNDW_TYPE_HFT223D               : fprintf(outputfile, "FFT window function: HFT223D\n");
            break;
    case FFT_WNDW_TYPE_HFT95                 : fprintf(outputfile, "FFT window function: HFT95\n");
            break;
    case FFT_WNDW_TYPE_KAISER_A2             : fprintf(outputfile, "FFT window function: Kaiser2\n");
            break;
    case FFT_WNDW_TYPE_KAISER_A3             : fprintf(outputfile, "FFT window function: Kaiser3\n");
            break;
    case FFT_WNDW_TYPE_KAISER_A4             : fprintf(outputfile, "FFT window function: Kaiser4\n");
            break;
    case FFT_WNDW_TYPE_KAISER_A5             : fprintf(outputfile, "FFT window function: Kaiser5\n");
            break;
  }
  switch(overlap)
  {
    case 1 : fprintf(outputfile, "Overlap: 0 %%\n");
             break;
    case 2 : fprintf(outputfile, "Overlap: 50 %%\n");
             break;
    case 3 : fprintf(outputfile, "Overlap: 67 %%\n");
             break;
    case 4 : fprintf(outputfile, "Overlap: 75 %%\n");
             break;
    case 5 : fprintf(outputfile, "Overlap: 80 %%\n");
             break;
  }
  fprintf(outputfile, "FFT resolution: %f Hz\n", freqstep);
  fprintf(outputfile, "Data Samples: %i\n", fft_data->sz_in);
  fprintf(outputfile, "Power Samples: %i\n", fft_data->sz_out);
  fprintf(outputfile, "Samplefrequency: %f Hz\n", signalcomp->edfparam_0->sf_f);

  for(i=0; i<fft_data->sz_out; i++)
  {
//    fprintf(outputfile, "%.16f\t%.16f\n", freqstep * i, buf2_psd[i]);
    fprintf(outputfile, "%e\t%e\n", freqstep * i, buf2_psd[i]);
  }

  fclose (outputfile);
}


void UI_FreqSpectrumWindow::sliderMoved(int)
{
  long long startstep,
            stopstep,
            spanstep;

  double max_freq,
         start_freq;

  char str_2048[2048]={""};

  if(fft_data == NULL)  return;

  if(VlogCheckBox->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_vlog = 1;

    log_minslider->setVisible(true);
  }
  else
  {
    mainwindow->spectrum_vlog = 0;

    log_minslider->setVisible(false);
  }

  if(sqrtCheckBox->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_sqrt = 1;

    snprintf(str_2048, 2048, "Amplitude Spectrum %s", signallabel);

    SpectrumDialog->setWindowTitle(str_2048);

    if(mainwindow->spectrum_vlog)
    {
      snprintf(str_2048, 2048, "log10(%s)", physdimension);

      curve1->setV_label(str_2048);
    }
    else
    {
      curve1->setV_label(physdimension);
    }
  }
  else
  {
    mainwindow->spectrum_sqrt = 0;

    snprintf(str_2048, 2048, "Power Spectral Density %s", signallabel);

    SpectrumDialog->setWindowTitle(str_2048);

    if(mainwindow->spectrum_vlog)
    {
      snprintf(str_2048, 2048, "log10((%s)^2/Hz)", physdimension);
    }
    else
    {
      snprintf(str_2048, 2048, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str_2048);
  }

  if(BWCheckBox->isChecked() == true)
  {
    curve1->setSignalColor(Qt::black);
    curve1->setBackgroundColor(Qt::white);
    curve1->setRasterColor(Qt::black);
    curve1->setMarker1Color(Qt::black);
    curve1->setBorderColor(Qt::white);
    curve1->setTextColor(Qt::black);
    curve1->setCrosshairColor(Qt::black);

    mainwindow->spectrum_bw = 1;
  }
  else
  {
    curve1->setSignalColor(Qt::green);
    curve1->setBackgroundColor(Qt::black);
    curve1->setRasterColor(Qt::gray);
    curve1->setMarker1Color(Qt::yellow);
    curve1->setBorderColor(Qt::lightGray);
    curve1->setTextColor(Qt::black);
    curve1->setCrosshairColor(Qt::red);

    mainwindow->spectrum_bw = 0;
  }

  spanstep = (long long)spanSlider->value() * (long long)fft_data->sz_out / 1000LL;

  startstep = (long long)centerSlider->value() * ((long long)fft_data->sz_out - spanstep) / 1000LL;

  stopstep = startstep + spanstep;

  if(mainwindow->spectrum_sqrt)
  {
    if(mainwindow->spectrum_vlog)  /* amplitude & log both checked */
    {
//       printf("maxvalue_sqrt_vlog:        %f\n"
//              "flywheel_value:            %i\n"
//              "amplitudeSlider->value():  %i\n"
//              "minvalue_sqrt_vlog:        %f\n"
//              "log_minslider->value():    %i\n",
//              maxvalue_sqrt_vlog, flywheel_value, amplitudeSlider->value(), minvalue_sqrt_vlog, log_minslider->value());

      curve1->drawCurve(buf5_amp_log + startstep, stopstep - startstep, (maxvalue_sqrt_vlog * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, minvalue_sqrt_vlog * (double)log_minslider->value() / 1000.0);
    }
    else  /* only amplitude checked */
    {
      curve1->drawCurve(buf3_amp + startstep, stopstep - startstep, (maxvalue_sqrt * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, 0.0);
    }
  }
  else
  {
    if(mainwindow->spectrum_vlog)  /* only log checked (power spectral density) */
    {
      curve1->drawCurve(buf4_psd_log + startstep, stopstep - startstep, (maxvalue_vlog * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, minvalue_vlog * (double)log_minslider->value() / 1000.0);
    }
    else  /* amplitude & log both not checked (power spectral density) */
    {
      curve1->drawCurve(buf2_psd + startstep, stopstep - startstep, (maxvalue * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, 0.0);
    }
  }

  max_freq = ((double)samplefreq / 2.0) * stopstep / fft_data->sz_out;

  start_freq = ((double)samplefreq / 2.0) * startstep / fft_data->sz_out;

//   printf("steps: %i\n"
//          "startstep: %lli\n"
//          "stopstep: %lli\n"
//          "spanstep: %lli\n"
//          "samplefreq: %f\n"
//          "max_freq: %f\n"
//          "start_freq: %f\n",
//          fft_data->sz_out,
//          startstep,
//          stopstep,
//          spanstep,
//          samplefreq,
//          max_freq,
//          start_freq);

  curve1->setH_RulerValues(start_freq, max_freq);

  strlcpy(str_2048, "Center ", 2048);
  convert_to_metric_suffix(str_2048 + strlen(str_2048), start_freq + ((max_freq - start_freq) / 2.0), 3, 2048 - strlen(str_2048));
//  remove_trailing_zeros(str_2048);
  strlcat(str_2048, "Hz", 2048);
  centerLabel->setText(str_2048);

  strlcpy(str_2048, "Span ", 2048);
  convert_to_metric_suffix(str_2048 + strlen(str_2048), max_freq - start_freq, 3, 2048 - strlen(str_2048));
//  remove_trailing_zeros(str_2048);
  strlcat(str_2048, "Hz", 2048);
  spanLabel->setText(str_2048);
}


void UI_FreqSpectrumWindow::run()
{
  int i, j, k, tmp;

  unsigned long long s, s2;

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  if((first_run) || (buf1_fft_input == NULL))
  {
    first_run = 0;

    fft_inputbufsize = samples;

    free(buf1_fft_input);
    buf1_fft_input = (double *)malloc(sizeof(double) * fft_inputbufsize + 16);
    if(buf1_fft_input == NULL)
    {
      malloc_err = 1;
      return;
    }

    samples = 0;

    for(s=signalcomp->sample_start; s<signalcomp->samples_on_screen; s++)
    {
      if(s>=signalcomp->sample_stop)  break;

      dig_value = 0.0;
      s2 = s + signalcomp->sample_timeoffset - signalcomp->sample_start;

      for(j=0; j<signalcomp->num_of_signals; j++)
      {
        if(signalcomp->edfhdr->bdf)
        {
          var.two[0] = *((unsigned short *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset
            + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)));

          var.four[2] = *((unsigned char *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset
            + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)
            + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          if(var.one_signed > signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max)
          {
            var.one_signed = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max;
          }
          else if(var.one_signed < signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min)
            {
              var.one_signed = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min;
            }

          f_tmp = var.one_signed;
        }

        if(signalcomp->edfhdr->edf)
        {
          tmp = *(((short *)(
            viewbuf
            + signalcomp->viewbufoffset
            + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
            + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].datrec_offset))
            + (s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record));

          if(tmp > signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max)
          {
            tmp = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_max;
          }
          else if(tmp < signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min)
            {
              tmp = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].dig_min;
            }

          f_tmp = tmp;
        }

        f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
        f_tmp *= signalcomp->factor[j];

        dig_value += f_tmp;
      }

      if(signalcomp->spike_filter)
      {
        if(s==signalcomp->sample_start)
        {
          spike_filter_restore_buf(signalcomp->spike_filter);
        }

        dig_value = run_spike_filter(dig_value, signalcomp->spike_filter);
      }

      for(k=0; k<signalcomp->math_func_cnt_before; k++)
      {
        dig_value = run_math_func(dig_value, signalcomp->math_func_before[k]);
      }

      for(k=0; k<signalcomp->filter_cnt; k++)
      {
        dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
      }

      for(k=0; k<signalcomp->ravg_filter_cnt; k++)
      {
        if(s==signalcomp->sample_start)
        {
          ravg_filter_restore_buf(signalcomp->ravg_filter[k]);
        }

        dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
      }

      for(k=0; k<signalcomp->fidfilter_cnt; k++)
      {
        if(s==signalcomp->sample_start)
        {
          memcpy(signalcomp->fidbuf[k], signalcomp->fidbuf2[k], fid_run_bufsize(signalcomp->fid_run[k]));
        }

        dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
      }

      for(k=0; k<signalcomp->fir_filter_cnt; k++)
      {
        if(s==signalcomp->sample_start)
        {
          fir_filter_restore_buf(signalcomp->fir_filter[k]);
        }

        dig_value = run_fir_filter(dig_value, signalcomp->fir_filter[k]);
      }

      for(k=0; k<signalcomp->math_func_cnt_after; k++)
      {
        dig_value = run_math_func(dig_value, signalcomp->math_func_after[k]);
      }

      if(signalcomp->plif_ecg_filter)
      {
        if(s==signalcomp->sample_start)
        {
          if(signalcomp->edfhdr->viewtime<=0)
          {
            plif_ecg_reset_subtract_filter(signalcomp->plif_ecg_filter, 0);
          }
          else
          {
            plif_ecg_subtract_filter_state_copy(signalcomp->plif_ecg_filter, signalcomp->plif_ecg_filter_sav);
          }
        }

        dig_value = plif_ecg_run_subtract_filter(dig_value, signalcomp->plif_ecg_filter);
      }

      if(signalcomp->plif_eeg_filter)
      {
        if(s==signalcomp->sample_start)
        {
          if(signalcomp->edfhdr->viewtime<=0)
          {
            plif_eeg_reset_subtract_filter(signalcomp->plif_eeg_filter);
          }
          else
          {
            plif_eeg_subtract_filter_state_copy(signalcomp->plif_eeg_filter, signalcomp->plif_eeg_filter_sav);
          }
        }

        dig_value = plif_eeg_run_subtract_filter(dig_value, signalcomp->plif_eeg_filter);
      }

      if(signalcomp->ecg_filter != NULL)
      {
        if(s==signalcomp->sample_start)
        {
          ecg_filter_restore_buf(signalcomp->ecg_filter);
        }

        dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
      }

      if(s>=signalcomp->sample_start)
      {
        buf1_fft_input[samples++] = dig_value * signalcomp->edfparam_0->bitvalue;
      }
    }

    if(samples > fft_inputbufsize)
    {
      malloc_err = 2;
      return;
    }

    samplefreq = signalcomp->edfparam_0->sf_f;

    if(dftblocksize > samples)
    {
      dftblocksize = samples;
    }

    if(dftblocksize & 1)
    {
      dftblocksize--;
    }
  }  // end of first_run

  free_fft_wrap(fft_data);
  fft_data = fft_wrap_create(buf1_fft_input, fft_inputbufsize, dftblocksize, window_type, overlap);
  if(fft_data == NULL)
  {
//     printf("buf1_fft_input: %p   fft_inputbufsize: %i   dftblocksize: %i   window_type: %i\n",
//            buf1_fft_input, fft_inputbufsize, dftblocksize, window_type);

    malloc_err = 3;
    free(buf1_fft_input);
    buf1_fft_input = NULL;
    return;
  }

  freqstep = samplefreq / (double)fft_data->dft_sz;

  free(buf2_psd);
  buf2_psd = (double *)calloc(1, sizeof(double) * fft_data->sz_out);
  if(buf2_psd == NULL)
  {
    malloc_err = 4;
    free(buf1_fft_input);
    buf1_fft_input = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf3_amp);
  buf3_amp = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf3_amp == NULL)
  {
    malloc_err = 5;
    free(buf1_fft_input);
    free(buf2_psd);
    buf1_fft_input = NULL;
    buf2_psd = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf4_psd_log);
  buf4_psd_log = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf4_psd_log == NULL)
  {
    malloc_err = 6;
    free(buf1_fft_input);
    free(buf2_psd);
    free(buf3_amp);
    buf1_fft_input = NULL;
    buf2_psd = NULL;
    buf3_amp = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf5_amp_log);
  buf5_amp_log = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf5_amp_log == NULL)
  {
    malloc_err = 7;
    free(buf1_fft_input);
    free(buf2_psd);
    free(buf3_amp);
    free(buf4_psd_log);
    buf1_fft_input = NULL;
    buf2_psd = NULL;
    buf3_amp = NULL;
    buf4_psd_log = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  maxvalue = 0.000001;
  maxvalue_sqrt = 0.000001;
  maxvalue_vlog = -50;
  maxvalue_sqrt_vlog = -50;
  minvalue_vlog = 0.0;
  minvalue_sqrt_vlog = 0.0;

#ifdef CHECK_POWERSPECTRUM
  printf("samples is %i   dftblocksize is %i   dftblocks is %i    samplesleft is %i   fft_outputbufsize is %i\n", samples, dftblocksize, fft_data->blocks, fft_data->smpls_left, fft_data->sz_out);

  double power1=0.0, power2=0.0;

  for(i=0; i<samples; i++)
  {
    power1 += (buf1_fft_input[i] * buf1_fft_input[i]);
  }
#endif

  fft_wrap_run(fft_data);

  if(signalcomp->ecg_filter == NULL)
  {
    fft_data->buf_out[0] /= 2.0;  // DC!
  }
  else
  {
    fft_data->buf_out[0] = 0.0;  // Remove DC because heart rate is always a positive value
  }

  for(i=0; i<fft_data->sz_out; i++)
  {
    buf2_psd[i] = fft_data->buf_out[i] / samplefreq;  /* amplitude & log both not checked (power spectral density) */

#ifdef CHECK_POWERSPECTRUM
    power2 += buf2_psd[i];
#endif

    buf3_amp[i] = sqrt(buf2_psd[i] * freqstep);  /* only amplitude checked */

    if(buf2_psd[i] <= SPECT_LOG_MINIMUM)
    {
      buf4_psd_log[i] = log10(SPECT_LOG_MINIMUM);
    }
    else
    {
      buf4_psd_log[i] = log10(buf2_psd[i]);  /* only log checked (power spectral density) */
    }

    if(buf3_amp[i] <= SPECT_LOG_MINIMUM)
    {
      buf5_amp_log[i] = log10(SPECT_LOG_MINIMUM);
    }
    else
    {
      buf5_amp_log[i] = log10(buf3_amp[i]);  /* amplitude & log both checked */
    }

    if(i)  // don't use the dc-bin for the autogain of the screen
    {
      if(buf2_psd[i] > maxvalue)
      {
        maxvalue = buf2_psd[i];
      }

      if(buf3_amp[i] > maxvalue_sqrt)
      {
        maxvalue_sqrt = buf3_amp[i];
      }

      if(buf4_psd_log[i] > maxvalue_vlog)
      {
        maxvalue_vlog = buf4_psd_log[i];
      }

      if(buf5_amp_log[i] > maxvalue_sqrt_vlog)
      {
        maxvalue_sqrt_vlog = buf5_amp_log[i];
      }

      if((buf4_psd_log[i] < minvalue_vlog) && (buf4_psd_log[i] >= SPECT_LOG_MINIMUM_LOG))
      {
        minvalue_vlog = buf4_psd_log[i];
      }

      if((buf5_amp_log[i] < minvalue_sqrt_vlog) && (buf5_amp_log[i] >= SPECT_LOG_MINIMUM_LOG))
      {
        minvalue_sqrt_vlog = buf5_amp_log[i];
      }
    }
  }

  if(minvalue_vlog < SPECT_LOG_MINIMUM_LOG)
    minvalue_vlog = SPECT_LOG_MINIMUM_LOG;

  if(minvalue_sqrt_vlog < SPECT_LOG_MINIMUM_LOG)
    minvalue_sqrt_vlog = SPECT_LOG_MINIMUM_LOG;

  if((maxvalue_sqrt_vlog < 2) && (maxvalue_sqrt_vlog > -2))
  {
    maxvalue_sqrt_vlog = 2;
  }

#ifdef CHECK_POWERSPECTRUM
  power1 /= samples;
  power2 *= freqstep;

  printf("\n power1 is %f\n power2 is %f\n\n", power1, power2);
#endif
}


void UI_FreqSpectrumWindow::update_curve()
{
  int i;

  if(signalcomp == NULL)
  {
    return;
  }

  if(busy)
  {
    return;
  }

  viewbuf = mainwindow->viewbuf;

  if(viewbuf == NULL)
  {
    return;
  }

  samples = signalcomp->samples_on_screen;

  if(signalcomp->samples_on_screen > signalcomp->sample_stop)
  {
    samples = signalcomp->sample_stop;
  }

  samples -= signalcomp->sample_start;

  if((samples < 10) || (viewbuf == NULL))
  {
    curve1->clear();

    return;
  }

  busy = 1;

  for(i=23; i>0; i--)
  {
    if(samples < dftsz_range[i])
    {
      dftsz_box->removeItem(i);
    }
  }

  if(samples < 1024)
  {
    if(samples < 1000)
    {
      dftsz_spinbox->setMaximum(samples);
    }
    else
    {
      dftsz_spinbox->setMaximum(1000);
    }

//    dftsz_box->setEnabled(false);
  }

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::thr_finished_func()
{
  char str_4096[4096];

  if(spectrumdialog_is_destroyed)
  {
    return;
  }

  if(malloc_err)
  {
    QApplication::restoreOverrideCursor();

    snprintf(str_4096, 4096,
            "The system was not able to provide enough resources (memory) to perform the requested action. (%i)",
            malloc_err);

    QMessageBox messagewindow(QMessageBox::Critical, "Error", str_4096);
    messagewindow.exec();

    curve1->clear();

    busy = 0;

    return;
  }

  strlcpy(str_4096, "FFT resolution: ", 4096);
  convert_to_metric_suffix(str_4096 + strlen(str_4096), freqstep, 3, 4096 - strlen(str_4096));
  remove_trailing_zeros(str_4096);
  snprintf(str_4096 + strlen(str_4096), 4096 - strlen(str_4096), "Hz   %i blocks of %i samples", fft_data->blocks_processed, fft_data->dft_sz);

  curve1->setUpperLabel1(str_4096);

  curve1->setUpperLabel2(signallabel);

  sliderMoved(0);

  curve1->setUpdatesEnabled(true);

  busy = 0;

  QApplication::restoreOverrideCursor();
}


void UI_FreqSpectrumWindow::SpectrumDialogDestroyed(QObject *)
{
  spectrumdialog_is_destroyed = 1;

  QObject::disconnect(this, SIGNAL(finished()), this, SLOT(thr_finished_func()));

  if(isRunning())
  {
    wait(ULONG_MAX);

    QApplication::restoreOverrideCursor();
  }

  spectrumdialog[spectrumdialognumber] = NULL;

  if(!class_is_deleted)
  {
    delete this;
  }
}


UI_FreqSpectrumWindow::~UI_FreqSpectrumWindow()
{
  int i;

  class_is_deleted = 1;

  if(!spectrumdialog_is_destroyed)
  {
    SpectrumDialog->close();
  }

  free(buf1_fft_input);
  free(buf2_psd);
  free(buf3_amp);
  free(buf4_psd_log);
  free(buf5_amp_log);
  buf1_fft_input = NULL;
  buf2_psd = NULL;
  buf3_amp = NULL;
  buf4_psd_log = NULL;
  buf5_amp_log = NULL;

  free_fft_wrap(fft_data);
  fft_data = NULL;

  spectrumdialog[spectrumdialognumber] = NULL;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(signalcomp->spectr_dialog[i] == spectrumdialognumber + 1)
    {
      signalcomp->spectr_dialog[i] = 0;

      break;
    }
  }
}



















