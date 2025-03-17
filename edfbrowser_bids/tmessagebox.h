/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2024 Teunis van Beelen
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


#ifndef UI_TMESSAGEBOXFORM_H
#define UI_TMESSAGEBOXFORM_H


#include <QObject>
#include <QMessageBox>


class tmessagebox : public QObject
{
  Q_OBJECT

public:
  tmessagebox(QMessageBox::Icon, const char *, const char *, const char *, QWidget *w_parent=NULL);
};

#endif









