/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2019 - 2024 Teunis van Beelen
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


#ifndef UI_ISHNE2EDFFORM_H
#define UI_ISHNE2EDFFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edflib.h"



class UI_Mainwindow;


class UI_IshneEDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_IshneEDFwindow(QWidget *, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow  *mainwindow;

private:

QDialog       *myobjectDialog;

QTextEdit    *textEdit1;

QPushButton   *pushButton1,
              *pushButton2;

char *recent_opendir,
     *recent_savedir;

unsigned short crc_ccitt_table[256];

void crc_ccitt_init(void);

unsigned short crc_ccitt(const unsigned char *, int, unsigned short);

int check_crc(FILE *, int);

private slots:

void SelectFileButton();

};




#endif


