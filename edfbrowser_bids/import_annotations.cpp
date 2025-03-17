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



#include "import_annotations.h"


#define XML_FORMAT       (0)
#define ASCIICSV_FORMAT  (1)
#define DCEVENT_FORMAT   (2)
#define EDFPLUS_FORMAT   (3)
#define MITWFDB_FORMAT   (4)

#define TAB_CNT          (5)

#define CVS_ONSET_TIME_FMT_RELATIVE             (0)
#define CVS_ONSET_TIME_FMT_ABS_TIME             (1)
#define CVS_ONSET_TIME_FMT_ABS_TIME_SUBSEC      (2)
#define CVS_ONSET_TIME_FMT_ABS_DATETIME         (3)
#define CVS_ONSET_TIME_FMT_ABS_DATETIME_SUBSEC  (4)


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



//#define IMPORT_ANNOTS_DEBUG



UI_ImportAnnotationswindow::UI_ImportAnnotationswindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  if(mainwindow->files_open < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot import annotations without opening an EDF/BDF-file first.");
    messagewindow.exec();
    return;
  }

  if(mainwindow->files_open > 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot import annotations when multiple files are opened.\n"
                                                              "Make sure only one EDF/BDF is opened.");
    messagewindow.exec();
    return;
  }

  ImportAnnotsDialog = new QDialog;
  ImportAnnotsDialog->setMinimumSize(550 * mainwindow->w_scaling, 470 * mainwindow->h_scaling);
  ImportAnnotsDialog->setWindowTitle("Import annotations/events");
  ImportAnnotsDialog->setModal(true);
  ImportAnnotsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  ImportAnnotsDialog->setSizeGripEnabled(true);

  tabholder = new QTabWidget;

  tab_index_array[ASCIICSV_FORMAT] = 0;
  tab_index_array[DCEVENT_FORMAT] = 1;
  tab_index_array[XML_FORMAT] = 2;
  tab_index_array[EDFPLUS_FORMAT] = 3;
  tab_index_array[MITWFDB_FORMAT] = 4;

  for(i=0; i<TAB_CNT; i++)
  {
    tab[i] = new QWidget;
  }

///////////////////////////////////////////////// ASCII/CSV //////////////////////////////////////////////////

  SeparatorLineEdit = new QLineEdit;
  SeparatorLineEdit->setMaxLength(3);
  SeparatorLineEdit->setText("tab");

  DescriptionLineEdit = new QLineEdit;
  DescriptionLineEdit->setMaxLength(20);
  DescriptionLineEdit->setEnabled(false);
  DescriptionLineEdit->setToolTip("Use this description for all events");

  OnsetColumnSpinBox = new QSpinBox;
  OnsetColumnSpinBox->setRange(1,256);
  OnsetColumnSpinBox->setValue(1);
  OnsetColumnSpinBox->setToolTip("Column number for the start time of the event");

  DurationColumnSpinBox = new QSpinBox;
  DurationColumnSpinBox->setRange(1,256);
  DurationColumnSpinBox->setValue(3);
  DurationColumnSpinBox->setToolTip("Column number for the duration of the event");

  StopColumnSpinBox = new QSpinBox;
  StopColumnSpinBox->setRange(1,256);
  StopColumnSpinBox->setValue(3);
  StopColumnSpinBox->setToolTip("Column number for the stop time of the event");

  DescriptionColumnSpinBox = new QSpinBox;
  DescriptionColumnSpinBox->setRange(1,256);
  DescriptionColumnSpinBox->setValue(2);
  DescriptionColumnSpinBox->setToolTip("Column number for the description of the event");

  DatastartSpinbox = new QSpinBox;
  DatastartSpinbox->setRange(1,100);
  DatastartSpinbox->setValue(1);

  RelativeTimeComboBox = new QComboBox;
  RelativeTimeComboBox->addItem("in seconds, relative to start of file");
  RelativeTimeComboBox->addItem("hh:mm:ss");
  RelativeTimeComboBox->addItem("hh:mm:ss.xxx");
  RelativeTimeComboBox->addItem("yyyy-mm-ddThh:mm:ss");
  RelativeTimeComboBox->addItem("yyyy-mm-ddThh:mm:ss.xxx");

  text_encoding_combobox = new QComboBox;
  text_encoding_combobox->addItem("UTF-8");
  text_encoding_combobox->addItem("ISO-8859-1 (Latin-1)");

  DescriptionColumnRadioButton = new QRadioButton();
  DescriptionColumnRadioButton->setChecked(true);
  DescriptionColumnRadioButton->setToolTip("Use the column for the description of the event");
  UseManualDescriptionRadioButton = new QRadioButton();
  UseManualDescriptionRadioButton->setToolTip("Use a custom description for all events");

  DurationCheckBox = new QCheckBox;
  DurationCheckBox->setTristate(false);
  DurationCheckBox->setCheckState(Qt::Unchecked);
  DurationCheckBox->setToolTip("Use the column for the duration of the event");

  StopTimeCheckBox = new QCheckBox;
  StopTimeCheckBox->setTristate(false);
  StopTimeCheckBox->setCheckState(Qt::Unchecked);
  StopTimeCheckBox->setToolTip("Use the column for the stop time of the event");

  equalFilenameCheckBox = new QCheckBox;
  equalFilenameCheckBox->setTristate(false);
  equalFilenameCheckBox->setCheckState(Qt::Unchecked);
  equalFilenameCheckBox->setToolTip("Only accept ASCII files with the same name as the EDF/BDF file and extension .csv, .tsv or .txt");

  QHBoxLayout *asciiSettingsHBoxLayout1 = new QHBoxLayout;
  asciiSettingsHBoxLayout1->addWidget(DescriptionColumnRadioButton);
  asciiSettingsHBoxLayout1->addWidget(DescriptionColumnSpinBox, 10);

  QHBoxLayout *asciiSettingsHBoxLayout2 = new QHBoxLayout;
  asciiSettingsHBoxLayout2->addWidget(UseManualDescriptionRadioButton);
  asciiSettingsHBoxLayout2->addWidget(DescriptionLineEdit, 10);

  QHBoxLayout *asciiSettingsHBoxLayout3 = new QHBoxLayout;
  asciiSettingsHBoxLayout3->addWidget(DurationCheckBox);
  asciiSettingsHBoxLayout3->addWidget(DurationColumnSpinBox, 10);

  QHBoxLayout *asciiSettingsHBoxLayout4 = new QHBoxLayout;
  asciiSettingsHBoxLayout4->addWidget(StopTimeCheckBox);
  asciiSettingsHBoxLayout4->addWidget(StopColumnSpinBox, 10);

  QFormLayout *asciiSettingsflayout = new QFormLayout;
  asciiSettingsflayout->addRow(" ", (QWidget *)NULL);
  asciiSettingsflayout->addRow("Column separator", SeparatorLineEdit);
  asciiSettingsflayout->addRow("Onset column", OnsetColumnSpinBox);
  asciiSettingsflayout->labelForField(OnsetColumnSpinBox)->setToolTip("Start time of the event");
  asciiSettingsflayout->addRow("Duration column", asciiSettingsHBoxLayout3);
  asciiSettingsflayout->labelForField(asciiSettingsHBoxLayout3)->setToolTip("Duration of the event");
  asciiSettingsflayout->addRow("End column", asciiSettingsHBoxLayout4);
  asciiSettingsflayout->labelForField(asciiSettingsHBoxLayout4)->setToolTip("Stop time of the event");
  asciiSettingsflayout->addRow("Description column", asciiSettingsHBoxLayout1);
  asciiSettingsflayout->labelForField(asciiSettingsHBoxLayout1)->setToolTip("Description of the event");
  asciiSettingsflayout->addRow("Manual description", asciiSettingsHBoxLayout2);
  asciiSettingsflayout->labelForField(asciiSettingsHBoxLayout2)->setToolTip("Custom description for all events");
  asciiSettingsflayout->addRow("Data starts at line", DatastartSpinbox);
  asciiSettingsflayout->addRow("Onset time coding is", RelativeTimeComboBox);
  asciiSettingsflayout->addRow("Text encoding", text_encoding_combobox);
  asciiSettingsflayout->addRow("Must have equal filename", equalFilenameCheckBox);
  asciiSettingsflayout->labelForField(equalFilenameCheckBox)->setToolTip("Only accept ASCII files with the same name as the EDF/BDF file and extension .csv, .tsv or .txt");

  QHBoxLayout *asciiSettingsHBoxLayout20 = new QHBoxLayout;
  asciiSettingsHBoxLayout20->addLayout(asciiSettingsflayout);
  asciiSettingsHBoxLayout20->addStretch(1000);

  QVBoxLayout *asciiSettingsVBoxLayout = new QVBoxLayout;
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout20);
  asciiSettingsVBoxLayout->addStretch(1000);

  tab[tab_index_array[ASCIICSV_FORMAT]]->setLayout(asciiSettingsVBoxLayout);

///////////////////////////////////////////////////////////////////////////////////////////////////

  DCEventSignalLabel = new QLabel;
  DCEventSignalLabel->setText("Signal");

  DCEventBitTimeLabel = new QLabel;
  DCEventBitTimeLabel->setText("Bit Time");

  DCEventTriggerLevelLabel = new QLabel;
  DCEventTriggerLevelLabel->setText("Trigger Level");

  DCEventSignalComboBox = new QComboBox;
  for(i=0; i<mainwindow->signalcomps; i++)
  {
    DCEventSignalComboBox->addItem(mainwindow->signalcomp[i]->signallabel);
  }

  BitTimeSpinbox = new QSpinBox;
  BitTimeSpinbox->setRange(1,1000);
  BitTimeSpinbox->setSuffix(" mS");
  BitTimeSpinbox->setValue(10);

  DCEventTriggerLevelSpinBox = new QDoubleSpinBox;
  DCEventTriggerLevelSpinBox->setDecimals(3);
  DCEventTriggerLevelSpinBox->setRange(-10000.0, 10000.0);
  DCEventTriggerLevelSpinBox->setValue(500.0);

  if(mainwindow->signalcomps)
  {
    DCEventSignalChanged(0);
  }

  QFormLayout *DCEventflayout = new QFormLayout;
  DCEventflayout->addRow(DCEventSignalLabel, DCEventSignalComboBox);
  DCEventflayout->addRow(DCEventBitTimeLabel, BitTimeSpinbox);
  DCEventflayout->addRow(DCEventTriggerLevelLabel, DCEventTriggerLevelSpinBox);

  QHBoxLayout *DCEventHBoxLayout20 = new QHBoxLayout;
  DCEventHBoxLayout20->addLayout(DCEventflayout);
  DCEventHBoxLayout20->addStretch(1000);

  QVBoxLayout *DCEventVBoxLayout = new QVBoxLayout;
  DCEventVBoxLayout->addLayout(DCEventHBoxLayout20);
  DCEventVBoxLayout->addStretch(1000);

  tab[tab_index_array[DCEVENT_FORMAT]]->setLayout(DCEventVBoxLayout);

///////////////////////////////////////////////////////////////////////////////////////////////////

  SampleTimeLabel = new QLabel;
  SampleTimeLabel->setText("Samplefrequency:");

  SampleTimeSpinbox = new QSpinBox;
  SampleTimeSpinbox->setRange(0,100000);
  SampleTimeSpinbox->setSuffix(" Hz");
  SampleTimeSpinbox->setValue(get_samplefreq_inf());
  if(SampleTimeSpinbox->value() > 0)
  {
    SampleTimeSpinbox->setEnabled(false);
  }

  importStandardLabel  = new QLabel;
  importStandardLabel->setText("Import Standard Annotations:");

  importStandardCheckBox = new QCheckBox;
  importStandardCheckBox->setTristate(false);
  importStandardCheckBox->setCheckState(Qt::Checked);

  importAuxLabel  = new QLabel;
  importAuxLabel->setText("Import Auxiliary Info:");

  importAuxCheckBox = new QCheckBox;
  importAuxCheckBox->setTristate(false);
  importAuxCheckBox->setCheckState(Qt::Checked);

  QFormLayout *mitwfdbflayout = new QFormLayout;
  mitwfdbflayout->addRow(SampleTimeLabel, SampleTimeSpinbox);
  mitwfdbflayout->addRow(importStandardLabel, importStandardCheckBox);
  mitwfdbflayout->addRow(importAuxLabel, importAuxCheckBox);

  QHBoxLayout *mitwfdbHBoxLayout20 = new QHBoxLayout;
  mitwfdbHBoxLayout20->addLayout(mitwfdbflayout);
  mitwfdbHBoxLayout20->addStretch(1000);

  QVBoxLayout *mitwfdbVBoxLayout1 = new QVBoxLayout;
  mitwfdbVBoxLayout1->addLayout(mitwfdbHBoxLayout20);
  mitwfdbVBoxLayout1->addStretch(1000);

  tab[tab_index_array[MITWFDB_FORMAT]]->setLayout(mitwfdbVBoxLayout1);

///////////////////////////////////////////////////////////////////////////////////////////////////

  tabholder->addTab(tab[tab_index_array[ASCIICSV_FORMAT]], "ASCII / CSV");
  tabholder->addTab(tab[tab_index_array[DCEVENT_FORMAT]],  "DC-event (8-bit serial code)");
  tabholder->addTab(tab[tab_index_array[XML_FORMAT]],      "XML");
  tabholder->addTab(tab[tab_index_array[EDFPLUS_FORMAT]],  "EDF+ / BDF+");
  tabholder->addTab(tab[tab_index_array[MITWFDB_FORMAT]],  "MIT / WFDB");

  IgnoreConsecutiveCheckBox = new QCheckBox(" Ignore consecutive events with the\n same description");
  IgnoreConsecutiveCheckBox->setTristate(false);
  IgnoreConsecutiveCheckBox->setCheckState(Qt::Unchecked);

  ImportButton = new QPushButton;
  ImportButton->setText("Import");

  CloseButton = new QPushButton;
  CloseButton->setText("Cancel");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  QHBoxLayout *horLayout = new QHBoxLayout;
  horLayout->addWidget(ImportButton);
  horLayout->addStretch(1000);
  horLayout->addWidget(helpButton);
  horLayout->addStretch(1000);
  horLayout->addWidget(CloseButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabholder, 1000);
  mainLayout->addWidget(IgnoreConsecutiveCheckBox);
  mainLayout->addSpacing(20);
  mainLayout->addLayout(horLayout);

  ImportAnnotsDialog->setLayout(mainLayout);

  SeparatorLineEdit->setText(mainwindow->import_annotations_var->separator);
  OnsetColumnSpinBox->setValue(mainwindow->import_annotations_var->onsetcolumn);
  DescriptionColumnSpinBox->setValue(mainwindow->import_annotations_var->descriptioncolumn);
  DescriptionLineEdit->setText(mainwindow->import_annotations_var->description);
  DurationColumnSpinBox->setValue(mainwindow->import_annotations_var->durationcolumn);
  StopColumnSpinBox->setValue(mainwindow->import_annotations_var->stopcolumn);
  DatastartSpinbox->setValue(mainwindow->import_annotations_var->datastartline);
  RelativeTimeComboBox->setCurrentIndex(mainwindow->import_annotations_var->onsettimeformat);
  BitTimeSpinbox->setValue(mainwindow->import_annotations_var->dceventbittime);
  DCEventTriggerLevelSpinBox->setValue(mainwindow->import_annotations_var->triggerlevel);
  text_encoding_combobox->setCurrentIndex(mainwindow->import_annotations_var->ascii_txt_encoding);

  if(mainwindow->import_annotations_var->manualdescription == 0)
  {
    DescriptionColumnRadioButton->setChecked(true);
    DescriptionColumnSpinBox->setEnabled(true);
    DescriptionLineEdit->setEnabled(false);
  }
  else
  {
    UseManualDescriptionRadioButton->setChecked(true);
    DescriptionColumnSpinBox->setEnabled(false);
    DescriptionLineEdit->setEnabled(true);
    if(mainwindow->import_annotations_var->format == ASCIICSV_FORMAT)
    {
      IgnoreConsecutiveCheckBox->setEnabled(false);
    }
  }

  if(mainwindow->import_annotations_var->useduration == 1)
  {
    DurationCheckBox->setCheckState(Qt::Checked);
    DurationColumnSpinBox->setEnabled(true);
  }
  else
  {
    DurationCheckBox->setCheckState(Qt::Unchecked);
    DurationColumnSpinBox->setEnabled(false);
  }

  if((mainwindow->import_annotations_var->usestoptime == 1) && (mainwindow->import_annotations_var->useduration != 1))
  {
    StopTimeCheckBox->setCheckState(Qt::Checked);
    StopColumnSpinBox->setEnabled(true);
  }
  else
  {
    StopTimeCheckBox->setCheckState(Qt::Unchecked);
    StopColumnSpinBox->setEnabled(false);
  }

  if(mainwindow->import_annotations_var->ignoreconsecutive == 1)
  {
    IgnoreConsecutiveCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    IgnoreConsecutiveCheckBox->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->import_annotations_var->csv_equal_filename == 1)
  {
    equalFilenameCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    equalFilenameCheckBox->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->import_annotations_var->format == EDFPLUS_FORMAT)
  {
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }

  tabholder->setCurrentIndex(tab_index_array[mainwindow->import_annotations_var->format]);

  QObject::connect(CloseButton,                     SIGNAL(clicked()),                ImportAnnotsDialog, SLOT(close()));
  QObject::connect(ImportButton,                    SIGNAL(clicked()),                this,               SLOT(ImportButtonClicked()));
  QObject::connect(DCEventSignalComboBox,           SIGNAL(currentIndexChanged(int)), this,               SLOT(DCEventSignalChanged(int)));
  QObject::connect(DescriptionColumnRadioButton,    SIGNAL(toggled(bool)),            this,               SLOT(descriptionRadioButtonClicked(bool)));
  QObject::connect(UseManualDescriptionRadioButton, SIGNAL(toggled(bool)),            this,               SLOT(descriptionRadioButtonClicked(bool)));
  QObject::connect(DurationCheckBox,                SIGNAL(stateChanged(int)),        this,               SLOT(DurationCheckBoxChanged(int)));
  QObject::connect(StopTimeCheckBox,                SIGNAL(stateChanged(int)),        this,               SLOT(StopTimeCheckBoxChanged(int)));
  QObject::connect(equalFilenameCheckBox,           SIGNAL(stateChanged(int)),        this,               SLOT(equalFilenameCheckBoxChanged(int)));
  QObject::connect(tabholder,                       SIGNAL(currentChanged(int)),      this,               SLOT(TabChanged(int)));
  QObject::connect(helpButton,                      SIGNAL(clicked()),                this,               SLOT(helpbuttonpressed()));

  ImportAnnotsDialog->exec();
}


void UI_ImportAnnotationswindow::DurationCheckBoxChanged(int state)
{
  if(state == Qt::Unchecked)
  {
    DurationColumnSpinBox->setEnabled(false);
  }
  else
  {
    DurationColumnSpinBox->setEnabled(true);

    StopColumnSpinBox->setEnabled(false);

    StopTimeCheckBox->setCheckState(Qt::Unchecked);
  }
}


void UI_ImportAnnotationswindow::StopTimeCheckBoxChanged(int state)
{
  if(state == Qt::Unchecked)
  {
    StopColumnSpinBox->setEnabled(false);
  }
  else
  {
    StopColumnSpinBox->setEnabled(true);

    DurationColumnSpinBox->setEnabled(false);

    DurationCheckBox->setCheckState(Qt::Unchecked);
  }
}


void UI_ImportAnnotationswindow::equalFilenameCheckBoxChanged(int state)
{
  if(state == Qt::Unchecked)
  {
    mainwindow->import_annotations_var->csv_equal_filename = 0;
  }
  else
  {
    mainwindow->import_annotations_var->csv_equal_filename = 1;
  }
}


void UI_ImportAnnotationswindow::descriptionRadioButtonClicked(bool)
{
  if(DescriptionColumnRadioButton->isChecked() == true)
  {
    DescriptionColumnSpinBox->setEnabled(true);
    DescriptionLineEdit->setEnabled(false);
    IgnoreConsecutiveCheckBox->setEnabled(true);
  }

  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    DescriptionColumnSpinBox->setEnabled(false);
    DescriptionLineEdit->setEnabled(true);
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }
}


void UI_ImportAnnotationswindow::TabChanged(int index)
{
  if((index == tab_index_array[XML_FORMAT]) ||
     (index == tab_index_array[DCEVENT_FORMAT]) ||
     (index == tab_index_array[MITWFDB_FORMAT]))
  {
    IgnoreConsecutiveCheckBox->setEnabled(true);
  }

  if(index == tab_index_array[EDFPLUS_FORMAT])
  {
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }

  if(index == tab_index_array[ASCIICSV_FORMAT])
  {
    if(UseManualDescriptionRadioButton->isChecked() == true)
    {
      IgnoreConsecutiveCheckBox->setEnabled(false);
    }
    else
    {
      IgnoreConsecutiveCheckBox->setEnabled(true);
    }
  }
}


void UI_ImportAnnotationswindow::DCEventSignalChanged(int index)
{
  char scratchpad_64[64];

  if((index < 0) || (!mainwindow->signalcomps))
  {
    DCEventTriggerLevelSpinBox->setSuffix("");

    return;
  }

  strlcpy(scratchpad_64, " ", 64);
  strlcat(scratchpad_64, mainwindow->signalcomp[index]->physdimension, 64);

  DCEventTriggerLevelSpinBox->setSuffix(scratchpad_64);
}


void UI_ImportAnnotationswindow::ImportButtonClicked()
{
  int i,
      input_format,
      error=0;

  char str1_4096[4096]={""};

  mal_formatted_lines = 0;

  ImportAnnotsDialog->setEnabled(false);

  i = tabholder->currentIndex();

  for(input_format = 0; input_format < TAB_CNT; input_format++)
  {
    if(tab_index_array[input_format] == i)
    {
      break;
    }
  }

  if(input_format >= TAB_CNT)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (input_format >= TAB_CNT).");
    messagewindow.exec();
    ImportAnnotsDialog->setEnabled(true);
    return;
  }

  mainwindow->import_annotations_var->format = input_format;

  if(input_format == MITWFDB_FORMAT)
  {
    error = import_from_mitwfdb();
  }

  if(input_format == DCEVENT_FORMAT)
  {
    error = import_from_dcevent();
  }

  if(input_format == EDFPLUS_FORMAT)
  {
    error = import_from_edfplus();
  }

  if(input_format == XML_FORMAT)
  {
    error = import_from_xml();
  }

  if(input_format == ASCIICSV_FORMAT)
  {
    error = import_from_ascii();
  }

  if(mainwindow->annotations_dock[0] == NULL)
  {
    mainwindow->annotations_dock[0] = new UI_Annotationswindow(mainwindow->edfheaderlist[0], mainwindow);

    mainwindow->addDockWidget(Qt::RightDockWidgetArea, mainwindow->annotations_dock[0]->docklist, Qt::Vertical);

    if(edfplus_annotation_size(&mainwindow->edfheaderlist[0]->annot_list) < 1)
    {
      mainwindow->annotations_dock[0]->docklist->hide();
    }
  }

  if(edfplus_annotation_size(&mainwindow->edfheaderlist[0]->annot_list) > 0)
  {
    mainwindow->annotations_dock[0]->docklist->show();

    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[0]->updateList(0);

    mainwindow->save_act->setEnabled(true);
  }

  mainwindow->maincurve->update();

  if(!error)
  {
    if((input_format == ASCIICSV_FORMAT) && (mal_formatted_lines > 0))
    {
      snprintf(str1_4096, 4096, "One or more lines were skipped because they were malformatted:\n"
                   "line(s):");
      for(i=0; i<mal_formatted_lines; i++)
      {
        snprintf(str1_4096 + strlen(str1_4096), 4096 - strlen(str1_4096)," %i,", mal_formatted_line_nrs[i]);
      }
      QMessageBox messagewindow(QMessageBox::Information, "Ready", str1_4096);
      messagewindow.exec();
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
  }

  ImportAnnotsDialog->setEnabled(true);

  if(!error)
  {
    ImportAnnotsDialog->close();
  }
}


int UI_ImportAnnotationswindow::import_from_mitwfdb(void)
{
  int len,
      annot_code,
      tc=0,
      skip,
      total_annots=0,
      ignore_consecutive=0,
      import_std_annots=1,
      import_aux_info=1,
      last_std_code=-99;

  long long bytes_read,
            filesize,
            sampletime;

  char path[MAX_PATH_LENGTH]={""},
       last_description_256_aux_256[256]={""},
       aux_str_256[256]={""};

  unsigned char a_buf_128[128]={""};

  annotblck_t annotation;

  FILE *inputfile=NULL;

  if(SampleTimeSpinbox->value() < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Please set the samplefrequency.\n"
      "The onset time of the annotations in MIT/WFDB format are expressed in samples offset from the start of the recording.\n"
      "Because your file contains different samplerates, you need to specify which samplerate should be used to\n"
      "calculate the onset time of the annotations."
    );
    messagewindow.exec();
    return 1;
  }

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  if(importStandardCheckBox->checkState() == Qt::Checked)
  {
    import_std_annots = 1;
  }
  else
  {
    import_std_annots = 0;
  }

  if(importAuxCheckBox->checkState() == Qt::Checked)
  {
    import_aux_info = 1;
  }
  else
  {
    import_aux_info = 0;
  }

  sampletime = TIME_FIXP_SCALING / SampleTimeSpinbox->value();

  strlcpy(path, QFileDialog::getOpenFileName(0, "Open MIT WFDB annotation file", QString::fromLocal8Bit(mainwindow->recent_opendir),
                                             "MIT annotation files (*.ari *.ecg *.trigger *.qrs *.atr *.apn *.st *.pwave *.marker *.seizures);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return 1;
  }

  fseeko(inputfile, 0LL, SEEK_END);
  filesize = ftello(inputfile);

  QProgressDialog progress("Converting annotations ...", "Abort", 0, filesize);

  fseeko(inputfile, 0LL, SEEK_SET);

  for(bytes_read=0LL; bytes_read < filesize; bytes_read += 2LL)
  {
    if(!(bytes_read % 100))
    {
      progress.setValue(bytes_read);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    skip = 0;

    if(fread(a_buf_128, 2, 1, inputfile) != 1)
    {
      break;
    }

    if(*((unsigned short *)a_buf_128) == 0)  // end of file
    {
      break;
    }

    annot_code = a_buf_128[1] >> 2;

    if(annot_code == 59)  /* SKIP */
    {
      if(fread(a_buf_128, 4, 1, inputfile) != 1)
      {
        break;
      }

      tc += (*((unsigned short *)a_buf_128) << 16);

      tc += *((unsigned short *)(a_buf_128 + 2));
    }
    else if(annot_code == 63)  /* AUX */
      {
        skip = *((unsigned short *)a_buf_128) & 0x3ff;

        len = skip;
        if(len > 255)  len = 255;

        if(skip % 2) skip++;

        skip -= len;

        if(fread(aux_str_256, len, 1, inputfile) != 1)
        {
          break;
        }

        aux_str_256[len] = 0;

        if(len && import_aux_info)
        {
          if((!ignore_consecutive) || (strcmp(aux_str_256, last_description_256_aux_256)))
          {
            memset(&annotation, 0, sizeof(annotblck_t));
            annotation.onset = (long long)tc * sampletime;
            strncpy(annotation.description, aux_str_256, MAX_ANNOTATION_LEN);

            annotation.description[MAX_ANNOTATION_LEN] = 0;

            annotation.edfhdr = mainwindow->edfheaderlist[0];

            if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
            {
              progress.reset();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
              messagewindow.exec();
              fclose(inputfile);
              return 1;
            }

            total_annots++;

            strlcpy(last_description_256_aux_256, aux_str_256, 256);
          }
        }
      }
      else if((annot_code >= 0) && (annot_code <= ACMAX))
        {
          tc += *((unsigned short *)a_buf_128) & 0x3ff;

          if(import_std_annots)
          {
            if((!ignore_consecutive) || (annot_code != last_std_code))
            {
              memset(&annotation, 0, sizeof(annotblck_t));
              annotation.onset = (long long)tc * sampletime;
              if(annot_code < 42)
              {
                strncpy(annotation.description, annotdescrlist[annot_code], MAX_ANNOTATION_LEN);
              }
              else
              {
                strncpy(annotation.description, "user-defined", MAX_ANNOTATION_LEN);
              }

              annotation.description[MAX_ANNOTATION_LEN] = 0;

              annotation.edfhdr = mainwindow->edfheaderlist[0];

              if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
                messagewindow.exec();
                fclose(inputfile);
                return 1;
              }

              total_annots++;

              last_std_code = annot_code;
            }
          }
        }

    if(skip)
    {
      if(fseek(inputfile, skip, SEEK_CUR) < 0)
      {
        break;
      }

      bytes_read += skip;
    }
  }

  fclose(inputfile);

  progress.reset();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_xml(void)
{
  int i, j,
      digits,
      ignore_consecutive=0;

  char path[MAX_PATH_LENGTH]={""},
       last_description_256[256]={""},
       result[XML_STRBUFLEN]={""},
       duration_str_32[32]={""};

  long long onset=0LL,
            l_temp,
            utc_time=0LL;

  annotblck_t annotation;

  date_time_t date_time;

  xml_hdl_t *xml_hdl;


  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  strlcpy(path, QFileDialog::getOpenFileName(0, "Open XML file", QString::fromLocal8Bit(mainwindow->recent_opendir), "XML files (*.xml *.XML);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);
  if(xml_hdl==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return 1;
  }

  if((xml_hdl->encoding != 1) && (xml_hdl->encoding != 2))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Encoding of XML-file must be UTF-8 or ISO-8859-1.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return 1;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "annotationlist"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot find root element \"annotationlist\".");
    messagewindow.exec();
    xml_close(xml_hdl);
    return 1;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(j=0; j<10; j++)  qApp->processEvents();

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  for(i=0; i<100000; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "annotation", i))
    {
      if(i == 0)
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot find child element \"annotation\".");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "onset", 0))
    {
      xml_go_up(xml_hdl);
      continue;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QApplication::restoreOverrideCursor();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot get content of element \"annotation\".");
      messagewindow.exec();
      xml_close(xml_hdl);
      return 1;
    }

    if(strlen(result) > 17)
    {
      if((result[4] == '-') && (result[7] == '-') && (result[10] == 'T') && (result[13] == ':') && (result[16] == ':'))
      {
        date_time.year = atoi(result);
        date_time.month = atoi(result + 5);
        date_time.day = atoi(result + 8);
        date_time.hour = atoi(result + 11);
        date_time.minute = atoi(result + 14);
        date_time.second = atoi(result + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_FIXP_SCALING;

        if(strlen(result) > 19)
        {
          if(result[19] == '.')
          {
            for(digits=0; digits<32; digits++)
            {
              if((result[20 + digits] < '0') || (result[20 + digits] > '9'))
              {
                break;
              }
            }
            result[20 + digits] = 0;
            if(digits)
            {
              l_temp = (atoi(result + 20) * TIME_FIXP_SCALING);
              for(; digits>0; digits--)
              {
                l_temp /= 10LL;
              }
              onset += l_temp;
            }
          }
          else
          {
            xml_go_up(xml_hdl);
            continue;
          }
        }
      }
    }
    else
    {
      xml_go_up(xml_hdl);
      continue;
    }

    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "duration", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot get content of element \"duration\".");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      strlcpy(duration_str_32, result, 32);
      duration_str_32[19] = 0;
      if((!(is_number(duration_str_32))) && (duration_str_32[0] != '-'))
      {
        remove_trailing_zeros(duration_str_32);
      }
      else
      {
        duration_str_32[0] = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "description", 0))
    {
      xml_go_up(xml_hdl);
      continue;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QApplication::restoreOverrideCursor();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot get content of element \"description\".");
      messagewindow.exec();
      xml_close(xml_hdl);
      return 1;
    }

    if((!ignore_consecutive) || (strcmp(result, last_description_256)))
    {
      memset(&annotation, 0, sizeof(annotblck_t));
      annotation.onset = onset;
      strlcpy(annotation.description, result, MAX_ANNOTATION_LEN);
      if(xml_hdl->encoding == 1)
      {
        latin1_to_utf8(annotation.description, MAX_ANNOTATION_LEN);
      }
      annotation.description[MAX_ANNOTATION_LEN] = 0;
      trim_spaces(annotation.description);
      strlcpy(annotation.duration, duration_str_32, 20);
      annotation.long_duration = edfplus_annotation_get_long_from_number(duration_str_32);
      annotation.edfhdr = mainwindow->edfheaderlist[0];
      if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      strlcpy(last_description_256, result, 256);
    }

    xml_go_up(xml_hdl);

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

  QApplication::restoreOverrideCursor();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_ascii(void)
{
  int i, j,
      column,
      line_nr,
      startline=1,
      onset_column=1,
      descr_column=2,
      duration_stop_column=3,
      onset_is_set,
      descr_is_set,
      duration_is_set,
      max_descr_length=40,
      onsettime_format=0,
      ignore_consecutive=0,
      use_duration=0,
      use_stoptime=0,
      manualdescription,
      len,
      tmp_len,
      txt_encoding=0;

  char path[MAX_PATH_LENGTH]={""},
       line_4096[4096]={""},
       str1_4096[4096]={""},
       separator[2]=";",
       description_256[256]={""},
       last_description_256[256]={""},
       duration_str_32[32]={""},
       *charpntr=NULL,
       *saveptr=NULL,
       str2_4096[4096]={""},
       filechooser_filter_str_4096[4096]={""};

  long long onset=0LL,
            last_onset=0LL;

  FILE *inputfile=NULL;

  annotblck_t annotation;


  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    manualdescription = 1;

    strlcpy(description_256, DescriptionLineEdit->text().toUtf8().data(), 256);
  }
  else
  {
    manualdescription = 0;
  }

  strlcpy(str1_4096, SeparatorLineEdit->text().toLatin1().data(), 4096);

  if(!strcmp(str1_4096, "tab"))
  {
    separator[0] = '\t';
  }
  else
  {
    if(strlen(str1_4096)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator must be one character or \"tab\".");
      messagewindow.exec();
      return 1;
    }

    if((str1_4096[0]<32)||(str1_4096[0]>126))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character is not a valid ASCII character.");
      messagewindow.exec();
      return 1;
    }

    if(str1_4096[0]=='.')
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character cannot be a dot.");
      messagewindow.exec();
      return 1;
    }

    if((str1_4096[0]>='0')&&(str1_4096[0]<='9'))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character cannot be a number.");
      messagewindow.exec();
      return 1;
    }

    separator[0] = str1_4096[0];
  }

  strlcpy(mainwindow->import_annotations_var->separator, str1_4096, 4);

  startline = DatastartSpinbox->value();

  if(manualdescription)
  {
    descr_column = -1;
  }
  else
  {
    descr_column = DescriptionColumnSpinBox->value() - 1;
  }

  onset_column = OnsetColumnSpinBox->value() - 1;

  onsettime_format = RelativeTimeComboBox->currentIndex();

  if(DurationCheckBox->checkState() == Qt::Checked)
  {
    use_duration = 1;

    use_stoptime = 0;
  }
  else
  {
    use_duration = 0;
  }

  if(StopTimeCheckBox->checkState() == Qt::Checked)
  {
    use_stoptime = 1;

    use_duration = 0;
  }
  else
  {
    use_stoptime = 0;
  }

  if(use_stoptime)
  {
    duration_stop_column = StopColumnSpinBox->value() - 1;
  }
  else
  {
    duration_stop_column = DurationColumnSpinBox->value() - 1;
  }

  if((descr_column == onset_column) && (!manualdescription))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Onset and Description cannot be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((duration_stop_column == onset_column) && use_duration)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Onset and Duration cannot be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((descr_column == duration_stop_column) && (!manualdescription) && use_duration)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Duration and Description cannot be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((duration_stop_column == onset_column) && use_stoptime)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Onset and Stoptime cannot be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((descr_column == duration_stop_column) && (!manualdescription) && use_stoptime)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Stoptime and Description cannot be in the same column.");
    messagewindow.exec();
    return 1;
  }

  mainwindow->import_annotations_var->onsettimeformat = onsettime_format;
  mainwindow->import_annotations_var->onsetcolumn = onset_column + 1;
  mainwindow->import_annotations_var->descriptioncolumn = descr_column + 1;
  if(use_duration)
  {
    mainwindow->import_annotations_var->durationcolumn = duration_stop_column + 1;
  }
  if(use_stoptime)
  {
    mainwindow->import_annotations_var->stopcolumn = duration_stop_column + 1;
  }
  mainwindow->import_annotations_var->useduration = use_duration;
  mainwindow->import_annotations_var->usestoptime = use_stoptime;
  mainwindow->import_annotations_var->datastartline = startline;
  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    mainwindow->import_annotations_var->manualdescription = 1;
  }
  else
  {
    mainwindow->import_annotations_var->manualdescription = 0;
  }
  strlcpy(mainwindow->import_annotations_var->description, DescriptionLineEdit->text().toLatin1().data(), 21);

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  txt_encoding = text_encoding_combobox->currentIndex();
  mainwindow->import_annotations_var->ascii_txt_encoding = txt_encoding;

  if(mainwindow->import_annotations_var->csv_equal_filename)
  {
    get_filename_from_path(str2_4096, mainwindow->edfheaderlist[0]->filename, 4096);

    remove_extension_from_filename(str2_4096);

//     snprintf(filechooser_filter_str_4096, 4096, "CSV file (%.1000s.csv);;TSV file (%.1000s.tsv);;TXT file (%.1000s.txt)",
//              str2_4096, str2_4096, str2_4096);

    snprintf(filechooser_filter_str_4096, 4096, "ASCII files (%.1000s.*)", str2_4096);

    strlcpy(path, QFileDialog::getOpenFileName(0, "Open ASCII file", QString::fromLocal8Bit(mainwindow->recent_opendir), filechooser_filter_str_4096).toLocal8Bit().data(), MAX_PATH_LENGTH);
  }
  else
  {
    strlcpy(path, QFileDialog::getOpenFileName(0, "Open ASCII file", QString::fromLocal8Bit(mainwindow->recent_opendir), "ASCII files (*.txt *.TXT *.csv *.CSV *.tsv *.TSV);;All files (*)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  for(i=strlen(path)-1; i>=0; i--)
  {
    if(path[i] == '.')
    {
      if((!strcmp(path + i, ".edf")) ||
         (!strcmp(path + i, ".EDF")) ||
         (!strcmp(path + i, ".bdf")) ||
         (!strcmp(path + i, ".BDF")))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "The ASCII/CSV/TXT importer cannot import annotations from EDF/BDF files.\n"
                                                                  "Use the tab \"EDF+ / BDF+\" instead.");
        messagewindow.exec();
        return 1;
      }

      break;
    }
  }

  if(mainwindow->import_annotations_var->csv_equal_filename)
  {
    for(i=strlen(path)-1; i>=0; i--)
    {
      if(path[i] == '.')
      {
        if((strcmp(path + i, ".txt")) &&
           (strcmp(path + i, ".TXT")) &&
           (strcmp(path + i, ".csv")) &&
           (strcmp(path + i, ".CSV")) &&
           (strcmp(path + i, ".tsv")) &&
           (strcmp(path + i, ".TSV")))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "You selected a file with an unknown extension.\n"
                                                                    "Filename extension must be \".txt\", \".csv\" or \".tsv\".");
          messagewindow.exec();
          return 1;
        }

        break;
      }
    }
  }

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return 1;
  }

  rewind(inputfile);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(j=0; j<10; j++)  qApp->processEvents();

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  if((use_duration == 0) && (use_stoptime == 0))  duration_stop_column = -1;

  mal_formatted_lines = 0;

  for(line_nr=1; !feof(inputfile); line_nr++)
  {
    if(line_nr == 0x7fffffff)  break;

    if(fgets(line_4096, 4096, inputfile) == NULL)
    {
      break;
    }

    if(line_nr < startline)  continue;

    len = strlen(line_4096);

    if(line_4096[len-1] == '\n')
    {
      line_4096[len-1] = 0;

      if(--len == 0)
      {
        continue;
      }
    }

    if(line_4096[len-1] == '\r')
    {
      line_4096[len-1] = 0;

      if(--len == 0)
      {
        continue;
      }
    }

    onset_is_set = 0;
    descr_is_set = 0;
    duration_is_set = 0;
    duration_str_32[0] = 0;

    for(column=0; column<32; column++)
    {
      if(column == 0)
      {
        charpntr = strtok_r_e(line_4096, separator, &saveptr);
      }
      else
      {
        charpntr = strtok_r_e(NULL, separator, &saveptr);
      }

      if(charpntr == NULL)
      {
        break;
      }
      else
      {
        if(column == onset_column)
        {
          if(!strlen(charpntr))  continue;
#ifdef IMPORT_ANNOTS_DEBUG
          printf("  onset: ->%s<-", charpntr);
#endif
          if(get_onset_time_from_ascii(charpntr, &onset, &last_onset, onsettime_format) == 0)
          {
            onset_is_set = 1;
          }
        }
        else if(column == descr_column)
          {
            if(!strlen(charpntr))  continue;
#ifdef IMPORT_ANNOTS_DEBUG
            printf("  description_256: ->%s<-", charpntr);
#endif
            strlcpy(description_256, charpntr, max_descr_length);
            if(txt_encoding == 1)
            {
              latin1_to_utf8(description_256, max_descr_length);
            }
            str_replace_ctrl_chars(description_256, '.');
            trim_spaces(description_256);
            descr_is_set = 1;
          }
          else if(column == duration_stop_column)
            {
#ifdef IMPORT_ANNOTS_DEBUG
              printf("  duration: ->%s<-", charpntr);
#endif
              strlcpy(duration_str_32, charpntr, 32);
              trim_spaces(duration_str_32);
              duration_str_32[19] = 0;
              duration_is_set = 1;
            }
      }
    }
#ifdef IMPORT_ANNOTS_DEBUG
    printf("  line %i\n", line_nr);
#endif
    if(onset_is_set && (descr_is_set || manualdescription))
    {
      if((!ignore_consecutive) || strcmp(description_256, last_description_256))
      {
        memset(&annotation, 0, sizeof(annotblck_t));
        annotation.onset = onset;
        strncpy(annotation.description, description_256, MAX_ANNOTATION_LEN);
        annotation.description[MAX_ANNOTATION_LEN] = 0;
        if((use_duration || use_stoptime) && duration_is_set)
        {
          if((!(is_number(duration_str_32))) && (duration_str_32[0] != '-'))
          {
            annotation.long_duration = edfplus_annotation_get_long_from_number(duration_str_32);

            if(use_stoptime)
            {
              if(annotation.onset >= annotation.long_duration)
              {
                annotation.long_duration = 0LL;
              }
              else
              {
                annotation.long_duration -= annotation.onset;

                tmp_len = snprintf(annotation.duration, 20, "%i", (int)(annotation.long_duration / TIME_FIXP_SCALING));

                if(annotation.long_duration % TIME_FIXP_SCALING)
                {
                  if(tmp_len < 18)
                  {
                    snprintf(annotation.duration + tmp_len, 20 - tmp_len, ".%07i", (int)(annotation.long_duration % TIME_FIXP_SCALING));
                    remove_trailing_zeros(duration_str_32);
                  }
                }
              }
            }
            else  /* use duration */
            {
              remove_trailing_zeros(duration_str_32);
              if(duration_str_32[0] == '+')
              {
                strlcpy(annotation.duration, duration_str_32 + 1, 20);
              }
              else
              {
                strlcpy(annotation.duration, duration_str_32, 20);
              }
            }
          }
        }
        annotation.edfhdr = mainwindow->edfheaderlist[0];
        if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
        {
          QApplication::restoreOverrideCursor();
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
          messagewindow.exec();
          fclose(inputfile);
          return 1;
        }

        strlcpy(last_description_256, description_256, 256);
      }
    }
    else
    {
      if(mal_formatted_lines < 32)
      {
        mal_formatted_line_nrs[mal_formatted_lines++] = line_nr;
      }
    }
  }

  QApplication::restoreOverrideCursor();

  fclose(inputfile);

  return 0;
}


int UI_ImportAnnotationswindow::import_from_edfplus(void)
{
  int i,
      annotlist_size;

  char path[MAX_PATH_LENGTH]={""},
       str1_2048[2048]={""};

  long long starttime_diff;

  FILE *inputfile=NULL;

  edfhdrblck_t *edfhdr=NULL;

  annotblck_t *annotation=NULL;


  strlcpy(path, QFileDialog::getOpenFileName(0, "Open EDF+/BDF+ file", QString::fromLocal8Bit(mainwindow->recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF )").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for reading.");
    messagewindow.exec();
    return 1;
  }

  rewind(inputfile);

  edfhdr = check_edf_file(inputfile, str1_2048, 2048, 0, 1);
  if(edfhdr==NULL)
  {
    strlcat(str1_2048, "\n File is not a valid EDF or BDF file.", 2048);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
    messagewindow.exec();
    fclose(inputfile);
    return 1;
  }

  if(!(edfhdr->edfplus || edfhdr->bdfplus))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not an EDF+ or BDF+ file.");
    messagewindow.exec();
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  strlcpy(edfhdr->filename, path, MAX_PATH_LENGTH);

  edfhdr->file_hdl = inputfile;

  EDF_annotations annotations;

  annotations.get_annotations(edfhdr, mainwindow->read_nk_trigger_signal);
  if(edfhdr->annots_not_read)
  {
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  annotlist_size = edfplus_annotation_size(&edfhdr->annot_list);
  if(annotlist_size < 1)
  {
    QMessageBox messagewindow(QMessageBox::Information, "Import annotations", "No annotations found.");
    messagewindow.exec();
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  starttime_diff = edfhdr->utc_starttime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime;

  starttime_diff *= TIME_FIXP_SCALING;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  for(i=0; i<annotlist_size; i++)
  {
    annotation = edfplus_annotation_get_item(&edfhdr->annot_list, i);
    annotation->onset += starttime_diff;
    annotation->edfhdr = mainwindow->edfheaderlist[0];
    edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, *annotation);
  }

  edfplus_annotation_sort(&mainwindow->edfheaderlist[0]->annot_list, NULL);

  mainwindow->get_unique_annotations(mainwindow->edfheaderlist[0]);

  edfplus_annotation_empty_list(&edfhdr->annot_list);
  free(edfhdr->edfparam);
  free(edfhdr);
  fclose(inputfile);

  QApplication::restoreOverrideCursor();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_dcevent(void)
{
  int i,
      ignore_consecutive=0,
      signal_nr,
      smpls_per_datrec,
      bytes_per_datrec,
      recsize,
      jumpbytes,
      bufoffset,
      triggervalue,
      bitwidth,
      bitposition,
      trigger_sample,
      next_sample,
      eventcode,
      tmp_value=0,
      edfformat,
      annotations_found=0;

  char scratchpad_256[256]={""},
       last_description_256[256],
       *buf=NULL;

  long long datrecs,
            trigger_datrec,
            time_per_sample,
            progress_steps;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FILE *inputfile=NULL;

  annotblck_t annotation;

  sigcompblck_t *signalcomp;


  last_description_256[0] = 0;

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  signal_nr = DCEventSignalComboBox->currentIndex();

  if(signal_nr < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You need to put at least one signal on the screen.");
    messagewindow.exec();
    return 1;
  }

  if(mainwindow->signalcomp[signal_nr]->num_of_signals > 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The signal cannot be a derivation of multiple signals.");
    messagewindow.exec();
    return 1;
  }

  mainwindow->import_annotations_var->dceventbittime = BitTimeSpinbox->value();

  mainwindow->import_annotations_var->triggerlevel = DCEventTriggerLevelSpinBox->value();

  signalcomp = mainwindow->signalcomp[signal_nr];

  smpls_per_datrec = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  recsize = signalcomp->edfhdr->recordsize;

  bufoffset = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].datrec_offset;

  time_per_sample = signalcomp->edfhdr->long_data_record_duration / smpls_per_datrec;

  if(signalcomp->edfhdr->edf)
  {
    edfformat = 1;

    bytes_per_datrec = smpls_per_datrec * 2;
  }
  else
  {
    bytes_per_datrec = smpls_per_datrec * 3;

    edfformat = 0;
  }

  jumpbytes = recsize - bytes_per_datrec;

  inputfile = signalcomp->edfhdr->file_hdl;

  bitwidth = ((long long)mainwindow->import_annotations_var->dceventbittime * 10000LL) / time_per_sample;

  if(bitwidth < 5)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Bit Time is set too low compared to the samplerate of the selected signal.");
    messagewindow.exec();
    return 1;
  }

  triggervalue = mainwindow->import_annotations_var->triggerlevel / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;

  triggervalue -= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].offset;

  if(triggervalue >= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].dig_max)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Trigger Level is equal or higher than physical maximum.");
    messagewindow.exec();
    return 1;
  }

  if(triggervalue <= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].dig_min)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Trigger Level is equal or lower than physical minimum.");
    messagewindow.exec();
    return 1;
  }

  if(fseeko(inputfile, signalcomp->edfhdr->hdrsize + bufoffset, SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile. (fseek)");
    messagewindow.exec();
    return 1;
  }

  buf = (char *)malloc(bytes_per_datrec);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (buf)");
    messagewindow.exec();
    return 1;
  }

  annotations_found = 0;

  QProgressDialog progress("Scanning file for DC-events...", "Abort", 0, (int)signalcomp->edfhdr->datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = signalcomp->edfhdr->datarecords / 100LL;
  if(progress_steps < 1LL)
  {
    progress_steps = 1LL;
  }

/*  BITPOSITION:
  0  nothing (idle)
  1  rising edge of startbit found
  2  middle of startbit found
  3  middle of bit 0 found
  ........................
  10 middle of bit 7 found
  11 middle of stopbit found
*/

  bitposition = 0;

  eventcode = 0;

  next_sample = 0;

  trigger_sample = 0;

  trigger_datrec = 0LL;

  for(datrecs=0LL; datrecs < signalcomp->edfhdr->datarecords; datrecs++)
  {
    if(annotations_found > 10000)
    {
      progress.reset();

      break;
    }

    if(!(datrecs % progress_steps))
    {
      progress.setValue((int)datrecs);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    if(datrecs)
    {
      fseek(inputfile, jumpbytes, SEEK_CUR);
    }

    if(fread(buf, bytes_per_datrec, 1, inputfile) != 1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile. (fread)");
      messagewindow.exec();
      free(buf);
      return 1;
    }

    for(i=0; i<smpls_per_datrec; i++)
    {
      if(edfformat)
      {
        tmp_value = *(((signed short *)buf) + i);
      }
      else
      {
        var.two[0] = *((unsigned short *)(buf + (i * 3)));
        var.four[2] = *((unsigned char *)(buf + (i * 3) + 2));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        tmp_value = var.one_signed;
      }

      if(bitposition)
      {
        if(i == next_sample)
        {
          if(bitposition == 1)
          {
            if(tmp_value < triggervalue)
            {
              bitposition = 0;

              continue;
            }
          }
          else
          {
            if(bitposition < 10)
            {
              if(tmp_value > triggervalue)
              {
                eventcode += (1 << (bitposition - 2));
              }
            }

            if(bitposition == 10)
            {
              if(tmp_value < triggervalue)
              {
                snprintf(scratchpad_256, 256, "Trigger ID=%i", eventcode);

                if((!ignore_consecutive) || (strcmp(scratchpad_256, last_description_256)))
                {
                  memset(&annotation, 0, sizeof(annotblck_t));
                  annotation.onset = ((trigger_datrec * signalcomp->edfhdr->long_data_record_duration) + (trigger_sample * time_per_sample));
                  annotation.edfhdr = signalcomp->edfhdr;
                  strncpy(annotation.description, scratchpad_256, MAX_ANNOTATION_LEN);
                  annotation.description[MAX_ANNOTATION_LEN] = 0;
                  if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
                  {
                    progress.reset();
                    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
                    messagewindow.exec();
                    free(buf);
                    return 1;
                  }

                  annotations_found++;

                  strlcpy(last_description_256, scratchpad_256, 256);
                }
              }

              bitposition = 0;

              continue;
            }
          }

          next_sample = (i + bitwidth) % smpls_per_datrec;

          bitposition++;
        }
      }
      else
      {
        if(tmp_value > triggervalue)
        {
          trigger_sample = i;

          trigger_datrec = datrecs;

          bitposition = 1;

          next_sample = (i + (bitwidth / 2)) % smpls_per_datrec;

          eventcode = 0;
        }
      }
    }
  }

  free(buf);

  inputfile = NULL;

  progress.reset();

  return 0;
}


void UI_ImportAnnotationswindow::helpbuttonpressed()
{
  mainwindow->open_manual("#Import_annotations");
}


int UI_ImportAnnotationswindow::get_samplefreq_inf(void)
{
  int i, smps=0;

  if(mainwindow->files_open != 1)  return 0;

  for(i=0; i<mainwindow->edfheaderlist[0]->edfsignals; i++)
  {
    if(mainwindow->edfheaderlist[0]->edfparam[i].annotation)  continue;

    if(i == 0)
    {
      smps = mainwindow->edfheaderlist[0]->edfparam[i].smp_per_record;
    }
    else
    {
      if(smps != mainwindow->edfheaderlist[0]->edfparam[i].smp_per_record)  return 0;
    }
  }

  if(!smps)  return 0;

  return ((long long)smps * TIME_FIXP_SCALING) / mainwindow->edfheaderlist[0]->long_data_record_duration;
}


int UI_ImportAnnotationswindow::get_onset_time_from_ascii(const char *str, long long *onset_time, long long *last_onset, int onset_format)
{
  int digits;

  long long l_temp, onset=0LL, utc_time=0LL;

  char scratchpad_64[64]={""};

  date_time_t date_time;

  strncpy(scratchpad_64, str, 30);
  scratchpad_64[30] = 0;

  if(onset_format == CVS_ONSET_TIME_FMT_RELATIVE)
  {
    onset = atoll_x(scratchpad_64, TIME_FIXP_SCALING);
    *onset_time = onset;

    return 0;
  }

  if(onset_format == CVS_ONSET_TIME_FMT_ABS_TIME)
  {
    if(strlen(scratchpad_64) > 6)
    {
      if((scratchpad_64[2] == ':') && (scratchpad_64[5] == ':'))
      {
        scratchpad_64[8] = 0;
        onset = atoi(scratchpad_64) * 3600LL;
        onset += (atoi(scratchpad_64 + 3) * 60LL);
        onset += (long long)(atoi(scratchpad_64 + 6));
        onset *= TIME_FIXP_SCALING;
        onset -= mainwindow->edfheaderlist[0]->starttime_hr;

        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_FIXP_SCALING);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
    if(strlen(scratchpad_64) > 5)
    {
      if((scratchpad_64[1] == ':') && (scratchpad_64[4] == ':'))
      {
        scratchpad_64[7] = 0;
        onset = atoi(scratchpad_64) * 3600LL;
        onset += (atoi(scratchpad_64 + 2) * 60LL);
        onset += (long long)(atoi(scratchpad_64 + 5));
        onset *= TIME_FIXP_SCALING;
        onset -= mainwindow->edfheaderlist[0]->starttime_hr;

        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_FIXP_SCALING);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(onset_format == CVS_ONSET_TIME_FMT_ABS_TIME_SUBSEC)
  {
    if(strlen(scratchpad_64) > 8)
    {
      if((scratchpad_64[2] == ':') && (scratchpad_64[5] == ':') && ((scratchpad_64[8] == '.') || (scratchpad_64[8] == ',')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad_64[9 + digits] < '0') || (scratchpad_64[9 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad_64[9 + digits] = 0;
        onset = atoi(scratchpad_64) * 3600LL;
        onset += (atoi(scratchpad_64 + 3) * 60LL);
        onset += (long long)(atoi(scratchpad_64 + 6));
        onset *= TIME_FIXP_SCALING;
        if(digits)
        {
          l_temp = (atoi(scratchpad_64 + 9) * TIME_FIXP_SCALING);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }
        onset -= mainwindow->edfheaderlist[0]->starttime_hr;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_FIXP_SCALING);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
    if(strlen(scratchpad_64) > 7)
    {
      if((scratchpad_64[1] == ':') && (scratchpad_64[4] == ':') && ((scratchpad_64[7] == '.') || (scratchpad_64[7] == ',')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad_64[8 + digits] < '0') || (scratchpad_64[8 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad_64[8 + digits] = 0;
        onset = atoi(scratchpad_64) * 3600LL;
        onset += (atoi(scratchpad_64 + 2) * 60LL);
        onset += (long long)(atoi(scratchpad_64 + 5));
        onset *= TIME_FIXP_SCALING;
        if(digits)
        {
          l_temp = (atoi(scratchpad_64 + 8) * TIME_FIXP_SCALING);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }
        onset -= mainwindow->edfheaderlist[0]->starttime_hr;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_FIXP_SCALING);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(onset_format == CVS_ONSET_TIME_FMT_ABS_DATETIME)
  {
    if(strlen(scratchpad_64) > 17)
    {
      if((scratchpad_64[4] == '-') && (scratchpad_64[7] == '-') && (scratchpad_64[13] == ':') && (scratchpad_64[16] == ':'))
      {
        scratchpad_64[19] = 0;
        date_time.year = atoi(scratchpad_64);
        date_time.month = atoi(scratchpad_64 + 5);
        date_time.day = atoi(scratchpad_64 + 8);
        date_time.hour = atoi(scratchpad_64 + 11);
        date_time.minute = atoi(scratchpad_64 + 14);
        date_time.second = atoi(scratchpad_64 + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_FIXP_SCALING;

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(onset_format == CVS_ONSET_TIME_FMT_ABS_DATETIME_SUBSEC)
  {
    if(strlen(scratchpad_64) > 19)
    {
      if((scratchpad_64[4] == '-') && (scratchpad_64[7] == '-') && (scratchpad_64[13] == ':') && (scratchpad_64[16] == ':') && ((scratchpad_64[19] == ',') || (scratchpad_64[19] == '.')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad_64[20 + digits] < '0') || (scratchpad_64[20 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad_64[20 + digits] = 0;
        date_time.year = atoi(scratchpad_64);
        date_time.month = atoi(scratchpad_64 + 5);
        date_time.day = atoi(scratchpad_64 + 8);
        date_time.hour = atoi(scratchpad_64 + 11);
        date_time.minute = atoi(scratchpad_64 + 14);
        date_time.second = atoi(scratchpad_64 + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_FIXP_SCALING;
        if(digits)
        {
          l_temp = (atoi(scratchpad_64 + 20) * TIME_FIXP_SCALING);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  return -1;
}















