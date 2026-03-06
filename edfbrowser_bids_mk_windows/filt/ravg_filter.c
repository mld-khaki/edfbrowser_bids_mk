/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2024 Teunis van Beelen
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




#include "ravg_filter.h"



/*
 *  type: 0 = highpass, 1 = lowpass
 */
ravgfiltset_t * create_ravg_filter(int type, int size)
{
  ravgfiltset_t *settings;

  if((size < 2) || (size > 10000))  return NULL;

  if((type < 0) || (type > 1))  return NULL;

  settings = (ravgfiltset_t *)calloc(1, sizeof(ravgfiltset_t));
  if(settings==NULL)  return NULL;

  settings->size = size;
  settings->type = type;
  settings->first_sample = 1;
  settings->buf_pntr = 0;
  settings->buf_pntr2 = 0;
  settings->mean = 0.0;
  settings->mean2 = 0.0;
  settings->buf = (double *)calloc(1, sizeof(double) * size);
  if(settings->buf == NULL)
  {
    free(settings);
    return NULL;
  }
  settings->buf2 = (double *)calloc(1, sizeof(double) * size);
  if(settings->buf2 == NULL)
  {
    free(settings->buf);
    free(settings);
    return NULL;
  }

  return settings;
}


ravgfiltset_t * create_ravg_filter_copy(ravgfiltset_t *src)
{
  ravgfiltset_t *settings;

  settings = (ravgfiltset_t *) calloc(1, sizeof(ravgfiltset_t));
  if(settings==NULL)
  {
    return NULL;
  }
  memcpy(settings, src, sizeof(ravgfiltset_t));

  settings->buf = (double *)calloc(1, sizeof(double) * settings->size);
  if(settings->buf == NULL)
  {
    free(settings);
    return NULL;
  }
  memcpy(settings->buf, src->buf, settings->size);

  settings->buf2 = (double *)calloc(1, sizeof(double) * settings->size);
  if(settings->buf2 == NULL)
  {
    free(settings->buf);
    free(settings);
    return NULL;
  }
  memcpy(settings->buf2, src->buf2, settings->size);

  return settings;
}


void reset_ravg_filter(double offset, ravgfiltset_t *settings)
{
  int i;

  for(i=0; i<settings->size; i++)
  {
    settings->buf[i] = offset;
  }

  memcpy(settings->buf2, settings->buf, settings->size * sizeof(double));

  settings->mean = offset;

  settings->mean2 = offset;

  settings->buf_pntr = 0;

  settings->buf_pntr2 = 0;
}


void ravg_filter_restore_buf(ravgfiltset_t *settings)
{
  memcpy(settings->buf, settings->buf2, settings->size * sizeof(double));

  settings->mean = settings->mean2;

  settings->buf_pntr = settings->buf_pntr2;
}


void ravg_filter_save_buf(ravgfiltset_t *settings)
{
  memcpy(settings->buf2, settings->buf, settings->size * sizeof(double));

  settings->mean2 = settings->mean;

  settings->buf_pntr2 = settings->buf_pntr;
}


double run_ravg_filter(double new_input, ravgfiltset_t *settings)
{
  int i;


  if(settings->first_sample)
  {
    settings->first_sample = 0;

    for(i=0; i<settings->size; i++)
    {
      settings->buf[i] = new_input;
    }

    settings->mean = new_input;

    return new_input;
  }

  settings->mean -= (settings->buf[settings->buf_pntr] / settings->size);

  settings->buf[settings->buf_pntr] = new_input;

  settings->buf_pntr++;

  if(settings->buf_pntr >= settings->size)
  {
    settings->buf_pntr = 0;
  }

  settings->mean += (new_input / settings->size);

  if(settings->type == 0)  /* highpass */
  {
    return settings->buf[(settings->buf_pntr + (settings->size / 2)) % settings->size] - settings->mean;
  }

  if(settings->type == 1)  /* lowpass */
  {
    return settings->mean;
  }

  return 0.0;
}


void free_ravg_filter(ravgfiltset_t *settings)
{
  if(settings == NULL)
  {
    return;
  }

  free(settings->buf);
  free(settings->buf2);
  free(settings);
}


double ravg_filter_response(int type, int size, double freq)
{
  if((type < 0) || (type > 1) || (size < 2) || (freq > 0.5))
  {
    return 1.0;
  }

  if(type)
  {
    if(freq < 0.000001)
    {
      return 1.0;
    }

    return fabs((sin(M_PI * freq * (double)size)) / ((double)size * sin(M_PI * freq)));
  }

  if(freq < 0.000001)
  {
    return 0.0;
  }

  return 1.0 - fabs((sin(M_PI * freq * (double)size)) / ((double)size * sin(M_PI * freq)));
}








