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



#ifndef ADD_MATHFORM1_H
#define ADD_MATHFORM1_H


#include "qt_headers.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"

#include "filt/math_func.h"


class UI_Mainwindow;


class UI_MATH_func_dialog : public QObject
{
  Q_OBJECT

public:
  UI_MATH_func_dialog(QWidget *);

  UI_Mainwindow *mainwindow;


private:

QDialog        *mathdialog;

QRadioButton  *sqrt_rbutton,
              *square_rbutton,
              *abs_rbutton,
              *pk_hold_rbutton,
              *before_rbutton,
              *after_rbutton;

QSpinBox      *pk_hold_spinbox;

QGroupBox     *func_bgroup,
              *when_bgroup;

QPushButton    *close_button,
               *apply_button;

QListWidget    *list;

QLabel         *listlabel;

private slots:

void apply_button_clicked();
void pk_hold_rbutton_clicked(bool);

};


#endif









