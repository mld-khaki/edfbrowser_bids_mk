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




#ifndef SHOWEDFHDRFORM1_H
#define SHOWEDFHDRFORM1_H



#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utc_date_time.h"
#include "utils.h"


class UI_Mainwindow;



class UI_EDFhdrwindow : public QObject
{
  Q_OBJECT

public:
  UI_EDFhdrwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

int signalslist_page,
    signalslist_num_pages,
    file_row_selected;

QDialog      *EDFhdrDialog;

QWidget      *tab1,
             *tab1a;

QListWidget  *filelist;

QTableWidget *signallist;

QPushButton  *closeButton,
             *prevPageButton,
             *nextPageButton;

QStackedWidget *stackwidget;

QLineEdit *line_edit1,
          *line_edit1a,
          *line_edit2,
          *line_edit2a,
          *line_edit3,
          *line_edit3a,
          *line_edit4,
          *line_edit4a,
          *line_edit5,
          *line_edit5a,
          *line_edit6,
          *line_edit7,
          *line_edit8,
          *line_edit9,
          *line_edit10,
          *line_edit11,
          *line_edit12,
          *line_edit13,
          *line_edit20,
          *line_edit20a,
          *line_edit21,
          *line_edit21a,
          *line_edit22,
          *line_edit22a,
          *line_edit23,
          *line_edit23a;

private slots:

void show_params(int);
void load_signal_params(int, int);
void prevPageButtonClicked();
void nextPageButtonClicked();

};



#endif // SHOWEDFHDRORM1_H


