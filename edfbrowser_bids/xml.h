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


#ifndef xml_INCLUDED
#define xml_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"



#define XML_STRBUFLEN        (4096)
/* XML max element depth */
#define XML_MAX_ED             (32)

#define XML_ERROR_GEN          (-1)
#define XML_ERROR_STRLEN       (-2)
#define XML_ERROR_MEMBUFSZ     (-3)
#define XML_ERROR_INV_HDL      (-4)
#define XML_ERROR_NOTFOUND     (-5)
#define XML_ERROR_MALLOC       (-6)


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  FILE *file;
  int encoding;
  int level;
  int offset[XML_MAX_ED];
  int one_tag[XML_MAX_ED];
  char *elementname[XML_MAX_ED];
  int elementname_sz[XML_MAX_ED];
  char *attributes[XML_MAX_ED];
  int attributes_sz[XML_MAX_ED];
  char *tag_search_result;
  int tag_search_result_sz;
  char *content[XML_MAX_ED];
  int content_sz[XML_MAX_ED];
} xml_hdl_t;


xml_hdl_t * xml_get_handle(const char *);
void xml_close(xml_hdl_t *);
void xml_go_up(xml_hdl_t *);
int xml_goto_nth_element_inside(xml_hdl_t *, const char *, int);
int xml_goto_next_element_with_same_name(xml_hdl_t *);
int xml_goto_next_element_at_same_level(xml_hdl_t *);
void xml_goto_root(xml_hdl_t *);
int xml_get_content_of_element(xml_hdl_t *, char *, int);
int xml_character_encoding_type(xml_hdl_t *);
int xml_get_attribute_of_element(xml_hdl_t *, const char *, char *, int);
int xml_get_name_of_element(xml_hdl_t *, char *, int);
void xml_fwrite_encode_entity(FILE *, const char *);
void xml_fnwrite_encode_entity(FILE *, const char *, int);
void xml_strlcpy_encode_entity(char *, const char *, int);
int xml_strncpy_encode_entity(char *, const char *, int);
void xml_strlcpy_decode_entity(char *, const char *, int);
int xml_strncpy_decode_entity(char *, const char *, int);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif












































