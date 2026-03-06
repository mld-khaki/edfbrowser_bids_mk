/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 - 2024 Teunis van Beelen
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




#include "mit2edf.h"


#define NOTQRS     (0)  /* not-QRS (not a getann/putann code) */
#define NORMAL     (1)  /* normal beat */
#define LBBB       (2)  /* left bundle branch block beat */
#define RBBB       (3)  /* right bundle branch block beat */
#define ABERR      (4)  /* aberrated atrial premature beat */
#define PVC        (5)  /* premature ventricular contraction */
#define FUSION     (6)  /* fusion of ventricular and normal beat */
#define NPC        (7)  /* nodal (junctional) premature beat */
#define APC        (8)  /* atrial premature contraction */
#define SVPB       (9)  /* premature or ectopic supraventricular beat */
#define VESC      (10)  /* ventricular escape beat */
#define NESC      (11)  /* nodal (junctional) escape beat */
#define PACE      (12)  /* paced beat */
#define UNKNOWN   (13)  /* unclassifiable beat */
#define NOISE     (14)  /* signal quality change */
#define ARFCT     (16)  /* isolated QRS-like artifact */
#define STCH      (18)  /* ST change */
#define TCH       (19)  /* T-wave change */
#define SYSTOLE   (20)  /* systole */
#define DIASTOLE  (21)  /* diastole */
#define NOTE      (22)  /* comment annotation */
#define MEASURE   (23)  /* measurement annotation */
#define PWAVE     (24)  /* P-wave peak */
#define BBB       (25)  /* left or right bundle branch block */
#define PACESP    (26)  /* non-conducted pacer spike */
#define TWAVE     (27)  /* T-wave peak */
#define RHYTHM    (28)  /* rhythm change */
#define UWAVE     (29)  /* U-wave peak */
#define LEARN     (30)  /* learning */
#define FLWAV     (31)  /* ventricular flutter wave */
#define VFON      (32)  /* start of ventricular flutter/fibrillation */
#define VFOFF     (33)  /* end of ventricular flutter/fibrillation */
#define AESC      (34)  /* atrial escape beat */
#define SVESC     (35)  /* supraventricular escape beat */
#define LINK      (36)  /* link to external data (aux contains URL) */
#define NAPC      (37)  /* non-conducted P-wave (blocked APB) */
#define PFUS      (38)  /* fusion of paced and normal beat */
#define WFON      (39)  /* waveform onset */
#define PQ        WFON  /* PQ junction (beginning of QRS) */
#define WFOFF     (40)  /* waveform end */
#define JPT       WFOFF /* J point (end of QRS) */
#define RONT      (41)  /* R-on-T premature ventricular contraction */

/* ... annotation codes between RONT+1 and ACMAX inclusive are user-defined */

#define ACMAX     (49)  /* value of largest valid annot code (must be < 50) */


static char annotdescrlist[52][64]=
{
  "not-QRS",                                      /*  0 */
  "normal beat",                                  /*  1 */
  "left bundle branch block beat",                /*  2 */
  "right bundle branch block beat",               /*  3 */
  "aberrated atrial premature beat",              /*  4 */
  "premature ventricular contraction",            /*  5 */
  "fusion of ventricular and normal beat",        /*  6 */
  "nodal (junctional) premature beat",            /*  7 */
  "atrial premature contraction",                 /*  8 */
  "premature or ectopic supraventricular beat",   /*  9 */
  "ventricular escape beat",                      /* 10 */
  "nodal (junctional) escape beat",               /* 11 */
  "paced beat",                                   /* 12 */
  "unclassifiable beat",                          /* 13 */
  "signal quality change",                        /* 14 */
  "<empty description>",                          /* 15 */
  "isolated QRS-like artifact",                   /* 16 */
  "<empty description>",                          /* 17 */
  "ST change",                                    /* 18 */
  "T-wave change",                                /* 19 */
  "systole",                                      /* 20 */
  "diastole",                                     /* 21 */
  "comment annotation",                           /* 22 */
  "measurement annotation",                       /* 23 */
  "P-wave peak",                                  /* 24 */
  "left or right bundle branch block",            /* 25 */
  "non-conducted pacer spike",                    /* 26 */
  "T-wave peak",                                  /* 27 */
  "rhythm change",                                /* 28 */
  "U-wave peak",                                  /* 29 */
  "learning",                                     /* 30 */
  "ventricular flutter wave",                     /* 31 */
  "start of ventricular flutter/fibrillation",    /* 32 */
  "end of ventricular flutter/fibrillation",      /* 33 */
  "atrial escape beat",                           /* 34 */
  "supraventricular escape beat",                 /* 35 */
  "link to external data (aux contains URL)",     /* 36 */
  "non-conducted P-wave (blocked APB)",           /* 37 */
  "fusion of paced and normal beat",              /* 38 */
  "waveform onset",                               /* 39 */
  "waveform end",                                 /* 40 */
  "R-on-T premature ventricular contraction",     /* 41 */
  "<empty description>",      /* 42 */
  "<empty description>",      /* 43 */
  "<empty description>",      /* 44 */
  "<empty description>",      /* 45 */
  "<empty description>",      /* 46 */
  "<empty description>",      /* 47 */
  "<empty description>",      /* 48 */
  "<empty description>",      /* 49 */
  "<empty description>",      /* 50 */
  "<empty description>"       /* 51 */
};



#define ANNOT_EXT_NUM       (10)
#define ANNOT_EXT_STR_LEN   (16)

static char annotextlist[ANNOT_EXT_NUM][ANNOT_EXT_STR_LEN]=
{
  ".ari",
  ".ecg",
  ".trigger",
  ".qrs",
  ".atr",
  ".apn",
  ".st",
  ".pwave",
  ".marker",
  ".seizures",
};



UI_MIT2EDFwindow::UI_MIT2EDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 480 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("MIT to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  textEdit1 = new QTextEdit;
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  textEdit1->append("MIT (PhysioBank) to EDF+ converter.\n");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(textEdit1, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_MIT2EDFwindow::SelectFileButton()
{
  FILE *header_inputfile=NULL,
       *data_inputfile=NULL,
       *annot_inputfile=NULL;

  int i, j, k, r,
      len,
      hdl,
      has_ext,
      *buf=NULL,
      edf_datrec_duration=0,
      edf_datrec_offset[MAXSIGNALS];

  short *mit_buf16=NULL,
        *edf_buf16=NULL;

  char header_filename[MAX_PATH_LENGTH]={""},
       str1_2048[2048]={""},
       edf_filename[MAX_PATH_LENGTH]={""},
       data_filename[MAX_PATH_LENGTH]={""},
       annot_filename[MAX_PATH_LENGTH]={""},
       filename_x[MAX_PATH_LENGTH]={""},
       scratchpad_4096[4096]={""},
       *charpntr=NULL;

  unsigned char a_buf[128];

  long long filesize;

  union {
    int one;
    unsigned char four[4];
  } var;

  pushButton1->setEnabled(false);

  strlcpy(header_filename, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "MIT header files (*.hea *.HEA);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(header_filename, ""))
  {
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_opendir, header_filename, MAX_PATH_LENGTH);

  header_inputfile = fopeno(header_filename, "rb");
  if(header_inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for reading.\n", header_filename);
    textEdit1->append(QString::fromLocal8Bit(str1_2048));
    pushButton1->setEnabled(true);
    return;
  }

  get_filename_from_path(filename_x, header_filename, MAX_PATH_LENGTH);

  snprintf(str1_2048, 2048, "Read file: %s", filename_x);
  textEdit1->append(QString::fromLocal8Bit(str1_2048));

  remove_extension_from_filename(filename_x);

  charpntr = fgets(scratchpad_4096, 4095, header_inputfile);
  if(charpntr == NULL)
  {
    textEdit1->append("Cannot read header file. (error 1)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  len = strlen(charpntr);
  if(len < 6)
  {
    textEdit1->append("Cannot read header file. (error 2)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  charpntr = strtok(charpntr, " ");
  if(charpntr == NULL)
  {
    textEdit1->append("Cannot read header file. (error 3)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(strcmp(charpntr, filename_x))
  {
    textEdit1->append("Cannot read header file. (error 4)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  charpntr = strtok(NULL, " ");
  if(charpntr == NULL)
  {
    textEdit1->append("Cannot read header file. (error 5)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.chns = atoi(charpntr);

  if(mit_hdr.chns < 1)
  {
    textEdit1->append("Error, number of signals is less than one. (error 6)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.chns > MAXSIGNALS)
  {
    textEdit1->append("Error, Too many signals in header. (error 7)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  charpntr = strtok(NULL, " ");
  if(charpntr == NULL)
  {
    textEdit1->append("Cannot read header file. (error 8)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  len = strlen(charpntr);

  mit_hdr.sf_base_fl = atof(charpntr);

  if(mit_hdr.sf_base_fl < 0.009999999)
  {
    textEdit1->append("Samplerate too low. (error 204)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.sf_base = atoi(charpntr);

  mit_hdr.sf_base_fraction = (mit_hdr.sf_base_fl * 1000) - (((int)mit_hdr.sf_base_fl) * 1000);

  if(mit_hdr.sf_base_fraction)
  {
    if(1000 % mit_hdr.sf_base_fraction)
    {
      textEdit1->append("The recording uses an irrational samplerate which is not supported by this converter. (error 201)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    mit_hdr.frame_smpl_pack_multiplier = (mit_hdr.sf_base * (1000 / mit_hdr.sf_base_fraction)) + 1;

    edf_datrec_duration = 100000 * (1000 / mit_hdr.sf_base_fraction);
  }
  else
  {
    mit_hdr.frame_smpl_pack_multiplier = mit_hdr.sf_base;

    edf_datrec_duration = 100000;
  }

//   printf("mit_hdr.sf_base_fraction: %i\n", mit_hdr.sf_base_fraction);
//
//   printf("mit_hdr.frame_smpl_pack_multiplier: %i\n", mit_hdr.frame_smpl_pack_multiplier);

  if(mit_hdr.sf_base < 1)
  {
    textEdit1->append("Error, samplefrequency is less than 1 Hz. (error 9)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.sf_base > 100000)
  {
    textEdit1->append("Error, samplefrequency is more than 100000 Hz. (error 10)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.sf_multiple = 0;

  strlcat(filename_x, ".dat", MAX_PATH_LENGTH);

  for(j=0; j<mit_hdr.chns; j++)
  {
    mit_hdr.frame_smpl_pack[j] = 1;

    mit_hdr.adc_gain[j] = 200.0;

    mit_hdr.adc_resolution[j] = 12;

    mit_hdr.adc_zero[j] = 0;

    mit_hdr.init_val[j] = 0;

    mit_hdr.baseline[j] = 0;

    mit_hdr.baseline_present[j] = 0;

    mit_hdr.unit_multiplier[j] = 1;  /* default 1 milliVolt */

    strlcpy(mit_hdr.unit[j], "mV", 9);

    snprintf(mit_hdr.label[j], 17, "chan. %i", j + 1);

    charpntr = fgets(scratchpad_4096, 4095, header_inputfile);
    if(charpntr == NULL)
    {
      textEdit1->append("Cannot read header file. (error 11)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    len = strlen(charpntr);
    if(len < 6)
    {
      textEdit1->append("Cannot read header file. (error 12)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    charpntr = strtok(charpntr, " ");
    if(charpntr == NULL)
    {
      textEdit1->append("Cannot read header file. (error 13)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    if(strcmp(charpntr, filename_x))
    {
      textEdit1->append("Error, filenames are different. (error 14)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      textEdit1->append("Cannot read header file. (error 15)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    len = strlen(charpntr);

    has_ext = 0;

    for(i=0; ; i++)
    {
      if(charpntr[i] == 0)
      {
        break;
      }

      if((charpntr[i] == ':') || (charpntr[i] == '+'))
      {
        has_ext = 1;
      }
    }

    if(has_ext)
    {
      textEdit1->append("The recording uses a format extension which is not (yet) supported by this converter. (error 202)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    mit_hdr.format[j] = atoi(charpntr);

    if((mit_hdr.format[j] != 212) &&
      (mit_hdr.format[j] != 16) &&
      (mit_hdr.format[j] != 32) &&
      (mit_hdr.format[j] != 61))
    {
      snprintf(str1_2048, 2048, "Error, unsupported format: %i  (error 16)\n", mit_hdr.format[j]);
      textEdit1->append(str1_2048);
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    if(j>0)
    {
      if(mit_hdr.format[j] != mit_hdr.format[0])
      {
        textEdit1->append("Error, different formats in the same file. (error 17)\n");
        fclose(header_inputfile);
        pushButton1->setEnabled(true);
        return;
      }
    }

    for(k=0; k<len; k++)
    {
      if((charpntr[k] < '0') || (charpntr[k] > '9'))
      {
        break;
      }
    }

    if(charpntr[k] == 'x')
    {
      if(mit_hdr.format[j] != 16)
      {
        textEdit1->append("Error, found different samplerates and format is not \"16\". (error 176)\n");
        fclose(header_inputfile);
        pushButton1->setEnabled(true);
        return;
      }

      mit_hdr.sf_multiple = 1;

      k++;

      if((charpntr[k] >= '0') && (charpntr[k] <= '9'))
      {
        mit_hdr.frame_smpl_pack[j] = atoi(charpntr + k);

        if(mit_hdr.frame_smpl_pack[j] < 1)
        {
          textEdit1->append("Error, wrong samplerate multiplier in header. (error 177)\n");
          fclose(header_inputfile);
          pushButton1->setEnabled(true);
          return;
        }
      }
    }

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    len = strlen(charpntr);

    mit_hdr.adc_gain[j] = atof(charpntr);

    if(abs(mit_hdr.adc_gain[j]) < 1e-9)  mit_hdr.adc_gain[j] = 200.0;

    for(; ; charpntr++)
    {
      if((charpntr[0] == 0) || (charpntr[0] == '(') || (charpntr[0] == '/'))
      {
        break;
      }
    }

    if(charpntr[0] == '(')
    {
      charpntr++;

      mit_hdr.baseline[j] = atoi(charpntr);

      mit_hdr.baseline_present[j] = 1;

      for(; ; charpntr++)
      {
        if((charpntr[0] == 0) || (charpntr[0] == '/'))
        {
          break;
        }
      }
    }

    if(charpntr[0] == '/')
    {
      charpntr++;

      strncpy(mit_hdr.unit[j], charpntr, 8);
      mit_hdr.unit[j][8] = 0;
    }
//     else
//     {
//       strlcpy(mit_hdr.unit[j], "uV", 9);
//
//       mit_hdr.unit_multiplier[j] = 1000;
//     }

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    len = strlen(charpntr);

    mit_hdr.adc_resolution[j] = atoi(charpntr);

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    len = strlen(charpntr);

    mit_hdr.adc_zero[j] = atoi(charpntr);

    if(!mit_hdr.baseline_present[j])
    {
      mit_hdr.baseline[j] = mit_hdr.adc_zero[j];
    }

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    len = strlen(charpntr);

    mit_hdr.init_val[j] = atoi(charpntr);

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    // skip

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    // skip

    charpntr = strtok(NULL, " ");
    if(charpntr == NULL)
    {
      continue;
    }

    len = strlen(charpntr);

    strncpy(mit_hdr.label[j], charpntr, 16);

    mit_hdr.label[j][16] = 0;
  }

  fclose(header_inputfile);

  strlcpy(data_filename, header_filename, MAX_PATH_LENGTH);

  remove_extension_from_filename(data_filename);

  strlcpy(edf_filename, data_filename, MAX_PATH_LENGTH);

  strlcpy(annot_filename, data_filename, MAX_PATH_LENGTH);

  strlcat(data_filename, ".dat", MAX_PATH_LENGTH);

  strlcat(edf_filename, ".edf", MAX_PATH_LENGTH);

  strlcat(annot_filename, ".atr", MAX_PATH_LENGTH);

  data_inputfile = fopeno(data_filename, "rb");
  if(data_inputfile==NULL)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for reading.\n", data_filename);
    textEdit1->append(QString::fromLocal8Bit(str1_2048));
    pushButton1->setEnabled(true);
    return;
  }

  get_filename_from_path(filename_x, data_filename, MAX_PATH_LENGTH);

  snprintf(str1_2048, 2048, "Read file: %s (format: %i)", filename_x, mit_hdr.format[0]);
  textEdit1->append(QString::fromLocal8Bit(str1_2048));

  remove_extension_from_filename(filename_x);

  fseeko(data_inputfile, 0LL, SEEK_END);
  filesize = ftello(data_inputfile);
  if(filesize < (mit_hdr.chns * mit_hdr.sf_base * 45 / 10))  //FIXME
  {
    textEdit1->append("Error, .dat filesize is too small.\n");
    fclose(data_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.sf_multiple)
  {
    mit_hdr.sf_div = 1;
  }
  else
  {
    for(mit_hdr.sf_div=10; mit_hdr.sf_div>0; mit_hdr.sf_div--)
    {
      if(mit_hdr.sf_div == 9)  continue;
      if(mit_hdr.sf_div == 7)  continue;
      if(mit_hdr.sf_div == 6)  continue;
      if(mit_hdr.sf_div == 3)  continue;

      if(!(mit_hdr.sf_base % mit_hdr.sf_div))  break;
    }
  }

  if(mit_hdr.sf_div < 1)  mit_hdr.sf_div = 1;

  mit_hdr.sf_block = mit_hdr.sf_base / mit_hdr.sf_div;

  if(!mit_hdr.sf_multiple)
  {
    edf_datrec_duration = 100000 / mit_hdr.sf_div;
  }

  hdl = edfopen_file_writeonly(edf_filename, EDFLIB_FILETYPE_EDFPLUS, mit_hdr.chns);
  if(hdl<0)
  {
    snprintf(str1_2048, 2048, "Cannot open file %s for writing.\n", edf_filename);
    textEdit1->append(QString::fromLocal8Bit(str1_2048));
    fclose(data_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.sf_div == 1)
  {
    if(edf_set_number_of_annotation_signals(hdl, 2))
    {
      textEdit1->append("Error: edf_set_number_of_annotation_signals()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(mit_hdr.sf_multiple)
    {
      if(edf_set_samplefrequency(hdl, i, mit_hdr.frame_smpl_pack[i] * mit_hdr.frame_smpl_pack_multiplier))
      {
        textEdit1->append("Error: edf_set_samplefrequency()\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        pushButton1->setEnabled(true);
        return;
      }
    }
    else
    {
      if(edf_set_samplefrequency(hdl, i, mit_hdr.sf_block))
      {
        textEdit1->append("Error: edf_set_samplefrequency()\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_digital_minimum(hdl, i, -32768))
    {
      textEdit1->append("Error: edf_set_digital_minimum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_digital_maximum(hdl, i, 32767))
    {
      textEdit1->append("Error: edf_set_digital_maximum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_label(hdl, i, mit_hdr.label[i]))
    {
      textEdit1->append("Error: edf_set_label()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(!strcmp(mit_hdr.unit[i], "mV"))
    {
      if(((double)((32767 + mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i]) <= 100)
      {
        if(((double)((-32768 + mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i]) >= -100)
        {
          strlcpy(mit_hdr.unit[i], "uV", 9);

          mit_hdr.unit_multiplier[i] *= 1000;
        }
      }
    }

//     printf("physmax: %f    physmin: %f  adcgain: %f   adczero: %i   baseline: %i   unit: %s   multiplier: %i\n",
//       (double)((32767 + mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i],
//       (double)((-32768 + mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i],
//       mit_hdr.adc_gain[i], mit_hdr.adc_zero[i], mit_hdr.baseline[i], mit_hdr.unit[i], mit_hdr.unit_multiplier[i]);

    if(edf_set_physical_maximum(hdl, i, (double)((32767 - mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i]))
    {
      textEdit1->append("Error: edf_set_physical_maximum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_physical_minimum(hdl, i, (double)((-32768 - mit_hdr.baseline[i]) * mit_hdr.unit_multiplier[i]) / mit_hdr.adc_gain[i]))
    {
      textEdit1->append("Error: edf_set_physical_minimum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_physical_dimension(hdl, i, mit_hdr.unit[i]))
    {
      textEdit1->append("Error: edf_set_physical_dimension()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  if(edf_set_datarecord_duration(hdl, edf_datrec_duration))
  {
    textEdit1->append("Error: edf_set_datarecord_duration()\n");
    fclose(data_inputfile);
    edfclose_file(hdl);
    pushButton1->setEnabled(true);
    return;
  }

/////////////////// Start conversion //////////////////////////////////////////

  int blocks, tmp1, tmp2, l_end=0, odd_even=0, mit_datrec_sz=0, edf_datrec_sz=0,
      mit_buf_offset;

  for(i=0; i<mit_hdr.chns; i++)
  {
    mit_datrec_sz += (mit_hdr.frame_smpl_pack[i] * 2);

    edf_datrec_sz += (mit_hdr.frame_smpl_pack[i] * mit_hdr.frame_smpl_pack_multiplier * 2);

    if(i == 0)
    {
      edf_datrec_offset[0] = 0;
    }
    else
    {
      edf_datrec_offset[i] = (mit_hdr.frame_smpl_pack[i-1] * mit_hdr.frame_smpl_pack_multiplier) + edf_datrec_offset[i-1];
    }
  }

  if((mit_hdr.format[0] == 16) && mit_hdr.sf_multiple)
  {
    mit_buf16 = (short *)malloc((mit_datrec_sz / 2) * mit_hdr.frame_smpl_pack_multiplier * sizeof(short));
    if(mit_buf16 == NULL)
    {
      textEdit1->append("Malloc() error (mit_buf16)\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    edf_buf16 = (short *)malloc((edf_datrec_sz / 2) * sizeof(short));
    if(edf_buf16 == NULL)
    {
      textEdit1->append("Malloc() error (edf_buf16)\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    blocks = filesize / (mit_datrec_sz * mit_hdr.frame_smpl_pack_multiplier);
  }
  else
  {
    buf = (int *)malloc(mit_hdr.sf_block * mit_hdr.chns * sizeof(int));
    if(buf == NULL)
    {
      textEdit1->append("Malloc() error (buf)\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    blocks = filesize / (mit_hdr.sf_block * mit_hdr.chns);
  }

  fseeko(data_inputfile, 0LL, SEEK_SET);

  QProgressDialog progress("Converting digitized signals ...", "Abort", 0, blocks);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  if(mit_hdr.format[0] == 212)
  {
    blocks *= 10;
    blocks /= 15;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          if(odd_even)
          {
            odd_even = 0;

            tmp1 = fgetc(data_inputfile);

            if(tmp1 == EOF)
            {
              goto OUT1;
            }

            buf[j * mit_hdr.sf_block + i] = (tmp2 & 0xf0) << 4;
            buf[j * mit_hdr.sf_block + i] += tmp1;
            if(buf[j * mit_hdr.sf_block + i] & 0x800)
            {
              buf[j * mit_hdr.sf_block + i] |= 0xfffff000;
            }
          }
          else
          {
            odd_even = 1;

            tmp1 = fgetc(data_inputfile);
            tmp2 = fgetc(data_inputfile);

            buf[j * mit_hdr.sf_block + i] = (tmp2 & 0x0f) << 8;
            buf[j * mit_hdr.sf_block + i] += tmp1;
            if(buf[j * mit_hdr.sf_block + i] & 0x800)
            {
              buf[j * mit_hdr.sf_block + i] |= 0xfffff000;
            }
          }
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if((mit_hdr.format[0] == 16) && mit_hdr.sf_multiple)
  {
    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 10))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      if(fread(mit_buf16, mit_datrec_sz * mit_hdr.frame_smpl_pack_multiplier, 1, data_inputfile) != 1)
      {
        progress.reset();
        textEdit1->append("A read error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }

      for(i=0; i<mit_hdr.frame_smpl_pack_multiplier; i++)
      {
        mit_buf_offset = (mit_datrec_sz / 2) * i;

        for(j=0; j<mit_hdr.chns; j++)
        {
          for(r=0; r<mit_hdr.frame_smpl_pack[j]; r++)
          {
            edf_buf16[edf_datrec_offset[j] + (i * mit_hdr.frame_smpl_pack[j]) + r] = mit_buf16[mit_buf_offset + r];
          }

          mit_buf_offset += mit_hdr.frame_smpl_pack[j];
        }
      }

      if(edf_blockwrite_digital_short_samples(hdl, edf_buf16))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if(((mit_hdr.format[0] == 16) || (mit_hdr.format[0] == 61)) && (!mit_hdr.sf_multiple))
  {
    if(mit_hdr.format[0] == 16)  l_end = 1;

    blocks /= 2;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          tmp1 = fgetc(data_inputfile);
          if(tmp1 == EOF)
          {
            goto OUT1;
          }

          if(l_end)
          {
            tmp1 += (fgetc(data_inputfile) << 8);
          }
          else
          {
            tmp1 <<= 8;

            tmp1 += fgetc(data_inputfile);
          }

          if(tmp1 & 0x8000)
          {
            tmp1 |= 0xffff0000;
          }

          buf[j * mit_hdr.sf_block + i] = tmp1;
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if(mit_hdr.format[0] == 32)
  {
    blocks /= 4;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          tmp1 = fgetc(data_inputfile);
          if(tmp1 == EOF)
          {
            goto OUT1;
          }

          var.four[0] = tmp1;
          var.four[1] = fgetc(data_inputfile);
          var.four[2] = fgetc(data_inputfile);
          var.four[3] = fgetc(data_inputfile);

          buf[j * mit_hdr.sf_block + i] = var.one;
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

OUT1:

  progress.reset();

  qApp->processEvents();

/////////////////// End conversion //////////////////////////////////////////

  fclose(data_inputfile);

  free(buf);

  free(mit_buf16);

  free(edf_buf16);

  int annot_code, skip, total_annots=0, annot_signal=0;

  long long bytes_read, tc;

  get_filename_from_path(filename_x, annot_filename, MAX_PATH_LENGTH);

  for(k=0; k<ANNOT_EXT_NUM; k++)
  {
    tc = 0LL;

    remove_extension_from_filename(annot_filename);

    strlcat(annot_filename, annotextlist[k], MAX_PATH_LENGTH);

    annot_inputfile = fopeno(annot_filename, "rb");
    if(annot_inputfile==NULL)
    {
      continue;
    }

    get_filename_from_path(filename_x, annot_filename, MAX_PATH_LENGTH);

    if(annot_inputfile==NULL)
    {
      snprintf(str1_2048, 2048, "Cannot open file %s for reading.\n"
                                 "Annotations cannot be included.", filename_x);
      textEdit1->append(QString::fromLocal8Bit(str1_2048));
    }
    else
    {
      snprintf(str1_2048, 2048, "Read file: %s", filename_x);
      textEdit1->append(QString::fromLocal8Bit(str1_2048));

      fseeko(annot_inputfile, 0LL, SEEK_END);
      filesize = ftello(annot_inputfile);

      progress.setLabelText("Converting annotations ...");
      progress.setMinimum(0);
      progress.setMaximum(filesize);

      fseeko(annot_inputfile, 0LL, SEEK_SET);

      for(bytes_read=0LL; bytes_read < filesize; bytes_read += 2LL)
      {
        if(!(bytes_read % 100))
        {
          progress.setValue(bytes_read);

          qApp->processEvents();

          if(progress.wasCanceled() == true)
          {
            textEdit1->append("Conversion aborted by user.\n");

            break;
          }
        }

        skip = 0;

        if(fread(a_buf, 2, 1, annot_inputfile) != 1)
        {
          break;
        }

        if(*((unsigned short *)a_buf) == 0)  // end of file
        {
          break;
        }

        annot_code = a_buf[1] >> 2;

        if(annot_code == 59)
        {
          if(fread(a_buf, 4, 1, annot_inputfile) != 1)
          {
            break;
          }

          tc += (*((unsigned short *)a_buf) << 16);

          tc += *((unsigned short *)(a_buf + 2));
        }
        else if(annot_code == 62)
          {
            annot_signal = *((unsigned short *)a_buf) & 0x3ff;

            if(annot_signal >= mit_hdr.chns)
            {
              annot_signal = 0;
            }
          }
          else if(annot_code == 63)
            {
              skip = *((unsigned short *)a_buf) & 0x3ff;

              if(skip % 2) skip++;
            }
            else if((annot_code >= 0) && (annot_code <= ACMAX))
              {
                tc += *((unsigned short *)a_buf) & 0x3ff;

                if(annot_code < 42)
                {
                  edfwrite_annotation_latin1_hr(hdl, (tc * 1000000LL) / mit_hdr.sf_base_fl, -1, annotdescrlist[annot_code]);
                }
                else
                {
                  edfwrite_annotation_latin1_hr(hdl, (tc * 1000000LL) / mit_hdr.sf_base_fl, -1, "user-defined");
                }

                total_annots++;
              }

        if(skip)
        {
          if(fseek(annot_inputfile, skip, SEEK_CUR) < 0)
          {
            break;
          }

          bytes_read += skip;
        }
      }

      fclose(annot_inputfile);
    }
  }

  if(total_annots)
  {
    snprintf(str1_2048, 2048, "Read %i annotations.", total_annots);
    textEdit1->append(str1_2048);
  }

  progress.reset();

  edfclose_file(hdl);

  textEdit1->append("Ready.\n");

  pushButton1->setEnabled(true);
}

















