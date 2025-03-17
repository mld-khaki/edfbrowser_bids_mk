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




#include "check_for_updates.h"





Check_for_updates::Check_for_updates(UI_Mainwindow *mw)
{
  QNetworkRequest request;
  QNetworkReply *reply;

  mainwindow = mw;

  manager = new QNetworkAccessManager(this);

  request.setUrl(QUrl("http://www.teuniz.net/edfbrowser/latest_version.txt"));
  request.setRawHeader("User-Agent", PROGRAM_NAME " " PROGRAM_VERSION " OS_UNKNOWN " THIS_APP_BITS_W);
#ifdef Q_OS_LINUX
  request.setRawHeader("User-Agent", PROGRAM_NAME " " PROGRAM_VERSION " Q_OS_LINUX " THIS_APP_BITS_W);
#elif defined (Q_OS_MACOS)
  request.setRawHeader("User-Agent", PROGRAM_NAME " " PROGRAM_VERSION " Q_OS_MACOS " THIS_APP_BITS_W);
#elif defined (Q_OS_WIN32)
  request.setRawHeader("User-Agent", PROGRAM_NAME " " PROGRAM_VERSION " Q_OS_WIN32 " THIS_APP_BITS_W);
#else
  request.setRawHeader("User-Agent", PROGRAM_NAME " " PROGRAM_VERSION " Q_OS_OTHER " THIS_APP_BITS_W);
#endif
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

  reply = manager->get(request);

  connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}


void Check_for_updates::replyFinished()
{
  long long int n;

  char buf_128[128];

  int this_version,
      latest_version;

  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

  if(reply->error() != QNetworkReply::NoError)
  {
    reply->deleteLater();

    return;
  }

  if(reply->bytesAvailable() < 31)
  {
    reply->deleteLater();

    return;
  }

  n = reply->read(buf_128, 100);

  if(n < 31)
  {
    reply->deleteLater();

    return;
  }

  reply->deleteLater();

  buf_128[30] = 0;

  if(strncmp(buf_128, "EDFbrowser latest version: ", 27))
  {
    return;
  }

  mainwindow->last_check_for_updates_time = time(NULL);

  if(is_integer_number(buf_128 + 27))
  {
    return;
  }

  latest_version = atoi(buf_128 + 27);

  if((latest_version < 1) || (latest_version > 1000000))
  {
    return;
  }

  snprintf(buf_128, 128, PROGRAM_VERSION);

  buf_128[1] = buf_128[0];

  this_version = atoi(buf_128 + 1);

  if(this_version >= latest_version)
  {
    return;
  }

  QMessageBox messagewindow(QMessageBox::Information,
                            "New version available",
                            "A newer version of EDFbrowser is available.\n"
                            "Do you want to download the new version now?",
                            QMessageBox::Yes | QMessageBox::No);

  if(messagewindow.exec() != QMessageBox::Yes)
  {
    return;
  }

  QDesktopServices::openUrl(QUrl("https://www.teuniz.net/edfbrowser/"));
}










