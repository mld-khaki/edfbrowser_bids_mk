/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008 - 2024 Teunis van Beelen
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





#include "utc_date_time.h"




void date_time_to_utc(long long *seconds, date_time_t date_time)
{
  *seconds = (date_time.day - 1) * 86400LL;

  if(date_time.month>1)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>2)
  {
    if(date_time.year%4)
    {
      *seconds += 2419200LL;
    }
    else
    {
      *seconds += 2505600LL;
    }
  }

  if(date_time.month>3)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>4)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>5)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>6)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>7)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>8)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>9)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>10)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>11)
  {
    *seconds += 2592000LL;
  }

  *seconds += (date_time.year - 1970) * 31536000LL;
  *seconds += ((date_time.year - 1970 + 1) / 4) * 86400LL;

  *seconds += date_time.hour * 3600;
  *seconds += date_time.minute * 60;
  *seconds += date_time.second;

  switch(date_time.month)
  {
    case  1 : strlcpy(date_time.month_str, "JAN", 4);
              break;
    case  2 : strlcpy(date_time.month_str, "FEB", 4);
              break;
    case  3 : strlcpy(date_time.month_str, "MAR", 4);
              break;
    case  4 : strlcpy(date_time.month_str, "APR", 4);
              break;
    case  5 : strlcpy(date_time.month_str, "MAY", 4);
              break;
    case  6 : strlcpy(date_time.month_str, "JUN", 4);
              break;
    case  7 : strlcpy(date_time.month_str, "JUL", 4);
              break;
    case  8 : strlcpy(date_time.month_str, "AUG", 4);
              break;
    case  9 : strlcpy(date_time.month_str, "SEP", 4);
              break;
    case 10 : strlcpy(date_time.month_str, "OCT", 4);
              break;
    case 11 : strlcpy(date_time.month_str, "NOV", 4);
              break;
    case 12 : strlcpy(date_time.month_str, "DEC", 4);
              break;
    default : strlcpy(date_time.month_str, "ERR", 4);
              break;
  }
}



void utc_to_date_time(long long seconds, date_time_t *date_time)
{
  date_time->year = 1970;

  while(1)
  {
    if(date_time->year%4)
    {
      if(seconds>=31536000)
      {
        date_time->year++;

        seconds -= 31536000;
      }
      else
      {
        break;
      }
    }
    else
    {
      if(seconds>=31622400)
      {
        date_time->year++;

        seconds -= 31622400;
      }
      else
      {
        break;
      }
    }
  }

  date_time->month = 1;

  if(seconds>=2678400)
  {
    date_time->month++;

    seconds -= 2678400;

    if(date_time->year%4)
    {
      if(seconds>=2419200)
      {
        date_time->month++;

        seconds -= 2419200;

        if(seconds>=2678400)
        {
          date_time->month++;

          seconds -= 2678400;

          if(seconds>=2592000)
          {
            date_time->month++;

            seconds -= 2592000;

            if(seconds>=2678400)
            {
              date_time->month++;

              seconds -= 2678400;

              if(seconds>=2592000)
              {
                date_time->month++;

                seconds -= 2592000;

                if(seconds>=2678400)
                {
                  date_time->month++;

                  seconds -= 2678400;

                  if(seconds>=2678400)
                  {
                    date_time->month++;

                    seconds -= 2678400;

                    if(seconds>=2592000)
                    {
                      date_time->month++;

                      seconds -= 2592000;

                      if(seconds>=2678400)
                      {
                        date_time->month++;

                        seconds -= 2678400;

                        if(seconds>=2592000)
                        {
                          date_time->month++;

                          seconds -= 2592000;

                          if(seconds>=2678400)
                          {
                            date_time->month++;

                            seconds -= 2678400;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else
    {
      if(seconds>=2505600)
      {
        date_time->month++;

        seconds -= 2505600;

        if(seconds>=2678400)
        {
          date_time->month++;

          seconds -= 2678400;

          if(seconds>=2592000)
          {
            date_time->month++;

            seconds -= 2592000;

            if(seconds>=2678400)
            {
              date_time->month++;

              seconds -= 2678400;

              if(seconds>=2592000)
              {
                date_time->month++;

                seconds -= 2592000;

                if(seconds>=2678400)
                {
                  date_time->month++;

                  seconds -= 2678400;

                  if(seconds>=2678400)
                  {
                    date_time->month++;

                    seconds -= 2678400;

                    if(seconds>=2592000)
                    {
                      date_time->month++;

                      seconds -= 2592000;

                      if(seconds>=2678400)
                      {
                        date_time->month++;

                        seconds -= 2678400;

                        if(seconds>=2592000)
                        {
                          date_time->month++;

                          seconds -= 2592000;

                          if(seconds>=2678400)
                          {
                            date_time->month++;

                            seconds -= 2678400;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  date_time->day = (seconds / 86400) + 1;

  seconds %= 86400;

  date_time->hour = seconds / 3600;

  seconds %= 3600;

  date_time->minute = seconds / 60;

  seconds %= 60;

  date_time->second = seconds;

  switch(date_time->month)
  {
    case  1 : strlcpy(date_time->month_str, "JAN", 4);
              break;
    case  2 : strlcpy(date_time->month_str, "FEB", 4);
              break;
    case  3 : strlcpy(date_time->month_str, "MAR", 4);
              break;
    case  4 : strlcpy(date_time->month_str, "APR", 4);
              break;
    case  5 : strlcpy(date_time->month_str, "MAY", 4);
              break;
    case  6 : strlcpy(date_time->month_str, "JUN", 4);
              break;
    case  7 : strlcpy(date_time->month_str, "JUL", 4);
              break;
    case  8 : strlcpy(date_time->month_str, "AUG", 4);
              break;
    case  9 : strlcpy(date_time->month_str, "SEP", 4);
              break;
    case 10 : strlcpy(date_time->month_str, "OCT", 4);
              break;
    case 11 : strlcpy(date_time->month_str, "NOV", 4);
              break;
    case 12 : strlcpy(date_time->month_str, "DEC", 4);
              break;
    default : strlcpy(date_time->month_str, "ERR", 4);
              break;
  }
}



