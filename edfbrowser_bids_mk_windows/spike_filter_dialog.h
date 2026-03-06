/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 - 2024 Teunis van Beelen
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



#ifndef ADD_SPIKE_FILTERFORM1_H
#define ADD_SPIKE_FILTERFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"

#include "filt/spike_filter.h"



class UI_Mainwindow;



class UI_SpikeFilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_SpikeFilterDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *spikefilterdialog;

QPushButton    *CancelButton,
               *ApplyButton;

QListWidget    *list;

QDoubleSpinBox *velocitySpinBox;

QSpinBox       *holdOffSpinBox;


private slots:

void ApplyButtonClicked();

};



#endif // ADD_SPIKE_FILTERFORM1_H








