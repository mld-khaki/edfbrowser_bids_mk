/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 - 2024 Teunis van Beelen
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



#ifndef ADD_PLIF_ECG_FILTERFORM1_H
#define ADD_PLIF_ECG_FILTERFORM1_H



#include "qt_headers.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"

#include "filt/plif_ecg_subtract_filter.h"



class UI_Mainwindow;



class UI_PLIF_ECG_filter_dialog : public QObject
{
  Q_OBJECT

public:
  UI_PLIF_ECG_filter_dialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *plifecgfilterdialog;

QPushButton    *CancelButton,
               *helpButton;

QListWidget    *list;

QComboBox      *plfBox;


private slots:

void ApplyButtonClicked();
void helpbuttonpressed();

};



#endif // ADD_PLIF_ECG_FILTERFORM1_H








