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


#include "view_session_dialog.h"



UI_ViewSessionwindow::UI_ViewSessionwindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  session_path[0] = 0;
  session_dir[0] = 0;

  if(mainwindow->recent_sessiondir[0]!=0)
  {
    strlcpy(session_dir, mainwindow->recent_sessiondir, MAX_PATH_LENGTH);
  }

  ViewSessionDialog = new QDialog;
  ViewSessionDialog->setMinimumSize(500 * mainwindow->w_scaling, 300 * mainwindow->h_scaling);
  ViewSessionDialog->setWindowTitle("View session");
  ViewSessionDialog->setModal(true);
  ViewSessionDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  ViewSessionDialog->setSizeGripEnabled(true);

  t_model = new QStandardItemModel(this);

  tree = new QTreeView;
  tree->setHeaderHidden(true);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);
  tree->setModel(t_model);
  tree->setAlternatingRowColors(true);

  SelectButton = new QPushButton;
  SelectButton->setText("Select session");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(SelectButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(tree, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  ViewSessionDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,  SIGNAL(clicked()), ViewSessionDialog, SLOT(close()));
  QObject::connect(SelectButton, SIGNAL(clicked()), this,              SLOT(SelectButtonClicked()));

  ViewSessionDialog->exec();
}




void UI_ViewSessionwindow::SelectButtonClicked()
{
  int i, j, r,
      signalcomps_read=0,
      signals_read,
      signal_cnt,
      filters_read,
      color,
      math_funcs_before_read,
      math_funcs_after_read,
      math_func,
      math_func_cnt_before=0,
      math_func_cnt_after=0,
      math_func_pk_hold_smpls=0,
      filter_cnt=0,
      spike_filter_cnt=0,
      ravg_filter_cnt=0,
      fidfilter_cnt=0,
      fir_filter_cnt=0,
      islpf,
      signalcomps,
      order,
      model,
      type,
      size,
      polarity=1,
      holdoff=100,
      plif_powerlinefrequency,
      n_taps,
      idx_used=0,
      use_relative_path=-1,
      ref_file=0,
      timesync_mode=0,
      hdr_idx=0,
      sigcomp_idx=0,
      min_hz=0,
      max_hz=0,
      segment_len=0,
      block_len=0,
      overlap=1,
      window_func=0,
      log=0,
      power_voltage=0,
      max_pwr=0,
      min_pwr=0,
      dftblocksize=0,
      video_seek=0;

  long long viewtime=0,
            timescale=0,
            utc_starttime=0;

  char result[XML_STRBUFLEN]="",
       composition_txt_2048[2048]="",
       label_256[256]="",
       str2_2048[2048]="",
       str3_64[64]="",
       edf_path_2048[2048]="",
       e_file_path[MAXFILES][MAX_PATH_LENGTH],
       path_r[MAX_PATH_LENGTH]="",
       video_path_2048[2048]="";

  double frequency,
         frequency2,
         voltpercm=1,
         ripple,
         d_tmp,
         velocity,
         factor[MAXSIGNALS],
         *fir_vars=NULL,
         max_voltage=0,
         bp_min_hz=0,
         bp_max_hz=0,
         lp_hz=0,
         scale_max_amp=0,
         screen_offset_pix=0,
         screen_offset_unit=0;

  FILE *f_test=NULL;

  QStandardItem *parentItem,
                *signalItem,
                *filterItem,
                *firfilterItem,
                *videoItem,
                *powerspectrumdockItem,
                *hypnogramItem,
                *cdsaItem,
                *aeegItem,
                *tmp_item=NULL,
                *math_item_before=NULL,
                *math_item_after=NULL;

  QColor tmp_color;

  xml_hdl_t *xml_hdl=NULL;

  date_time_t dt;

  for(i=0; i<MAXFILES; i++)
  {
    e_file_path[i][0] = 0;
  }

  strlcpy(session_path, QFileDialog::getOpenFileName(0, "Choose a session", QString::fromLocal8Bit(session_dir), "Session files (*.esf *.ESF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(session_path, ""))
  {
    return;
  }

  get_directory_from_path(session_dir, session_path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(session_path);
  if(xml_hdl==NULL)
  {
    QMessageBox::critical(NULL, "Error", "Cannot open file for reading.");
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_session"))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }

  ViewSessionDialog->setWindowTitle(QString::fromLocal8Bit(session_path));

  t_model->clear();

  parentItem = t_model->invisibleRootItem();

  if(xml_goto_nth_element_inside(xml_hdl, "relative_path", 0))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  use_relative_path = atoi(result);
  if((use_relative_path < 0) || (use_relative_path > 1))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  if(use_relative_path)
  {
    parentItem->appendRow(new QStandardItem("paths are relative"));
  }
  else
  {
    parentItem->appendRow(new QStandardItem("paths are absolute"));
  }
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "ref_file", 0))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  ref_file = atoi(result);
  if(ref_file < 0)
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "timesync_mode", 0))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  timesync_mode = atoi(result);
  if((timesync_mode < 0) || (timesync_mode > 3))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  switch(timesync_mode)
  {
    case VIEWTIME_SYNCED_OFFSET   : parentItem->appendRow(new QStandardItem("Timesync mode: offset (sync start of file)"));
                                    break;
    case VIEWTIME_SYNCED_ABSOLUT  : parentItem->appendRow(new QStandardItem("Timesync mode: absolute (sync clocktime)"));
                                    break;
    case VIEWTIME_UNSYNCED        : parentItem->appendRow(new QStandardItem("Timesync mode: no synchronization"));
                                    break;
    case VIEWTIME_USER_DEF_SYNCED : parentItem->appendRow(new QStandardItem("Timesync mode: user defined synchronization"));
                                    break;
  }
  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "pagetime", 0))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  timescale = atoll(result);
  snprintf(str2_2048, 2048, "Timescale: %i:%02i:%02i.%04i",
          ((int)(timescale / TIME_FIXP_SCALING)) / 3600,
          (((int)(timescale / TIME_FIXP_SCALING)) % 3600) / 60,
          ((int)(timescale / TIME_FIXP_SCALING)) % 60,
          ((int)(timescale % TIME_FIXP_SCALING)) / 1000);
  remove_trailing_zeros(str2_2048);
  parentItem->appendRow(new QStandardItem(str2_2048));
  xml_go_up(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "ecg_view_mode", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(!strcmp(result, "1"))
    {
      parentItem->appendRow(new QStandardItem("ECG view mode: enabled"));
    }

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "edf_files", 0))
  {
    view_session_format_error(__FILE__, __LINE__, xml_hdl);
    return;
  }
  for(i=0; i<MAXFILES; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "file", i))
    {
      break;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    strlcpy(e_file_path[i], result, MAX_PATH_LENGTH);

    if(i == ref_file)
    {
      snprintf(edf_path_2048, 2048, "File (reference): %.2000s", result);
    }
    else
    {
      snprintf(edf_path_2048, 2048, "File: %.2000s", result);
    }

    if(use_relative_path)
    {
      get_directory_from_path(path_r, session_path, MAX_PATH_LENGTH);
      strlcat(path_r, "/", MAX_PATH_LENGTH);
      strlcat(path_r, result, MAX_PATH_LENGTH);
      sanitize_path(path_r);
      f_test = fopeno(path_r, "rb");
    }
    else
    {
      f_test = fopeno(result, "rb");
    }

    if(f_test == NULL)
    {
      strlcat(edf_path_2048, "  (not found!)", 2048);
      tmp_item = new QStandardItem(edf_path_2048);
      tmp_item->setIcon(QIcon(":/images/delete_16x16.png"));
      parentItem->appendRow(tmp_item);
    }
    else
    {
      fclose(f_test);
      f_test = NULL;
      parentItem->appendRow(new QStandardItem(edf_path_2048));
    }
    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "viewtime", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    viewtime = atoll(result);
    if(viewtime < 0)
    {
      viewtime *= -1;
      snprintf(str2_2048, 2048, "File position: -%i:%02i:%02i.%04i",
                           ((int)(viewtime / TIME_FIXP_SCALING)) / 3600,
                           (((int)(viewtime / TIME_FIXP_SCALING)) % 3600) / 60,
                           ((int)(viewtime / TIME_FIXP_SCALING)) % 60,
                           ((int)(viewtime % TIME_FIXP_SCALING)) / 1000);
    }
    else
    {
      snprintf(str2_2048, 2048, "File position: %i:%02i:%02i.%04i",
                           ((int)(viewtime / TIME_FIXP_SCALING)) / 3600,
                           (((int)(viewtime / TIME_FIXP_SCALING)) % 3600) / 60,
                           ((int)(viewtime / TIME_FIXP_SCALING)) % 60,
                           ((int)(viewtime % TIME_FIXP_SCALING)) / 1000);
    }
    remove_trailing_zeros(str2_2048);
    parentItem->appendRow(new QStandardItem(str2_2048));
    xml_go_up(xml_hdl);
  }
  xml_go_up(xml_hdl);

  for(signalcomps=0; ; signalcomps++)
  {
    xml_goto_root(xml_hdl);

    signals_read = 0;

    filter_cnt = 0;
    ravg_filter_cnt = 0;
    fidfilter_cnt = 0;
    fir_filter_cnt = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalcomposition", signalcomps_read))
    {
      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "file", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    snprintf(edf_path_2048, 2048, "File: %.2000s", result);
    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "num_of_signals", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    signal_cnt = atoi(result);
    if((signal_cnt<1)||(signal_cnt>256))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }

    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "voltpercm", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    voltpercm = atof(result);
    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "screen_offset_unit", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      screen_offset_unit = atof(result);
      screen_offset_pix = -screen_offset_unit / (mainwindow->y_pixelsizefactor * voltpercm);
      xml_go_up(xml_hdl);
    }
    else if(!xml_goto_nth_element_inside(xml_hdl, "screen_offset", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        screen_offset_pix = atof(result);
        screen_offset_unit = -screen_offset_pix * mainwindow->y_pixelsizefactor * voltpercm;
        xml_go_up(xml_hdl);
      }

    if(!(xml_goto_nth_element_inside(xml_hdl, "polarity", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      polarity = atoi(result);
      if(polarity != -1)
      {
        polarity = 1;
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    color = atoi(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "spike_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      spike_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_before", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      math_func_cnt_before = atoi(result);
      if(math_func_cnt_before < 0)  math_func_cnt_before = 0;
      if(math_func_cnt_before > MAXMATHFUNCS)  math_func_cnt_before = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_after", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      math_func_cnt_after = atoi(result);
      if(math_func_cnt_after < 0)  math_func_cnt_after = 0;
      if(math_func_cnt_after > MAXMATHFUNCS)  math_func_cnt_after = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ravg_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      ravg_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "fir_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      fir_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "fidfilter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      fidfilter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    composition_txt_2048[0] = 0;

    if(!(xml_goto_nth_element_inside(xml_hdl, "alias", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(result[0] != 0)
      {
        strlcpy(composition_txt_2048, "alias: ", 2048);
        strlcat(composition_txt_2048, result, 2048);
        strlcat(composition_txt_2048, "   ", 2048);
      }
      xml_go_up(xml_hdl);
    }

    for(signals_read=0; signals_read<signal_cnt; signals_read++)
    {
      idx_used = 0;

      if(xml_goto_nth_element_inside(xml_hdl, "signal", signals_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "factor", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      factor[signals_read] = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "label", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "edfindex", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        idx_used = 1;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(idx_used)
      {
        strlcpy(label_256, "index: ", 256);
      }
      else
      {
        strlcpy(label_256, "label: ", 256);
      }
      strlcat(label_256, result, 256);

      snprintf(str3_64, 64, "%+f", factor[signals_read]);

      remove_trailing_zeros(str3_64);

      snprintf(composition_txt_2048 + strlen(composition_txt_2048), 2048 - strlen(composition_txt_2048), "%sx %s", str3_64, label_256);

      remove_trailing_spaces(composition_txt_2048);

      strlcat(composition_txt_2048, "   ", 2048);

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(polarity == -1)
    {
      strlcat(composition_txt_2048, "inverted: yes", 2048);
    }

    signalItem = new QStandardItem(composition_txt_2048);

    parentItem->appendRow(signalItem);

    snprintf(composition_txt_2048, 2048, "amplitude: %f", voltpercm);

    remove_trailing_zeros(composition_txt_2048);

    snprintf(composition_txt_2048 + strlen(composition_txt_2048), 2048 - strlen(composition_txt_2048), "/cm  offset: %f", screen_offset_unit);

    remove_trailing_zeros(composition_txt_2048);

    strlcat(composition_txt_2048, "  color: ", 2048);

    switch(color)
    {
      case Qt::white       : strlcat(composition_txt_2048, "white", 2048);
                             signalItem->setIcon(QIcon(":/images/white_icon_16x16.png"));
                             break;
      case Qt::black       : strlcat(composition_txt_2048, "black", 2048);
                             signalItem->setIcon(QIcon(":/images/black_icon_16x16.png"));
                             break;
      case Qt::red         : strlcat(composition_txt_2048, "red", 2048);
                             signalItem->setIcon(QIcon(":/images/red_icon_16x16.png"));
                             break;
      case Qt::darkRed     : strlcat(composition_txt_2048, "dark red", 2048);
                             signalItem->setIcon(QIcon(":/images/darkred_icon_16x16.png"));
                             break;
      case Qt::green       : strlcat(composition_txt_2048, "green", 2048);
                             signalItem->setIcon(QIcon(":/images/green_icon_16x16.png"));
                             break;
      case Qt::darkGreen   : strlcat(composition_txt_2048, "dark green", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgreen_icon_16x16.png"));
                             break;
      case Qt::blue        : strlcat(composition_txt_2048, "blue", 2048);
                             signalItem->setIcon(QIcon(":/images/blue_icon_16x16.png"));
                             break;
      case Qt::darkBlue    : strlcat(composition_txt_2048, "dark blue", 2048);
                             signalItem->setIcon(QIcon(":/images/darkblue_icon_16x16.png"));
                             break;
      case Qt::cyan        : strlcat(composition_txt_2048, "cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/cyan_icon_16x16.png"));
                             break;
      case Qt::darkCyan    : strlcat(composition_txt_2048, "dark cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/darkcyan_icon_16x16.png"));
                             break;
      case Qt::magenta     : strlcat(composition_txt_2048, "magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/magenta_icon_16x16.png"));
                             break;
      case Qt::darkMagenta : strlcat(composition_txt_2048, "dark magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/darkmagenta_icon_16x16.png"));
                             break;
      case Qt::yellow      : strlcat(composition_txt_2048, "yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/yellow_icon_16x16.png"));
                             break;
      case Qt::darkYellow  : strlcat(composition_txt_2048, "dark yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/darkyellow_icon_16x16.png"));
                             break;
      case Qt::gray        : strlcat(composition_txt_2048, "gray", 2048);
                             signalItem->setIcon(QIcon(":/images/gray_icon_16x16.png"));
                             break;
      case Qt::darkGray    : strlcat(composition_txt_2048, "dark gray", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgray_icon_16x16.png"));
                             break;
      case Qt::lightGray   : strlcat(composition_txt_2048, "light gray", 2048);
                             signalItem->setIcon(QIcon(":/images/lightgray_icon_16x16.png"));
                             break;
    }

    signalItem->appendRow(new QStandardItem(edf_path_2048));

    signalItem->appendRow(new QStandardItem(composition_txt_2048));


    if(math_func_cnt_before)
    {
      math_item_before = new QStandardItem("Math functions (before filtering)");

      signalItem->appendRow(math_item_before);

      for(math_funcs_before_read=0; math_funcs_before_read<math_func_cnt_before; math_funcs_before_read++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "math_func_before", math_funcs_before_read))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        math_func = atoi(result);
        if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        xml_go_up(xml_hdl);

        if(xml_goto_nth_element_inside(xml_hdl, "pk_hold_smpls", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        math_func_pk_hold_smpls = atoi(result);
        if(math_func_pk_hold_smpls < 0)
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        strlcpy(str3_64, "Math function: ", 64);
        get_math_func_descr(math_func, str3_64 + strlen(str3_64), 64 - strlen(str3_64));
        if(math_func == MATH_FUNC_PK_HOLD)
        {
          snprintf(str3_64 + strlen(str3_64), 64 - strlen(str3_64), " %i smpls", math_func_pk_hold_smpls);
        }
        math_item_before->appendRow(new QStandardItem(str3_64));

        xml_go_up(xml_hdl);
        xml_go_up(xml_hdl);
      }
    }

    filterItem = new QStandardItem("Filters");

    filterItem->setIcon(QIcon(":/images/filter_lowpass_small.png"));

    signalItem->appendRow(filterItem);

    for(filters_read=0; filters_read<spike_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "spike_filter", filters_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "velocity", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      velocity = atof(result);
      if(velocity < 0.0001)  velocity = 0.0001;
      if(velocity > 10E9)  velocity = 10E9;

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "holdoff", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      holdoff = atoi(result);
      if(holdoff < 10)  holdoff = 10;
      if(holdoff > 1000)  holdoff = 1000;

      snprintf(composition_txt_2048, 2048, "Spike: Velocity: %.8f", velocity);

      remove_trailing_zeros(composition_txt_2048);

      snprintf(composition_txt_2048 + strlen(composition_txt_2048), 2048 - strlen(composition_txt_2048), "  Hold-off: %i milli-Sec.", holdoff);

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "filter", filters_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "LPF", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      islpf = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      frequency = atof(result);

      if(islpf)
      {
        snprintf(composition_txt_2048, 2048, "LPF: %fHz", frequency);
      }
      else
      {
        snprintf(composition_txt_2048, 2048, "HPF: %fHz", frequency);
      }

      remove_trailing_zeros(composition_txt_2048);

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<ravg_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "ravg_filter", filters_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      size = atoi(result);

      if(type == 0)
      {
        snprintf(composition_txt_2048, 2048, "highpass moving average %ismpls", size);
      }

      if(type == 1)
      {
        snprintf(composition_txt_2048, 2048, "lowpass moving average %ismpls", size);
      }

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fidfilter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fidfilter", filters_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      frequency = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency2", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      frequency2 = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "ripple", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      ripple = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "order", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      order = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "model", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      model = atoi(result);

      if(type == 0)
      {
        if(model == 0)
        {
          snprintf(composition_txt_2048, 2048, "highpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt_2048, 2048, "highpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt_2048, 2048, "highpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          snprintf(composition_txt_2048, 2048, "lowpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt_2048, 2048, "lowpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt_2048, 2048, "lowpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 2)
      {
        snprintf(composition_txt_2048, 2048, "notch %fHz Q-factor %i  ", frequency, order);
      }

      if(type == 3)
      {
        if(model == 0)
        {
          snprintf(composition_txt_2048, 2048, "bandpass Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt_2048, 2048, "bandpass Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt_2048, 2048, "bandpass Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          snprintf(composition_txt_2048, 2048, "bandstop Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt_2048, 2048, "bandstop Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt_2048, 2048, "bandstop Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      remove_trailing_zeros(composition_txt_2048);

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(math_func_cnt_after)
    {
      math_item_after = new QStandardItem("Math functions (after filtering)");

      signalItem->appendRow(math_item_after);

      for(math_funcs_after_read=0; math_funcs_after_read<math_func_cnt_after; math_funcs_after_read++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "math_func_after", math_funcs_after_read))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        math_func = atoi(result);
        if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        xml_go_up(xml_hdl);

        if(xml_goto_nth_element_inside(xml_hdl, "pk_hold_smpls", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        math_func_pk_hold_smpls = atoi(result);
        if(math_func_pk_hold_smpls < 0)
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        strlcpy(str3_64, "Math function: ", 64);
        get_math_func_descr(math_func, str3_64 + strlen(str3_64), 64 - strlen(str3_64));
        if(math_func == MATH_FUNC_PK_HOLD)
        {
          snprintf(str3_64 + strlen(str3_64), 64 - strlen(str3_64), " %i smpls", math_func_pk_hold_smpls);
        }
        math_item_after->appendRow(new QStandardItem(str3_64));

        xml_go_up(xml_hdl);
        xml_go_up(xml_hdl);
      }
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      snprintf(composition_txt_2048, 2048, "ECG powerline interference subtraction: %iHz", plif_powerlinefrequency);

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_eeg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      snprintf(composition_txt_2048, 2048, "EEG powerline interference subtraction: %iHz", plif_powerlinefrequency);

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fir_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fir_filter", filters_read))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(!xml_goto_nth_element_inside(xml_hdl, "description", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        strlcpy(str2_2048, result, 512);

        xml_go_up(xml_hdl);
      }
      else
      {
        str2_2048[0] = 0;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      n_taps = atoi(result);
      if((n_taps < 2) || (n_taps > 4000))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      xml_go_up(xml_hdl);

      fir_vars = (double *)malloc(sizeof(double) * n_taps);
      if(fir_vars == NULL)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "tap", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      fir_vars[0] = atof(result);

      for(r=1; r<n_taps; r++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        fir_vars[r] = atof(result);
      }

      xml_go_up(xml_hdl);

      if(!strlen(str2_2048))
      {
        snprintf(str2_2048, 2048, "Custom FIR filter with %i taps", n_taps);
      }

      firfilterItem = new QStandardItem(str2_2048);

      filterItem->appendRow(firfilterItem);

      for(r=0; r<n_taps; r++)
      {
        snprintf(str2_2048, 2048, " %.24f ", fir_vars[r]);

        firfilterItem->appendRow(new QStandardItem(str2_2048));
      }

      free(fir_vars);
      fir_vars = NULL;

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      type = atoi(result);

      if(type == 1)
      {
        snprintf(composition_txt_2048, 2048, "ECG heartrate detection");
      }

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "zratio_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);

      if(type == 1)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "crossoverfreq", 0))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }
        d_tmp = atof(result);

        snprintf(composition_txt_2048, 2048, "Z-ratio  cross-over frequency is %.1f", d_tmp);
      }

      filterItem->appendRow(new QStandardItem(composition_txt_2048));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    signalcomps_read++;
  }

  xml_goto_root(xml_hdl);

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "powerspectrumdock", i))
    {
      break;
    }

    powerspectrumdockItem = new QStandardItem("Power Spectrum");

    parentItem->appendRow(powerspectrumdockItem);

    if(xml_goto_nth_element_inside(xml_hdl, "signalnum", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      sigcomp_idx = atoi(result);
      if(sigcomp_idx < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Signal index: %i", sigcomp_idx + 1);
      powerspectrumdockItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "dftblocksize", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      dftblocksize = atoi(result);
      if(dftblocksize < 1)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "FFT blocksize: %i samples", dftblocksize);
      powerspectrumdockItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "overlap", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      overlap = atoi(result);
      if(overlap < 1)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      switch(overlap)
      {
        case 1 : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: 0%"));
                 break;
        case 2 : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: 50%"));
                 break;
        case 3 : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: 67%"));
                 break;
        case 4 : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: 75%"));
                 break;
        case 5 : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: 80%"));
                 break;
        default : powerspectrumdockItem->appendRow(new QStandardItem("Overlap: ??%"));
                 break;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "window_type", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      window_func = atoi(result);
      if((window_func < 0) || (window_func > 12))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      switch(window_func)
      {
        case FFT_WNDW_TYPE_RECT                  : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: None"));
                break;
        case FFT_WNDW_TYPE_HAMMING               : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Hamming"));
                break;
        case FFT_WNDW_TYPE_4TERM_BLACKMANHARRIS  : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: 4-term Blackman-Harris"));
                break;
        case FFT_WNDW_TYPE_7TERM_BLACKMANHARRIS  : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: 7-term Blackman-Harris"));
                break;
        case FFT_WNDW_TYPE_NUTTALL3B             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Nuttall3b"));
                break;
        case FFT_WNDW_TYPE_NUTTALL4C             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Nuttall4c"));
                break;
        case FFT_WNDW_TYPE_HANN                  : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Hann"));
                break;
        case FFT_WNDW_TYPE_HFT223D               : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: HFT223D"));
                break;
        case FFT_WNDW_TYPE_HFT95                 : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: HFT95"));
                break;
        case FFT_WNDW_TYPE_KAISER_A2             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Kaiser2"));
                break;
        case FFT_WNDW_TYPE_KAISER_A3             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Kaiser3"));
                break;
        case FFT_WNDW_TYPE_KAISER_A4             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Kaiser4"));
                break;
        case FFT_WNDW_TYPE_KAISER_A5             : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: Kaiser5"));
                break;
        default                                  : powerspectrumdockItem->appendRow(new QStandardItem("FFT window function: ??"));
                break;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "hypnogram", i))
    {
      break;
    }

    hypnogramItem = new QStandardItem("hypnogram");

    parentItem->appendRow(hypnogramItem);

    if(xml_goto_nth_element_inside(xml_hdl, "hdr_idx", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      hdr_idx = atoi(result);
      if(hdr_idx < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "File: %s", e_file_path[hdr_idx]);
      hypnogramItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "stage_name", j))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        snprintf(str2_2048, 2048, "Stage %i name: %.2000s", j + 1, result);

        xml_go_up(xml_hdl);

        if(xml_goto_nth_element_inside(xml_hdl, "annot_name", j))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        strlcat(str2_2048, "   Annotation: ", 2048);
        strlcat(str2_2048, result, 2048);
        hypnogramItem->appendRow(new QStandardItem(str2_2048));

        xml_go_up(xml_hdl);
      }
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "use_overlays", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(atoi(result) == 1)
      {
        hypnogramItem->appendRow(new QStandardItem(" use overlays: yes"));
      }
      else
      {
        hypnogramItem->appendRow(new QStandardItem(" use overlays: no"));
      }

      xml_go_up(xml_hdl);
    }

    for(j=0; j<HYPNOGRAM_OVNUM; j++)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "annot_ov_name", j))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        xml_go_up(xml_hdl);

        snprintf(str2_2048, 2048, "Overlay annotation: %s", result);
        tmp_item = new QStandardItem(str2_2048);

        mainwindow->get_rgbcolor_settings(xml_hdl, "annot_ov_color", j, &tmp_color);
        if(tmp_color.isValid() == true)
        {
          tmp_item->setBackground(QBrush(tmp_color));
        }

        hypnogramItem->appendRow(tmp_item);
      }
    }

    xml_go_up(xml_hdl);
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "cdsa", i))
    {
      break;
    }

    cdsaItem = new QStandardItem("CDSA");

    parentItem->appendRow(cdsaItem);

    if(xml_goto_nth_element_inside(xml_hdl, "sigcomp_idx", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      sigcomp_idx = atoi(result);
      if(sigcomp_idx < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Signal index: %i", sigcomp_idx + 1);
      cdsaItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "min_hz", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      min_hz = atoi(result);
      if(min_hz < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Lower frequency bin: %i Hz", min_hz);
      cdsaItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "max_hz", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      max_hz = atoi(result);
      if(max_hz < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Upper frequency bin: %i Hz", max_hz);
      cdsaItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "segment_len", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      segment_len = atoi(result);
      if(segment_len < 1)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Segment length: %i seconds", segment_len);
      cdsaItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "block_len", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      block_len = atoi(result);
      if(block_len < 1)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Block length: %i seconds", block_len);
      cdsaItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "overlap", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      overlap = atoi(result);
      if(overlap < 1)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      switch(overlap)
      {
        case 1 : cdsaItem->appendRow(new QStandardItem("Overlap: 0%"));
                 break;
        case 2 : cdsaItem->appendRow(new QStandardItem("Overlap: 50%"));
                 break;
        case 3 : cdsaItem->appendRow(new QStandardItem("Overlap: 67%"));
                 break;
        case 4 : cdsaItem->appendRow(new QStandardItem("Overlap: 75%"));
                 break;
        case 5 : cdsaItem->appendRow(new QStandardItem("Overlap: 80%"));
                 break;
        default : cdsaItem->appendRow(new QStandardItem("Overlap: ??%"));
                 break;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "window_func", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      window_func = atoi(result);
      if((window_func < 0) || (window_func > 12))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      switch(window_func)
      {
        case FFT_WNDW_TYPE_RECT                  : cdsaItem->appendRow(new QStandardItem("FFT window function: None"));
                break;
        case FFT_WNDW_TYPE_HAMMING               : cdsaItem->appendRow(new QStandardItem("FFT window function: Hamming"));
                break;
        case FFT_WNDW_TYPE_4TERM_BLACKMANHARRIS  : cdsaItem->appendRow(new QStandardItem("FFT window function: 4-term Blackman-Harris"));
                break;
        case FFT_WNDW_TYPE_7TERM_BLACKMANHARRIS  : cdsaItem->appendRow(new QStandardItem("FFT window function: 7-term Blackman-Harris"));
                break;
        case FFT_WNDW_TYPE_NUTTALL3B             : cdsaItem->appendRow(new QStandardItem("FFT window function: Nuttall3b"));
                break;
        case FFT_WNDW_TYPE_NUTTALL4C             : cdsaItem->appendRow(new QStandardItem("FFT window function: Nuttall4c"));
                break;
        case FFT_WNDW_TYPE_HANN                  : cdsaItem->appendRow(new QStandardItem("FFT window function: Hann"));
                break;
        case FFT_WNDW_TYPE_HFT223D               : cdsaItem->appendRow(new QStandardItem("FFT window function: HFT223D"));
                break;
        case FFT_WNDW_TYPE_HFT95                 : cdsaItem->appendRow(new QStandardItem("FFT window function: HFT95"));
                break;
        case FFT_WNDW_TYPE_KAISER_A2             : cdsaItem->appendRow(new QStandardItem("FFT window function: Kaiser2"));
                break;
        case FFT_WNDW_TYPE_KAISER_A3             : cdsaItem->appendRow(new QStandardItem("FFT window function: Kaiser3"));
                break;
        case FFT_WNDW_TYPE_KAISER_A4             : cdsaItem->appendRow(new QStandardItem("FFT window function: Kaiser4"));
                break;
        case FFT_WNDW_TYPE_KAISER_A5             : cdsaItem->appendRow(new QStandardItem("FFT window function: Kaiser5"));
                break;
        default                                  : cdsaItem->appendRow(new QStandardItem("FFT window function: ??"));
                break;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "log", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      log = atoi(result);
      if((log < 0) || (log > 1))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(log)
      {
        cdsaItem->appendRow(new QStandardItem("Logarithmic: yes"));
      }
      else
      {
        cdsaItem->appendRow(new QStandardItem("Logarithmic: no"));
      }

      xml_go_up(xml_hdl);
    }

    if(log)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "max_pwr", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        max_pwr = atoi(result);

        snprintf(str2_2048, 2048, "Max. level: %idB", max_pwr);

        cdsaItem->appendRow(new QStandardItem(str2_2048));

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "min_pwr", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        min_pwr = atoi(result);

        snprintf(str2_2048, 2048, "Min. level: %idB", min_pwr);

        cdsaItem->appendRow(new QStandardItem(str2_2048));

        xml_go_up(xml_hdl);
      }

    }
    else
    {
      if(xml_goto_nth_element_inside(xml_hdl, "max_voltage", 0))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

        max_voltage = atof(result);

        snprintf(str2_2048, 2048, "Max. level: %e", max_voltage);

        cdsaItem->appendRow(new QStandardItem(str2_2048));

        xml_go_up(xml_hdl);
      }
    }

    if(xml_goto_nth_element_inside(xml_hdl, "power_voltage", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      power_voltage = atoi(result);
      if((power_voltage < 0) || (power_voltage > 1))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(power_voltage)
      {
        cdsaItem->appendRow(new QStandardItem("Power: yes"));
      }
      else
      {
        cdsaItem->appendRow(new QStandardItem("Power: no"));
      }

      xml_go_up(xml_hdl);
    }

      xml_go_up(xml_hdl);
  }

  for(i=0; i<MAXAEEGDOCKS; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "aeeg", i))
    {
      break;
    }

    aeegItem = new QStandardItem("aEEG");

    parentItem->appendRow(aeegItem);

    if(xml_goto_nth_element_inside(xml_hdl, "sigcomp_idx", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      sigcomp_idx = atoi(result);
      if(sigcomp_idx < 0)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Signal index: %i", sigcomp_idx + 1);
      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "bp_min_hz", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      bp_min_hz = atof(result);
      if(bp_min_hz < 0.999)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Bandpass min. frequency: %.1f Hz", bp_min_hz);
      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "bp_max_hz", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      bp_max_hz = atof(result);
      if(bp_max_hz < 1.999)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Bandpass max. frequency: %.1f Hz", bp_max_hz);
      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "segment_len", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      segment_len = atoi(result);
      if(segment_len < 5)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Segment length: %i seconds", segment_len);
      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "lp_hz", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      lp_hz = atof(result);
      if((lp_hz < 0.009999) || (lp_hz > 5.00001))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Envelope lowpass filter: %.2f sec", lp_hz);
      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "scale_max_amp", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      scale_max_amp = atof(result);

      snprintf(str2_2048, 2048, "Max. amplitude: %.1f", scale_max_amp);

      aeegItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  video_path_2048[0] = 0;

  if(!xml_goto_nth_element_inside(xml_hdl, "video", 0))
  {
    videoItem = new QStandardItem("Video");

    parentItem->appendRow(videoItem);

    if(xml_goto_nth_element_inside(xml_hdl, "file", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(strlen(result) < 5)
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(video_path_2048, 2048, "Video: %.2000s", result);

      if(use_relative_path)
      {
        get_directory_from_path(path_r, session_path, MAX_PATH_LENGTH);
        strlcat(path_r, "/", MAX_PATH_LENGTH);
        strlcat(path_r, result, MAX_PATH_LENGTH);
        sanitize_path(path_r);
        f_test = fopeno(path_r, "rb");
      }
      else
      {
        f_test = fopeno(result, "rb");
      }

      if(f_test == NULL)
      {
        strlcat(video_path_2048, "  (not found!)", 2048);
        tmp_item = new QStandardItem(video_path_2048);
        tmp_item->setIcon(QIcon(":/images/delete_16x16.png"));
        videoItem->appendRow(tmp_item);
      }
      else
      {
        fclose(f_test);
        f_test = NULL;
        videoItem->appendRow(new QStandardItem(video_path_2048));
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "starttime", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      utc_starttime = atoll(result);
      if((utc_starttime < 0) || (utc_starttime > 16725225600))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      utc_to_date_time(utc_starttime, &dt);

      snprintf(str2_2048, 2048, "Start: %i-%s-%04i %02i:%02i:%02i",
                        dt.day, dt.month_str, dt.year, dt.hour, dt.minute, dt.second);

      videoItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "seek", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      video_seek = atoi(result);
      if((video_seek < 0) || (video_seek > 3000000))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      snprintf(str2_2048, 2048, "Position: %i:%02i:%02i", video_seek / 3600, (video_seek % 3600) / 60, video_seek % 60);

      videoItem->appendRow(new QStandardItem(str2_2048));

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "paused", 0))
    {
      view_session_format_error(__FILE__, __LINE__, xml_hdl);
      return;
    }
    else
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        view_session_format_error(__FILE__, __LINE__, xml_hdl);
        return;
      }

      if(!strcmp(result, "0"))
      {
        videoItem->appendRow(new QStandardItem("Paused: no"));
      }
      else if(!strcmp(result, "1"))
        {
          videoItem->appendRow(new QStandardItem("Paused: yes"));
        }
        else
        {
          view_session_format_error(__FILE__, __LINE__, xml_hdl);
          return;
        }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

//  tree->expandAll();
}


int UI_ViewSessionwindow::view_session_format_error(const char *file_name, int line_number, xml_hdl_t *hdl)
{
  char str[2048]="";

  snprintf(str, 2048, "There seems to be an error in this session file.\nFile: %s\nline: %i", file_name, line_number);

  QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
  messagewindow.exec();
  xml_close(hdl);
  return 0;
}













