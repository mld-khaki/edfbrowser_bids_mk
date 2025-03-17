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




#ifndef SIGNALSFORM1_H
#define SIGNALSFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "utc_date_time.h"
#include "special_button.h"
#include "utils.h"
#include "edf_helper.h"


class UI_Mainwindow;




class UI_Signalswindow : public QObject
{
  Q_OBJECT

public:
  UI_Signalswindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *SignalsDialog;

  QListWidget  *filelist,
               *signallist;

  QTableWidget *compositionlist;

  QLabel       *label1,
               *label2,
               *label3,
               *label4,
               *label5,
               *label6,
               *label7;

  QPushButton  *CloseButton,
               *SelectAllButton,
               *DisplayButton,
               *DisplayCompButton,
               *AddButton,
               *SubtractButton,
               *RemoveButton,
               *HelpButton;

  SpecialButton *ColorButton;

  int smp_per_record,
      curve_color,
      default_color_list[32],
      default_color_idx,
      color_selected;

  char physdimension[64];

  double bitvalue;

  void AddSubtractButtonsClicked(int);

 private slots:

  void show_signals(int);
  void SelectAllButtonClicked();
  void DisplayButtonClicked();
  void DisplayCompButtonClicked();
  void AddButtonClicked();
  void SubtractButtonClicked();
  void RemoveButtonClicked();
  void ColorButtonClicked(SpecialButton *);
  void HelpButtonClicked();
  void selected_signals_changed();

};



#endif // SIGNALSFORM1_H


