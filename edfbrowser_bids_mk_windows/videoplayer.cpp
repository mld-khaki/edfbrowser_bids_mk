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


#include "mainwindow.h"


//#define DEBUG_VIDEOPLAYER

#ifdef DEBUG_VIDEOPLAYER
  FILE *debug_vpr;
#endif


void UI_Mainwindow::start_stop_video()
{
  int process_start_retries,
      sock_connect_retries,
      err,
      port,
      connected=0;

  char str[4096]={""};

  QEventLoop evlp;

  QMessageBox msgbox(QMessageBox::Critical, "Error", "text", QMessageBox::Close);

  QString qstr;

  if(video_player->status != VIDEO_STATUS_STOPPED)
  {
    stop_video_generic(0);
    video_pause_requested = 0;
    return;
  }

  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;
  }

  if(live_stream_active)
  {
    msgbox.setText("Cannot open a video during a live stream.");
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

  if(video_player->status != VIDEO_STATUS_STOPPED)
  {
    msgbox.setText("There is already a video running.");
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

  if(signalcomps < 1)
  {
    msgbox.setText("Put some signals on the screen first.");
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

//   if(annot_editor_active)
//   {
//     msgbox.setText("Close the annotation editor first.");
//     msgbox.exec();
//     video_pause_requested = 0;
//     return;
//   }

  if(!session_start_video)
  {
    strlcpy(videopath, QFileDialog::getOpenFileName(this, "Select media file", QString::fromLocal8Bit(recent_video_opendir),
                                                   "Video files (*.mkv *.mp4 *.mpg *.mpeg *.avi *.webm *.ogv *.ogg *.wmv *.mov *.m4v);;Audio files (*.wav *.ogg *.flac *.mp3 *.aac *.m4a *.wma);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(videopath, ""))
  {
    video_pause_requested = 0;
    return;
  }

  get_directory_from_path(recent_video_opendir, videopath, MAX_PATH_LENGTH);

  if(!session_start_video)
  {
    video_player->utc_starttime = parse_date_time_stamp(videopath);

    if(video_player->utc_starttime < 0LL)
    {
      msgbox.setIcon(QMessageBox::Warning);
      msgbox.setWindowTitle("Warning");
      msgbox.setText(" \nCannot find startdate and starttime in video filename.\n"
                     " \nAssume video starttime equals EDF/BDF starttime?\n ");
      msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
      msgbox.setDefaultButton(QMessageBox::Yes);
      if(msgbox.exec() == QMessageBox::Cancel)
      {
        video_pause_requested = 0;
        return;
      }

      video_player->utc_starttime = edfheaderlist[sel_viewtime]->utc_starttime;
    }
  }
  else
  {
    video_player->utc_starttime = session_video_starttime;
  }

  video_player->stop_det_counter = 0;

  video_player->fpos = 0;

  video_player->starttime_diff = (int)(edfheaderlist[sel_viewtime]->utc_starttime - video_player->utc_starttime);

  msgbox.setIcon(QMessageBox::Critical);
  msgbox.setWindowTitle("Error");
  msgbox.setStandardButtons(QMessageBox::Close);

  if((edfheaderlist[sel_viewtime]->utc_starttime + edfheaderlist[sel_viewtime]->recording_len_sec) < video_player->utc_starttime)
  {
    msgbox.setText("The video registration and the EDF/BDF registration do not overlap (in time)");
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

  if((video_player->utc_starttime + 259200LL) < edfheaderlist[sel_viewtime]->utc_starttime)
  {
    msgbox.setText("The video registration and the EDF/BDF registration do not overlap (in time)");
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

  QStringList arguments;

  msgbox.setIcon(QMessageBox::NoIcon);
  msgbox.setWindowTitle("Starting");
  msgbox.setStandardButtons(QMessageBox::Abort);
  msgbox.setText("   \n Starting the video player, please wait ... \n   ");
  msgbox.show();

  for(process_start_retries=0; process_start_retries<3; process_start_retries++)
  {
    connected = 0;

    arguments.clear();

    port = (time(NULL) % 1000) + 3000 + process_start_retries;

    snprintf(str, 4096, "localhost:%i", port);

    video_process = new QProcess(0);

#ifdef Q_OS_WIN32

    qstr = QDir::toNativeSeparators(QString::fromLocal8Bit(videopath));

    strlcpy(videopath, qstr.toLocal8Bit().data(), MAX_PATH_LENGTH);

    arguments << "-I" << "rc" << "--rc-host" << str << "--rc-quiet" << "--video-on-top" << "--width" << "150" << "--height" << "150" << "--ignore-config";

    video_process->start("C:\\Program Files\\VideoLAN\\VLC\\vlc.exe", arguments);

    if(video_process->waitForStarted(5000) == false)
    {
      video_process->start("C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe", arguments);

      if(video_process->waitForStarted(5000) == false)
      {
        QTimer::singleShot(300, &evlp, SLOT(quit()));
        evlp.exec();

        delete video_process;
        video_process = NULL;

        continue;
      }
    }
#else
    arguments << "-I" << "rc" << "--rc-host" << str << "--video-on-top" << "--width" << "150" << "--height" << "150" << "--ignore-config";

#ifdef Q_OS_MACOS
    video_process->start("/Applications/VLC.app/Contents/MacOS/VLC", arguments);
#else
    video_process->start("vlc", arguments);
#endif

    if(video_process->waitForStarted(5000) == false)
    {
      QTimer::singleShot(300, &evlp, SLOT(quit()));
      evlp.exec();

      delete video_process;
      video_process = NULL;

      continue;
    }
#endif
//    msgbox.setText("   \n Opening a socket to the video player, please wait ... \n   ");

    for(sock_connect_retries=0; sock_connect_retries<3; sock_connect_retries++)
    {
      QTimer::singleShot(3000, &evlp, SLOT(quit()));
      evlp.exec();

      vlc_sock = new QTcpSocket;
      vlc_sock->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
      vlc_sock->setSocketOption(QAbstractSocket::KeepAliveOption, QVariant(1));
      vlc_sock->connectToHost(QHostAddress("127.0.0.1"), port);
      if(vlc_sock->waitForConnected(5000) == true)
      {
        connected = 1;

        break;
      }
    }

    if(connected)  break;

    if(sock_connect_retries == 3)
    {
      if(process_start_retries == 2)
      {
        err = vlc_sock->error();

        snprintf(str, 4096, "   \n Cannot connect to the video player via localhost loopback port (error %i) \n   ", err);

        msgbox.setWindowTitle("Error");
        msgbox.setStandardButtons(QMessageBox::Close);
        msgbox.setText(str);
        msgbox.exec();

        msgbox.setText(" \n Closing the video player, please wait... \n   ");
        msgbox.show();

        if(vlc_sock->state() != QAbstractSocket::UnconnectedState)
        {
          vlc_sock->disconnectFromHost();
          vlc_sock->waitForDisconnected(5000);

          QTimer::singleShot(500, &evlp, SLOT(quit()));
          evlp.exec();
        }

        video_process->kill();

        QTimer::singleShot(500, &evlp, SLOT(quit()));
        evlp.exec();

        delete video_process;

        video_process = NULL;

        delete vlc_sock;

        vlc_sock = NULL;

        msgbox.close();

        video_pause_requested = 0;

        return;
      }
      else
      {
        if(vlc_sock->state() != QAbstractSocket::UnconnectedState)
        {
          vlc_sock->disconnectFromHost();
          vlc_sock->waitForDisconnected(5000);

          QTimer::singleShot(500, &evlp, SLOT(quit()));
          evlp.exec();
        }

        video_process->kill();

        QTimer::singleShot(500, &evlp, SLOT(quit()));
        evlp.exec();

        delete video_process;

        video_process = NULL;

        delete vlc_sock;

        vlc_sock = NULL;
      }
    }
  }

  if(process_start_retries == 3)
  {
    msgbox.setWindowTitle("Error");
#ifdef Q_OS_WIN32
    msgbox.setText("  \n Cannot start the video player. \n"
                   "  \n Check if VLC is installed in C:\\Program Files\\VideoLAN\\VLC\\ \n "
                   "  or\n "
                   "  \n C:\\Program Files (x86)\\VideoLAN\\VLC\\\n ");
#else
#ifdef Q_OS_MACOS
    msgbox.setText("  \n Cannot start the video player. \n  "
                   "  \n Check if VLC is installed in /Applications/. \n  ");
#else
    msgbox.setText("  \n Cannot start the video player. \n  "
                   "  \n Check your installation of VLC. \n  ");
#endif
#endif
    msgbox.setStandardButtons(QMessageBox::Close);
    msgbox.exec();
    video_pause_requested = 0;
    return;
  }

#ifdef DEBUG_VIDEOPLAYER
  debug_vpr = fopen("debug_vpr.txt", "wb");
#endif

  msgbox.close();

  playback_file_Act->setText("[pause]");

  playback_file_Act->setIcon(QIcon(":/images/media-playback-pause-symbolic.symbolic.png"));

  video_player->status = VIDEO_STATUS_STARTUP_1;

  video_player->poll_timer = 100;

  video_player->cntdwn_timer = 5000;

  video_player->speed = 1;

  video_poll_timer->start(video_player->poll_timer);

  video_act->setText("Stop video");

  navtoolbar->setVisible(true);

  slidertoolbar->setVisible(true);

  connect(video_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(video_process_error(QProcess::ProcessError)));

//  connect(vlc_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(vlc_sock_error(QAbstractSocket::SocketError)));
}


void UI_Mainwindow::video_poll_timer_func()
{
  int i, p=0, err=0, len, vpos=0;

  static int repeat=0;

  char str_4096[4096]={""};

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  if(video_player->status != VIDEO_STATUS_PAUSED)
  {
    video_player->cntdwn_timer -= video_player->poll_timer;
  }

  if(video_player->cntdwn_timer <= 0)
  {
#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_cntdwn_timer: <0\n");
#endif
    stop_video_generic(2);

    return;
  }
#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_status: %i\n", video_player->status);
#endif
  len = mpr_read(str_4096, 4095);

  if((len < 1) && (video_player->status == VIDEO_STATUS_PLAYING))
  {
    video_poll_timer->start(video_player->poll_timer);
#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_poll_timer_set & no message & return\n");
#endif
    return;
  }

  if(video_player->status < VIDEO_STATUS_PLAYING)
  {
    if(video_player->status == VIDEO_STATUS_STARTUP_1)
    {
      while(mpr_read(str_4096, 4095) > 0) ;

      mpr_write("clear\n");

      video_player->status = VIDEO_STATUS_STARTUP_2;
#ifdef DEBUG_VIDEOPLAYER
      fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
      repeat = 3;
    }
    else if(video_player->status == VIDEO_STATUS_STARTUP_2)
      {
        if(repeat)
        {
          repeat--;
        }
        else
        {
          strlcpy(str_4096, "add ", 4096);

          strlcat(str_4096, videopath, 4096);

          strlcat(str_4096, "\n", 4096);

          mpr_write(str_4096);

          video_player->status = VIDEO_STATUS_STARTUP_3;
#ifdef DEBUG_VIDEOPLAYER
          fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
          repeat = 5;
        }
      }
      else if(video_player->status == VIDEO_STATUS_STARTUP_3)
        {
          if(repeat)
          {
            repeat--;
          }
          else
          {
            mpr_write("vzoom 0.25\n");

            video_player->status = VIDEO_STATUS_STARTUP_4;
#ifdef DEBUG_VIDEOPLAYER
            fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
            repeat = 5;
          }
        }
        else if(video_player->status == VIDEO_STATUS_STARTUP_4)
          {
            if(repeat)
            {
              repeat--;
            }
            else
            {
              mpr_write("vzoom 0.25\n");

              video_player->status = VIDEO_STATUS_STARTUP_5;
#ifdef DEBUG_VIDEOPLAYER
              fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
              repeat = 5;
            }
          }
          else if(video_player->status == VIDEO_STATUS_STARTUP_5)
            {
              if(repeat)
              {
                repeat--;
              }
              else
              {
                mpr_write("volume 255\n");

                faster_Act->setVisible(true);

                slower_Act->setVisible(true);

                if(session_start_video && (session_video_seek > 0))
                {
                  video_player->status = VIDEO_STATUS_STARTUP_6;
#ifdef DEBUG_VIDEOPLAYER
                  fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
                  video_player_seek(session_video_seek);

                  session_video_seek = 0;

                  repeat = 5;
                }
                else
                {
                  video_player->status = VIDEO_STATUS_PLAYING;
#ifdef DEBUG_VIDEOPLAYER
                  fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
                  video_pause_requested = 0;

                  session_video_seek = 0;

                  session_start_video = 0;

                  mpr_write("get_time\n");
                }
              }
            }
            else if(video_player->status == VIDEO_STATUS_STARTUP_6)
              {
                if(repeat)
                {
                  repeat--;
                }
                else
                {
                  video_player->status = VIDEO_STATUS_PLAYING;
#ifdef DEBUG_VIDEOPLAYER
                  fprintf(debug_vpr, "edfbr_status_changed: %i\n", video_player->status);
#endif
                  if(session_start_video && video_pause_requested)
                  {
                    video_player_toggle_pause();
                  }

                  video_pause_requested = 0;

                  session_video_seek = 0;

                  session_start_video = 0;

                  mpr_write("get_time\n");
                }
              }

    video_player->cntdwn_timer = 5000;
#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_cntdwn_timer set\n");
#endif
    video_poll_timer->start(video_player->poll_timer);
#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_poll_timer set\n");
#endif
    return;
  }

#ifdef DEBUG_VIDEOPLAYER
    fprintf(debug_vpr, "edfbr_poll_timer_checking...\n");
#endif
  if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
  {
    for(p=0; p<len; p++)
    {
      if((str_4096[p] != '>') && (str_4096[p] != ' '))
      {
        break;
      }
    }

    if((len > (p + 2)) && (str_4096[len-1] == '\n'))
    {
      err = 0;

      for(i=p; i<(len-1); i++)
      {
        if((str_4096[i] < '0') || (str_4096[i] > '9'))

        err = 1;

        break;
      }

      if(!err)
      {
        vpos = atoi(str_4096 + p);

        if(video_player->fpos != vpos)
        {
          jump_to_time_millisec(((video_player->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime + vpos) * 1000LL) - (pagetime / 20000LL));

          video_player->fpos = vpos;

          video_player->stop_det_counter = 0;
        }

        video_player->cntdwn_timer = 5000;
      }
      else
      {
        for(i=0; i<(len-11); i++)
        {
          if(!strncmp(str_4096 + i, "stop state:", 11))
          {
            stop_video_generic(1);

            return;
          }
        }
      }
    }
    else if(str_4096[p] == '\r')
    {
      video_player->stop_det_counter += video_player->poll_timer;

      if(video_player->stop_det_counter > 1500)
      {
        stop_video_generic(1);

        return;
      }
    }

    mpr_write("get_time\n");
  }

  video_poll_timer->start(video_player->poll_timer);
}


void UI_Mainwindow::video_player_faster()
{
  char str[512];

  if(video_player->status != VIDEO_STATUS_PLAYING)
  {
    return;
  }

  if(video_player->speed >= 4)
  {
    video_player->speed = 4;

    return;
  }

  video_player->speed *= 2;

  snprintf(str, 512, "rate %i\n", video_player->speed);

  mpr_write(str);
}


void UI_Mainwindow::video_player_slower()
{
  char str[512];

  if(video_player->status != VIDEO_STATUS_PLAYING)
  {
    return;
  }

  if(video_player->speed <= 1)
  {
    video_player->speed = 1;

    return;
  }

  video_player->speed /= 2;

  snprintf(str, 512, "rate %i\n", video_player->speed);

  mpr_write(str);
}


void UI_Mainwindow::video_player_seek(int sec)
{
  char str[512];

  if(((video_player->status != VIDEO_STATUS_PLAYING) && (video_player->status != VIDEO_STATUS_PAUSED)) &&
    (!(session_start_video && (session_video_seek > 0) && (video_player->status == VIDEO_STATUS_STARTUP_6))))
  {
    return;
  }

  sec += video_player->starttime_diff;

  if(sec < 0)  sec = 0;

  snprintf(str, 512, "seek %i\n", sec);

  mpr_write(str);

  video_player->cntdwn_timer = 5000;
}


void UI_Mainwindow::video_player_toggle_pause()
{
  if(video_player->status == VIDEO_STATUS_STOPPED)
  {
    start_stop_video();

    return;
  }

  if((video_player->status != VIDEO_STATUS_PLAYING) && (video_player->status != VIDEO_STATUS_PAUSED))
  {
    return;
  }

  mpr_write("pause\n");

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player->status = VIDEO_STATUS_PAUSED;

    playback_file_Act->setText("[play]");

    playback_file_Act->setIcon(QIcon(":/images/media-playback-start-symbolic.symbolic.png"));

    video_player->cntdwn_timer = 5000;
  }
  else
  {
    video_player->status = VIDEO_STATUS_PLAYING;

    playback_file_Act->setText("[pause]");

    playback_file_Act->setIcon(QIcon(":/images/media-playback-pause-symbolic.symbolic.png"));
  }
}


void UI_Mainwindow::stop_video_generic(int stop_reason)
{
  QEventLoop evlp;

  video_pause_requested = 0;

  faster_Act->setVisible(false);

  slower_Act->setVisible(false);

  video_poll_timer->stop();

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  video_player->status = VIDEO_STATUS_STOPPED;

  playback_file_Act->setText("[play]");

  playback_file_Act->setIcon(QIcon(":/images/media-playback-start-symbolic.symbolic.png"));

  if(video_process == NULL)  return;

  disconnect(video_process, 0, 0, 0);

  if(vlc_sock != NULL)
  {
//    disconnect(vlc_sock, 0, 0, 0);
  }

  QMessageBox msgbox(QMessageBox::NoIcon, "Wait", " \n Closing video, please wait ... \n  ");
  if(stop_reason == 1)
  {
    msgbox.setText(" \n Video has finished \n  \n Closing the video player, please wait ... \n  ");
  }
  if(stop_reason == 2)
  {
    msgbox.setText(" \n Video timeout error \n  \n Closing the video player, please wait ... \n  ");
  }
  msgbox.show();

#ifdef Q_OS_WIN32
  mpr_write("quit\n");
#else
  mpr_write("shutdown\n");
#endif

  QObject::connect(&msgbox, SIGNAL(finished(int)), &evlp, SLOT(quit()));
  QTimer::singleShot(2000, &evlp, SLOT(quit()));
  evlp.exec();

  video_process->waitForFinished(3000);

  QTimer::singleShot(500, &evlp, SLOT(quit()));
  evlp.exec();

  video_process->kill();

  QTimer::singleShot(500, &evlp, SLOT(quit()));
  evlp.exec();

  delete video_process;

  video_process = NULL;

  if(vlc_sock != NULL)
  {
    if(vlc_sock->state())
    {
      vlc_sock->disconnectFromHost();
      vlc_sock->waitForDisconnected(5000);
    }

    delete vlc_sock;

    vlc_sock = NULL;
  }

  video_act->setText("Start video");

  msgbox.close();

#ifdef DEBUG_VIDEOPLAYER
  fclose(debug_vpr);
#endif
}


void UI_Mainwindow::vlc_sock_error(QAbstractSocket::SocketError)
{
  char str[2048],
       str2[1024];

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  strlcpy(str2, vlc_sock->errorString().toLatin1().data(), 1024);

  stop_video_generic(0);

  strlcpy(str, " \n The socked that connects to the mediaplayer reported an error: \n \n ", 2048);

  strlcat(str, str2, 2048);

  QMessageBox msgbox(QMessageBox::Critical, "Error", str);
  msgbox.exec();
}


void UI_Mainwindow::video_process_error(QProcess::ProcessError)
{
  char str[2048],
       str2[1024];

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  strlcpy(str2, video_process->errorString().toLatin1().data(), 1024);

  stop_video_generic(0);

  strlcpy(str, " \n The process that runs the mediaplayer reported an error: \n \n ", 2048);

  strlcat(str, str2, 2048);

  QMessageBox msgbox(QMessageBox::Critical, "Error", str);
  msgbox.exec();
}


void UI_Mainwindow::mpr_write(const char *cmd_str)
{
#ifdef DEBUG_VIDEOPLAYER
  fprintf(debug_vpr, "edfbr: %s", cmd_str);
#endif

  if(vlc_sock == NULL)
  {
    return;
  }

  if(vlc_sock->state() != QAbstractSocket::ConnectedState)  return;

  vlc_sock->write(cmd_str);

  vlc_sock->waitForBytesWritten(200);
}


int UI_Mainwindow::mpr_read(char *buf, int sz)
{
  if(vlc_sock == NULL)
  {
    return 0;
  }

#ifdef DEBUG_VIDEOPLAYER
  int n;

  n = vlc_sock->readLine(buf, sz);

  if(n > 0)
  {
    fprintf(debug_vpr, "vlc: %i bytes: %s ", n, buf);

    for(int i=0; i<n; i++)
    {
      fprintf(debug_vpr, " 0x%02X", buf[i]);
    }

    fprintf(debug_vpr, "\n");
  }

  return n;
#else
  return vlc_sock->readLine(buf, sz);
#endif
}








