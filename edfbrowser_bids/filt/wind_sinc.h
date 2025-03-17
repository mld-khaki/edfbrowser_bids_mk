/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2023 - 2024 Teunis van Beelen
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


#ifndef T_WIND_SINC_INCLUDED
#define T_WIND_SINC_INCLUDED


#define T_FIR_TYPE_HP    (0)
#define T_FIR_TYPE_LP    (1)
#define T_FIR_TYPE_BP    (2)
#define T_FIR_TYPE_BS    (3)
#define T_FIR_MAX_TYPES  (4)


#ifdef __cplusplus
extern "C" {
#endif


int wind_sinc(double, int, double, double, double, double **);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif






