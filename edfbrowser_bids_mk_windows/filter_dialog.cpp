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


#define FILTERTYPE_HIGHPASS  (0)
#define FILTERTYPE_LOWPASS   (1)
#define FILTERTYPE_NOTCH     (2)
#define FILTERTYPE_BANDPASS  (3)
#define FILTERTYPE_BANDSTOP  (4)

#define FILTERMODEL_BUTTERWORTH    (0)
#define FILTERMODEL_RESONATOR      (0)
#define FILTERMODEL_CHEBYSHEV      (1)
#define FILTERMODEL_BESSEL         (2)
#define FILTERMODEL_MOVINGAVERAGE  (3)

#define FILT_RESP_ARR_SZ     (1000)


#include "filter_dialog.h"


UI_FilterDialog::UI_FilterDialog(QWidget *w_parent)
{
  int i;

  char str1_256[256]="";

  filter_spec = NULL;

  QListWidgetItem *item;

  last_order = 1;

  last_samples = 16;

  last_qfactor = 20;

  last_model = 0;

  last_ripple = -1.0;

  mainwindow = (UI_Mainwindow *)w_parent;

  array     = (double *)malloc(sizeof(double) * FILT_RESP_ARR_SZ);
  array_pha = (double *)malloc(sizeof(double) * FILT_RESP_ARR_SZ);

  filterdialog = new QDialog;
  filterdialog->setMinimumSize(620 * mainwindow->w_scaling, 365 * mainwindow->h_scaling);
  filterdialog->setWindowTitle("Add a filter");
  filterdialog->setModal(true);
  filterdialog->setAttribute(Qt::WA_DeleteOnClose, true);
  filterdialog->setSizeGripEnabled(true);

  typeboxlabel = new QLabel;
  typeboxlabel->setText("Type");

  freqbox1label = new QLabel;
  freqbox1label->setText("Frequency");

  freqbox2label = new QLabel;
  freqbox2label->setText("Frequency 2");
  freqbox2label->setVisible(false);

  orderboxlabel = new QLabel;
  orderboxlabel->setText("Order");

  modelboxlabel = new QLabel;
  modelboxlabel->setText("Model");

  orderlabel = new QLabel;
  orderlabel->setText("Slope roll-off:");

  ordervaluelabel = new QLabel;
  ordervaluelabel->setText("6 dB / octave");

  typebox = new QComboBox;
  typebox->addItem("Highpass");
  typebox->addItem("Lowpass");
  typebox->addItem("Notch");
  typebox->addItem("Bandpass");
  typebox->addItem("Bandstop");

  freq1box = new QDoubleSpinBox;
  freq1box->setDecimals(6);
  freq1box->setSuffix(" Hz");
  freq1box->setMinimum(0.0001);
  freq1box->setMaximum(100000000.0);
  freq1box->setValue(1.0);

  freq2box = new QDoubleSpinBox;
  freq2box->setDecimals(6);
  freq2box->setSuffix(" Hz");
  freq2box->setMinimum(0.0001);
  freq2box->setMaximum(100000000.0);
  freq2box->setValue(2.0);
  freq2box->setVisible(false);

  orderbox = new QSpinBox;
  orderbox->setMinimum(1);
  orderbox->setMaximum(8);
  orderbox->setSingleStep(1);
  orderbox->setValue(1);

  modelbox = new QComboBox;
  modelbox->addItem("Butterworth");
  modelbox->addItem("Chebyshev");
  modelbox->addItem("Bessel");
  modelbox->addItem("Moving Average");

  ripplebox = new QDoubleSpinBox;
  ripplebox->setDecimals(6);
  ripplebox->setSuffix(" dB");
  ripplebox->setMinimum(0.1);
  ripplebox->setMaximum(6.0);
  ripplebox->setValue(1.0);
  ripplebox->setVisible(false);

  listlabel = new QLabel;
  listlabel->setText("Select signals:");

  signals_list = new QListWidget;
  signals_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  signals_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  signals_list->setMinimumWidth(150 * mainwindow->w_scaling);

  CancelButton = new QPushButton;
  CancelButton->setText("&Cancel");

  ApplyButton = new QPushButton;
  ApplyButton->setText("&Apply");

  curve1 = new FilterCurve;
  curve1->setH_RasterSize(10);
  curve1->setV_RasterSize(14);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    item = new QListWidgetItem;
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      strlcpy(str1_256, mainwindow->signalcomp[i]->alias, 256);
    }
    else
    {
      strlcpy(str1_256, mainwindow->signalcomp[i]->signallabel, 256);
    }

    snprintf(str1_256 + strlen(str1_256), 256 - strlen(str1_256), "  (%f Hz)",
             mainwindow->signalcomp[i]->edfparam_0->sf_f);
    remove_trailing_zeros(str1_256);
    item->setText(str1_256);
    item->setData(Qt::UserRole, QVariant(i));
    signals_list->addItem(item);
  }

  signals_list->setCurrentRow(0);

  selected_signals_changed();

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(typeboxlabel);
  vlayout3->addWidget(typebox);
  vlayout3->addWidget(modelboxlabel);
  vlayout3->addWidget(modelbox);

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addStretch(1000);
  vlayout4->addWidget(freqbox1label);
  vlayout4->addWidget(freq1box);
  vlayout4->addWidget(orderboxlabel);
  vlayout4->addWidget(orderbox);

  QVBoxLayout *vlayout5 = new QVBoxLayout;
  vlayout5->addStretch(1000);
  vlayout5->addWidget(freqbox2label);
  vlayout5->addWidget(freq2box);
  vlayout5->addWidget(orderlabel);
  vlayout5->addWidget(ordervaluelabel);
  vlayout5->addWidget(ripplebox);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(vlayout3);
  hlayout2->addSpacing(20);
  hlayout2->addLayout(vlayout4);
  hlayout2->addSpacing(20);
  hlayout2->addLayout(vlayout5);

  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addWidget(ApplyButton);
  hlayout6->addStretch(1000);
  hlayout6->addWidget(CancelButton);
  hlayout6->addSpacing(20);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addSpacing(10);
  vlayout1->addWidget(curve1, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout6);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(listlabel);
  vlayout2->addWidget(signals_list);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addLayout(vlayout1, 1000);
  hlayout1->addSpacing(20);
  hlayout1->addLayout(vlayout2, 300);

  filterdialog->setLayout(hlayout1);

  QObject::connect(ApplyButton,  SIGNAL(clicked()),                this,         SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton, SIGNAL(clicked()),                filterdialog, SLOT(close()));
  QObject::connect(freq1box,     SIGNAL(valueChanged(double)),     this,         SLOT(freq1boxvaluechanged(double)));
  QObject::connect(typebox,      SIGNAL(currentIndexChanged(int)), this,         SLOT(filtertypeboxvaluechanged(int)));
  QObject::connect(orderbox,     SIGNAL(valueChanged(int)),        this,         SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox,     SIGNAL(currentIndexChanged(int)), this,         SLOT(filtermodelboxvaluechanged(int)));
  QObject::connect(ripplebox,    SIGNAL(valueChanged(double)),     this,         SLOT(rippleboxvaluechanged(double)));
  QObject::connect(freq2box,     SIGNAL(valueChanged(double)),     this,         SLOT(freq2boxvaluechanged(double)));
  QObject::connect(signals_list, SIGNAL(itemSelectionChanged()),   this,         SLOT(selected_signals_changed()));

  updatecurve();

  filterdialog->exec();
}


UI_FilterDialog::~UI_FilterDialog()
{
  free(array);
  free(array_pha);
}


void UI_FilterDialog::updatecurve(void)
{
  int i,
      type=0,
      rate=3200,
      order=1,
      model=0;

  double frequency=150.0,
         ripple=1.0,
         frequency2=200.0;

  char *err=NULL;

  FidFilter *ff=NULL;


  order = orderbox->value();

  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    for(i=0; i<FILT_RESP_ARR_SZ; i++)
    {
      array[i] = ravg_filter_response(type, order, (double)i / (2.0 * FILT_RESP_ARR_SZ));
    }

    curve1->drawCurve(array, FILT_RESP_ARR_SZ, 1.4, 0.0);

    return;
  }

  ripple = -(ripplebox->value());

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    if(freq1box->value() > (freq2box->value() * 0.91))
    {
      return;
    }

    frequency = frequency2 * (freq1box->value() / freq2box->value());
  }

  spec_str_1[0] = 0;

  if(type == FILTERTYPE_HIGHPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      snprintf(spec_str_1, 256, "HpBu%i/%f", order, frequency);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      snprintf(spec_str_1, 256, "HpCh%i/%f/%f", order, ripple, frequency);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      snprintf(spec_str_1, 256, "HpBe%i/%f", order, frequency);
    }
  }

  if(type == FILTERTYPE_LOWPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      snprintf(spec_str_1, 256, "LpBu%i/%f", order, frequency);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      snprintf(spec_str_1, 256, "LpCh%i/%f/%f", order, ripple, frequency);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      snprintf(spec_str_1, 256, "LpBe%i/%f", order, frequency);
    }
  }

  if(type == FILTERTYPE_NOTCH)
  {
    snprintf(spec_str_1, 256, "BsRe/%i/%f", order, frequency);
  }

  if(type == FILTERTYPE_BANDPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      snprintf(spec_str_1, 256, "BpBu%i/%f-%f", order, frequency, frequency2);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      snprintf(spec_str_1, 256, "BpCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      snprintf(spec_str_1, 256, "BpBe%i/%f-%f", order, frequency, frequency2);
    }
  }

  if(type == FILTERTYPE_BANDSTOP)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      snprintf(spec_str_1, 256, "BsBu%i/%f-%f", order, frequency, frequency2);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      snprintf(spec_str_1, 256, "BsCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      snprintf(spec_str_1, 256, "BsBe%i/%f-%f", order, frequency, frequency2);
    }
  }

  strlcpy(spec_str_2, spec_str_1, 256);

  filter_spec = spec_str_2;

  err = fid_parse(rate, &filter_spec, &ff);

  if(err != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", err);
    messagewindow.exec();
    free(err);
    return;
  }

  for(i=0; i<FILT_RESP_ARR_SZ; i++)
  {
    array[i] = fid_response_pha(ff, (double)i / (double)rate, array_pha + i);
  }

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    curve1->drawCurve(array, FILT_RESP_ARR_SZ, 1.4, 0.0, array_pha, FILT_RESP_ARR_SZ, 0.0, 1.0, frequency, frequency2);
  }
  else
  {
    curve1->drawCurve(array, FILT_RESP_ARR_SZ, 1.4, 0.0, array_pha, FILT_RESP_ARR_SZ, 0.0, 1.0, frequency);
  }

  free(ff);
}


void UI_FilterDialog::rippleboxvaluechanged(double value)
{
  last_ripple = value;

  updatecurve();
}


void UI_FilterDialog::filtermodelboxvaluechanged(int model)
{
  int type;

  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::disconnect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  type = typebox->currentIndex();

  last_model = model;

  freqbox1label->setText("Frequency");
  freqbox1label->setVisible(true);
  freq1box->setVisible(true);

  if(type != FILTERTYPE_NOTCH)
  {
    orderboxlabel->setText("Order");
    orderboxlabel->setVisible(true);

    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      if(model == FILTERMODEL_BESSEL)
      {
        orderbox->setMaximum(10);
      }
      else
      {
        orderbox->setMaximum(16);
      }
      orderbox->setSingleStep(2);
      orderbox->setMinimum(2);
      orderbox->setValue(last_order * 2);
    }
    else
    {
      orderbox->setMaximum(8);
      orderbox->setSingleStep(1);
      orderbox->setMinimum(1);
      orderbox->setValue(last_order);
    }
    orderbox->setVisible(true);
  }

  if(model == FILTERMODEL_BUTTERWORTH)
  {
    ripplebox->setVisible(false);
    orderlabel->setText("Slope roll-off:");
    orderlabel->setVisible(true);
    ordervaluelabel->setText(QString::number(6 * last_order, 'f', 0).append(" dB / octave"));
    ordervaluelabel->setVisible(true);
  }

  if(model == FILTERMODEL_CHEBYSHEV)
  {
    orderlabel->setText("passband ripple");
    orderlabel->setVisible(true);
    ordervaluelabel->setVisible(false);
    ripplebox->setVisible(true);
  }

  if(model == FILTERMODEL_BESSEL)
  {
    ripplebox->setVisible(false);
    ordervaluelabel->setVisible(true);
    ordervaluelabel->setText("  ");
    orderlabel->setVisible(true);
    orderlabel->setText("  ");
  }

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    orderlabel->setVisible(true);
    orderlabel->setText("  ");
    ordervaluelabel->setVisible(false);
    freqbox1label->setVisible(false);
    freqbox2label->setVisible(false);
    freq1box->setVisible(false);
    ripplebox->setVisible(false);
    orderboxlabel->setText("Samples");
    orderbox->setMaximum(10000);
    orderbox->setSingleStep(1);
    orderbox->setMinimum(2);
    orderbox->setValue(last_samples);
  }

  updatecurve();

  QObject::connect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));
}


void UI_FilterDialog::orderboxvaluechanged(int order)
{
  int type,
      model;

  char str1_256[256]={""};

  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  if(type == FILTERTYPE_NOTCH)
  {
    snprintf(str1_256, 256, "%f Hz", freq1box->value() / orderbox->value());
    remove_trailing_zeros(str1_256);
    ordervaluelabel->setText(str1_256);
    ordervaluelabel->setVisible(true);

    last_qfactor = order;
  }
  else
  {
    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      last_order = order / 2;
    }
    else
    {
      if(model == FILTERMODEL_MOVINGAVERAGE)
      {
        last_samples = order;
      }
      else
      {
        last_order = order;
      }
    }

    if(model == FILTERMODEL_BUTTERWORTH)
    {
      orderlabel->setText("Slope roll-off:");
      orderlabel->setVisible(true);
      ordervaluelabel->setText(QString::number(6 * last_order, 'f', 0).append(" dB / octave"));
      ordervaluelabel->setVisible(true);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      orderlabel->setText("passband ripple");
      orderlabel->setVisible(true);
      ordervaluelabel->setVisible(false);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      ripplebox->setVisible(false);
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
    }

    if(model == FILTERMODEL_MOVINGAVERAGE)
    {
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
      freqbox1label->setVisible(false);
    }
  }

  updatecurve();
}


void UI_FilterDialog::filtertypeboxvaluechanged(int type)
{
  char str1_256[256]={""};

  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::disconnect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  if(type == FILTERTYPE_NOTCH)
  {
    ripplebox->setVisible(false);
    orderboxlabel->setText("Notch Q-factor");
    orderboxlabel->setVisible(true);
    orderbox->setMaximum(100);
    orderbox->setSingleStep(1);
    orderbox->setMinimum(3);
    orderbox->setValue(last_qfactor);
    orderbox->setVisible(true);
    orderlabel->setText("-3 dB bandwidth:");
    snprintf(str1_256, 256, "%f Hz", freq1box->value() / orderbox->value());
    remove_trailing_zeros(str1_256);
    ordervaluelabel->setText(str1_256);
    ordervaluelabel->setVisible(true);
    modelbox->clear();
    modelbox->addItem("Resonator");
    modelbox->setVisible(true);
  }
  else
  {
    orderboxlabel->setText("Order");
    orderboxlabel->setVisible(true);
    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      if(last_model == FILTERMODEL_BESSEL)
      {
        orderbox->setMaximum(10);
      }
      else
      {
        orderbox->setMaximum(16);
      }
      orderbox->setSingleStep(2);
      orderbox->setMinimum(2);
      orderbox->setValue(last_order * 2);
      orderbox->setVisible(true);
    }
    else
    {
      orderbox->setMaximum(8);
      orderbox->setSingleStep(1);
      orderbox->setMinimum(1);
      orderbox->setValue(last_order);
      orderbox->setVisible(true);
    }

    if(last_model == FILTERMODEL_MOVINGAVERAGE)
    {
      if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS))
      {
        ripplebox->setVisible(false);
        orderlabel->setVisible(true);
        ordervaluelabel->setVisible(true);
        orderlabel->setText("  ");
        ordervaluelabel->setText("  ");
        orderboxlabel->setText("Samples");
        orderboxlabel->setVisible(true);
        orderbox->setMaximum(10000);
        orderbox->setSingleStep(1);
        orderbox->setMinimum(2);
        orderbox->setValue(last_samples);
        orderbox->setVisible(true);
      }
      else
      {
        last_model = FILTERMODEL_BUTTERWORTH;
      }
    }

    if(last_model == FILTERMODEL_BUTTERWORTH)
    {
      ripplebox->setVisible(false);
      orderlabel->setText("Slope roll-off:");
      orderlabel->setVisible(true);
      ordervaluelabel->setText(QString::number(6 * last_order, 'f', 0).append(" dB / octave"));
      ordervaluelabel->setVisible(true);
    }

    if(last_model == FILTERMODEL_CHEBYSHEV)
    {
      ordervaluelabel->setVisible(false);
      ripplebox->setVisible(true);
      orderlabel->setText("passband ripple");
      orderlabel->setVisible(true);
    }

    if(last_model == FILTERMODEL_BESSEL)
    {
      ripplebox->setVisible(false);
      orderlabel->setVisible(true);
      ordervaluelabel->setVisible(true);
      orderlabel->setText("  ");
      ordervaluelabel->setText("  ");
    }

    modelbox->clear();
    modelbox->addItem("Butterworth");
    modelbox->addItem("Chebyshev");
    modelbox->addItem("Bessel");
    if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS))
    {
      modelbox->addItem("Moving Average");
    }
    modelbox->setCurrentIndex(last_model);
  }

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    freqbox1label->setText("Frequency 1");
    freqbox1label->setVisible(true);
    freqbox2label->setText("Frequency 2");
    freqbox2label->setVisible(true);
    freq1box->setVisible(true);
    freq2box->setVisible(true);
  }
  else
  {
    freqbox2label->setVisible(false);
    freq2box->setVisible(false);

    if(last_model == FILTERMODEL_MOVINGAVERAGE)
    {
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
      freqbox1label->setVisible(false);
    }
    else
    {
      freqbox1label->setText("Frequency");
      freqbox1label->setVisible(true);
    }
  }

  QObject::connect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  updatecurve();
}


void UI_FilterDialog::freq1boxvaluechanged(double)
{
  char str1_256[256]={""};

//   if(freq2box->value() < (value * 1.12))
//   {
//     freq2box->setValue(value * 1.12);
//   }

  if(typebox->currentIndex() == FILTERTYPE_NOTCH)
  {
    snprintf(str1_256, 256, "%f Hz", freq1box->value() / orderbox->value());
    remove_trailing_zeros(str1_256);
    ordervaluelabel->setText(str1_256);
  }

  updatecurve();
}


void UI_FilterDialog::freq2boxvaluechanged(double)
{
//   QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freq1boxvaluechanged(double)));
//
//   if(freq1box->value() > (value * 0.9))
//   {
//     freq1box->setValue(value * 0.9);
//   }
//
//   QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freq1boxvaluechanged(double)));

  updatecurve();
}


void UI_FilterDialog::ApplyButtonClicked()
{
  int i, s, n,
      type,
      model,
      order;

  double frequency1,
         frequency2,
         ripple;

  char *err=NULL;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist = signals_list->selectedItems();

  n = selectedlist.size();

  if(!n)
  {
    filterdialog->close();
    return;
  }

  frequency1 = freq1box->value();

  frequency2 = freq2box->value();

  order = orderbox->value();

  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  ripple = -(ripplebox->value());

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    if((mainwindow->signalcomp[s]->filter_cnt + mainwindow->signalcomp[s]->fidfilter_cnt) > MAXFILTERS - 1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                "The maximum amount of filters per signal has been reached.\n"
                                "Remove some filters first.");
      messagewindow.exec();
      return;
    }
  }

  if((model == FILTERMODEL_BUTTERWORTH) || (model == FILTERMODEL_CHEBYSHEV) || (model == FILTERMODEL_BESSEL))
  {
    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = signals_list->row(item);

      if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS) || (type == FILTERTYPE_NOTCH))
      {
        if(frequency1 >= mainwindow->signalcomp[s]->edfparam_0->sf_f / 2.0)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                    "The frequency of the filter(s) must be less than: samplerate / 2");
          messagewindow.exec();
          return;
        }
      }
      else
      {
        if(frequency2 >= mainwindow->signalcomp[s]->edfparam_0->sf_f / 2.0)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                    "The frequency of the filter(s) must be less than: samplerate / 2");
          messagewindow.exec();
          return;
        }
      }
    }

    spec_str_1[0] = 0;

    if(type == FILTERTYPE_HIGHPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1, 256, "HpBu%i/%f", order, frequency1);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1, 256, "HpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1, 256, "HpBe%i/%f", order, frequency1);
      }
    }

    if(type == FILTERTYPE_LOWPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1, 256, "LpBu%i/%f", order, frequency1);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1, 256, "LpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1, 256, "LpBe%i/%f", order, frequency1);
      }
    }

    if(type == FILTERTYPE_NOTCH)
    {
      snprintf(spec_str_1, 256, "BsRe/%i/%f", order, frequency1);
    }

    if(type == FILTERTYPE_BANDPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1, 256, "BpBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1, 256, "BpCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1, 256, "BpBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    if(type == FILTERTYPE_BANDSTOP)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        snprintf(spec_str_1, 256, "BsBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        snprintf(spec_str_1, 256, "BsCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        snprintf(spec_str_1, 256, "BsBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = signals_list->row(item);

      strlcpy(spec_str_2, spec_str_1, 256);

      filter_spec = spec_str_2;

      err = fid_parse(mainwindow->signalcomp[s]->edfparam_0->sf_f,
                      &filter_spec,
                      &mainwindow->signalcomp[s]->fidfilter[mainwindow->signalcomp[s]->fidfilter_cnt]);

      if(err != NULL)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", err);
        messagewindow.exec();
        free(err);
        filterdialog->close();
        return;
      }

      mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_new(mainwindow->signalcomp[s]->fidfilter[mainwindow->signalcomp[s]->fidfilter_cnt],
                                                                                      &mainwindow->signalcomp[s]->fidfuncp[mainwindow->signalcomp[s]->fidfilter_cnt]);

      mainwindow->signalcomp[s]->fidbuf[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_newbuf(mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt]);
      mainwindow->signalcomp[s]->fidbuf2[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_newbuf(mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt]);

      mainwindow->signalcomp[s]->fidfilter_freq[mainwindow->signalcomp[s]->fidfilter_cnt] = frequency1;

      mainwindow->signalcomp[s]->fidfilter_freq2[mainwindow->signalcomp[s]->fidfilter_cnt] = frequency2;

      mainwindow->signalcomp[s]->fidfilter_ripple[mainwindow->signalcomp[s]->fidfilter_cnt] = ripple;

      mainwindow->signalcomp[s]->fidfilter_order[mainwindow->signalcomp[s]->fidfilter_cnt] = order;

      mainwindow->signalcomp[s]->fidfilter_model[mainwindow->signalcomp[s]->fidfilter_cnt] = model;

      mainwindow->signalcomp[s]->fidfilter_type[mainwindow->signalcomp[s]->fidfilter_cnt] = type;

      mainwindow->signalcomp[s]->fidfilter_setup[mainwindow->signalcomp[s]->fidfilter_cnt] = 1;

      mainwindow->signalcomp[s]->fidfilter_cnt++;
    }
  }

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = signals_list->row(item);

      mainwindow->signalcomp[s]->ravg_filter[mainwindow->signalcomp[s]->ravg_filter_cnt] = create_ravg_filter(type, order);
      if(mainwindow->signalcomp[s]->ravg_filter[mainwindow->signalcomp[s]->ravg_filter_cnt] == NULL)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a moving average filter.");
        messagewindow.exec();
        filterdialog->close();
        return;
      }

      mainwindow->signalcomp[s]->ravg_filter_type[mainwindow->signalcomp[s]->ravg_filter_cnt] = type;

      mainwindow->signalcomp[s]->ravg_filter_size[mainwindow->signalcomp[s]->ravg_filter_cnt] = order;

      mainwindow->signalcomp[s]->ravg_filter_setup[mainwindow->signalcomp[s]->ravg_filter_cnt] = 1;

      mainwindow->signalcomp[s]->ravg_filter_cnt++;
    }
  }

  filterdialog->close();

  mainwindow->setup_viewbuf();
}


void UI_FilterDialog::selected_signals_changed()
{
  int n;

  char str1_128[128]={""};

  QList<QListWidgetItem *> selectedlist = signals_list->selectedItems();

  n = selectedlist.size();

  snprintf(str1_128, 128, "Selected signals: %i", n);

  listlabel->setText(str1_128);
}


















