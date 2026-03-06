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


#ifndef CHECK_EDF_FILE1_H
#define CHECK_EDF_FILE1_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "utc_date_time.h"
#include "utils.h"


#ifdef __cplusplus
extern "C" {
#endif

edfhdrblck_t * check_edf_file(FILE *, char *, int, int, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // CHECK_EDF_FILE1_H





