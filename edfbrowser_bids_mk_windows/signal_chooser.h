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




#ifndef SIGNAL_CHOOSERFORM1_H
#define SIGNAL_CHOOSERFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "viewcurve.h"
#include "adjustfiltersettings.h"
#include "utils.h"
#include "edf_helper.h"
#include "cdsa_dialog.h"
#include "cdsa_dock.h"
#include "aeeg_dialog.h"
#include "aeeg_dock.h"
#include "hypnogram_dialog.h"
#include "hypnogram_dock.h"

#include "filt/ecg_filter.h"
#include "filt/plif_ecg_subtract_filter.h"
#include "filt/plif_eeg_subtract_filter.h"



class UI_Mainwindow;
class ViewCurve;



class UI_SignalChooser : public QObject
{
  Q_OBJECT

public:
  UI_SignalChooser(QWidget *, int, int *sgnl_nr = NULL);

  UI_Mainwindow *mainwindow;


private:

QDialog      *signalchooser_dialog;

QPushButton  *CloseButton,
             *UpButton,
             *DownButton,
             *DeleteButton,
             *InvertButton,
             *EditButton;

QListWidget  *list;

int task,
    *signal_nr;

void load_signalcomps(void);

int get_selectionlist(int *);

private slots:

void call_sidemenu(QListWidgetItem *);
void signalUp();
void signalDown();
void signalDelete();
void signalInvert();
void signalEdit();
void item_activated(QListWidgetItem *);

};



#endif // SIGNAL_CHOOSERFORM1_H


