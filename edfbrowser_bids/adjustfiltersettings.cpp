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



/*

type:

0 : highpass
1 : lowpass
2 : notch
3 : bandpass
4 : bandstop

model:

0 : Butterworth
1 : Chebyshev
2 : Bessel
3 : moving average

0 : resonator

*/

#define FILTERTYPE_HIGHPASS  (0)
#define FILTERTYPE_LOWPASS   (1)
#define FILTERTYPE_NOTCH     (2)
#define FILTERTYPE_BANDPASS  (3)
#define FILTERTYPE_BANDSTOP  (4)

#define FILTERMODEL_BUTTERWORTH    (0)
#define FILTERMODEL_CHEBYSHEV      (1)
#define FILTERMODEL_BESSEL         (2)
#define FILTERMODEL_MOVINGAVERAGE  (3)

#define FILTERBRAND_FIDFILTER      (0)
#define FILTERBRAND_MOVING_AVG     (1)


#include "adjustfiltersettings.h"



AdjustFilterSettings::AdjustFilterSettings(sigcompblck_t *signal_comp, QWidget *w_parent)
{
  char str1_2048[2048]={""};

  signalcomp = signal_comp;

  maincurve = (ViewCurve *)w_parent;

  mainwindow = maincurve->mainwindow;

  filtersettings_dialog = new QDialog(w_parent);

  filtersettings_dialog->setMinimumSize(350 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  strlcpy(str1_2048, "Filter settings ", 2048);
  if(signalcomp->alias[0] != 0)
  {
    strlcat(str1_2048, signalcomp->alias, 2048);
  }
  else
  {
    strlcat(str1_2048, signalcomp->signallabel, 2048);
  }
  filtersettings_dialog->setWindowTitle(str1_2048);
  filtersettings_dialog->setModal(true);
  filtersettings_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  filtersettings_dialog->setSizeGripEnabled(true);

  filterbox = new QComboBox;

  orderbox = new QSpinBox;
  orderbox->setSuffix("th order");
  orderbox->setMinimum(1);
  orderbox->setMaximum(8);
  orderbox->setValue(1);
  orderbox->setSingleStep(1);

  freq1box = new QDoubleSpinBox;
  freq1box->setDecimals(6);
  freq1box->setSuffix(" Hz");
  freq1box->setMinimum(0.0001);
  freq1box->setMaximum(100000.0);
  freq1box->setValue(1.0);
  freq1box->setSingleStep(1.0);

  freq2box = new QDoubleSpinBox;
  freq2box->setDecimals(6);
  freq2box->setSuffix(" Hz");
  freq2box->setMinimum(0.0001);
  freq2box->setMaximum(100000.0);
  freq2box->setValue(2.0);
  freq2box->setSingleStep(1.0);

  stepsizebox = new QComboBox;
  stepsizebox->addItem("0.01Hz");
  stepsizebox->addItem("0.1Hz");
  stepsizebox->addItem("1Hz");
  stepsizebox->addItem("10Hz");
  stepsizebox->addItem("100Hz");
  stepsizebox->setCurrentIndex(2);

  RemoveButton = new QPushButton;
  RemoveButton->setText("Remove");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  flayout = new QFormLayout;
  flayout->addRow("Filter", filterbox);
  flayout->addRow("Order", orderbox);
  flayout->addRow("Frequency", freq1box);
  flayout->addRow("Frequency 2", freq2box);
  flayout->addRow("Stepsize", stepsizebox);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(RemoveButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout1);

  filtersettings_dialog->setLayout(vlayout1);

  filter_cnt = 0;

  loadFilterSettings();

  QObject::connect(freq1box,     SIGNAL(valueChanged(double)),                      this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box,     SIGNAL(valueChanged(double)),                      this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox,     SIGNAL(valueChanged(int)),                         this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(filterbox,    SIGNAL(currentIndexChanged(int)),                  this, SLOT(filterboxchanged(int)));
  QObject::connect(stepsizebox,  SIGNAL(currentIndexChanged(int)),                  this, SLOT(stepsizeboxchanged(int)));
  QObject::connect(RemoveButton, SIGNAL(clicked()),                                 this, SLOT(removeButtonClicked()));
  QObject::connect(CloseButton,  SIGNAL(clicked()),                filtersettings_dialog, SLOT(close()));

  filtersettings_dialog->exec();
}


void AdjustFilterSettings::loadFilterSettings(void)
{
  int i;

  char str1_2048[2048]={""};

  filterbox->clear();

  filter_cnt = 0;

  for(i=0; i<signalcomp->fidfilter_cnt; i++)
  {
    if(filter_cnt >= (MAXFILTERS * 2))
    {
      break;
    }

    type = signalcomp->fidfilter_type[i];

    model = signalcomp->fidfilter_model[i];

    order = signalcomp->fidfilter_order[i];

    ripple = signalcomp->fidfilter_ripple[i];

    frequency1 = signalcomp->fidfilter_freq[i];

    frequency2 = signalcomp->fidfilter_freq2[i];

    if(type == FILTERTYPE_HIGHPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(str1_2048, 2048, "HighPass Butterworth");
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(str1_2048, 2048, "HighPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(str1_2048, 2048, "HighPass Bessel");
      }
    }

    if(type == FILTERTYPE_LOWPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(str1_2048, 2048, "LowPass Butterworth");
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(str1_2048, 2048, "LowPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(str1_2048, 2048, "LowPass Bessel");
      }
    }

    if(type == FILTERTYPE_NOTCH)
    {
      snprintf(str1_2048, 2048, "Notch (resonator)");
    }

    if(type == FILTERTYPE_BANDPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(str1_2048, 2048, "BandPass Butterworth");
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(str1_2048, 2048, "BandPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(str1_2048, 2048, "BandPass Bessel");
      }
    }

    if(type == FILTERTYPE_BANDSTOP)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(str1_2048, 2048, "BandStop Butterworth");
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(str1_2048, 2048, "BandStop Chebyshev %.1fdB ripple", ripple);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(str1_2048, 2048, "BandStop Bessel");
      }
    }

    snprintf(str1_2048 + strlen(str1_2048), 2048 - strlen(str1_2048), " %f", frequency1);

    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      snprintf(str1_2048 + strlen(str1_2048), 2048 - strlen(str1_2048), " - %f", frequency2);
    }

    strlcat(str1_2048, " Hz", 2048);

    remove_trailing_zeros(str1_2048);

    filterbox->addItem(str1_2048);

    brand[filter_cnt++] = FILTERBRAND_FIDFILTER;
  }

  for(i=0; i<signalcomp->ravg_filter_cnt; i++)
  {
    if(filter_cnt >= (MAXFILTERS * 2))
    {
      break;
    }

    type = signalcomp->ravg_filter_type[i];

    size = signalcomp->ravg_filter_size[i];

    if(type == FILTERTYPE_HIGHPASS)
    {
      snprintf(str1_2048, 2048, "Highpass Moving Average %i samples", size);
    }

    if(type == FILTERTYPE_LOWPASS)
    {
      snprintf(str1_2048, 2048, "Lowpass Moving Average %i samples", size);
    }

    filterbox->addItem(str1_2048);

    brand[filter_cnt++] = FILTERBRAND_MOVING_AVG;
  }

  filterboxchanged(filterbox->currentIndex());
}


void AdjustFilterSettings::orderboxvaluechanged(int)
{
  int j;

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::freqbox1valuechanged(double value)
{
  int j;

  if((type == 3) || (type == 4))
  {
    if(freq2box->value() < (value * 1.12))
    {
      QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));

      freq2box->setValue(value * 1.12);

      QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
    }
  }

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::freqbox2valuechanged(double value)
{
  int j;

  if(freq1box->value() > (value * 0.9))
  {
    QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));

    freq1box->setValue(value * 0.9);

    QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  }

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::filterboxchanged(int i)
{
  int n;


  if(i < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[i] == FILTERBRAND_FIDFILTER)
  {
    n = i;

    frequency1 = signalcomp->fidfilter_freq[n];

    frequency2 = signalcomp->fidfilter_freq2[n];

    type = signalcomp->fidfilter_type[n];

    model = signalcomp->fidfilter_model[n];

    order = signalcomp->fidfilter_order[n];

    ripple = signalcomp->fidfilter_ripple[n];

    freq1box->setValue(frequency1);

    ((QLabel *)(flayout->labelForField(orderbox)))->setText("Order");
    ((QLabel *)(flayout->labelForField(freq1box)))->setText("Frequency");
    freq1box->setVisible(true);
    stepsizebox->clear();
    stepsizebox->addItem("0.01Hz");
    stepsizebox->addItem("0.1Hz");
    stepsizebox->addItem("1Hz");
    stepsizebox->addItem("10Hz");
    stepsizebox->addItem("100Hz");
    stepsizebox->setCurrentIndex(2);

    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      freq2box->setValue(frequency2);
      freq2box->setVisible(true);
      ((QLabel *)(flayout->labelForField(freq2box)))->setText("Frequency 2");
      orderbox->setMinimum(2);
      orderbox->setSingleStep(2);
      orderbox->setMaximum(16);
    }
    else
    {
      freq2box->setValue(0.0);
      freq2box->setVisible(false);
      ((QLabel *)(flayout->labelForField(freq2box)))->setText("");
      orderbox->setMinimum(1);
      orderbox->setSingleStep(1);
      orderbox->setMaximum(8);
    }

    if(type == FILTERTYPE_NOTCH)
    {
      orderbox->setPrefix("Q-factor ");
      orderbox->setSuffix("");
      orderbox->setMaximum(100);
      orderbox->setMinimum(3);
    }
    else
    {
      orderbox->setPrefix("");
      orderbox->setSuffix("th order");
    }

    orderbox->setValue(order);
  }

  if(brand[i] == FILTERBRAND_MOVING_AVG)
  {
    n = i - signalcomp->fidfilter_cnt;

    type = signalcomp->ravg_filter_type[n];

    size = signalcomp->ravg_filter_size[n];

    stepsizebox->clear();
    stepsizebox->addItem("1");
    stepsizebox->addItem("10");
    stepsizebox->addItem("100");
    stepsizebox->addItem("1000");
    stepsizebox->setCurrentIndex(0);

    ((QLabel *)(flayout->labelForField(orderbox)))->setText("Size");
    ((QLabel *)(flayout->labelForField(freq1box)))->setText("");
    ((QLabel *)(flayout->labelForField(freq2box)))->setText("");
    freq1box->setVisible(false);
    freq2box->setVisible(false);
    orderbox->setMinimum(2);
    orderbox->setSingleStep(1);
    orderbox->setMaximum(10000);
    orderbox->setPrefix("");
    orderbox->setSuffix(" samples");
    orderbox->setValue(size);
  }

  QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));
}


void AdjustFilterSettings::removeButtonClicked()
{
  int i;

  filter_nr = filterbox->currentIndex();

  if(filter_nr < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[filter_nr] == FILTERBRAND_FIDFILTER)
  {
    free(signalcomp->fidfilter[filter_nr]);
    fid_run_free(signalcomp->fid_run[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf2[filter_nr]);

    for(i=filter_nr; i<(signalcomp->fidfilter_cnt - 1); i++)
    {
      signalcomp->fidfilter[i] = signalcomp->fidfilter[i + 1];
      signalcomp->fid_run[i] = signalcomp->fid_run[i + 1];
      signalcomp->fidbuf[i] = signalcomp->fidbuf[i + 1];
      signalcomp->fidbuf2[i] = signalcomp->fidbuf2[i + 1];
    }

    signalcomp->fidfilter_cnt--;
  }
  else if(brand[filter_nr] == FILTERBRAND_MOVING_AVG)
    {
      filter_nr -= signalcomp->fidfilter_cnt;

      free_ravg_filter(signalcomp->ravg_filter[filter_nr]);

      for(i=filter_nr; i<(signalcomp->ravg_filter_cnt - 1); i++)
      {
        signalcomp->ravg_filter[i] = signalcomp->ravg_filter[i + 1];
      }

      signalcomp->ravg_filter_cnt--;
    }

  loadFilterSettings();

  mainwindow->setup_viewbuf();
}


void AdjustFilterSettings::update_filter()
{
  char *filter_spec=NULL,
      spec_str_1_256[256],
      spec_str_2_256[256],
      *err=NULL;


  filter_nr = filterbox->currentIndex();

  if(filter_nr < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[filter_nr] == FILTERBRAND_FIDFILTER)
  {
    frequency1 = freq1box->value();
    frequency2 = freq2box->value();
    order = orderbox->value();

    if(type < 3)
    {
      if(frequency1 >= signalcomp->edfparam_0->sf_f / 2.0)
      {
        frequency1 = signalcomp->edfparam_0->sf_f / 2.0;

        frequency1 -= freq1box->singleStep();

        freq1box->setValue(frequency1);
      }
    }
    else
    {
      if(frequency2 >= signalcomp->edfparam_0->sf_f / 2.0)
      {
        frequency2 = signalcomp->edfparam_0->sf_f / 2.0;

        frequency2 -= freq2box->singleStep();

        freq2box->setValue(frequency2);
      }

      if(frequency1 >= frequency2 * 0.9)
      {
        frequency1 = frequency2 * 0.9;

        freq1box->setValue(frequency1);
      }
    }

    if(type < 3)
    {
      if(frequency1 >= signalcomp->edfparam_0->sf_f / 2.0)
      {
        QMessageBox::critical(filtersettings_dialog, "Error", "The frequency must be less than: samplerate / 2", QMessageBox::Close);
        return;
      }
    }
    else
    {
      if(frequency2 >= signalcomp->edfparam_0->sf_f / 2.0)
      {
        QMessageBox::critical(filtersettings_dialog, "Error", "The frequency must be less than: samplerate / 2", QMessageBox::Close);
        return;
      }
    }

    spec_str_1_256[0] = 0;

    if(type == FILTERTYPE_HIGHPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1_256, 256, "HpBu%i/%f", order, frequency1);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1_256, 256, "HpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1_256, 256, "HpBe%i/%f", order, frequency1);
      }
    }

    if(type == FILTERTYPE_LOWPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1_256, 256, "LpBu%i/%f", order, frequency1);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1_256, 256, "LpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1_256, 256, "LpBe%i/%f", order, frequency1);
      }
    }

    if(type == FILTERTYPE_NOTCH)
    {
      snprintf(spec_str_1_256, 256, "BsRe/%i/%f", order, frequency1);
    }

    if(type == FILTERTYPE_BANDPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1_256, 256, "BpBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1_256, 256, "BpCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1_256, 256, "BpBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    if(type == FILTERTYPE_BANDSTOP)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1_256, 256, "BsBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1_256, 256, "BsCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1_256, 256, "BsBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    strlcpy(spec_str_2_256, spec_str_1_256, 256);

    filter_spec = spec_str_2_256;

    free(signalcomp->fidfilter[filter_nr]);
    fid_run_free(signalcomp->fid_run[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf2[filter_nr]);

    err = fid_parse(signalcomp->edfparam_0->sf_f, &filter_spec, &signalcomp->fidfilter[filter_nr]);

    if(err != NULL)
    {
      QMessageBox::critical(filtersettings_dialog, "Error", err, QMessageBox::Close);
      free(err);
      return;
    }

    signalcomp->fid_run[filter_nr] = fid_run_new(signalcomp->fidfilter[filter_nr], &signalcomp->fidfuncp[filter_nr]);

    signalcomp->fidbuf[filter_nr] = fid_run_newbuf(signalcomp->fid_run[filter_nr]);
    signalcomp->fidbuf2[filter_nr] = fid_run_newbuf(signalcomp->fid_run[filter_nr]);

    signalcomp->fidfilter_freq[filter_nr] = frequency1;
    signalcomp->fidfilter_freq2[filter_nr] = frequency2;
    signalcomp->fidfilter_order[filter_nr] = order;
  }

  if(brand[filter_nr] == FILTERBRAND_MOVING_AVG)
  {
    filter_nr -= signalcomp->fidfilter_cnt;

    size = orderbox->value();

    free_ravg_filter(signalcomp->ravg_filter[filter_nr]);

    signalcomp->ravg_filter[filter_nr] = create_ravg_filter(type, size);
    if(signalcomp->ravg_filter[filter_nr] == NULL)
    {
      QMessageBox::critical(filtersettings_dialog, "Error", "An internal error occurred while creating a moving average filter.", QMessageBox::Close);
      return;
    }

    signalcomp->ravg_filter_size[filter_nr] = size;
  }

  mainwindow->setup_viewbuf();

  QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));
}


void AdjustFilterSettings::stepsizeboxchanged(int index)
{
  int n;

  n = filterbox->currentIndex();

  if(n < 0)
  {
    return;
  }

  if(brand[n] == FILTERBRAND_FIDFILTER)
  {
    switch(index)
    {
      case 0: freq1box->setSingleStep(0.01);
              freq2box->setSingleStep(0.01);
              break;
      case 1: freq1box->setSingleStep(0.1);
              freq2box->setSingleStep(0.1);
              break;
      case 2: freq1box->setSingleStep(1.0);
              freq2box->setSingleStep(1.0);
              break;
      case 3: freq1box->setSingleStep(10.0);
              freq2box->setSingleStep(10.0);
              break;
      case 4: freq1box->setSingleStep(100.0);
              freq2box->setSingleStep(100.0);
              break;
    }
  }

  if(brand[n] == FILTERBRAND_MOVING_AVG)
  {
    switch(index)
    {
      case 0: orderbox->setSingleStep(1);
              break;
      case 1: orderbox->setSingleStep(10);
              break;
      case 2: orderbox->setSingleStep(100);
              break;
      case 3: orderbox->setSingleStep(1000);
              break;
    }
  }
}


