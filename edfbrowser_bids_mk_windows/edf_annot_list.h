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


#ifndef EDFPLUS_ANNOT_LIST_H
#define EDFPLUS_ANNOT_LIST_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

#define MAX_ANNOTATION_LEN_II   512


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  void *edfhdr;
  long long onset;
  char duration[20];
  long long long_duration;
  long long file_offset;
  int datrec;
  char description[MAX_ANNOTATION_LEN_II + 1];
  int modified;
  int x_pos;
  int grabbed;
  int hidden;
  unsigned int ident;
  int selected_in_dock;
} annotblck_t;

typedef struct
{
  annotblck_t *items;
  int sz;
  int used_sz;
  int mem_sz;
  int *idx;
} annotlist_t;


int edfplus_annotation_add_item(annotlist_t *, annotblck_t);
int edfplus_annotation_size(annotlist_t *);
int edfplus_annotation_cnt(annotlist_t *, annotblck_t *);
void edfplus_annotation_empty_list(annotlist_t *);
void edfplus_annotation_remove_item(annotlist_t *, int);
annotblck_t * edfplus_annotation_get_item(annotlist_t *, int);
annotblck_t * edfplus_annotation_get_item_visible_only(annotlist_t *, int);
annotblck_t * edfplus_annotation_get_item_visible_only_cached(annotlist_t *, int, int *, int *);
int edfplus_annotation_get_index(annotlist_t *, annotblck_t *);
void edfplus_annotation_sort(annotlist_t *, void (*)(void));
annotlist_t * edfplus_annotation_create_list_copy(annotlist_t *);
int edfplus_annotation_remove_duplicates(annotlist_t *);
int edfplus_annotation_delete_multiple(annotlist_t *, const char *);
int edfplus_annotation_rename_multiple(annotlist_t *, const char *, const char *);
int edfplus_annotation_get_max_annotation_strlen(annotlist_t *);
void edfplus_annotation_copy_list(annotlist_t *, annotlist_t *);
long long edfplus_annotation_get_long_from_number(const char *);
int edfplus_annotation_get_index_at(annotlist_t *, long long, int);
void edfplus_annotation_cancel_all_selected_in_dock(annotlist_t *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif




