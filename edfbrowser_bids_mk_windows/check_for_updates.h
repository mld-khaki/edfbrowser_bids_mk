/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013 - 2024 Teunis van Beelen
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


#ifndef CHECKFORUPDATES_H
#define CHECKFORUPDATES_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "utils.h"
#include "mainwindow.h"



class Check_for_updates : public QObject
{
  Q_OBJECT

public:
  Check_for_updates(UI_Mainwindow *);

private:

  QNetworkAccessManager *manager;

  UI_Mainwindow *mainwindow;

private slots:

  void replyFinished();

};




#endif


