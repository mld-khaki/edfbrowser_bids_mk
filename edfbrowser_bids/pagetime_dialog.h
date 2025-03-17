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




#ifndef PAGETIMEFORM1_H
#define PAGETIMEFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"


class UI_Mainwindow;



class UI_Userdefined_timepage_Dialog : public QObject
{
  Q_OBJECT

public:

  UI_Userdefined_timepage_Dialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;

private:

QDialog      *set_diplaytime_dialog;

QPushButton  *CloseButton,
             *OKButton;

QTimeEdit    *timeEdit1;

QTime        time1;

QSpinBox     *ScaleBox;

QRadioButton *r_button_1,
             *r_button_2;

private slots:

void okbutton_pressed();

};



#endif // PAGETIMEFORM1_H


