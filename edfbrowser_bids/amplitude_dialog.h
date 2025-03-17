/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2024 Teunis van Beelen
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




#ifndef AMPLITUDEFORM1_H
#define AMPLITUDEFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"


class UI_Mainwindow;



class UI_Userdefined_amplitude_Dialog : public QObject
{
  Q_OBJECT

public:

  UI_Userdefined_amplitude_Dialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;

private:

QDialog      *set_amplitude_dialog;

QPushButton  *CloseButton,
             *OKButton;

QDoubleSpinBox *ScaleBox;

private slots:

void okbutton_pressed();

};



#endif // AMPLITUDEFORM1_H


