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



#include "aeeg_dialog.h"


#define MARGIN_MEDIAN_SZ    (20)




UI_aeeg_window::UI_aeeg_window(QWidget *w_parent, sigcompblck_t *signal_comp, int numb, aeeg_dock_param_t *p_par)
{
  char str_1024[1024]="";

  myobjectDialog = NULL;

  mainwindow = (UI_Mainwindow *)w_parent;

  no_dialog_params = p_par;

  signalcomp = signal_comp;

  aeeg_instance_nr = numb;

  sf = signalcomp->edfparam_0->sf_int;
  if(!sf)
  {
    sf = signalcomp->edfparam_0->sf_f + 0.5;
  }

  if(no_dialog_params != NULL)
  {
    no_dialog_params->sf = sf;

    start_button_clicked();

    return;
  }

  snprintf(str_1024, 1024, "aEEG   %s", signalcomp->signallabel);

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(300 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle(str_1024);
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  segmentlen_spinbox = new QSpinBox;
  segmentlen_spinbox->setSuffix(" sec");
  segmentlen_spinbox->setMinimum(5);
  if(signalcomp->edfhdr->recording_len_sec < 300)
  {
    segmentlen_spinbox->setMaximum(signalcomp->edfhdr->recording_len_sec);
  }
  else
  {
    segmentlen_spinbox->setMaximum(300);
  }
  segmentlen_spinbox->setValue(mainwindow->aeeg_segmentlen);

  bp_min_hz_spinbox = new QDoubleSpinBox;
  bp_min_hz_spinbox->setSuffix(" Hz");
  bp_min_hz_spinbox->setDecimals(1);
  bp_min_hz_spinbox->setMinimum(1);
  bp_min_hz_spinbox->setMaximum((sf / 2) - 2);
  bp_min_hz_spinbox->setValue(mainwindow->aeeg_bp_min_hz);

  bp_max_hz_spinbox = new QDoubleSpinBox;
  bp_max_hz_spinbox->setSuffix(" Hz");
  bp_max_hz_spinbox->setDecimals(1);
  bp_max_hz_spinbox->setMinimum(2);
  bp_max_hz_spinbox->setMaximum((sf / 2) - 1);
  bp_max_hz_spinbox->setValue(mainwindow->aeeg_bp_max_hz);

  lp_hz_spinbox = new QDoubleSpinBox;
  lp_hz_spinbox->setSuffix(" Hz");
  lp_hz_spinbox->setDecimals(2);
  lp_hz_spinbox->setMinimum(0.01);
  lp_hz_spinbox->setMaximum(5);
  lp_hz_spinbox->setValue(mainwindow->aeeg_lp_hz);

  scale_max_amp_spinbox = new QDoubleSpinBox;
  scale_max_amp_spinbox->setSuffix(" uV");
  scale_max_amp_spinbox->setDecimals(3);
  scale_max_amp_spinbox->setMinimum(10);
  scale_max_amp_spinbox->setMaximum(500);
  scale_max_amp_spinbox->setValue(mainwindow->aeeg_scale_max_amp);

  plot_margins_checkbox = new QCheckBox;
  plot_margins_checkbox->setTristate(false);
  if(mainwindow->aeeg_plot_margins)
  {
    plot_margins_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    plot_margins_checkbox->setCheckState(Qt::Unchecked);
  }

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  flayout = new QFormLayout;
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("Segment length", segmentlen_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("BP min. freq.", bp_min_hz_spinbox);
  flayout->addRow("BP max. freq.", bp_max_hz_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("LP envelope", lp_hz_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("Max. amplitude", scale_max_amp_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("Plot margins", plot_margins_checkbox);
  flayout->addRow(" ", (QWidget *)(NULL));

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(flayout, 1000);
  hlayout2->addStretch(1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(close_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(default_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(start_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(close_button, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  if((sf >= 100) && (signalcomp->edfhdr->recording_len_sec >= 30))
  {
    QObject::connect(default_button, SIGNAL(clicked()), this, SLOT(default_button_clicked()));
    QObject::connect(start_button,   SIGNAL(clicked()), this, SLOT(start_button_clicked()));
  }

  myobjectDialog->exec();
}


void UI_aeeg_window::default_button_clicked()
{
  segmentlen_spinbox->setValue(15);
  bp_min_hz_spinbox->setValue(2);
  bp_max_hz_spinbox->setValue(15);
  lp_hz_spinbox->setValue(0.5);
  scale_max_amp_spinbox->setValue(100);
  plot_margins_checkbox->setCheckState(Qt::Checked);

  mainwindow->aeeg_segmentlen = 15;
  mainwindow->aeeg_bp_min_hz = 2;
  mainwindow->aeeg_bp_max_hz = 15;
  mainwindow->aeeg_lp_hz = 0.5;
  mainwindow->aeeg_scale_max_amp = 100;
  mainwindow->aeeg_plot_margins = 1;
}


void UI_aeeg_window::start_button_clicked()
{
  int i, j,
      err,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      medians_in_recording,
      ret_err=0,
      max_idx=0,
      min_idx=0,
      plot_margins=0,
      uv_scaling=1;


  long long samples_in_file;

  double bp_hz_min,
         bp_hz_max,
         lp_hz,
         *smplbuf=NULL,
         *min_seg_val=NULL,
         *max_seg_val=NULL,
         *max_median_val=NULL,
         *min_median_val=NULL,
         max_margin_median_buf[MARGIN_MEDIAN_SZ]={0},
         min_margin_median_buf[MARGIN_MEDIAN_SZ]={0},
         scale_max_amp;

  char str_32[32]={""},
       filt_spec_str_bp[256]={""},
       *filt_spec_ptr_bp=NULL,
       *fid_err_bp=NULL,
       filt_spec_str_lp[256]={""},
       *filt_spec_ptr_lp=NULL,
       *fid_err_lp=NULL;

  aeeg_dock_param_t dock_param;

  memset(&dock_param, 0, sizeof(aeeg_dock_param_t));

  strlcpy(str_32, signalcomp->physdimension, 32);
  trim_spaces(str_32);
  if((strcmp(str_32, "uV")) && (strcmp(str_32, "mV")) && (strcmp(str_32, "V")))
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Unknown physical dimension (unit), expected uV or mV or V", QMessageBox::Close);
    msgBox.exec();
    return;
  }

  if(!strcmp(str_32, "uV"))
  {
    uv_scaling = 1;
  }
  else if(!strcmp(str_32, "mV"))
    {
      uv_scaling = 1000;
    }
    else if(!strcmp(str_32, "V"))
      {
        uv_scaling = 1000000;
      }

  if(myobjectDialog != NULL)
  {
    segmentlen = segmentlen_spinbox->value();
    bp_hz_min = bp_min_hz_spinbox->value();
    bp_hz_max = bp_max_hz_spinbox->value();
    lp_hz = lp_hz_spinbox->value();
    scale_max_amp = scale_max_amp_spinbox->value();
    if(plot_margins_checkbox->checkState() == Qt::Checked)
    {
      plot_margins = 1;
    }
    else
    {
      plot_margins = 0;
    }

    if((bp_hz_max - bp_hz_min) <= 4.999)
    {
      QMessageBox msgBox(QMessageBox::Critical, "Error", "(Max.freq. - min.freq.) must be >= 5 Hz.", QMessageBox::Close);
      msgBox.exec();
      return;
    }
  }
  else  // no dialog
  {
    segmentlen = no_dialog_params->segment_len;
    bp_hz_min = no_dialog_params->bp_min_hz;
    bp_hz_max = no_dialog_params->bp_max_hz;
    lp_hz = no_dialog_params->lp_hz;
    scale_max_amp = no_dialog_params->scale_max_amp;
    plot_margins = no_dialog_params->plot_margins;
  }
  mainwindow->aeeg_segmentlen = segmentlen;
  mainwindow->aeeg_bp_min_hz = bp_hz_min;
  mainwindow->aeeg_bp_max_hz = bp_hz_max;
  mainwindow->aeeg_lp_hz = lp_hz;
  mainwindow->aeeg_scale_max_amp = scale_max_amp;
  mainwindow->aeeg_plot_margins = plot_margins;

  smpls_in_segment = sf * segmentlen;

  samples_in_file = signalcomp->edfparam_0->smpls;

  segments_in_recording = samples_in_file / (long long)smpls_in_segment;

  max_idx = (smpls_in_segment * mainwindow->aeeg_max_nearby_pct) / 100.0 + 0.5;

  min_idx = (smpls_in_segment * mainwindow->aeeg_min_nearby_pct) / 100.0 + 0.5;

  medians_in_recording = segments_in_recording / MARGIN_MEDIAN_SZ;

//   printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);
//
//   printf("start_button_clicked(): sf: %i\n", sf);
//
//   printf("start_button_clicked(): segmentlen: %i\n", segmentlen);
//
//   printf("start_button_clicked(): smpls_in_segment: %i\n", smpls_in_segment);
//
//   printf("start_button_clicked(): segments_in_recording: %i\n", segments_in_recording);
//
//   printf("start_button_clicked(): bp_hz_min: %f\n", bp_hz_min);
//
//   printf("start_button_clicked(): bp_hz_max: %f\n", bp_hz_max);
//
//   printf("start_button_clicked(): lp_hz: %f\n", lp_hz);
//
//   printf("start_button_clicked(): scale_max_amp: %f\n", scale_max_amp);
//
//   printf("start_button_clicked(): max_idx: %i\n", max_idx);
//
//   printf("start_button_clicked(): min_idx: %i\n", min_idx);
//
//   printf("start_button_clicked(): medians_in_recording: %i\n", medians_in_recording);

  if(medians_in_recording < 1)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The recording length is too short, at least 20 x segment length seconds are needed.", QMessageBox::Close);
    msgBox.exec();
    return;
  }

  max_seg_val = (double *)malloc(segments_in_recording * sizeof(double));
  if(max_seg_val == NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
    msgBox.exec();
    return;
  }

  min_seg_val = (double *)malloc(segments_in_recording * sizeof(double));
  if(min_seg_val == NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
    msgBox.exec();
    free(max_seg_val);
    return;
  }

  max_median_val = (double *)malloc(medians_in_recording * sizeof(double));
  if(max_median_val == NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
    msgBox.exec();
    free(max_seg_val);
    free(min_seg_val);
    return;
  }

  min_median_val = (double *)malloc(medians_in_recording * sizeof(double));
  if(min_median_val == NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
    msgBox.exec();
    free(max_seg_val);
    free(min_seg_val);
    free(max_median_val);
    return;
  }

  ret_err = 0;

  flt_blck_rd_t *fbr = create_flt_blck_rd(signalcomp, smpls_in_segment, 0, 1, &ret_err, &smplbuf);
  if(fbr == NULL)
  {
    if(ret_err == 1)
    {
      QMessageBox::critical(myobjectDialog, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    }
    else
    {
      QMessageBox::critical(myobjectDialog, "Error", "Internal error (-1)");
    }
    free(max_seg_val);
    free(min_seg_val);
    free(max_median_val);
    free(min_median_val);
    return;
  }

  FidFilter *fidfilter_bp=NULL;
  FidRun *fid_run_bp=NULL;
  FidFunc *fidfuncp_bp=NULL;
  void *fidbuf_bp=NULL;

  snprintf(filt_spec_str_bp, 256, "BpBu%i/%f-%f", 8, bp_hz_min, bp_hz_max);

  filt_spec_ptr_bp = filt_spec_str_bp;

  fid_err_bp = fid_parse(sf, &filt_spec_ptr_bp, &fidfilter_bp);
  if(fid_err_bp != NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-2)", QMessageBox::Close);
    msgBox.exec();
    free(fid_err_bp);
    free(max_seg_val);
    free(min_seg_val);
    free(max_median_val);
    free(min_median_val);
    free_flt_blck_rd(fbr);
    free(smplbuf);
    return;
  }

  fid_run_bp = fid_run_new(fidfilter_bp, &fidfuncp_bp);

  fidbuf_bp = fid_run_newbuf(fid_run_bp);

  FidFilter *fidfilter_lp=NULL;
  FidRun *fid_run_lp=NULL;
  FidFunc *fidfuncp_lp=NULL;
  void *fidbuf_lp=NULL;

  snprintf(filt_spec_str_lp, 256, "LpBu%i/%f", 4, lp_hz);

  filt_spec_ptr_lp = filt_spec_str_lp;

  fid_err_lp = fid_parse(sf, &filt_spec_ptr_lp, &fidfilter_lp);
  if(fid_err_lp != NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-3)", QMessageBox::Close);
    msgBox.exec();
    free(fid_err_bp);
    free(fid_err_lp);
    free(max_seg_val);
    free(min_seg_val);
    free(max_median_val);
    free(min_median_val);
    free_flt_blck_rd(fbr);
    free(smplbuf);
    return;
  }

  fid_run_lp = fid_run_new(fidfilter_lp, &fidfuncp_lp);

  fidbuf_lp = fid_run_newbuf(fid_run_lp);

  QProgressDialog progress("Processing...", "Abort", 0, segments_in_recording);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(500);

  for(i=0; i<segments_in_recording; i++)
  {
    progress.setValue(i);

    qApp->processEvents();

    if(progress.wasCanceled() == true)
    {
      progress.reset();
      free(max_seg_val);
      free(min_seg_val);
      free(max_median_val);
      free(min_median_val);
      free(fidfilter_bp);
      fid_run_free(fid_run_bp);
      fid_run_freebuf(fidbuf_bp);
      free(fidfilter_lp);
      fid_run_free(fid_run_lp);
      fid_run_freebuf(fidbuf_lp);
      free_flt_blck_rd(fbr);
      free(smplbuf);
      return;
    }

    err = run_flt_blck_rd(fbr, i * smpls_in_segment);
    if(err)
    {
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", "Internal error (-4)");
      return;
    }

    max_seg_val[i] = 0;
    min_seg_val[i] = DBL_MAX;

    for(j=0; j<smpls_in_segment; j++)
    {
      smplbuf[j] = fidfuncp_bp(fidbuf_bp, smplbuf[j] * uv_scaling);  // bandpass 2 - 15 Hz, apply scaling in case of mV or V

      smplbuf[j] = fabs(smplbuf[j]);  // rectifier

      smplbuf[j] = fidfuncp_lp(fidbuf_lp, smplbuf[j]) * 2;  // lowpass envelope smoothing and gain 2x
    }

    qsort(smplbuf, smpls_in_segment, sizeof(double), dbl_cmp);  // sorting

    max_seg_val[i] = smplbuf[max_idx];
    min_seg_val[i] = smplbuf[min_idx];

    max_margin_median_buf[i % MARGIN_MEDIAN_SZ] = smplbuf[max_idx];
    min_margin_median_buf[i % MARGIN_MEDIAN_SZ] = smplbuf[min_idx];

    if(!((i + 1) % MARGIN_MEDIAN_SZ))
    {
      qsort(min_margin_median_buf, MARGIN_MEDIAN_SZ, sizeof(double), dbl_cmp);
      qsort(max_margin_median_buf, MARGIN_MEDIAN_SZ, sizeof(double), dbl_cmp);

      min_median_val[i / MARGIN_MEDIAN_SZ] = (min_margin_median_buf[MARGIN_MEDIAN_SZ / 2] +
                                              min_margin_median_buf[(MARGIN_MEDIAN_SZ / 2) + 1]) / 2;

      max_median_val[i / MARGIN_MEDIAN_SZ] = (max_margin_median_buf[MARGIN_MEDIAN_SZ / 2] +
                                              max_margin_median_buf[(MARGIN_MEDIAN_SZ / 2) + 1]) / 2;
    }
  }

  free(fidfilter_bp);
  fid_run_free(fid_run_bp);
  fid_run_freebuf(fidbuf_bp);
  free(fidfilter_lp);
  fid_run_free(fid_run_lp);
  fid_run_freebuf(fidbuf_lp);
  free_flt_blck_rd(fbr);
  free(smplbuf);

  progress.reset();

  dock_param.signalcomp = signalcomp;
  dock_param.sf = sf;
  dock_param.bp_min_hz = mainwindow->aeeg_bp_min_hz;
  dock_param.bp_max_hz = mainwindow->aeeg_bp_max_hz;
  dock_param.lp_hz = mainwindow->aeeg_lp_hz;
  dock_param.segment_len = segmentlen;
  dock_param.segments_in_recording = segments_in_recording;
  dock_param.instance_num = aeeg_instance_nr;
  dock_param.max_seg_val = max_seg_val;
  dock_param.min_seg_val = min_seg_val;
  dock_param.min_median_val = min_median_val;
  dock_param.max_median_val = max_median_val;
  dock_param.medians_in_recording = medians_in_recording;
  dock_param.scale_max_amp = scale_max_amp;
  strlcpy(dock_param.unit, "uV", 32);
  remove_trailing_spaces(dock_param.unit);
  dock_param.plot_margins = plot_margins;

  mainwindow->aeeg_dock[aeeg_instance_nr] = new UI_aeeg_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->aeeg_dock[aeeg_instance_nr]->aeeg_dock);

  mainwindow->insertToolBarBreak(mainwindow->aeeg_dock[aeeg_instance_nr]->aeeg_dock);

  signalcomp->aeeg_idx[aeeg_instance_nr] = aeeg_instance_nr + 1;

  if(myobjectDialog != NULL)
  {
    myobjectDialog->close();
  }
}


int UI_aeeg_window::dbl_cmp(const void *a, const void *b)
{
  double val1, val2;

  val1 = *(double *)a;
  val2 = *(double *)b;

  if(val1 > val2)
  {
    return 1;
  }

  return 0;
}





























