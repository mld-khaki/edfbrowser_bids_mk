/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2021 - 2024 Teunis van Beelen
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


// #define RC_HOST_DEBUG


const char rc_cmd_key_lst[RC_CMD_LIST_SZ][32]=
{
  "*LIST",      /*  0 */
  "*IDN",       /*  1 */
  "*QUIT",      /*  2 */
  "*RST",       /*  3 */
  "*CLS",       /*  4 */
  "*OPC",       /*  5 */
  "ERROR",      /*  6 */
  "FILE",       /*  7 */
  "OPEN",       /*  8 */
  "CLOSE",      /*  9 */
  "ALL",        /* 10 */
  "MONTAGE",    /* 11 */
  "LOAD",       /* 12 */
  "SIGNAL",     /* 13 */
  "ADD",        /* 14 */
  "LABEL",      /* 15 */
  "AMPLITUDE",  /* 16 */
  "FIT",        /* 17 */
  "OFFSET",     /* 18 */
  "ADJUST",     /* 19 */
  "ZERO",       /* 20 */
  "INVERT",     /* 21 */
  "REMOVE",     /* 22 */
  "TIMESCALE",  /* 23 */
  "MMSECOND",   /* 24 */
  "VIEWTIME",   /* 25 */
  "TIMELOCK",   /* 26 */
  "REFERENCE",  /* 27 */
  "SYSTEM",     /* 28 */
  "LOCKED",     /* 29 */
  "MODE",       /* 30 */
  "SESSION",    /* 31 */
  "PLAY",       /* 32 */
  "STOP",       /* 33 */
  "STREAM"      /* 34 */
};


int UI_Mainwindow::rc_cmd2key(const char *cmd_str)
{
  int i, len, idx, query=0;

  char str1_32[32]={""};

  strlcpy(str1_32, cmd_str, 32);

  len = strlen(str1_32);
  if(len < 3)  return -1;

  ascii_toupper(str1_32);

  if(str1_32[len-1] == '?')
  {
    query = RC_CMD_QUERY;

    len--;
  }

  for(i=0, idx=-1; i<RC_CMD_LIST_SZ; i++)
  {
    if(!strncmp(str1_32, rc_cmd_key_lst[i], len))
    {
      if(idx >= 0)
      {
        return -2;
      }

      idx = i;
    }
  }

  if(idx < 0)
  {
    return -3;
  }

  return (idx + query);
}


void UI_Mainwindow::rc_host_server_new_connection()
{
  int i;

  QTcpSocket *tsock=NULL;

  printf("rc host server: new connection\n");

  if(rc_host_sock == NULL)
  {
    rc_host_sock = rc_host_server->nextPendingConnection();

    if(rc_host_sock != NULL)
    {
      printf("setup rc host socket\n");

      for(i=0; i<RC_ERR_QUEUE_SZ; i++)
      {
        rc_err_queue[i] = 0;
      }
      rc_err_queue_idx = 0;

      QObject::connect(rc_host_sock, SIGNAL(disconnected()), this, SLOT(rc_host_sock_disconnected_handler()));
      QObject::connect(rc_host_sock, SIGNAL(readyRead()),    this, SLOT(rc_host_sock_rxdata_handler()));
    }
  }
  else
  {
    tsock = rc_host_server->nextPendingConnection();
    if(tsock != NULL)
    {
      tsock->close();
    }
  }
}


void UI_Mainwindow::rc_host_sock_disconnected_handler()
{
  printf("rc host socket disconnected\n");

  rc_host_sock = NULL;
}


void UI_Mainwindow::rc_host_sock_rxdata_handler()
{
  int i, n, len, n_sub_cmds, err;

  int cmds_parsed_key[CMD_MAX_SUB_CMDS]={-1,-1,-1,-1,-1,-1,-1,-1};

  static int rx_idx=0;

  char tx_msg_str_2048[2048]="",
       cmd_args_1024[1024]="",
       cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN]={"","","","","","","",""};

  static char rx_msg_str[1024];

  static QTcpSocket *sock=NULL;

  if(rc_host_sock != sock)
  {
    sock = rc_host_sock;

    rx_idx = 0;
  }

  for(n=1; n>0; )
  {
    if(rc_host_sock == NULL)
    {
      sock = NULL;

      return;
    }

    n = rc_host_sock->read(rx_msg_str + rx_idx, 1);
    if(n < 1)
    {
      rc_cmd_in_progress = 0;

      break;
    }
#ifdef RC_HOST_DEBUG
    printf("%c", rx_msg_str[rx_idx]);  /* test debug */
#endif
    if(rx_msg_str[rx_idx] != '\n')
    {
      rx_idx++;
      rx_idx %= 1024;

      continue;
    }

    rx_msg_str[rx_idx] = 0;

    if(rx_idx > 0)
    {
      if(rx_msg_str[rx_idx-1] == '\r')
      {
        rx_msg_str[--rx_idx] = 0;
      }
    }

    if(rx_idx == 0)
    {
      continue;
    }

    rx_idx = 0;

    trim_spaces(rx_msg_str);

    n_sub_cmds = parse_rc_command(rx_msg_str, cmds_parsed, cmds_parsed_key, cmd_args_1024, 1024);
#ifdef RC_HOST_DEBUG
    printf("n_sub_cmds: %i\n", n_sub_cmds);
    for(i=0; i<n_sub_cmds; i++)
    {
      printf("sub command %i: ->%s<-  key: %i\n", i, cmds_parsed[i], cmds_parsed_key[i]);
    }
    printf("cmd_args_1024: ->%s<-\n", cmd_args_1024);
#endif
    if(n_sub_cmds < 1)
    {
      register_rc_err(201);
      continue;
    }

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == (RC_CMD_OPC | RC_CMD_QUERY)))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      if(rc_cmd_in_progress)
      {
        len = snprintf(tx_msg_str_2048, 1024, "0\n");
      }
      else
      {
        len = snprintf(tx_msg_str_2048, 1024, "1\n");
      }
      rc_host_sock->write(tx_msg_str_2048, len);
      continue;
    }

    if(rc_cmd_in_progress)
    {
      register_rc_err(301);
      continue;
    }

    rc_cmd_in_progress = 1;

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == (RC_CMD_LIST | RC_CMD_QUERY)))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      len = snprintf(tx_msg_str_2048, 2048,
              "*LIST?\n"
              "*IDN?\n"
              "*QUIT\n"
              "*RST\n"
              "*CLS\n"
              "*OPC?\n"
              "FILe:OPEn <path>\n"
              "FILe:CLOse:ALL\n"
              "FILe:PLAy\n"
              "FILe:STOp\n"
              "STReam:OPEn <path>\n"
              "MONtage:LOAd <path>\n"
              "SESsion:LOAd <path>\n"
              "SIGnal:ADD:LABel <label> <file number>\n"
              "SIGnal:AMPlitude:ALL <units>\n"
              "SIGnal:AMPlitude:LABel <label> <units>\n"
              "SIGnal:AMPlitude:FIT:ALL\n"
              "SIGnal:AMPlitude:FIT:LABel <label>\n"
              "SIGnal:OFFset:ADJust:ALL\n"
              "SIGnal:OFFset:ADJust:LABel <label>\n"
              "SIGnal:OFFset:ZERo:ALL\n"
              "SIGnal:OFFset:ZERo:LABEL <label>\n"
              "SIGnal:INVert:ALL <0|1|2>\n"
              "SIGnal:INVert:LABel <label> <0|1|2>\n"
              "SIGnal:REMove:LABel <label>\n"
              "SIGnal:REMove:ALL\n"
              "TIMEScale?\n"
              "TIMEScale <seconds>\n"
              "VIEwtime?\n"
              "VIEwtime <seconds>\n"
              "TIMELock:MODe?\n"
              "TIMELock:MODe <0|1|2|3>\n"
              "TIMELock:REFerence?\n"
              "TIMELock:REFerence <file number>\n"
              "SYStem:ERRor?\n"
              "SYStem:LOCked?\n"
              "SYStem:LOCked <0|1>\n");

      rc_host_sock->write(tx_msg_str_2048, len);
      continue;
    }

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == (RC_CMD_IDN | RC_CMD_QUERY)))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      len = snprintf(tx_msg_str_2048, 1024, PROGRAM_NAME "," PROGRAM_VERSION "," THIS_APP_BITS_W "\n");
      rc_host_sock->write(tx_msg_str_2048, len);
      continue;
    }

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == RC_CMD_QUIT))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      exit_program();
      continue;
    }

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == RC_CMD_RST))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      close_all_files();
      continue;
    }

    if((n_sub_cmds == 1) && (cmds_parsed_key[0] == RC_CMD_CLS))
    {
      if(strlen(cmd_args_1024))
      {
        register_rc_err(203);
        continue;
      }
      for(i=0; i<RC_ERR_QUEUE_SZ; i++)
      {
        rc_err_queue[i] = 0;
      }
      rc_err_queue_idx = 0;
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_FILE)
    {
      err = process_rc_cmd_file(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_STREAM)
    {
      err = process_rc_cmd_stream(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_MONTAGE)
    {
      err = process_rc_cmd_montage(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_SESSION)
    {
      err = process_rc_cmd_session(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_SIGNAL)
    {
      err = process_rc_cmd_signal(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_TIMESCALE)
    {
      err = process_rc_cmd_timescale(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_VIEWTIME)
    {
      err = process_rc_cmd_viewtime(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_TIMELOCK)
    {
      err = process_rc_cmd_timelock(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    if((cmds_parsed_key[0] & RC_CMD_MASK) == RC_CMD_SYSTEM)
    {
      err = process_rc_cmd_system(cmd_args_1024, cmds_parsed_key, n_sub_cmds);
      if(err)
      {
        register_rc_err(err);
      }
      continue;
    }

    register_rc_err(202);
  }
}


int UI_Mainwindow::parse_rc_command(const char *cmd_str, char cmd_parsed_str[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], int *cmds_key, char *arg_n_str, int arg_n_len)
{
  int i, j, k,
      last_char_colon=0;

  cmd_parsed_str[0][0] = 0;

  arg_n_str[0] = 0;

  for(j=0; j<CMD_MAX_SUB_CMDS; j++)
  {
    cmds_key[j] = -1;

    cmd_parsed_str[j][0] = 0;
  }

  for(i=0, j=0, k=0; i<511; i++)
  {
    if(j >= CMD_MAX_SUB_CMDS)
    {
      return -2;
    }

    if(k >= (CMD_PARSE_STR_LEN - 1))
    {
      cmd_parsed_str[j][CMD_PARSE_STR_LEN - 1] = 0;

      return -3;
    }

    cmd_parsed_str[j][k] = cmd_str[i];

    if(cmd_parsed_str[j][k] == 0)
    {
      if(!k)
      {
        return -4;
      }
      else
      {
        if(cmd_parsed_str[j][k-1] == ':')
        {
          return -5;
        }
      }

      cmds_key[j] = rc_cmd2key(cmd_parsed_str[j]);
      if(cmds_key[j] < 0)
      {
        return -6;
      }

      if(j)
      {
        if(cmds_key[j-1] & RC_CMD_QUERY)
        {
          return -7;
        }
      }

      ascii_toupper(cmd_parsed_str[j++]);

      return j;
    }

    if(cmd_parsed_str[j][k] == ':')
    {
      if(last_char_colon || !k)
      {
        cmd_parsed_str[j][0] = 0;

        return -8;
      }

      last_char_colon++;

      cmd_parsed_str[j][k] = 0;

      cmds_key[j] = rc_cmd2key(cmd_parsed_str[j]);
      if(cmds_key[j] < 0)
      {
        return -9;
      }

      ascii_toupper(cmd_parsed_str[j++]);

      if(j >= CMD_MAX_SUB_CMDS)
      {
        return -10;
      }

      k = 0;

      continue;
    }
    else
    {
      last_char_colon = 0;
    }

    if(cmd_parsed_str[j][k] == ' ')
    {
      cmd_parsed_str[j][k] = 0;

      if(!k)
      {
        return -11;
      }

      if(cmd_parsed_str[j][k-1] == ':')
      {
        return -12;
      }

      cmds_key[j] = rc_cmd2key(cmd_parsed_str[j]);
      if(cmds_key[j] < 0)
      {
        return -13;
      }

      if(j)
      {
        if(cmds_key[j-1] & RC_CMD_QUERY)
        {
          return -14;
        }
      }

      ascii_toupper(cmd_parsed_str[j++]);

      strlcpy(arg_n_str, cmd_str + i + 1, arg_n_len);

      trim_spaces(arg_n_str);

      return j;
    }

    k++;
  }

  return -1;
}


int UI_Mainwindow::process_rc_cmd_file(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  if(n_sub_cmds < 2)
  {
    return 202;
  }

  if(cmds_parsed_key[1] == RC_CMD_CLOSE)
  {
    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_ALL))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      close_all_files();
      return 0;
    }
    else
    {
      return 202;
    }
  }
  else if(cmds_parsed_key[1] == RC_CMD_OPEN)
    {
      if(n_sub_cmds == 2)
      {
        if(!strlen(cmd_args))
        {
          return 204;
        }
        if(files_open >= MAXFILES)  return 103;
        if(annot_editor_active && files_open)  return 101;
        if((files_open > 0) && (live_stream_active))  return 102;

        strlcpy(drop_path, cmd_args, MAX_PATH_LENGTH);
        rc_file_open_requested = 1;
        open_new_file();
        drop_path[0] = 0;
        rc_file_open_requested = 0;
        return rc_file_open_err;
      }
      else
      {
        return 202;
      }
    }
    else if(cmds_parsed_key[1] == RC_CMD_PLAY)
      {
        if(n_sub_cmds == 2)
        {
          if(!files_open)  return 205;

          playback_file();
          return 0;
        }
        else
        {
          return 202;
        }
      }
      else if(cmds_parsed_key[1] == RC_CMD_STOP)
        {
          if(n_sub_cmds == 2)
          {
            if((!files_open) || live_stream_active)  return 205;

            stop_playback();
            return 0;
          }
          else
          {
            return 202;
          }
        }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_stream(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  if(n_sub_cmds < 2)
  {
    return 202;
  }

  if(cmds_parsed_key[1] == RC_CMD_OPEN)
  {
    if(n_sub_cmds == 2)
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(files_open)  return 109;

      strlcpy(drop_path, cmd_args, MAX_PATH_LENGTH);
      rc_file_open_requested = 1;
      open_stream();
      drop_path[0] = 0;
      rc_file_open_requested = 0;
      return rc_file_open_err;
    }
    else
    {
      return 202;
    }
  }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_montage(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int err, file_num=0;

  char str1_1024[1024]="",
       *ptr=NULL;

  if(n_sub_cmds < 2)
  {
    return 202;
  }

  if((n_sub_cmds == 2) && (cmds_parsed_key[1] == RC_CMD_LOAD))
  {
    strlcpy(str1_1024, cmd_args, 1024);
    err = rc_get_last_cmd_args_token(str1_1024, &ptr);
    if(err)
    {
      return err;
    }
    if(!files_open)  return 205;
    if(signalcomps >= MAXSIGNALS)  return 205;

    if(is_integer_number(ptr))
    {
      return 203;
    }

    file_num = atoi(ptr);
    if((file_num < 1) || (file_num > MAXFILES))
    {
      return 208;
    }
    file_num--;

    if(file_num >= files_open)
    {
      return 207;
    }

    rc_load_mtg_file_num = file_num;

    strlcpy(montagepath, str1_1024, MAX_PATH_LENGTH);
    UI_LoadMontagewindow load_mtg(this, montagepath);
    montagepath[0] = 0;
    return rc_load_mtg_err;
  }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_session(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  rc_load_session_err = 0;

  if(n_sub_cmds < 2)
  {
    return 202;
  }

  if((n_sub_cmds == 2) && (cmds_parsed_key[1] == RC_CMD_LOAD))
  {
    strlcpy(session_path, cmd_args, MAX_PATH_LENGTH);
    load_session();
    session_path[0] = 0;
    return rc_load_session_err;
  }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_signal(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int i, n, ival, err, file_num=0;

  char str1_1024[1024]="",
       str2_1024[1024]="",
       *ptr=NULL;

  double value2=100,
         original_value=100;

  sigcompblck_t *newsignalcomp=NULL;

  if(n_sub_cmds < 3)
  {
    return 202;
  }

  if(cmds_parsed_key[1] == RC_CMD_ADD)
  {
    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_LABEL))
    {
      strlcpy(str1_1024, cmd_args, 1024);
      err = rc_get_last_cmd_args_token(str1_1024, &ptr);
      if(err)
      {
        return err;
      }

      if(!files_open)  return 205;
      if(signalcomps >= MAXSIGNALS)  return 205;
      if(strip_label_types)
      {
        strip_types_from_label(str1_1024);
      }
      trim_spaces(str1_1024);

      if(is_integer_number(ptr))
      {
        return 203;
      }

      file_num = atoi(ptr);
      if((file_num < 1) || (file_num > 32))
      {
        return 208;
      }
      file_num--;

      if(file_num >= files_open)
      {
        return 207;
      }

      for(i=0; i<edfheaderlist[file_num]->edfsignals; i++)
      {
        if(edfheaderlist[file_num]->edfparam[i].annotation)  continue;
        strlcpy(str2_1024, edfheaderlist[file_num]->edfparam[i].label, 1024);
        if(strip_label_types)
        {
          strip_types_from_label(str2_1024);
        }
        trim_spaces(str2_1024);
        if(!strcmp(str2_1024, str1_1024))
        {
          newsignalcomp = (sigcompblck_t *)calloc(1, sizeof(sigcompblck_t));
          if(newsignalcomp==NULL)  return 206;
          newsignalcomp->uid = uid_seq++;
          newsignalcomp->num_of_signals = 1;
          newsignalcomp->edfhdr = edfheaderlist[file_num];
          newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
          if(default_amplitude_use_physmax_div)
          {
            newsignalcomp->voltpercm = (newsignalcomp->edfhdr->edfparam[i].phys_max - newsignalcomp->edfhdr->edfparam[i].phys_min)
                                         / (default_amplitude_physmax_div * 2);
          }
          else
          {
            newsignalcomp->voltpercm = default_amplitude;
          }
          if(newsignalcomp->voltpercm < 0.0)
          {
            newsignalcomp->voltpercm = -newsignalcomp->voltpercm;
          }
          newsignalcomp->color = maincurve->signal_color;
          newsignalcomp->hasruler = 0;
          newsignalcomp->polarity = 1;

          newsignalcomp->edfsignal[0] = i;
          newsignalcomp->factor[0] = 1;
          if(newsignalcomp->edfhdr->edfparam[i].bitvalue < 0.0)
          {
            newsignalcomp->voltpercm = default_amplitude * -1;
          }
          newsignalcomp->sensitivity = newsignalcomp->edfhdr->edfparam[i].bitvalue / ((double)newsignalcomp->voltpercm * y_pixelsizefactor);

          strlcpy(newsignalcomp->signallabel, newsignalcomp->edfhdr->edfparam[i].label, 256);
          if(strip_label_types)
          {
            newsignalcomp->signallabel_type_stripped = strip_types_from_label(newsignalcomp->signallabel);
          }
          trim_spaces(newsignalcomp->signallabel);

          newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

          newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

          strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[i].physdimension, 9);
          remove_trailing_spaces(newsignalcomp->physdimension);

          newsignalcomp->edfparam_0 = &newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]];

          signalcomp[signalcomps] = newsignalcomp;
          signalcomps++;

          setup_viewbuf();

          break;
        }
      }
      if(i == edfheaderlist[file_num]->edfsignals)
      {
        return 207;
      }

      return 0;
    }
    else
    {
      return 202;
    }
  }

  if(cmds_parsed_key[1] == RC_CMD_REMOVE)
  {
    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_ALL))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      remove_all_signals();
      return 0;
    }
    else if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_LABEL))
      {
        if(!strlen(cmd_args))
        {
          return 204;
        }
        i = get_signalcomp_number(cmd_args);
        if(i >= 0)
        {
          remove_signalcomp(i);
          setup_viewbuf();
          return 0;
        }
        else
        {
          return 207;
        }
      }
      else
      {
        return 202;
      }
  }

  if(cmds_parsed_key[1] == RC_CMD_AMPLITUDE)
  {
    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_ALL))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(!signalcomps)  return 205;

      if(is_number(cmd_args))  return 203;

      value2 = atof(cmd_args);

      if((value2 > 1000000.001) || (value2 < 0.0000000999))  return 208;

      for(i=0; i<signalcomps; i++)
      {
        if(signalcomp[i]->edfparam_0->bitvalue < 0.0)
        {
          value2 *= -1.0;
        }

        signalcomp[i]->sensitivity = (signalcomp[i]->edfparam_0->bitvalue / value2) / y_pixelsizefactor;

        original_value = signalcomp[i]->voltpercm;

        signalcomp[i]->voltpercm = value2;

        signalcomp[i]->screen_offset_pix *= (original_value / value2);

        signalcomp[i]->screen_offset_unit *= (original_value / value2);
      }

      maincurve->drawCurve_stage_1();

      return 0;
    }

    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_LABEL))
    {
      strlcpy(str1_1024, cmd_args, 1024);
      err = rc_get_last_cmd_args_token(str1_1024, &ptr);
      if(err)
      {
        return err;
      }

      if(!files_open)  return 205;

      value2 = atof(ptr);
      if((value2 > 1000000.001) || (value2 < 0.0000000999))  return 208;

      n = get_signalcomp_number(str1_1024);
      if(n < 0)  return 207;

      if(signalcomp[n]->edfparam_0->bitvalue < 0.0)
      {
        value2 *= -1.0;
      }

      signalcomp[n]->sensitivity = (signalcomp[n]->edfparam_0->bitvalue / value2) / y_pixelsizefactor;

      original_value = signalcomp[n]->voltpercm;

      signalcomp[n]->voltpercm = value2;

      signalcomp[n]->screen_offset_pix *= (original_value / value2);

      signalcomp[n]->screen_offset_unit *= (original_value / value2);

      maincurve->drawCurve_stage_1();

      return 0;
    }

    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_FIT) && (cmds_parsed_key[3] == RC_CMD_ALL))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      fit_signals_to_pane();
      return 0;
    }

    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_FIT) && (cmds_parsed_key[3] == RC_CMD_LABEL))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        fit_signals_to_pane(n);
        return 0;
      }
      else
      {
        return 207;
      }
    }
  }

  if(cmds_parsed_key[1] == RC_CMD_OFFSET)
  {
    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_ADJUST) && (cmds_parsed_key[3] == RC_CMD_ALL))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      fit_signals_dc_offset();
      return 0;
    }

    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_ADJUST) && (cmds_parsed_key[3] == RC_CMD_LABEL))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        fit_signals_dc_offset(n);
        return 0;
      }
      else
      {
        return 207;
      }
    }

    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_ZERO) && (cmds_parsed_key[3] == RC_CMD_ALL))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      set_dc_offset_to_zero();
      return 0;
    }

    if((n_sub_cmds == 4) && (cmds_parsed_key[2] == RC_CMD_ZERO) && (cmds_parsed_key[3] == RC_CMD_LABEL))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        set_dc_offset_to_zero(n);
        return 0;
      }
      else
      {
        return 207;
      }
    }
  }

  if(cmds_parsed_key[1] == RC_CMD_INVERT)
  {
    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_ALL))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(is_integer_number(cmd_args))  return 203;

      n = atoi(cmd_args);
      if((n < 0) || (n > 2))  return 208;

      signalcomp_invert(n);

      return 0;
    }

    if((n_sub_cmds == 3) && (cmds_parsed_key[2] == RC_CMD_LABEL))
    {
      strlcpy(str1_1024, cmd_args, 1024);
      err = rc_get_last_cmd_args_token(str1_1024, &ptr);
      if(err)
      {
        return err;
      }

      ival = atoi(ptr);
      if((ival < 0) || (ival > 2))  return 208;

      n = get_signalcomp_number(str1_1024);
      if(n < 0)  return 207;

      signalcomp_invert(ival, n);

      return 0;
    }
    else
    {
      return 202;
    }
  }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_timescale(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int len;

  long long ltmp;

  char tx_msg_str_1024[1024]="";

  if(n_sub_cmds < 1)
  {
    return 202;
  }

  if((n_sub_cmds == 1) && (cmds_parsed_key[0] == (RC_CMD_TIMESCALE | RC_CMD_QUERY)))
  {
    if(strlen(cmd_args))
    {
      return 203;
    }
#ifdef Q_OS_WIN32
    len = __mingw_snprintf(tx_msg_str_1024, 1024, "%llu.%07llu\n", pagetime / TIME_FIXP_SCALING, pagetime % TIME_FIXP_SCALING);
#else
    len = snprintf(tx_msg_str_1024, 1024, "%llu.%07llu\n", pagetime / TIME_FIXP_SCALING, pagetime % TIME_FIXP_SCALING);
#endif
    rc_host_sock->write(tx_msg_str_1024, len);
    return 0;
  }
  else if((n_sub_cmds == 1) && (cmds_parsed_key[0] == RC_CMD_TIMESCALE))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(is_number(cmd_args))  return 203;
      ltmp = atoll_x(cmd_args, TIME_FIXP_SCALING);
      if((ltmp < 100LL) || (ltmp > (7LL * 24LL * 3600LL * TIME_FIXP_SCALING)))  return 208;
      pagetime = ltmp;
      setup_viewbuf();
      return 0;
    }
    else if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMESCALE) && (cmds_parsed_key[1] == (RC_CMD_MMSECOND | RC_CMD_QUERY)))
      {
        if(strlen(cmd_args))
        {
          return 203;
        }
        len = snprintf(tx_msg_str_1024, 1024, "%f\n", (maincurve->width() * x_pixelsizefactor * 10) / (((double)pagetime) / TIME_FIXP_SCALING));
        rc_host_sock->write(tx_msg_str_1024, len);
        return 0;
      }
      else if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMESCALE) && (cmds_parsed_key[1] == RC_CMD_MMSECOND))
        {
          if(!strlen(cmd_args))
          {
            return 204;
          }
          if(is_number(cmd_args))  return 203;
          ltmp = atoll_x(cmd_args, TIME_FIXP_SCALING);
          if((ltmp < 100LL) || (ltmp > (100000LL * TIME_FIXP_SCALING)))  return 208;
          pagetime = maincurve->width() * x_pixelsizefactor * TIME_FIXP_SCALING * ((10.0 * TIME_FIXP_SCALING) / ltmp);
          setup_viewbuf();
          return 0;
        }
        else
        {
          return 202;
        }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_viewtime(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int len;

  long long ltmp;

  char tx_msg_str_1024[1024]="";

  if(n_sub_cmds < 1)
  {
    return 202;
  }

  if((n_sub_cmds == 1) && (cmds_parsed_key[0] == (RC_CMD_VIEWTIME | RC_CMD_QUERY)))
  {
    if(strlen(cmd_args))
    {
      return 203;
    }
    if(!files_open)
    {
      len = snprintf(tx_msg_str_1024, 1024, "0\n");
      rc_host_sock->write(tx_msg_str_1024, len);
      return 0;
    }
#ifdef Q_OS_WIN32
    len = __mingw_snprintf(tx_msg_str_1024, 1024, "%lli.%07lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_FIXP_SCALING, edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING);
#else
    len = snprintf(tx_msg_str_1024, 1024, "%lli.%07lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_FIXP_SCALING, edfheaderlist[sel_viewtime]->viewtime % TIME_FIXP_SCALING);
#endif
    rc_host_sock->write(tx_msg_str_1024, len);
    return 0;
  }
  else if((n_sub_cmds == 1) && (cmds_parsed_key[0] == RC_CMD_VIEWTIME))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(is_number(cmd_args))  return 203;
      if(!files_open)  return 205;
      ltmp = atoll_x(cmd_args, TIME_FIXP_SCALING);
      if((ltmp < (-30LL * TIME_FIXP_SCALING)) || (ltmp > (7LL * 24LL * 3600LL * TIME_FIXP_SCALING)))  return 208;
      set_viewtime(ltmp);
      return 0;
    }
    else
    {
      return 202;
    }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_timelock(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int len, mode=0, file_num=0;

  char tx_msg_str_1024[1024]="";

  if(n_sub_cmds < 1)
  {
    return 202;
  }

  if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMELOCK) && (cmds_parsed_key[1] == (RC_CMD_MODE | RC_CMD_QUERY)))
  {
    if(strlen(cmd_args))
    {
      return 203;
    }
    len = snprintf(tx_msg_str_1024, 1024, "%i\n", viewtime_sync);
    rc_host_sock->write(tx_msg_str_1024, len);
    return 0;
  }
  else if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMELOCK) && (cmds_parsed_key[1] == RC_CMD_MODE))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(is_integer_number(cmd_args))  return 203;
      mode = atoi(cmd_args);
      if((mode < 0) || (mode > 3))  return 208;
      set_timesync(mode);
      return 0;
    }

  if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMELOCK) && (cmds_parsed_key[1] == (RC_CMD_REFERENCE | RC_CMD_QUERY)))
  {
    if(strlen(cmd_args))
    {
      return 203;
    }
    len = snprintf(tx_msg_str_1024, 1024, "%i\n", sel_viewtime);
    rc_host_sock->write(tx_msg_str_1024, len);
    return 0;
  }
  else if((n_sub_cmds == 2) && (cmds_parsed_key[0] == RC_CMD_TIMELOCK) && (cmds_parsed_key[1] == RC_CMD_REFERENCE))
    {
      if(!strlen(cmd_args))
      {
        return 204;
      }
      if(is_integer_number(cmd_args))  return 203;
      file_num = atoi(cmd_args);
      if((file_num < 1) || (file_num > 32))  return 208;
      file_num--;
      if(file_num >= files_open)  return 207;
      set_timesync_reference(file_num);
      return 0;
    }
    else
    {
      return 202;
    }

  return 202;
}


int UI_Mainwindow::process_rc_cmd_system(const char *cmd_args, int *cmds_parsed_key, int n_sub_cmds)
{
  int len, lock;

  char tx_msg_str_1024[1024]="";

  if(n_sub_cmds < 2)
  {
    return 202;
  }

  if(n_sub_cmds == 2)
  {
    if((cmds_parsed_key[0] == RC_CMD_SYSTEM) && (cmds_parsed_key[1] == (RC_CMD_LOCKED | RC_CMD_QUERY)))
    {
      if(strlen(cmd_args))
      {
        return 203;
      }
      if(rc_system_locked)
      {
        len = snprintf(tx_msg_str_1024, 1024, "1\n");
      }
      else
      {
        len = snprintf(tx_msg_str_1024, 1024, "0\n");
      }
      rc_host_sock->write(tx_msg_str_1024, len);
      return 0;
    }
    else if((cmds_parsed_key[0] == RC_CMD_SYSTEM) && (cmds_parsed_key[1] == RC_CMD_LOCKED))
      {
        if(!strlen(cmd_args))
        {
          return 203;
        }
        if(is_integer_number(cmd_args))  return 203;
        lock = atoi(cmd_args);
        if((lock < 0) || (lock > 1))  return 208;
        if(lock == 1)
        {
          rc_system_locked = 1;
          setEnabled(false);
        }
        else
        {
          rc_system_locked = 0;
          setEnabled(true);
        }
        return 0;
      }
      else if((cmds_parsed_key[0] == RC_CMD_SYSTEM) && (cmds_parsed_key[1] == (RC_CMD_ERROR | RC_CMD_QUERY)))
        {
          if(strlen(cmd_args))
          {
            return 203;
          }
          rc_err_queue_idx += (RC_ERR_QUEUE_SZ - 1);
          rc_err_queue_idx %= RC_ERR_QUEUE_SZ;

          len = snprintf(tx_msg_str_1024, 1024, "%i\n", rc_err_queue[rc_err_queue_idx]);
          rc_host_sock->write(tx_msg_str_1024, len);
          rc_err_queue[rc_err_queue_idx] = 0;
          return 0;
        }
        else
        {
          return 202;
        }
  }

  return 202;
}


void UI_Mainwindow::register_rc_err(int err)
{
  if(!err)  return;

  rc_err_queue[rc_err_queue_idx++] = err;
  rc_err_queue_idx %= RC_ERR_QUEUE_SZ;
}


int UI_Mainwindow::rc_get_last_cmd_args_token(char *cmd_args, char **dest)
{
  int i, len;

  if(strlen(cmd_args) < 3)
  {
    return 204;
  }

  len = strlen(cmd_args);

  for(i=0, *dest=NULL; i<len; i++)
  {
    if(cmd_args[i] == ' ')
    {
      *dest = &cmd_args[i];
    }
  }

  if(*dest == NULL)  return 204;

  **dest = 0;

  (*dest)++;

  trim_spaces(cmd_args);

  if(!strlen(*dest))  return 204;

  if(is_number(*dest))  return 203;

  if(!strlen(cmd_args))  return 204;

  return 0;
}
















