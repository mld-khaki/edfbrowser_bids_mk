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


#ifndef EDFPLUS_HELPER_H
#define EDFPLUS_HELPER_H


#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "utc_date_time.h"


#ifdef __cplusplus
extern "C" {
#endif

int edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdrblck_t *);
int edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdrblck_t *);
int strip_types_from_label(char *);
int utc_to_edf_startdate(long long, char *);
int to_edf_startdate(int, int, int, char *);
void run_filter_list(sigcompblck_t *, unsigned long long, double *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif





