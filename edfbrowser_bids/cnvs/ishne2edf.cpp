/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2019 - 2024 Teunis van Beelen
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



#include "ishne2edf.h"


/*
 *                      |--------- CRC Domain (header) ----------|
 * +--------------+-----+------------------+---------------------+----------+
 * | Magic Number | CRC | fixed-size block | variable-size block | ECG-data |
 * +--------------+-----+------------------+---------------------+----------+
 * |      8       |  2  |       512        |         var         |    var   |
 * +--------------+-----+------------------+---------------------+----------+
 */

#define ISHNE_MAX_CHNS   (12)

#define MAGICNUMBER_HDROFF        (0)  /* string "ISHNE1.0"  8 bytes */
#define CRC_HDROFF                (8)  /* CRC-CCITT checksum 2 bytes */
#define VARLENBLCKSZ_HDROFF      (10)  /* Size (in bytes) of variable length block, integer, 4 bytes */
#define ECGSMPLSZ_HDROFF         (14)  /* Size (in samples) of ECG, integer, 4 bytes */
#define VARLENBLCKSTART_HDROFF   (18)  /* Offset of variable length block (in bytes from beginning of file, i.e. 8 + 2 + 512 = 522), integer, 4 bytes */
#define ECGSMPLBLCKSTART_HDROFF  (22)  /* Offset of ECG block (in bytes from beginning of file), integer, 4 bytes */
#define FILEVER_HDROFF           (26)  /* Version of the file, short int, 2 bytes */
#define SUBJ1STNAME_HDROFF       (28)  /* Subject First Name, char[40], 40 bytes */
#define SUBJLASTNAME_HDROFF      (68)  /* Subject Last Name, char[40], 40 bytes */
#define SUBJID_HDROFF           (108)  /* Subject ID, char[20], 20 bytes */
#define SUBJSEX_HDROFF          (128)  /* Subject Sex (0: unknown, 1: male, 2: female), short int, 2 bytes */
#define SUBJRACE_HDROFF         (130)  /* Subject Race (0: unknown, 1: Caucasian, 2 Black, 3 Oriental, 4-9 Reserved), short int, 2 bytes */
#define SUBJBIRTHDATE_HDROFF    (132)  /* Date of Birth (European: day, month, year), 3 short int, 6 bytes */
#define STARTDATE_HDROFF        (138)  /* Date of recording (European), 3 short int, 6 bytes */
#define CREATEDATE_HDROFF       (144)  /* Date of creation of output file (European), 3 short int, 6 bytes */
#define STARTTIME_HDROFF        (150)  /* Start time (European: hour [0-23], min, sec), 3 short int, 6 bytes */
#define NUMLEADS_HDROFF         (156)  /* Number of stored leads, short int, 2 bytes */
#define LEADSPEC_HDROFF         (158)  /* Lead specification (see included Table), 12 short int, 24 bytes */
#define LEADQUAL_HDROFF         (182)  /* Lead quality (see included Table), 12 short int, 24 bytes */
#define AMPRES_HDROFF           (206)  /* Amplitude resolution in integer no. of nV, 12 short int, 24 bytes */
#define PACEMAKCODE_HDROFF      (230)  /* Pacemaker code (see text for description), short int, 2 bytes */
#define RECORDERTYPE_HDROFF     (232)  /* Type of recorder (either analog or digital), char[40], 40 bytes */
#define SMPLRATE_HDROFF         (272)  /* Sampling rate (in hertz), short int, 2 bytes */
#define PROPECG_HDROFF          (274)  /* Proprietary of ECG (if any), char[80], 80 bytes */
#define COPYRIGHT_HDROFF        (354)  /* Copyright and restriction of diffusion (if any), char[80], 80 bytes */
#define RESERV_HDROFF           (434)  /* Reserved, char[88], 88 bytes */


UI_IshneEDFwindow::UI_IshneEDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 480 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("ISHNE ECG to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  textEdit1 = new QTextEdit;
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  textEdit1->append("ISHNE ECG to EDF converter\n");

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  crc_ccitt_init();

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(textEdit1, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_IshneEDFwindow::SelectFileButton()
{
int i, j,
    var_block_sz,
    ecg_smpl_sz,
    var_block_offset,
    ecg_block_offset,
    chns,
    sf,
    lead_spec[ISHNE_MAX_CHNS],
    amp_resolution[ISHNE_MAX_CHNS],
    sex,
    pacemaker,
    edf_hdl=-99,
    birthdate_valid=0,
    total_blocks=0,
    blocks_written=0;

  unsigned long long int file_sz=0;

  long long onset=0;

  short *inbuf=NULL,
        *outbuf=NULL;

  char path[MAX_PATH_LENGTH]={""},
       hdr_4096[4096]={""},
       scratchpad_4096[4096]={""},
       str1_128[128]={""},
       rbuf[4]={0,0,0,0},
       annot_descr_128[128]="";

  const char *lead_label[20]={"Unknown","Generic bipolar","X bipolar","Y bipolar","Z bipolar","I","II","III","aVR","aVL",
                              "aVF","V1","V2","V3","V4","V5","V6","ES","AS","AI"};

  const char *pacemaker_list[]={"No pacemaker","Pacemaker type not known","Single chamber unipolar",
                                "Dual chamber unipolar","Single chamber bipolar","Dual chamber bipolar",""};

  FILE *inputfile=NULL,
       *annotfile=NULL;

  QProgressDialog progress("Converting ECG data ...", "Abort", 0, 0, myobjectDialog);
                  progress.reset();

struct
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
} start_dt;

struct
{
  int day;
  int month;
  int year;
} birth_d;

///////////////////////////////////////// OPEN THE ECG FILE ///////////////////////

  strlcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "ECG files (*.ecg *.ECG)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopen(path, "rb");

  if(inputfile == NULL)
  {
    snprintf(scratchpad_4096, 4096, "Error, cannot open file:\n%s\n", path);
    textEdit1->append(QString::fromLocal8Bit(scratchpad_4096));
    return;
  }

  snprintf(scratchpad_4096, 4096, "Processing ecg file:\n%s", path);
  textEdit1->append(QString::fromLocal8Bit(scratchpad_4096));

  remove_extension_from_filename(path);
  strlcat(path, ".ann", MAX_PATH_LENGTH);

  annotfile = fopen(path, "rb");

  fseek(inputfile, 0, SEEK_END);

  file_sz = ftell(inputfile);

  if(file_sz < 522)
  {
    textEdit1->append("Error, file is too small.");
    goto OUT_EXIT;
  }

  rewind(inputfile);

  if(fread(hdr_4096, 522, 1, inputfile) != 1)
  {
    textEdit1->append("A read error occurred when trying to read the header.");
    goto OUT_EXIT;
  }

////////////////////////////////// GET THE HEADER DATA ////////////////////////

  if(strncmp(hdr_4096 + MAGICNUMBER_HDROFF, "ISHNE1.0", 8))
  {
    textEdit1->append("Error, wrong magic string in header.");
    goto OUT_EXIT;
  }

  var_block_sz = *((int *)(hdr_4096 + VARLENBLCKSZ_HDROFF));
  if(var_block_sz < 0)
  {
    textEdit1->append("Error, parameter size of variable length block in header is lower than zero.");
    goto OUT_EXIT;
  }

  ecg_smpl_sz = *((int *)(hdr_4096 + ECGSMPLSZ_HDROFF));
  if(ecg_smpl_sz < 0)
  {
    textEdit1->append("Error, parameter ECG sample size in header is lower than zero.");
    goto OUT_EXIT;
  }

  var_block_offset = *((int *)(hdr_4096 + VARLENBLCKSTART_HDROFF));
  if(var_block_offset != 522)
  {
    textEdit1->append("Error, parameter offset of variable length block in header differs from 522");
    goto OUT_EXIT;
  }

  ecg_block_offset = *((int *)(hdr_4096 + ECGSMPLBLCKSTART_HDROFF));
  if(ecg_block_offset != (var_block_sz + 522))
  {
    textEdit1->append("Error, parameter offset of ECG block in header differs from 522 + variable length block");
    goto OUT_EXIT;
  }

  if((unsigned long)ecg_block_offset > file_sz)
  {
    textEdit1->append("Error, start of ECG block is after end of file.");
    goto OUT_EXIT;
  }

  chns = *((short *)(hdr_4096 + NUMLEADS_HDROFF));
  if(chns < 1)
  {
    textEdit1->append("Error, number of leads is less than 1.");
    goto OUT_EXIT;
  }

  if(chns > ISHNE_MAX_CHNS)
  {
    textEdit1->append("Error, number of leads is more than 12.");
    goto OUT_EXIT;
  }

  sf = *((short *)(hdr_4096 + SMPLRATE_HDROFF));
  if(sf < 1)
  {
    textEdit1->append("Error, sampling rate is less than 1Hz.");
    goto OUT_EXIT;
  }

  sex = *((short *)(hdr_4096 + SUBJSEX_HDROFF));
  if((sex != 1) && (sex != 2))
  {
    sex = 0;
  }

  pacemaker = *((short *)(hdr_4096 + PACEMAKCODE_HDROFF));
  if((pacemaker < 0) && (pacemaker > 5))
  {
    pacemaker = 0;
  }

  total_blocks = ecg_smpl_sz / sf;
  if(total_blocks < 1)
  {
    textEdit1->append("Error, not enough samples in file (recording duration is less than one second).");
    goto OUT_EXIT;
  }

  if(file_sz < (unsigned int)(ecg_block_offset + (ecg_smpl_sz * chns * 2)))
  {
    textEdit1->append("Error, file size is less than file size based on header parameters.");
    goto OUT_EXIT;
  }

  if(check_crc(inputfile, var_block_offset + var_block_sz))
  {
    textEdit1->append("CRC error, file header is corrupt.");
    goto OUT_EXIT;
  }

////////////////////////////// GET THE LEAD DATA ////////////////////////////////////

  for(i=0; i<chns; i++)
  {
    lead_spec[i] = *((short *)(hdr_4096 + 158 + (i * 2)));
    if((lead_spec[i] < 0) || (lead_spec[i] > 19))
    {
      snprintf(scratchpad_4096, 4096, "Error, lead specification of lead %i is out of range.", i + 1);
      textEdit1->append(scratchpad_4096);
      goto OUT_EXIT;
    }

    amp_resolution[i] = *((short *)(hdr_4096 + 206 + (i * 2)));
    if(amp_resolution[i] < 1)
    {
      snprintf(scratchpad_4096, 4096, "Error, amplitude resolution of lead %i is less than 1nV.", i + 1);
      textEdit1->append(scratchpad_4096);
      goto OUT_EXIT;
    }
  }

/////////////////////////////////////// GET THE START DATE AND TIME /////////////////////

  start_dt.day = *((short *)(hdr_4096 + STARTDATE_HDROFF));
  if((start_dt.day < 1) || (start_dt.day > 31))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.month = *((short *)(hdr_4096 + STARTDATE_HDROFF + 2));
  if((start_dt.month < 1) || (start_dt.month > 12))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.year = *((short *)(hdr_4096 + STARTDATE_HDROFF + 4));
  if((start_dt.year < 1000) || (start_dt.year > 3000))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.hour = *((short *)(hdr_4096 + STARTTIME_HDROFF));
  if((start_dt.hour < 0) || (start_dt.hour > 23))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  start_dt.minute = *((short *)(hdr_4096 + STARTTIME_HDROFF + 2));
  if((start_dt.minute < 0) || (start_dt.minute > 59))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  start_dt.second = *((short *)(hdr_4096 + STARTTIME_HDROFF + 4));
  if((start_dt.second < 0) || (start_dt.second > 59))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  birthdate_valid = 1;

  birth_d.year = *((short *)(hdr_4096 + SUBJBIRTHDATE_HDROFF + 4));
  if((birth_d.year < 1000) || (birth_d.year > 3000))
  {
    birthdate_valid = 0;
  }

  birth_d.month = *((short *)(hdr_4096 + SUBJBIRTHDATE_HDROFF + 2));
  if((birth_d.month < 1) || (birth_d.month > 12))
  {
    birthdate_valid = 0;
  }

  birth_d.day = *((short *)(hdr_4096 + SUBJBIRTHDATE_HDROFF));
  if((birth_d.day < 1) || (birth_d.day > 31))
  {
    birthdate_valid = 0;
  }

//   if(!birthdate_valid)
//   {
//     snprintf(scratchpad_4096, 4096, "Skipping illegal date of birth: %04i-%02i-%02i", birth_d.year, birth_d.month, birth_d.day);
//     textEdit1->append(scratchpad_4096);
//   }

/////////////////////////////////////// STORE TO EDF ///////////////////////////////

  remove_extension_from_filename(path);
  strlcat(path, ".edf", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    goto OUT_EXIT;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, chns);
  if(edf_hdl < 0)
  {
    textEdit1->append("Error, cannot open EDF file for writing\n");
    goto OUT_EXIT;
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_samplefrequency(edf_hdl, i, sf))
    {
      textEdit1->append("Error, edf_set_samplefrequency()\n");
      goto OUT_EXIT;
    }

    if(edf_set_digital_maximum(edf_hdl, i, 32767))
    {
      textEdit1->append("Error, edf_set_digital_maximum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_digital_minimum(edf_hdl, i, -32768))
    {
      textEdit1->append("Error, edf_set_digital_minimum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_physical_dimension(edf_hdl, i, "uV"))
    {
      textEdit1->append("Error, edf_set_physical_dimension()\n");
      goto OUT_EXIT;
    }

    if(edf_set_physical_maximum(edf_hdl, i, 32.767 * amp_resolution[i]))
    {
      textEdit1->append("Error, edf_set_physical_maximum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_physical_minimum(edf_hdl, i, -32.768 * amp_resolution[i]))
    {
      textEdit1->append("Error, edf_set_physical_minimum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_label(edf_hdl, i, lead_label[lead_spec[i]]))
    {
      textEdit1->append("Error, edf_set_label()\n");
      goto OUT_EXIT;
    }
  }

  if((start_dt.year < 1985) || (start_dt.year > 2084))
  {
    start_dt.year = 1985;
    start_dt.month = 1;
    start_dt.day = 1;
    start_dt.hour = 0;
    start_dt.minute = 0;
    start_dt.second = 0;
  }

  if(edf_set_startdatetime(edf_hdl, start_dt.year, start_dt.month, start_dt.day, start_dt.hour, start_dt.minute, start_dt.second))
  {
    textEdit1->append("Error, edf_set_startdatetime()\n");
    goto OUT_EXIT;
  }

  strncpy(str1_128, hdr_4096 + 28, 40);
  str1_128[40] = 0;

  trim_spaces(str1_128);
  strlcat(str1_128, " ", 128);

  strncpy(scratchpad_4096, hdr_4096 + 68, 40);
  scratchpad_4096[40] = 0;

  trim_spaces(scratchpad_4096);

  strlcat(str1_128, scratchpad_4096, 128);
  remove_trailing_spaces(str1_128);

  if(strlen(str1_128))
  {
    if(edf_set_patientname(edf_hdl, str1_128))
    {
      textEdit1->append("Error, edf_set_patientname()\n");
      goto OUT_EXIT;
    }
  }

  strncpy(str1_128, hdr_4096 + 108, 20);
  str1_128[20] = 0;

  trim_spaces(str1_128);

  if(strlen(str1_128))
  {
    if(edf_set_patientcode(edf_hdl, str1_128))
    {
      textEdit1->append("Error, edf_set_patientcode()\n");
      goto OUT_EXIT;
    }
  }

  if(sex == 1)
  {
    if(edf_set_sex(edf_hdl, 1))
    {
      textEdit1->append("Error, edf_set_sex()\n");
      goto OUT_EXIT;
    }
  }
  else if(sex == 2)
    {
      if(edf_set_sex(edf_hdl, 0))
      {
        textEdit1->append("Error, edf_set_sex()\n");
        goto OUT_EXIT;
      }
    }

  if(birthdate_valid)
  {
    if(edf_set_birthdate(edf_hdl, birth_d.year, birth_d.month, birth_d.day))
    {
      textEdit1->append("Error, edf_set_birthdate()\n");
      goto OUT_EXIT;
    }
  }

  strncpy(str1_128, hdr_4096 + 232, 40);
  str1_128[40] = 0;

  trim_spaces(str1_128);

  if(strlen(str1_128))
  {
    if(edf_set_equipment(edf_hdl, str1_128))
    {
      textEdit1->append("Error, edf_set_equipment()\n");
      goto OUT_EXIT;
    }
  }

  if(edf_set_recording_additional(edf_hdl, pacemaker_list[pacemaker]))
  {
    fprintf(stderr, "Error, edf_set_recording_additional()\n");
    goto OUT_EXIT;
  }

  if(annotfile)
  {
    if(edf_set_number_of_annotation_signals(edf_hdl, 2))
    {
      textEdit1->append("Error, edf_set_number_of_annotation_signals()\n");
      goto OUT_EXIT;
    }
  }

  edf_set_annot_chan_idx_pos(edf_hdl, EDF_ANNOT_IDX_POS_START);

/////////////////// Start conversion //////////////////////////////////////////

//   printf("sf: %i   chns: %i   var_block_offset: %i   var_block_sz: %i   ecg_block_offset: %i   ecg_smpl_sz: %i   total_blocks: %i\n",
//          sf, chns, var_block_offset, var_block_sz, ecg_block_offset, ecg_smpl_sz, total_blocks);

  inbuf = (short *)malloc(sf * chns * sizeof(short));
  if(inbuf == NULL)
  {
    textEdit1->append("Malloc() error\n");
    goto OUT_EXIT;
  }

  outbuf = (short *)malloc(sf * chns * sizeof(short));
  if(outbuf == NULL)
  {
    textEdit1->append("Malloc() error\n");
    goto OUT_EXIT;
  }

  fseek(inputfile, ecg_block_offset, SEEK_SET);

  progress.setMaximum(total_blocks);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  for(blocks_written=0; blocks_written<total_blocks; blocks_written++)
  {
    if(!(blocks_written % 10))
    {
      progress.setValue(blocks_written);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        textEdit1->append("Conversion aborted by user.\n");
        break;
      }
    }

    if(fread(inbuf, sf * chns * 2, 1, inputfile) != 1)
    {
      textEdit1->append("Error, could not read from input file.");
      goto OUT_EXIT;
    }

    for(i=0; i<chns; i++)
    {
      for(j=0; j<sf; j++)
      {
        *(outbuf + (i * sf) + j) = *(inbuf + (j * chns) + i);
      }
    }

    if(edf_blockwrite_digital_short_samples(edf_hdl, outbuf))
    {
      textEdit1->append("Error, edf_blockwrite_digital_short_samples()\n");
      goto OUT_EXIT;
    }
  }

/////// CHECK ANNOTATIONS FILE ///////////////////////////

  if(annotfile == NULL)
  {
    textEdit1->append("Done\n");
    goto OUT_EXIT;
  }

  snprintf(scratchpad_4096, 4096, "Processing annotations file:\n%s", path);
  textEdit1->append(QString::fromLocal8Bit(scratchpad_4096));

  fseek(annotfile, 0, SEEK_END);

  file_sz = ftell(annotfile);

  if(file_sz < 522)
  {
    textEdit1->append("Error, annotations file is too small.");
    goto OUT_EXIT;
  }

  rewind(annotfile);

  if(fread(hdr_4096, 522, 1, annotfile) != 1)
  {
    textEdit1->append("A read error occurred when trying to read the header.");
    goto OUT_EXIT;
  }

  if(strncmp(hdr_4096, "ANN  1.0", 8))
  {
    textEdit1->append("Error, wrong magic string in header.");
    goto OUT_EXIT;
  }

////////////////////////////////// GET THE HEADER DATA ////////////////////////

  var_block_sz = *((int *)(hdr_4096 + 10));
  if(var_block_sz < 0)
  {
    textEdit1->append("Error, parameter size of variable length block in header is lower than zero.");
    goto OUT_EXIT;
  }

  ecg_smpl_sz = *((int *)(hdr_4096 + 14));
  if(ecg_smpl_sz < 0)
  {
    textEdit1->append("Error, parameter ECG sample size in header is lower than zero.");
    goto OUT_EXIT;
  }

  if(var_block_sz == 0)
  {
    var_block_offset = 522;
  }
  else
  {
    var_block_offset = *((int *)(hdr_4096 + 18));
  }

  if(var_block_offset != 522)
  {
    textEdit1->append("Error, parameter offset of variable length block in header differs from 522");
    goto OUT_EXIT;
  }

  ecg_block_offset = *((int *)(hdr_4096 + 22));
  if(ecg_block_offset != (var_block_sz + 522))
  {
    textEdit1->append("Error, parameter offset of ECG block in header differs from 522 + variable length block");
    goto OUT_EXIT;
  }

  if((unsigned long)ecg_block_offset > file_sz)
  {
    textEdit1->append("Error, start of ECG block is after end of file.");
    goto OUT_EXIT;
  }

  chns = *((short *)(hdr_4096 + 156));
  if(chns < 1)
  {
    textEdit1->append("Error, number of leads is less than 1.");
    goto OUT_EXIT;
  }

  if(chns > ISHNE_MAX_CHNS)
  {
    textEdit1->append("Error, number of leads is more than 12.");
    goto OUT_EXIT;
  }

  sf = *((short *)(hdr_4096 + 272));
  if(sf < 1)
  {
    textEdit1->append("Error, sampling rate is less than 1Hz.");
    goto OUT_EXIT;
  }

  sex = *((short *)(hdr_4096 + 128));
  if((sex < 0) || (sex > 2))
  {
    textEdit1->append("Error, subject sex is out of range.");
    goto OUT_EXIT;
  }

  if(check_crc(annotfile, var_block_offset + var_block_sz))
  {
    textEdit1->append("CRC error, file header is corrupt.");
    goto OUT_EXIT;
  }

////////////////////////////// GET THE LEAD DATA ////////////////////////////////////

  for(i=0; i<chns; i++)
  {
    lead_spec[i] = *((short *)(hdr_4096 + 158 + (i * 2)));
    if((lead_spec[i] < 0) || (lead_spec[i] > 19))
    {
      snprintf(scratchpad_4096, 4096, "Error, lead specification of lead %i is out of range.", i + 1);
      textEdit1->append(scratchpad_4096);
      goto OUT_EXIT;
    }

    amp_resolution[i] = *((short *)(hdr_4096 + 206 + (i * 2)));
    if(amp_resolution[i] < 1)
    {
      snprintf(scratchpad_4096, 4096, "Error, amplitude resolution of lead %i is less than 1nV.", i + 1);
      textEdit1->append(scratchpad_4096);
      goto OUT_EXIT;
    }
  }

/////////////////////////////////////// GET THE START DATE AND TIME /////////////////////

  start_dt.day = *((short *)(hdr_4096 + 138));
  if((start_dt.day < 1) || (start_dt.day > 31))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.month = *((short *)(hdr_4096 + 140));
  if((start_dt.month < 1) || (start_dt.month > 12))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.year = *((short *)(hdr_4096 + 142));
  if((start_dt.year < 1000) || (start_dt.year > 3000))
  {
    textEdit1->append("Error, illegal start date.");
    goto OUT_EXIT;
  }

  start_dt.hour = *((short *)(hdr_4096 + 150));
  if((start_dt.hour < 0) || (start_dt.hour > 23))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  start_dt.minute = *((short *)(hdr_4096 + 152));
  if((start_dt.minute < 0) || (start_dt.minute > 59))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  start_dt.second = *((short *)(hdr_4096 + 154));
  if((start_dt.second < 0) || (start_dt.second > 59))
  {
    textEdit1->append("Error, illegal start time.");
    goto OUT_EXIT;
  }

  birthdate_valid = 1;

  birth_d.year = *((short *)(hdr_4096 + 136));
  if((birth_d.year < 1000) || (birth_d.year > 3000))
  {
    birthdate_valid = 0;
  }

  birth_d.month = *((short *)(hdr_4096 + 134));
  if((birth_d.month < 1) || (birth_d.month > 12))
  {
    birthdate_valid = 0;
  }

  birth_d.day = *((short *)(hdr_4096 + 132));
  if((birth_d.day < 1) || (birth_d.day > 31))
  {
    birthdate_valid = 0;
  }

/////////////////// Start conversion //////////////////////////////////////////

  fseek(annotfile, ecg_block_offset, SEEK_SET);

  while(1)
  {
    if(fread(rbuf, 4, 1, annotfile) != 1)
    {
      break;
    }

    if(rbuf[0] == 'N')
    {
      strlcpy(annot_descr_128, "Normal beat", 128);
    }
    else if(rbuf[0] == 'V')
      {
        strlcpy(annot_descr_128, "Premature ventricular contraction", 128);
      }
      else if(rbuf[0] == 'S')
        {
          strlcpy(annot_descr_128, "Supraventricular premature or ectopic beat", 128);
        }
        else if(rbuf[0] == 'C')
          {
            strlcpy(annot_descr_128, "Calibration Pulse", 128);
          }
          else if(rbuf[0] == 'B')
            {
              strlcpy(annot_descr_128, "Bundle branch block beat", 128);
            }
            else if(rbuf[0] == 'P')
              {
                strlcpy(annot_descr_128, "Pace", 128);
              }
              else if(rbuf[0] == 'X')
                {
                  strlcpy(annot_descr_128, "Artefact", 128);
                }
                else if(rbuf[0] == '!')
                  {
                    strlcpy(annot_descr_128, "Timeout", 128);
                  }
                  else if(rbuf[0] == 'U')
                    {
                      strlcpy(annot_descr_128, "Unknown", 128);
                    }
                    else
                    {
                      strlcpy(annot_descr_128, "Unspecified", 128);
                    }

    onset += *((unsigned short *)(rbuf + 2));

    edfwrite_annotation_latin1_hr(edf_hdl, (onset * 1000000LL) / sf, -1, annot_descr_128);
  }

  textEdit1->append("Done\n");

OUT_EXIT:

  progress.reset();

  if(edf_hdl >= 0)  edfclose_file(edf_hdl);

  if(inputfile != NULL)  fclose(inputfile);

  if(annotfile != NULL)  fclose(annotfile);

  free(inbuf);
  free(outbuf);
}


int UI_IshneEDFwindow::check_crc(FILE *inputfile, int len)
{
  unsigned short crc=0xFFFF, crc2=0x0000;

  unsigned char *buf=NULL;

  buf = (unsigned char *)malloc(len);
  if(buf == NULL)
  {
    return -99;
  }

  rewind(inputfile);

  if(fread(buf, len, 1, inputfile) != 1)
  {
    free(buf);
    return -88;
  }

  crc2 = *((unsigned short *)(buf + 8));

  crc = crc_ccitt(buf + 10, len - 10, crc);

//  printf("crc: %04X   crc2: %04X\n", crc, crc2);

  if(crc != crc2)
  {
    free(buf);
    return -1;
  }

  free(buf);

  return 0;
}


unsigned short UI_IshneEDFwindow::crc_ccitt(const unsigned char *message, int nbytes, unsigned short remainder)
{
  int byte_;

  unsigned char data;

  for(byte_=0; byte_<nbytes; byte_++)  /* Divide the message by the polynomial, a byte at a time. */
  {
    data = message[byte_] ^ (remainder >> 8);

    remainder = crc_ccitt_table[data] ^ (remainder << 8);
  }

  return remainder;  /* The final remainder is the CRC. */
}


void UI_IshneEDFwindow::crc_ccitt_init(void)
{
  int dividend;

  unsigned short remainder;

  unsigned char bit;

  for(dividend=0; dividend<256; dividend++)  /* Compute the remainder of each possible dividend. */
  {
    remainder = dividend << 8;  /* Start with the dividend followed by zeros. */

    for(bit=8; bit>0; bit--)  /* Perform modulo-2 division, a bit at a time. */
    {
      if(remainder & 32768)  /* Try to divide the current data bit. */
      {
        remainder = (remainder << 1) ^ 0x1021;  /* polynomial */
      }
      else
      {
        remainder = (remainder << 1);
      }
    }

    crc_ccitt_table[dividend] = remainder;  /* Store the result into the table. */
  }
}























