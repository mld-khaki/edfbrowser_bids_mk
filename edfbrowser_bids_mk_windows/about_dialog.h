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




#ifndef ABOUT_FORM1_H
#define ABOUT_FORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Aboutwindow : public QObject
{
  Q_OBJECT

public:
  UI_Aboutwindow(UI_Mainwindow *);

private:

QDialog      *AboutDialog;

QPushButton  *pushButton1;

QTextEdit    *textedit1;

QHBoxLayout  *hlayout1;

QVBoxLayout  *vlayout1;

};



#endif // ABOUT_FORM1_H


