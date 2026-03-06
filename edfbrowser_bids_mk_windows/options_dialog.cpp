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



#include "options_dialog.h"


#define DEFAULT_COLOR_LIST_SZ  (6)


static const char font_sz_example_txt[]={
  "The quick brown fox jumps over the lazy dog. 0123456789 AaBbCcDdEeWwXxYyZz\n\n"
  "European Data Format (EDF) is a standard file format designed for exchange and storage of medical time series."
  " Being an open and non-proprietary format, EDF+/BDF+ is commonly used to archive, exchange and analyse data from"
  " commercial devices in a format that is independent of the acquisition system. In this way, the data can be"
  " retrieved and analyzed by independent software. EDF+/BDF+ software (browsers, checkers, ...) and example files"
  " are freely available."};


UI_OptionsDialog::UI_OptionsDialog(QWidget *w_parent)
{
  int i;

  char str1_512[512]="";

  mainwindow = (UI_Mainwindow *)w_parent;

  optionsdialog = new QDialog(w_parent);

  optionsdialog->setWindowTitle("Settings");
  optionsdialog->setModal(true);
  optionsdialog->setAttribute(Qt::WA_DeleteOnClose, true);
  optionsdialog->setSizeGripEnabled(true);

  tabholder = new QTabWidget;

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout_tmp;

  QVBoxLayout *vlayout_tmp;

/////////////////////////////////////// tab 1 Colors ///////////////////////////////////////////////////////////////////////

  tab1 = new QWidget;

  QFormLayout *flayout1_1 = new QFormLayout;
  flayout1_1->setSpacing(20);

  BgColorButton = new SpecialButton;
  BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(BgColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Background color", hlayout_tmp);

  SrColorButton = new SpecialButton;
  SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(SrColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Small ruler color", hlayout_tmp);

  BrColorButton = new SpecialButton;
  BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(BrColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Big ruler color", hlayout_tmp);

  MrColorButton = new SpecialButton;
  MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(MrColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Mouse rectangle color", hlayout_tmp);

  TxtColorButton = new SpecialButton;
  TxtColorButton->setColor(mainwindow->maincurve->text_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(TxtColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Text color", hlayout_tmp);

  SigColorButton = new SpecialButton;
  SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(SigColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Signals color", hlayout_tmp);

  checkbox16 = new QCheckBox;
  checkbox16->setTristate(false);
  checkbox16->setToolTip("When adding signals to the screen, vary the traces' color");
  if(mainwindow->use_diverse_signal_colors)
  {
    checkbox16->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox16->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox16);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Vary signal colors", hlayout_tmp);

  checkbox3 = new QCheckBox;
  checkbox3->setTristate(false);
  if(mainwindow->show_baselines)
  {
    checkbox3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox3->setCheckState(Qt::Unchecked);
  }
  BaseColorButton = new SpecialButton;
  BaseColorButton->setColor(mainwindow->maincurve->baseline_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox3);
  hlayout_tmp->addWidget(BaseColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Baseline color", hlayout_tmp);

  FrColorButton = new SpecialButton;
  FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(FrColorButton);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Floating ruler color", hlayout_tmp);

  checkbox1 = new QCheckBox;
  checkbox1->setTristate(false);
  if(mainwindow->maincurve->blackwhite_printing)
  {
    checkbox1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox1->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox1);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Print in grayscale", hlayout_tmp);

  checkbox4 = new QCheckBox;
  checkbox4->setTristate(false);
  if(mainwindow->clip_to_pane)
  {
    checkbox4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Clip signals to pane", hlayout_tmp);

  checkbox5 = new QCheckBox;
  checkbox5->setTristate(false);
  checkbox5->setToolTip("Traces are plotted one after another and, as a result, the last trace plotted can overwrite the other traces.\n"
                        "When unchecked (default), the plotting order will be from bottom to top.\n"
                        "If checked, the plotting order will be from top to bottom.");
  if(mainwindow->signal_plotorder)
  {
    checkbox5->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox5->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox5);
  hlayout_tmp->addStretch(1000);
  flayout1_1->addRow("Reverse signal plot order", hlayout_tmp);
  flayout1_1->labelForField(hlayout_tmp)->setToolTip("Traces are plotted one after another and, as a result, the last trace plotted can overwrite the other traces.\n"
                                                     "When unchecked (default), the plotting order will be from bottom to top.\n"
                                                     "If checked, the plotting order will be from top to bottom.");

  QFormLayout *flayout1_2 = new QFormLayout;
  flayout1_2->setSpacing(20);

  checkbox2 = new QCheckBox;
  checkbox2->setTristate(false);
  checkbox2->setToolTip("If disabled, the annotation markers will not be drawn in the waveform window");
  if(mainwindow->show_annot_markers)
  {
    checkbox2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2->setCheckState(Qt::Unchecked);
  }
  AnnotMkrButton = new SpecialButton;
  AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);
  AnnotMkrButton->setToolTip("The first color is the default, the second color is used to indicate if it's selected");
  AnnotMkrSelButton = new SpecialButton;
  AnnotMkrSelButton->setColor(mainwindow->maincurve->annot_marker_selected_color);
  AnnotMkrSelButton->setToolTip("The first color is the default, the second color is used to indicate if it's selected");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox2);
  hlayout_tmp->addWidget(AnnotMkrButton);
  hlayout_tmp->addWidget(AnnotMkrSelButton);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Annotation marker", hlayout_tmp);
  flayout1_2->labelForField(hlayout_tmp)->setToolTip("The vertical dashed line that indicates the annotation's onset");

  checkbox2_3 = new QCheckBox;
  checkbox2_3->setTristate(false);
  if(mainwindow->channel_linked_annotations)
  {
    checkbox2_3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2_3->setCheckState(Qt::Unchecked);
  }
  checkbox2_3->setToolTip("If enabled and an annotation description is \"signal linked\" e.g. \"some event@@EEG F4\",\n"
                          "the annotation marker and overlay color will only be drawn over that trace instead of from top to bottom");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox2_3);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Use signal linked annotations", hlayout_tmp);
  flayout1_2->labelForField(hlayout_tmp)->setToolTip("If enabled and an annotation description is \"signal linked\" e.g. \"some event@@EEG F4\",\n"
                                                     "the annotation marker and overlay color will only be drawn over that trace instead of from top to bottom");

  checkbox2_1 = new QCheckBox;
  checkbox2_1->setTristate(false);
  if(mainwindow->annotations_show_duration)
  {
    checkbox2_1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2_1->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox2_1);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Show duration at marker", hlayout_tmp);

  checkbox2_2 = new QCheckBox;
  checkbox2_2->setTristate(false);
  if(mainwindow->annotations_duration_background_type)
  {
    checkbox2_2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2_2->setCheckState(Qt::Unchecked);
  }
  checkbox2_2->setToolTip("Show the overlay color of the annotation's duration only at the bottom of the screen");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox2_2);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Show only at screen bottom", hlayout_tmp);
  flayout1_2->labelForField(hlayout_tmp)->setToolTip("Show the overlay color of the annotation's duration only at the bottom of the screen");

  annotlistdock_edited_txt_color_button = new SpecialButton;
  annotlistdock_edited_txt_color_button->setColor(mainwindow->annot_list_edited_txt_color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(annotlistdock_edited_txt_color_button);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Annotations list edited text color", hlayout_tmp);

  AnnotDurationButton = new SpecialButton;
  AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);
  AnnotDurationButton->setToolTip("The first color is the default");
  AnnotDurationSelectedButton = new SpecialButton;
  AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);
  AnnotDurationSelectedButton->setToolTip("The second color is used to indicate if it's selected");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(AnnotDurationButton);
  hlayout_tmp->addWidget(AnnotDurationSelectedButton);
  hlayout_tmp->addStretch(1000);
  flayout1_2->addRow("Annotation's duration overlay color", hlayout_tmp);
  flayout1_2->labelForField(hlayout_tmp)->setToolTip("The first color is the default, the second color is used to indicate if it's selected");

  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    AnnotDurationPredefinedButton[i] = new SpecialButton;
    AnnotDurationPredefinedButton[i]->setColor(mainwindow->mc_annot_ov_color_predefined[i]);
    AnnotDurationPredefinedButton[i]->setToolTip("This overlay color is used when the description of the annotation matches the description here");
    AnnotDurationPredefinedLineEdit[i] = new QLineEdit;
    AnnotDurationPredefinedLineEdit[i]->setMaxLength(31);
    AnnotDurationPredefinedLineEdit[i]->setText(QString::fromUtf8(mainwindow->mc_annot_ov_name_predefined[i]));
    AnnotDurationPredefinedLineEdit[i]->setToolTip("This overlay color is used when the description of the annotation matches the description here");
    hlayout_tmp = new QHBoxLayout;
    hlayout_tmp->setAlignment(Qt::AlignCenter);
    hlayout_tmp->addWidget(AnnotDurationPredefinedLineEdit[i]);
    hlayout_tmp->addWidget(AnnotDurationPredefinedButton[i]);
    hlayout_tmp->addStretch(1000);
    flayout1_2->addRow("Overlay color for annotation", hlayout_tmp);
    flayout1_2->labelForField(hlayout_tmp)->setToolTip("This overlay color is used when the description of the annotation matches the description here");
  }

  colorSchema_Dark_Button = new QPushButton;
  colorSchema_Dark_Button->setText("\"Dark\"");

  colorSchema_NK_Button = new QPushButton;
  colorSchema_NK_Button->setText("\"NK\"");

  colorSchema_Blue_on_Gray_Button = new QPushButton;
  colorSchema_Blue_on_Gray_Button->setText("\"Blue on gray\"");

  colorSchema_ECG_Button = new QPushButton;
  colorSchema_ECG_Button->setText("\"ECG\"");
  colorSchema_ECG_Button->setToolTip("The ECG schema will activate the ECG grid which is intended to be used exclusively for ECG signals\n"
                                     " where the physical dimension (unit) of the signals are expressed in either uV, mV or V.");

  saveColorSchemaButton = new QPushButton;
  saveColorSchemaButton->setText("Save");
  saveColorSchemaButton->setToolTip("Save your customized color schema");

  loadColorSchemaButton = new QPushButton;
  loadColorSchemaButton->setText("Load");
  loadColorSchemaButton->setToolTip("Load your customized color schema");

  grid_normal_radiobutton = new QRadioButton("Normal");
  grid_normal_radiobutton->setToolTip("The ECG grid is intended to be used exclusively for ECG signals where the\n"
                                      "physical dimension (unit) of the signals are expressed in either uV, mV or V.");
  grid_ecg_radiobutton = new QRadioButton("ECG");
  grid_ecg_radiobutton->setToolTip("The ECG grid is intended to be used exclusively for ECG signals where the\n"
                                   "physical dimension (unit) of the signals are expressed in either uV, mV or V.");
  if(mainwindow->ecg_view_mode)
  {
    grid_ecg_radiobutton->setChecked(true);
  }
  else
  {
    grid_normal_radiobutton->setChecked(true);
  }
  grid_radio_group = new QButtonGroup(this);
  grid_radio_group->addButton(grid_normal_radiobutton, 0);
  grid_radio_group->addButton(grid_ecg_radiobutton, 1);
#if QT_VERSION < 0x060000
  QObject::connect(grid_radio_group, SIGNAL(buttonClicked(int)), this, SLOT(grid_radio_group_clicked(int)));
#else
  QObject::connect(grid_radio_group, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(grid_radio_group_clicked(QAbstractButton *)));
#endif

  QVBoxLayout *vlayout1_4 = new QVBoxLayout;
  vlayout1_4->addStretch(100);
  vlayout1_4->addWidget(colorSchema_Dark_Button);
  vlayout1_4->addWidget(colorSchema_NK_Button);
  vlayout1_4->addWidget(colorSchema_Blue_on_Gray_Button);
  vlayout1_4->addWidget(colorSchema_ECG_Button);
  vlayout1_4->addSpacing(30 * mainwindow->h_scaling);
  vlayout1_4->addWidget(saveColorSchemaButton);
  vlayout1_4->addWidget(loadColorSchemaButton);
  vlayout1_4->addStretch(100);

  QVBoxLayout *vlayout1_5 = new QVBoxLayout;
  vlayout1_5->addStretch(100);
  vlayout1_5->addWidget(grid_normal_radiobutton);
  vlayout1_5->addWidget(grid_ecg_radiobutton);
  vlayout1_5->addStretch(100);

  QHBoxLayout *hlayout1_2 = new QHBoxLayout;
  hlayout1_2->addStretch(100);
  hlayout1_2->addLayout(vlayout1_4);
  hlayout1_2->addStretch(100);

  QHBoxLayout *hlayout1_3 = new QHBoxLayout;
  hlayout1_3->addStretch(100);
  hlayout1_3->addLayout(vlayout1_5);
  hlayout1_3->addStretch(100);

  groupbox1 = new QGroupBox("Colorschema");
  groupbox1->setLayout(hlayout1_2);

  groupbox2 = new QGroupBox("Grid");
  groupbox2->setLayout(hlayout1_3);

  QVBoxLayout *vlayout1_1 = new QVBoxLayout;
  vlayout1_1->addLayout(flayout1_1);
  vlayout1_1->addStretch(1000);

  QVBoxLayout *vlayout1_2 = new QVBoxLayout;
  vlayout1_2->addLayout(flayout1_2);
  vlayout1_2->addStretch(1000);

  QVBoxLayout *vlayout1_3 = new QVBoxLayout;
  vlayout1_3->addWidget(groupbox1);
  vlayout1_3->addWidget(groupbox2);
  vlayout1_3->addStretch(1000);

  QFrame *frame1 = new QFrame;
  frame1->setFrameStyle(QFrame::VLine | QFrame::Plain);

  QFrame *frame2 = new QFrame;
  frame2->setFrameStyle(QFrame::VLine | QFrame::Plain);

  QHBoxLayout *hlayout1_1 = new QHBoxLayout;
  hlayout1_1->addLayout(vlayout1_1);
  hlayout1_1->addSpacing(20);
  hlayout1_1->addWidget(frame1);
  hlayout1_1->addSpacing(20);
  hlayout1_1->addLayout(vlayout1_2);
  hlayout1_1->addSpacing(20);
  hlayout1_1->addWidget(frame2);
  hlayout1_1->addSpacing(20);
  hlayout1_1->addLayout(vlayout1_3);
  hlayout1_1->addStretch(500);

  tab1->setLayout(hlayout1_1);

  QObject::connect(BgColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(BgColorButtonClicked(SpecialButton *)));
  QObject::connect(SrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(SrColorButtonClicked(SpecialButton *)));
  QObject::connect(BrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(BrColorButtonClicked(SpecialButton *)));
  QObject::connect(MrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(MrColorButtonClicked(SpecialButton *)));
  QObject::connect(TxtColorButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(TxtColorButtonClicked(SpecialButton *)));
  QObject::connect(SigColorButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(SigColorButtonClicked(SpecialButton *)));
  QObject::connect(BaseColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(BaseColorButtonClicked(SpecialButton *)));
  QObject::connect(FrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(FrColorButtonClicked(SpecialButton *)));
  QObject::connect(AnnotMkrButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotMkrButtonClicked(SpecialButton *)));
  QObject::connect(AnnotMkrSelButton,       SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotMkrSelButtonClicked(SpecialButton *)));
  QObject::connect(AnnotDurationButton,     SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationButtonClicked(SpecialButton *)));
  QObject::connect(AnnotDurationSelectedButton,     SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationSelectedButtonClicked(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[0], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_1(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[1], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_2(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[2], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_3(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[3], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_4(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[4], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_5(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[5], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_6(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[6], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_7(SpecialButton *)));
  QObject::connect(AnnotDurationPredefinedButton[7], SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationPredefinedButtonClicked_8(SpecialButton *)));
  QObject::connect(checkbox1,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox1Clicked(int)));
  QObject::connect(checkbox2,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox2Clicked(int)));
  QObject::connect(checkbox2_1,             SIGNAL(stateChanged(int)),        this, SLOT(checkbox2_1Clicked(int)));
  QObject::connect(checkbox2_2,             SIGNAL(stateChanged(int)),        this, SLOT(checkbox2_2Clicked(int)));
  QObject::connect(checkbox2_3,             SIGNAL(stateChanged(int)),        this, SLOT(checkbox2_3Clicked(int)));
  QObject::connect(checkbox3,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox3Clicked(int)));
  QObject::connect(checkbox4,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox4Clicked(int)));
  QObject::connect(checkbox5,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox5Clicked(int)));
  QObject::connect(annotlistdock_edited_txt_color_button, SIGNAL(clicked(SpecialButton *)), this, SLOT(annotlistdock_edited_txt_color_button_clicked(SpecialButton *)));
  QObject::connect(checkbox16,              SIGNAL(stateChanged(int)),        this, SLOT(checkbox16Clicked(int)));
  QObject::connect(saveColorSchemaButton,   SIGNAL(clicked()),                this, SLOT(saveColorSchemaButtonClicked()));
  QObject::connect(loadColorSchemaButton,   SIGNAL(clicked()),                this, SLOT(loadColorSchemaButtonClicked()));
  QObject::connect(colorSchema_Blue_on_Gray_Button, SIGNAL(clicked()),        this, SLOT(loadColorSchema_blue_gray()));
  QObject::connect(colorSchema_NK_Button,   SIGNAL(clicked()),                this, SLOT(loadColorSchema_NK()));
  QObject::connect(colorSchema_Dark_Button, SIGNAL(clicked()),                this, SLOT(loadColorSchema_Dark()));
  QObject::connect(colorSchema_ECG_Button,  SIGNAL(clicked()),                this, SLOT(loadColorSchema_ECG()));
  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    QObject::connect(AnnotDurationPredefinedLineEdit[i], SIGNAL(textChanged(QString)), this, SLOT(AnnotDurationPredefinedLineEdit_changed()));
  }

  tabholder->addTab(tab1, "Colors");

/////////////////////////////////////// tab 6 Crosshairs ////////////////////////////////////////////////////////////////////////

  tab6 = new QWidget;

  QFormLayout *flayout6_1 = new QFormLayout;
  flayout6_1->setSpacing(20);

  Crh1ColorButton = new SpecialButton;
  Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(Crh1ColorButton);
  hlayout_tmp->addStretch(1000);
  flayout6_1->addRow("First Crosshair color", hlayout_tmp);

  Crh2ColorButton = new SpecialButton;
  Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(Crh2ColorButton);
  hlayout_tmp->addStretch(1000);
  flayout6_1->addRow("Second Crosshair color", hlayout_tmp);

  checkbox6 = new QCheckBox;
  checkbox6->setTristate(false);
  if(mainwindow->maincurve->crosshair_1.has_hor_line)
  {
    checkbox6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox6->setCheckState(Qt::Unchecked);
  }
  checkbox6->setToolTip("Show a horizontal line like a real crosshair");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox6);
  hlayout_tmp->addStretch(1000);
  flayout6_1->addRow("Crosshair horizontal line", hlayout_tmp);
  flayout6_1->labelForField(hlayout_tmp)->setToolTip("Show a horizontal line like a real crosshair");

  spinbox1_1 = new QSpinBox;
  spinbox1_1->setSuffix(" px");
  spinbox1_1->setMinimum(0);
  spinbox1_1->setMaximum(32);
  spinbox1_1->setValue(mainwindow->maincurve->crosshair_1.dot_sz);
  spinbox1_1->setToolTip("Radius of center dot of the crosshairs in pixels, 0 means no dot");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox1_1);
  hlayout_tmp->addStretch(1000);
  flayout6_1->addRow("Crosshair circle", hlayout_tmp);
  flayout6_1->labelForField(hlayout_tmp)->setToolTip("Radius of center dot of the crosshairs in pixels, 0 means no dot");

  QVBoxLayout *vlayout6_1 = new QVBoxLayout;
  vlayout6_1->addLayout(flayout6_1);
  vlayout6_1->addStretch(1000);

  QHBoxLayout *hlayout6_1 = new QHBoxLayout;
  hlayout6_1->addLayout(vlayout6_1);
  hlayout6_1->addStretch(1000);
  tab6->setLayout(hlayout6_1);

  QObject::connect(Crh1ColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh1ColorButtonClicked(SpecialButton *)));
  QObject::connect(Crh2ColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh2ColorButtonClicked(SpecialButton *)));
  QObject::connect(checkbox6,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox6Clicked(int)));
  QObject::connect(spinbox1_1,              SIGNAL(valueChanged(int)),        this, SLOT(spinBox1_1ValueChanged(int)));

  tabholder->addTab(tab6, "Crosshairs");

/////////////////////////////////////// tab 2 Calibration ///////////////////////////////////////////////////////////////////////

  tab2 = new QWidget;

  checkbox2_1 = new QCheckBox("Manually override automatic DPI settings");
  checkbox2_1->setTristate(false);
  if(mainwindow->auto_dpi)
  {
    checkbox2_1->setCheckState(Qt::Unchecked);
  }
  else
  {
    checkbox2_1->setCheckState(Qt::Checked);
  }

  slabel2_1 = new SpecialButton;
  slabel2_1->setMinimumSize(10, 445);
  slabel2_1->setMaximumSize(10, 445);
  slabel2_1->setColor(Qt::black);

  slabel2_3 = new SpecialButton;
  slabel2_3->setMinimumSize(355, 10);
  slabel2_3->setMaximumSize(355, 10);
  slabel2_3->setColor(Qt::black);

  label2_2 = new QLabel("Measure the length of the black\nrectangles and enter the values.");
  if(mainwindow->auto_dpi)
  {
    label2_2->setEnabled(false);
  }

  spinbox2_1 = new QSpinBox;
  spinbox2_1->setSuffix(" mm");
  spinbox2_1->setMinimum(10);
  spinbox2_1->setMaximum(500);
  spinbox2_1->setValue((int)(4450 * mainwindow->y_pixelsizefactor));

  spinbox2_2 = new QSpinBox;
  spinbox2_2->setSuffix(" mm");
  spinbox2_2->setMinimum(10);
  spinbox2_2->setMaximum(500);
  spinbox2_2->setValue((int)(3550 * mainwindow->x_pixelsizefactor));

  ApplyButton = new QPushButton;
  ApplyButton->setText("Apply");

  if(checkbox2_1->checkState() == Qt::Unchecked)
  {
    spinbox2_1->setEnabled(false);
    spinbox2_2->setEnabled(false);
    ApplyButton->setEnabled(false);
  }

  QVBoxLayout *vlayout2_1 = new QVBoxLayout;
  vlayout2_1->addWidget(slabel2_1);
  vlayout2_1->addStretch(1000);

  QHBoxLayout *hlayout2_2 = new QHBoxLayout;
  hlayout2_2->addWidget(label2_2);
  hlayout2_2->addStretch(1000);

  QHBoxLayout *hlayout2_3 = new QHBoxLayout;
  hlayout2_3->addWidget(spinbox2_1);
  hlayout2_3->addStretch(1000);

  QHBoxLayout *hlayout2_4 = new QHBoxLayout;
  hlayout2_4->addWidget(slabel2_3);
  hlayout2_4->addStretch(1000);

  QHBoxLayout *hlayout2_5 = new QHBoxLayout;
  hlayout2_5->addWidget(spinbox2_2);
  hlayout2_5->addStretch(1000);

  QHBoxLayout *hlayout2_6 = new QHBoxLayout;
  hlayout2_6->addWidget(checkbox2_1);
  hlayout2_6->addStretch(1000);

  QHBoxLayout *hlayout2_7 = new QHBoxLayout;
  hlayout2_7->addWidget(ApplyButton);
  hlayout2_7->addStretch(1000);

  QVBoxLayout *vlayout2_2 = new QVBoxLayout;
  vlayout2_2->addStretch(100);
  vlayout2_2->addLayout(hlayout2_2);
  vlayout2_2->addStretch(100);
  vlayout2_2->addLayout(hlayout2_3);
  vlayout2_2->addStretch(200);
  vlayout2_2->addLayout(hlayout2_4);
  vlayout2_2->addLayout(hlayout2_5);
  vlayout2_2->addStretch(200);
  vlayout2_2->addLayout(hlayout2_6);
  vlayout2_2->addStretch(200);
  vlayout2_2->addLayout(hlayout2_7);
  vlayout2_2->addStretch(500);

  QHBoxLayout *hlayout2_1 = new QHBoxLayout;
  hlayout2_1->addSpacing(20);
  hlayout2_1->addLayout(vlayout2_1);
  hlayout2_1->addSpacing(20);
  hlayout2_1->addLayout(vlayout2_2);
  hlayout2_1->addStretch(1000);

  tab2->setLayout(hlayout2_1);

  QObject::connect(ApplyButton, SIGNAL(clicked()),         this, SLOT(ApplyButtonClicked()));
  QObject::connect(checkbox2_1, SIGNAL(stateChanged(int)), this, SLOT(calibrate_checkbox_stateChanged(int)));

  tabholder->addTab(tab2, "Calibration");

/////////////////////////////////////// tab 7 annotation editor ////////////////////////////////////////////////////////////////////////

  tab7 = new QWidget;

  QFormLayout *flayout7_1 = new QFormLayout;
  flayout7_1->setSpacing(20);

  flayout7_1->addRow("User configurable buttons", (QWidget *)NULL);

  for(i=0; i<8; i++)
  {
    checkbox7_1[i] = new QCheckBox;
    checkbox7_1[i]->setTristate(false);
    if(mainwindow->annot_edit_user_button_enabled[i])
    {
      checkbox7_1[i]->setCheckState(Qt::Checked);
    }
    else
    {
      checkbox7_1[i]->setCheckState(Qt::Unchecked);
    }
    checkbox7_1[i]->setToolTip("Enables a button to quickly create a predefined annotation");

    lineedit7_1[i] = new QLineEdit;
    lineedit7_1[i]->setMaxLength(16);
    lineedit7_1[i]->setText(QString::fromUtf8(mainwindow->annot_edit_user_button_name[i]));
    if(checkbox7_1[i]->checkState() != Qt::Checked)
    {
      lineedit7_1[i]->setEnabled(false);
    }
    lineedit7_1[i]->setToolTip("Description of the new annotation");

    checkbox7_8[i] = new QCheckBox("Page middle");
    checkbox7_8[i]->setTristate(false);
    if(mainwindow->annot_editor_user_button_onset_on_page_middle[i])
    {
      checkbox7_8[i]->setCheckState(Qt::Checked);
    }
    else
    {
      checkbox7_8[i]->setCheckState(Qt::Unchecked);
    }
    if(checkbox7_1[i]->checkState() != Qt::Checked)
    {
      checkbox7_8[i]->setEnabled(false);
    }
    checkbox7_8[i]->setToolTip("If enabled, set the onset time at the middle of the page instead of at the start of the page.");

//     hlayout_tmp = new QHBoxLayout;
//     hlayout_tmp->setAlignment(Qt::AlignCenter);
//     hlayout_tmp->addWidget(checkbox7_8[i]);
//     hlayout_tmp->addStretch(1000);
//     flayout7_2->addRow("Onset on page middle", hlayout_tmp);
//     QObject::connect(checkbox7_8[i], SIGNAL(stateChanged(int)), this, SLOT(checkbox7_8[i]Clicked(int)));
//     flayout7_2->labelForField(hlayout_tmp)->setToolTip("If enabled, set the onset time at the middle of the page instead of at the start of the page.");
//     if(!mainwindow->annot_editor_user_button_update_annot_onset)
//     {
//       checkbox7_8[i]->setEnabled(false);
//     }

    hlayout_tmp = new QHBoxLayout;
    hlayout_tmp->setAlignment(Qt::AlignCenter);
    hlayout_tmp->addWidget(checkbox7_1[i]);
    hlayout_tmp->addWidget(lineedit7_1[i]);
    hlayout_tmp->addWidget(checkbox7_8[i]);
    hlayout_tmp->addStretch(1000);
    snprintf(str1_512, 512, "Button %i", i + 1);
    flayout7_1->addRow(str1_512, hlayout_tmp);
    flayout7_1->labelForField(hlayout_tmp)->setToolTip("If enabled, it creates a button to quickly create a predefined annotation");
  }

  flayout7_1->addRow("Keyboard shortcuts are '1', '2', '3', etc.", (QWidget *)NULL);

  flayout7_1->addRow(" ", (QWidget *)NULL);

  QFrame *hline7_1 = new QFrame;
  hline7_1->setFrameShape(QFrame::HLine);
  hline7_1->setFrameShadow(QFrame::Sunken);
  hline7_1->setLineWidth(2);
  flayout7_1->addRow(hline7_1);

  checkbox7_2 = new QCheckBox;
  checkbox7_2->setTristate(false);
  checkbox7_2->setToolTip("Enabling this option will automatically update the onsettime field of the annotation editor\n"
                          "when scrolling/navigating and a cross-hair is active.");
  if(mainwindow->auto_update_annot_onset)
  {
    checkbox7_2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_2->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_2);
  hlayout_tmp->addStretch(1000);
  flayout7_1->addRow("Auto update annotation-editor onsettime", hlayout_tmp);
  QObject::connect(checkbox7_2, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_2Clicked(int)));
  flayout7_1->labelForField(hlayout_tmp)->setToolTip("Enabling this option will automatically update the onsettime field of the annotation editor\n"
                                                     "when scrolling/navigating and a cross-hair is active.");

  checkbox7_9 = new QCheckBox;
  checkbox7_9->setTristate(false);
  checkbox7_9->setToolTip("Enabling this option will set the resolution of the annotation editor to 1 microSecond."
                          "\nIf disabled the resolution is 1 milliSecond (default)"
                          "\nThis affects also the resolution of the crosshairs, the viewtime and pagetime indicators\n"
                          "and the annotation markers.");
  if(mainwindow->annot_editor_highres)
  {
    checkbox7_9->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_9->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_9);
  hlayout_tmp->addStretch(1000);
  flayout7_1->addRow("High time-resolution (uSec.)", hlayout_tmp);
  QObject::connect(checkbox7_9, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_9Clicked(int)));
  flayout7_1->labelForField(hlayout_tmp)->setToolTip("Enabling this option will set the resolution of the annotation editor to 1 microSecond."
                                                     "\nIf disabled the resolution is 1 milliSecond (default)"
                                                     "\nThis affects also the resolution of the crosshairs, the viewtime and pagetime indicators\n"
                                                     "and the annotation markers.");

  QVBoxLayout *vlayout7_1 = new QVBoxLayout;
  vlayout7_1->addLayout(flayout7_1);
  vlayout7_1->addStretch(1000);

  QFormLayout *flayout7_2 = new QFormLayout;
  flayout7_2->setSpacing(20);

  flayout7_2->addRow("When a user button is clicked:", (QWidget *)NULL);

  checkbox7_5 = new QCheckBox;
  checkbox7_5->setTristate(false);
  checkbox7_5->setToolTip("Enabling this option will automatically update the description field of the annotation editor\n"
                          "with the name of the user button when that button is clicked.");
  if(mainwindow->annot_editor_user_button_update_annot_description)
  {
    checkbox7_5->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_5->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_5);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Set annotation editor description", hlayout_tmp);
  QObject::connect(checkbox7_5, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_5Clicked(int)));
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("Enabling this option will automatically update the description field of the annotation editor\n"
                                                     "with the name of the user button when that button is clicked.");

  checkbox7_3 = new QCheckBox;
  checkbox7_3->setTristate(false);
  checkbox7_3->setToolTip("Enabling this option will automatically update the onset time field of the annotation editor\n"
                          "with the current viewtime (file position) when a user button is clicked.");
  if(mainwindow->annot_editor_user_button_update_annot_onset)
  {
    checkbox7_3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_3->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_3);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Set annotation editor onsettime", hlayout_tmp);
  QObject::connect(checkbox7_3, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_3Clicked(int)));
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("Enabling this option will automatically update the onset time field of the annotation-editor\n"
                                                     "with the current viewtime (file position) when a user button is clicked.");

  checkbox7_4 = new QCheckBox;
  checkbox7_4->setTristate(false);
  checkbox7_4->setToolTip("Enabling this option will automatically update the duration field of the annotation-editor\n"
                          "with the stage / epoch length when a user button is clicked.");
  if(mainwindow->annot_editor_user_button_update_annot_duration)
  {
    checkbox7_4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_4->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_4);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Set annotation editor duration", hlayout_tmp);
  QObject::connect(checkbox7_4, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_4Clicked(int)));
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("Enabling this option will automatically update the duration field of the annotation-editor\n"
                                                     "with the stage / epoch length when a user button is clicked.");

  checkbox7_6 = new QCheckBox;
  checkbox7_6->setTristate(false);
  checkbox7_6->setToolTip("Enabling this option will automatically change the viewtime (file position) and jump to the next stage / epoch.");
  if(mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    checkbox7_6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_6->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_6);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Jump to next page", hlayout_tmp);
  QObject::connect(checkbox7_6, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_6Clicked(int)));
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("Enabling this option will automatically change the viewtime (file position) and jump to the next stage / epoch.");

  checkbox7_7 = new QCheckBox;
  checkbox7_7->setTristate(false);
  checkbox7_7->setToolTip("If enabled, the page will always start at an integer multiple of the stage / epoch length.");
  if(mainwindow->annot_editor_user_button_stay_on_epoch_boundary)
  {
    checkbox7_7->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox7_7->setCheckState(Qt::Unchecked);
  }
  if(!mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    checkbox7_7->setEnabled(false);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox7_7);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Viewtime must stay on stage / epoch boundary", hlayout_tmp);
  QObject::connect(checkbox7_7, SIGNAL(stateChanged(int)), this, SLOT(checkbox7_7Clicked(int)));
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("If enabled, the page will always start at an integer multiple of the stage / epoch length.");

  spinbox7_2 = new QSpinBox;
  spinbox7_2->setSuffix(" sec.");
  spinbox7_2->setRange(1, 300);
  spinbox7_2->setValue((int)(mainwindow->annot_editor_user_button_page_len / TIME_FIXP_SCALING));
  if(!mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    spinbox7_2->setEnabled(false);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox7_2);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Page length (timescale)", hlayout_tmp);
  QObject::connect(spinbox7_2, SIGNAL(valueChanged(int)), this, SLOT(spinBox7_2ValueChanged(int)));

  spinbox7_1 = new QSpinBox;
  spinbox7_1->setSuffix(" sec.");
  spinbox7_1->setRange(1, 300);
  spinbox7_1->setValue((int)(mainwindow->annot_editor_user_button_epoch_len / TIME_FIXP_SCALING));
  if(!mainwindow->annot_editor_user_button_jump_to_next_page)
  {
    spinbox7_1->setEnabled(false);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox7_1);
  hlayout_tmp->addStretch(1000);
  flayout7_2->addRow("Stage / epoch length", hlayout_tmp);
  QObject::connect(spinbox7_1, SIGNAL(valueChanged(int)), this, SLOT(spinBox7_1ValueChanged(int)));

  QFrame *hline7_2 = new QFrame;
  hline7_2->setFrameShape(QFrame::HLine);
  hline7_2->setFrameShadow(QFrame::Sunken);
  hline7_2->setLineWidth(2);
  flayout7_2->addRow(hline7_2);

  hlayout_tmp = new QHBoxLayout;
  flayout7_2->addRow("Predefined annotations (pop-up menu)", hlayout_tmp);
  flayout7_2->labelForField(hlayout_tmp)->setToolTip("These are the descriptions of the annotations which will appear and can be\n"
                                                     "selected when drawing a rectangle while pressing the Control or Shift key\n"
                                                     "or when the crosshairs are active and pressing 'a'");

  annot_sidemenu_table = new QTableWidget;
  annot_sidemenu_table->setSelectionMode(QAbstractItemView::NoSelection);
  annot_sidemenu_table->setColumnCount(1);
  annot_sidemenu_table->setRowCount(MAX_ANNOTEDIT_SIDE_MENU_ANNOTS);
  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    annot_sidemenu_table->setCellWidget(i, 0, new QLineEdit);
    ((QLineEdit *)annot_sidemenu_table->cellWidget(i, 0))->setMaxLength(16);
    ((QLineEdit *)annot_sidemenu_table->cellWidget(i, 0))->setText(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[i]));
  }
  QStringList horizontal_labels;
  horizontal_labels += "Annotation / Event";
  annot_sidemenu_table->setHorizontalHeaderLabels(horizontal_labels);
  annot_sidemenu_table->resizeColumnsToContents();

  QVBoxLayout *vlayout7_2 = new QVBoxLayout;
  vlayout7_2->addLayout(flayout7_2);
  vlayout7_2->addWidget(annot_sidemenu_table, 1000);
  annot_sidemenu_table->setToolTip("These are the descriptions of the annotations which will appear and\n"
                                   "can be selected when drawing a rectangle while pressing the Ctrl key\n"
                                   "or when the crosshairs are active and pressing 'a'");

  QFrame *vline7_1 = new QFrame;
  vline7_1->setFrameShape(QFrame::VLine);
  vline7_1->setFrameShadow(QFrame::Sunken);
  vline7_1->setLineWidth(2);

  QHBoxLayout *hlayout7_1 = new QHBoxLayout;
  hlayout7_1->addLayout(vlayout7_1);
  hlayout7_1->addWidget(vline7_1);
  hlayout7_1->addLayout(vlayout7_2);
  hlayout7_1->addStretch(1000);
  tab7->setLayout(hlayout7_1);

  for(i=0; i<8; i++)
  {
    QObject::connect(checkbox7_1[i], SIGNAL(stateChanged(int)),   this, SLOT(tab7_settings_changed()));
    QObject::connect(lineedit7_1[i], SIGNAL(textEdited(QString)), this, SLOT(tab7_settings_changed()));
    QObject::connect(checkbox7_8[i], SIGNAL(stateChanged(int)),   this, SLOT(tab7_settings_changed()));
  }

  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    QObject::connect((QLineEdit *)annot_sidemenu_table->cellWidget(i, 0), SIGNAL(textChanged(QString)), this, SLOT(tab7_settings_changed()));
  }

  tabholder->addTab(tab7, "Annotation editor");

/////////////////////////////////////// tab 3 Powerspectrum ///////////////////////////////////////////////////////////////////////

  tab3 = new QWidget;

  QLabel *label3_1 = new QLabel("Frequency regions of the colorbars:");

  colorBarTable = new QTableWidget;
  colorBarTable->setSelectionMode(QAbstractItemView::NoSelection);
  colorBarTable->setColumnCount(4);
  colorBarTable->setRowCount(MAXSPECTRUMMARKERS);
  for(i=0; i < MAXSPECTRUMMARKERS; i++)
  {
    colorBarTable->setCellWidget(i, 0, new QCheckBox);
   ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setTristate(false);
    if(i < mainwindow->spectrum_colorbar->items)
    {
      ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }
    else
    {
      ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
    QObject::connect(colorBarTable->cellWidget(i, 0), SIGNAL(stateChanged(int)), this, SLOT(checkBoxChanged(int)));

    colorBarTable->setCellWidget(i, 1, new QDoubleSpinBox);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setDecimals(3);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setSuffix(" Hz");
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setRange(0.001, 100000.0);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setValue(mainwindow->spectrum_colorbar->freq[i]);
    QObject::connect((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)), SIGNAL(valueChanged(double)), this, SLOT(spinBoxValueChanged(double)));

    colorBarTable->setCellWidget(i, 2, new SpecialButton);
    ((SpecialButton *)(colorBarTable->cellWidget(i, 2)))->setGlobalColor(mainwindow->spectrum_colorbar->color[i]);
    QObject::connect((SpecialButton *)(colorBarTable->cellWidget(i, 2)), SIGNAL(clicked(SpecialButton *)), this, SLOT(colorBarButtonClicked(SpecialButton *)));

    colorBarTable->setCellWidget(i, 3, new QLineEdit);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->setText(mainwindow->spectrum_colorbar->label[i]);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->setMaxLength(16);
    QObject::connect((QLineEdit *)(colorBarTable->cellWidget(i, 3)), SIGNAL(textEdited(const QString  &)), this, SLOT(labelEdited(const QString  &)));
  }

  QStringList horizontallabels;
  horizontallabels += "";
  horizontallabels += "Frequency";
  horizontallabels += "Color";
  horizontallabels += "Label";
  colorBarTable->setHorizontalHeaderLabels(horizontallabels);

  colorBarTable->resizeColumnsToContents();

  QLabel *label3_2 = new QLabel("Height of colorbars are relative to the");

  radiobutton5_1 = new QRadioButton;
  radiobutton5_1->setText("sum");
  if(mainwindow->spectrum_colorbar->method == 0)
  {
    radiobutton5_1->setChecked(true);  // sum
  }

  radiobutton5_2 = new QRadioButton;
  radiobutton5_2->setText("peak");
  if(mainwindow->spectrum_colorbar->method == 1)
  {
    radiobutton5_2->setChecked(true);  // peak
  }

  radiobutton5_3 = new QRadioButton;
  radiobutton5_3->setText("average");
  if(mainwindow->spectrum_colorbar->method == 2)
  {
    radiobutton5_3->setChecked(true);  // average
  }

  QLabel *label3_3 = new QLabel("of the power in the colorbar region.");

  QFormLayout *flayout3_1 = new QFormLayout;
  flayout3_1->setSpacing(20);

  dspinbox3_2 = new QDoubleSpinBox;
  dspinbox3_2->setMinimum(0.0001);
  dspinbox3_2->setMaximum(100000.0);
  dspinbox3_2->setValue(mainwindow->spectrum_colorbar->max_colorbar_value);
  checkbox3_1 = new QCheckBox("Auto");
  checkbox3_1->setTristate(false);
  if(mainwindow->spectrum_colorbar->auto_adjust)
  {
    checkbox3_1->setCheckState(Qt::Checked);

    dspinbox3_2->setEnabled(false);
  }
  else
  {
    checkbox3_1->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(dspinbox3_2);
  hlayout_tmp->addSpacing(20);
  hlayout_tmp->addWidget(checkbox3_1);
  hlayout_tmp->addStretch(1000);
  flayout3_1->addRow("Colorbar sensitivity:", hlayout_tmp);

  QHBoxLayout *hlayout3_3 = new QHBoxLayout;
  hlayout3_3->addWidget(label3_2);
  hlayout3_3->addStretch(1000);

  QHBoxLayout *hlayout3_4 = new QHBoxLayout;
  hlayout3_4->addWidget(radiobutton5_1);
  hlayout3_4->addStretch(1000);

  QHBoxLayout *hlayout3_5 = new QHBoxLayout;
  hlayout3_5->addWidget(radiobutton5_2);
  hlayout3_5->addStretch(1000);

  QHBoxLayout *hlayout3_6 = new QHBoxLayout;
  hlayout3_6->addWidget(radiobutton5_3);
  hlayout3_6->addStretch(1000);

  QHBoxLayout *hlayout3_7 = new QHBoxLayout;
  hlayout3_7->addWidget(label3_3);
  hlayout3_7->addStretch(1000);

  DefaultButton2 = new QPushButton;
  DefaultButton2->setText("Restore default");

  ApplyButton2 = new QPushButton;
  ApplyButton2->setText("Apply");
  ApplyButton2->setEnabled(false);

  QHBoxLayout *hlayout3_2 = new QHBoxLayout;
  hlayout3_2->addWidget(ApplyButton2);
  hlayout3_2->addSpacing(20);
  hlayout3_2->addStretch(500);
  hlayout3_2->addWidget(DefaultButton2);
  hlayout3_2->addStretch(500);

  QVBoxLayout *vlayout3_1 = new QVBoxLayout;
  vlayout3_1->addWidget(label3_1);
  vlayout3_1->addWidget(colorBarTable, 1000);

  QVBoxLayout *vlayout3_2 = new QVBoxLayout;
  vlayout3_2->addSpacing(40);
  vlayout3_2->addLayout(hlayout3_3);
  vlayout3_2->addLayout(hlayout3_4);
  vlayout3_2->addLayout(hlayout3_5);
  vlayout3_2->addLayout(hlayout3_6);
  vlayout3_2->addLayout(hlayout3_7);
  vlayout3_2->addSpacing(40);
  vlayout3_2->addLayout(flayout3_1);
  vlayout3_2->addStretch(1000);
  vlayout3_2->addLayout(hlayout3_2);

  QHBoxLayout *hlayout3_1 = new QHBoxLayout;
  hlayout3_1->addLayout(vlayout3_1, 1000);
  hlayout3_1->addSpacing(20);
  hlayout3_1->addLayout(vlayout3_2);

  tab3->setLayout(hlayout3_1);

  QObject::connect(radiobutton5_1, SIGNAL(toggled(bool)),        this, SLOT(radioButton5Toggled(bool)));
  QObject::connect(radiobutton5_2, SIGNAL(toggled(bool)),        this, SLOT(radioButton5Toggled(bool)));
  QObject::connect(radiobutton5_3, SIGNAL(toggled(bool)),        this, SLOT(radioButton5Toggled(bool)));
  QObject::connect(dspinbox3_2,    SIGNAL(valueChanged(double)), this, SLOT(dspinBox3_2ValueChanged(double)));
  QObject::connect(ApplyButton2,   SIGNAL(clicked()),            this, SLOT(ApplyButton2Clicked()));
  QObject::connect(DefaultButton2, SIGNAL(clicked()),            this, SLOT(DefaultButton2Clicked()));
  QObject::connect(checkbox3_1,    SIGNAL(stateChanged(int)),    this, SLOT(checkbox3_1Clicked(int)));

  tabholder->addTab(tab3, "Power Spectrum");

/////////////////////////////////////// tab 4 Other ///////////////////////////////////////////////////////////////////////

  tab4 = new QWidget;

  QFormLayout *flayout4_1 = new QFormLayout;
  flayout4_1->setSpacing(20);

  checkbox4_1 = new QCheckBox;
  checkbox4_1->setTristate(false);
  if(mainwindow->auto_reload_mtg)
  {
    checkbox4_1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_1->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_1);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Reload last used montage", hlayout_tmp);
  QObject::connect(checkbox4_1, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_1Clicked(int)));

  spinbox4_3 = new QSpinBox;
  spinbox4_3->setSuffix(" MB");
  spinbox4_3->setMinimum(100);
  spinbox4_3->setMaximum(100000);
  spinbox4_3->setSingleStep(1);
  spinbox4_3->setValue((int)(mainwindow->maxfilesize_to_readin_annotations / 1048576LL));
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox4_3);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Do not read annotations, Biosemi Status signal\n"
                    "or Nihon Kohden triggers when filesize\n"
                    "is more than:", hlayout_tmp);
  QObject::connect(spinbox4_3, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_3ValueChanged(int)));

  checkbox4_2 = new QCheckBox;
  checkbox4_2->setTristate(false);
  if(mainwindow->read_biosemi_status_signal)
  {
    checkbox4_2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_2->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_2);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Read Biosemi Status signal", hlayout_tmp);
  QObject::connect(checkbox4_2, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_2Clicked(int)));

  checkbox4_3 = new QCheckBox;
  checkbox4_3->setTristate(false);
  if(mainwindow->read_nk_trigger_signal)
  {
    checkbox4_3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_3->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_3);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Read Nihon Kohden Trigger/Marker signal", hlayout_tmp);
  QObject::connect(checkbox4_3, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_3Clicked(int)));

  spinbox4_1 = new QSpinBox;
  spinbox4_1->setSuffix(" mSec");
  spinbox4_1->setMinimum(100);
  spinbox4_1->setMaximum(3000);
  spinbox4_1->setSingleStep(1);
  spinbox4_1->setValue(mainwindow->live_stream_update_interval);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox4_1);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("livestream update interval", hlayout_tmp);
  QObject::connect(spinbox4_1, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_1ValueChanged(int)));

  combobox4_1 = new QComboBox;
  combobox4_1->addItem("50 Hz");
  combobox4_1->addItem("60 Hz");
  if(mainwindow->powerlinefreq == 50)
  {
    combobox4_1->setCurrentIndex(0);
  }
  if(mainwindow->powerlinefreq == 60)
  {
    combobox4_1->setCurrentIndex(1);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(combobox4_1);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Powerline Frequency", hlayout_tmp);
  QObject::connect(combobox4_1, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_1IndexChanged(int)));

  spinbox4_2 = new QSpinBox;
  spinbox4_2->setPrefix("Timescale / ");
  spinbox4_2->setMinimum(0);
  spinbox4_2->setMaximum(100);
  spinbox4_2->setSingleStep(1);
  spinbox4_2->setValue(mainwindow->mousewheelsens);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox4_2);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Mousewheel stepsize\n"
                     "(0 is no scroll)", hlayout_tmp);
  QObject::connect(spinbox4_2, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_2ValueChanged(int)));

  checkbox4_4 = new QCheckBox;
  checkbox4_4->setTristate(false);
  if(mainwindow->use_threads)
  {
    checkbox4_4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_4->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_4);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Use Multi-Threading", hlayout_tmp);
  QObject::connect(checkbox4_4, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_4Clicked(int)));
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("Use all available CPU cores to render the signal waveforms on the screen");
  checkbox4_4->setToolTip("Use all available CPU cores to render the signal waveforms on the screen");

  checkbox4_14 = new QCheckBox;
  checkbox4_14->setTristate(false);
  if(mainwindow->session_relative_paths)
  {
    checkbox4_14->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_14->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_14);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Use relative paths when saving a session", hlayout_tmp);
  QObject::connect(checkbox4_14, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_14Clicked(int)));
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("If checked, use relative paths for the EDF files instead of absolute paths when storing the session");
  checkbox4_14->setToolTip("If checked, use relative paths for the EDF files instead of absolute paths when storing the session");

  checkbox4_11 = new QCheckBox;
  checkbox4_11->setTristate(false);
  if(mainwindow->edf_debug)
  {
    checkbox4_11->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_11->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_11);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Show EDF debug info", hlayout_tmp);
  QObject::connect(checkbox4_11, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_11Clicked(int)));
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("Show the file offset of objects like header fields, annotations, etc.\n"
                                                     "Useful for developers");
  checkbox4_11->setToolTip("Show the file offset of objects like header fields, annotations, etc.\n"
                            "Useful for developers");

  checkbox4_15 = new QCheckBox;
  checkbox4_15->setTristate(false);
  if(mainwindow->strip_label_types)
  {
    checkbox4_15->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_15->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_15);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Strip label types", hlayout_tmp);
  QObject::connect(checkbox4_15, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_15Clicked(int)));
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("If enabled, the type prefix of a signal will be stripped from the label e.g. EEG FP1 becomes FP1, ECG V2 becomes V2\n"
                                                     "Remove and re-load the signals in order to take the changes into effect.");
  checkbox4_15->setToolTip("If enabled, the type prefix of a signal will be stripped from the label e.g. EEG FP1 becomes FP1, ECG V2 becomes V2\n"
                           "Remove and re-load the signals in order to take the changes into effect.");

  checkbox4_16 = new QCheckBox;
  checkbox4_16->setTristate(false);
  checkbox4_16->setToolTip("If enabled, max. N signals will be visible at a time, the other signals can be reached using a vertical scrollbar");
  if(mainwindow->mc_v_scrollarea_auto)
  {
    checkbox4_16->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_16->setCheckState(Qt::Unchecked);
  }
  spinbox4_6 = new QSpinBox;
  spinbox4_6->setMinimum(1);
  spinbox4_6->setMaximum(64);
  spinbox4_6->setSingleStep(1);
  spinbox4_6->setSuffix(" signals");
  spinbox4_6->setValue(mainwindow->mc_v_scrollarea_max_signals);
  spinbox4_6->setToolTip("If enabled, max. N signals will be visible at a time, the other signals can be reached using a vertical scrollbar");
  if(mainwindow->mc_v_scrollarea_auto)
  {
    spinbox4_6->setEnabled(true);
  }
  else
  {
    spinbox4_6->setEnabled(false);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_16);
  hlayout_tmp->addWidget(spinbox4_6);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Enable a vertical scrollbar and show max.", hlayout_tmp);
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("If enabled, max. N signals will be visible at a time, the other signals can be reached using a vertical scrollbar");
  QObject::connect(checkbox4_16, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_16Clicked(int)));
  QObject::connect(spinbox4_6, SIGNAL(valueChanged(int)), this, SLOT(spinbox4_6ValueChanged(int)));

  checkbox4_18 = new QCheckBox;
  checkbox4_18->setTristate(false);
  checkbox4_18->setToolTip("When adding signals to display in the signals dialog and the signal label starts with \"EEG \", invert the plotting of the signal");
  if(mainwindow->default_invert_eeg_signals)
  {
    checkbox4_18->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_18->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_18);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Invert EEG signals", hlayout_tmp);
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("When adding signals to display in the signals dialog and the signal label starts with \"EEG \", invert the plotting of the signal");
  QObject::connect(checkbox4_18, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_18Clicked(int)));

  spinbox4_7 = new QSpinBox;
  spinbox4_7->setMinimum(0);
  spinbox4_7->setMaximum(86400);
  spinbox4_7->setSuffix(" Sec/page");
  spinbox4_7->setToolTip("Sets the default timescale when opening a file (if zero, show whole recording)");
  spinbox4_7->setValue(mainwindow->default_time_scale);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox4_7);
  hlayout_tmp->addStretch(1000);
  flayout4_1->addRow("Default Timescale", hlayout_tmp);
  flayout4_1->labelForField(hlayout_tmp)->setToolTip("Sets the default timescale when opening a file (if zero, show whole recording)");
  QObject::connect(spinbox4_7, SIGNAL(valueChanged(int)), this, SLOT(spinbox4_7ValueChanged(int)));

  QFormLayout *flayout4_2 = new QFormLayout;
  flayout4_2->setSpacing(20);

  checkbox4_5 = new QCheckBox;
  checkbox4_5->setTristate(false);
  if(mainwindow->check_for_updates)
  {
    checkbox4_5->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_5->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_5);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Check for updates during startup", hlayout_tmp);
  QObject::connect(checkbox4_5, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_5Clicked(int)));

  checkbox4_13 = new QCheckBox;
  checkbox4_13->setTristate(false);
  if(mainwindow->rc_host_server_public)
  {
    checkbox4_13->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_13->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_13);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Set remote control port public", hlayout_tmp);
  QObject::connect(checkbox4_13, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_13Clicked(int)));
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("If enabled, and EDFbrowser is started with the option --rc-host-port,\n"
                                                     "the remote control port will be publicly accessible.\n");
  checkbox4_13->setToolTip("If enabled, and EDFbrowser is started with the option --rc-host-port,\n"
                           "the remote control port will be publicly accessible.\n");

  combobox4_2 = new QComboBox;
  combobox4_2->addItem("relative");
  combobox4_2->addItem("real (relative)");
  combobox4_2->addItem("date real (relative)");
  combobox4_2->setCurrentIndex(mainwindow->viewtime_indicator_type);
  combobox4_2->setToolTip("Formatting of the viewtime/fileposition indicator");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(combobox4_2);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Viewtime / fileposition indicator", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("Formatting of the viewtime/fileposition indicator");
  QObject::connect(combobox4_2, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_2IndexChanged(int)));

  checkbox4_7 = new QCheckBox;
  checkbox4_7->setTristate(false);
  if(mainwindow->display_pagetime_mmsec)
  {
    checkbox4_7->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_7->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_7);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Pagetime: show mm/sec.", hlayout_tmp);
  QObject::connect(checkbox4_7, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_7Clicked(int)));

  combobox4_3 = new QComboBox;
  combobox4_3->addItem("Subject info");
  combobox4_3->addItem("Filename");
  combobox4_3->addItem("Filename with full path");
  combobox4_3->setCurrentIndex(mainwindow->mainwindow_title_type);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(combobox4_3);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Window title content", hlayout_tmp);
  QObject::connect(combobox4_3, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_3IndexChanged(int)));

  QRadioButton *def_amp_radio_button0 = new QRadioButton;
  QRadioButton *def_amp_radio_button1 = new QRadioButton;
  QRadioButton *def_amp_radio_button2 = new QRadioButton;
  def_amp_radio_group = new QButtonGroup(this);
  def_amp_radio_group->addButton(def_amp_radio_button0, 0);
  def_amp_radio_group->addButton(def_amp_radio_button1, 1);
  def_amp_radio_group->addButton(def_amp_radio_button2, 2);
#if QT_VERSION < 0x060000
  QObject::connect(def_amp_radio_group, SIGNAL(buttonClicked(int)), this, SLOT(def_amp_radio_group_clicked(int)));
#else
  QObject::connect(def_amp_radio_group, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(def_amp_radio_group_clicked(QAbstractButton *)));
#endif
  dspinbox4_4 = new QDoubleSpinBox;
  dspinbox4_4->setMinimum(0.001);
  dspinbox4_4->setMaximum(10000000);
  dspinbox4_4->setSuffix(" /cm");
  dspinbox4_4->setValue(mainwindow->default_amplitude);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(def_amp_radio_button0);
  hlayout_tmp->addWidget(dspinbox4_4);
  hlayout_tmp->addStretch(1000);
  vlayout_tmp = new QVBoxLayout;
  vlayout_tmp->setAlignment(Qt::AlignCenter);
  vlayout_tmp->addLayout(hlayout_tmp);

  spinbox4_5 = new QSpinBox;
  spinbox4_5->setMinimum(1);
  spinbox4_5->setMaximum(100);
  spinbox4_5->setPrefix("PhysMax / ");
  spinbox4_5->setSuffix(" /cm");
  spinbox4_5->setValue(mainwindow->default_amplitude_physmax_div);
  spinbox4_5->setToolTip("If selected, the default amplitude (units/cm) will be the physical maximum (as set in the EDF header) divided by this value");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(def_amp_radio_button1);
  hlayout_tmp->addWidget(spinbox4_5);
  hlayout_tmp->addStretch(1000);
  vlayout_tmp->addLayout(hlayout_tmp);

  label_4_1 = new QLabel;
  label_4_1->setText("Fit signals to pane");
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(def_amp_radio_button2);
  hlayout_tmp->addWidget(label_4_1);
  hlayout_tmp->addStretch(1000);
  vlayout_tmp->addLayout(hlayout_tmp);

  if(mainwindow->default_fit_signals_to_pane)
  {
    def_amp_radio_button0->setChecked(false);
    def_amp_radio_button1->setChecked(false);
    def_amp_radio_button2->setChecked(true);
    dspinbox4_4->setEnabled(false);
    spinbox4_5->setEnabled(false);
  }
  else if(mainwindow->default_amplitude_use_physmax_div)
    {
      def_amp_radio_button0->setChecked(false);
      def_amp_radio_button2->setChecked(false);
      def_amp_radio_button1->setChecked(true);
      dspinbox4_4->setEnabled(false);
      label_4_1->setEnabled(false);
    }
    else
    {
      def_amp_radio_button1->setChecked(false);
      def_amp_radio_button2->setChecked(false);
      def_amp_radio_button0->setChecked(true);
      spinbox4_5->setEnabled(false);
      label_4_1->setEnabled(false);
    }

  flayout4_2->addRow("Default amplitude", vlayout_tmp);
  flayout4_2->labelForField(vlayout_tmp)->setToolTip("Default vertical scale when opening a file");
  QObject::connect(dspinbox4_4, SIGNAL(valueChanged(double)), this, SLOT(dspinbox4_4ValueChanged(double)));
  QObject::connect(spinbox4_5,  SIGNAL(valueChanged(int)),    this, SLOT(spinbox4_5ValueChanged(int)));

  checkbox4_6 = new QCheckBox;
  checkbox4_6->setTristate(false);
  checkbox4_6->setToolTip("Enabling this option will avoid the \"stairstep\" effect and will make the signal look smoother.");
  if(mainwindow->linear_interpol)
  {
    checkbox4_6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_6->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_6);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Use linear interpolation for plotting", hlayout_tmp);
  QObject::connect(checkbox4_6, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_6Clicked(int)));
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("Draw a straight line between samplepoints (smoothing),\n"
                                                     "this avoids the stairstep effect when zooming in and/or at low samplerates");
  checkbox4_6->setToolTip("Draw a straight line between samplepoints (smoothing),\n"
                          "this avoids the stairstep effect when zooming in and/or at low samplerates");

  lineedit4_1 = new QLineEdit;
  lineedit4_1->setMaxLength(31);
  lineedit4_1->setText(QString::fromUtf8(mainwindow->ecg_qrs_rpeak_descr));
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(lineedit4_1);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("R-peak description string", hlayout_tmp);
  QObject::connect(lineedit4_1, SIGNAL(textEdited(const QString)), this, SLOT(lineedit4_1_changed(const QString)));

  checkbox4_8 = new QCheckBox;
  checkbox4_8->setTristate(false);
  checkbox4_8->setToolTip("If checked, the signal's name will be concatenated to the R-peak description,\n"
                          "e.g.: R-peak V2");
  if(mainwindow->use_signallabel_in_annot_descr)
  {
    checkbox4_8->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_8->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_8);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Add signallabel to R-peak description", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("If checked, the signal's name will be concatenated to the R-peak description,\n"
                                                     "e.g.: R-peak V2");
  QObject::connect(checkbox4_8, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_8Clicked(int)));

  checkbox4_9 = new QCheckBox;
  checkbox4_9->setTristate(false);
  checkbox4_9->setToolTip("If checked, the ruler will adjust it's width in order to show integer numbers for Hz");
  if(mainwindow->maincurve->floating_ruler_use_var_width)
  {
    checkbox4_9->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_9->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_9);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Floating ruler use variable width", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("If checked, the ruler will adjust the width in order to show integer numbers for Hz");
  QObject::connect(checkbox4_9, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_9Clicked(int)));

  checkbox4_10 = new QCheckBox;
  checkbox4_10->setTristate(false);
  checkbox4_10->setToolTip("If checked, when clicking on an annotation in the annotation list, the\n"
                           "file position will be set to the onset time of that annotation\n"
                           "(the annotation marker will appear at the start of the page).\n"
                           "If not checked, the annotation marker will appear in the middle of the page.");
  if(mainwindow->annot_onset_at_start_of_page_on_jump)
  {
    checkbox4_10->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_10->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_10);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Annotation onset at start of page", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("If checked, when clicking on an annotation in the annotation list, the\n"
                                                     "file position will be set to the onset time of that annotation\n"
                                                     "(the annotation marker will appear at the start of the page).\n"
                                                     "If not checked, the annotation marker will appear in the middle of the page.");
  QObject::connect(checkbox4_10, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_10Clicked(int)));

  checkbox4_12 = new QCheckBox;
  checkbox4_12->setTristate(false);
  checkbox4_12->setToolTip("Annotation filter affects the annotation list only, not the annotation markers in the signal window");
  if(mainwindow->annot_filter->hide_in_list_only)
  {
    checkbox4_12->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_12->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_12);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Annotations: filter list only", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("Annotation filter affects the annotation list only, not the annotation markers in the signal window");
  QObject::connect(checkbox4_12, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_12Clicked(int)));

  checkbox4_17 = new QCheckBox;
  checkbox4_17->setTristate(false);
  checkbox4_17->setToolTip("If enabled annotationlist will scroll to items visible on the page when browsing");
  if(mainwindow->annotlist_scrolltoitem_while_browsing)
  {
    checkbox4_17->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_17->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(checkbox4_17);
  hlayout_tmp->addStretch(1000);
  flayout4_2->addRow("Annotationlist auto scroll", hlayout_tmp);
  flayout4_2->labelForField(hlayout_tmp)->setToolTip("If enabled annotationlist will scroll to items visible on the page when browsing");
  QObject::connect(checkbox4_17, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_17Clicked(int)));

  QFrame *frame3 = new QFrame;
  frame3->setFrameStyle(QFrame::VLine | QFrame::Plain);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addLayout(flayout4_1);
  hlayout_tmp->addSpacing(20);
  hlayout_tmp->addWidget(frame3);
  hlayout_tmp->addSpacing(20);
  hlayout_tmp->addLayout(flayout4_2);
  hlayout_tmp->addStretch(1000);

  vlayout_tmp = new QVBoxLayout;
  vlayout_tmp->addLayout(hlayout_tmp);
  vlayout_tmp->addStretch(1000);

  tab4->setLayout(vlayout_tmp);

  tabholder->addTab(tab4, "Other");

/////////////////////////////////////// tab 5 Font ///////////////////////////////////////////////////////////////////////

  tab5 = new QWidget;

  QFormLayout *flayout5_1 = new QFormLayout;
  flayout5_1->setSpacing(40);

  spinbox5_1 = new QSpinBox;
  spinbox5_1->setRange(8, 24);
  spinbox5_1->setValue(mainwindow->font_size);
  spinbox5_1->setMinimumWidth(50 * mainwindow->w_scaling);
  textEdit5_1 = new QTextEdit;
  textEdit5_1->setFont(*mainwindow->normfont);
  textEdit5_1->setPlainText(font_sz_example_txt);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox5_1);
  hlayout_tmp->setAlignment(spinbox5_1, Qt::AlignTop);
  hlayout_tmp->addWidget(textEdit5_1, 500);
  hlayout_tmp->setAlignment(textEdit5_1, Qt::AlignTop);
  hlayout_tmp->addStretch(100);
  flayout5_1->addRow("Font size", hlayout_tmp);
  ((QLabel *)(flayout5_1->labelForField(hlayout_tmp)))->setAlignment(Qt::AlignTop);

  spinbox5_2 = new QSpinBox;
  spinbox5_2->setRange(8, 24);
  spinbox5_2->setValue(mainwindow->monofont_size);
  spinbox5_2->setMinimumWidth(50 * mainwindow->w_scaling);
  textEdit5_2 = new QTextEdit;
  textEdit5_2->setFont(*mainwindow->monofont);
  textEdit5_2->setPlainText(font_sz_example_txt);
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(spinbox5_2);
  hlayout_tmp->setAlignment(spinbox5_2, Qt::AlignTop);
  hlayout_tmp->addWidget(textEdit5_2, 500);
  hlayout_tmp->setAlignment(textEdit5_2, Qt::AlignTop);
  hlayout_tmp->addStretch(100);
  flayout5_1->addRow("Monofont size", hlayout_tmp);
  ((QLabel *)(flayout5_1->labelForField(hlayout_tmp)))->setAlignment(Qt::AlignTop);

  DefaultButton5 = new QPushButton;
#if QT_VERSION >= 0x050200
  DefaultButton5->setText("System default");
#else
  DefaultButton5->setText("Default");
#endif
  ApplyButton5 = new QPushButton;
  ApplyButton5->setText("Apply");
  ApplyButton5->setEnabled(false);

  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(ApplyButton5);
  hlayout_tmp->addStretch(500);
  hlayout_tmp->addWidget(DefaultButton5);
  hlayout_tmp->addStretch(1000);

  vlayout_tmp = new QVBoxLayout;
  vlayout_tmp->addSpacing(40);
  vlayout_tmp->addLayout(flayout5_1);
  vlayout_tmp->addSpacing(40);
  vlayout_tmp->addLayout(hlayout_tmp);
  vlayout_tmp->addStretch(1000);

  tab5->setLayout(vlayout_tmp);

  tabholder->addTab(tab5, "Font");

  QObject::connect(spinbox5_1,     SIGNAL(valueChanged(int)), this, SLOT(spinBox5_1ValueChanged(int)));
  QObject::connect(spinbox5_2,     SIGNAL(valueChanged(int)), this, SLOT(spinBox5_2ValueChanged(int)));
  QObject::connect(ApplyButton5,   SIGNAL(clicked()),         this, SLOT(ApplyButton5Clicked()));
  QObject::connect(DefaultButton5, SIGNAL(clicked()),         this, SLOT(DefaultButton5Clicked()));

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  QHBoxLayout *horLayout = new QHBoxLayout;
  horLayout->addStretch(1000);
  horLayout->addWidget(CloseButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabholder);
  mainLayout->addSpacing(20);
  mainLayout->addLayout(horLayout);

  optionsdialog->setMinimumSize(900 * mainwindow->w_scaling, 700 * mainwindow->h_scaling);

  optionsdialog->setLayout(mainLayout);

  tabholder->setCurrentIndex(mainwindow->options_dialog_idx);

  QObject::connect(tabholder,   SIGNAL(currentChanged(int)), this,          SLOT(tabholder_idx_changed(int)));
  QObject::connect(CloseButton, SIGNAL(clicked()),           optionsdialog, SLOT(close()));

  optionsdialog->exec();
}


void UI_OptionsDialog::spinBox1_1ValueChanged(int val)
{
  mainwindow->maincurve->crosshair_1.dot_sz = val;
  mainwindow->maincurve->crosshair_2.dot_sz = val;

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::spinBox4_3ValueChanged(int filesize)
{
  mainwindow->maxfilesize_to_readin_annotations = (long long)filesize * 1048576LL;
}


void UI_OptionsDialog::spinBox4_2ValueChanged(int stepsize)
{
  mainwindow->mousewheelsens = stepsize;
}


void UI_OptionsDialog::combobox4_1IndexChanged(int index)
{
  if(index == 0)
  {
    mainwindow->powerlinefreq = 50;
  }

  if(index == 1)
  {
    mainwindow->powerlinefreq = 60;
  }
}


void UI_OptionsDialog::combobox4_2IndexChanged(int index)
{
  mainwindow->viewtime_indicator_type = index;

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::combobox4_3IndexChanged(int index)
{
  mainwindow->mainwindow_title_type = index;

  mainwindow->setMainwindowTitle(mainwindow->edfheaderlist[mainwindow->sel_viewtime]);
}


void UI_OptionsDialog::spinBox4_1ValueChanged(int interval)
{
  mainwindow->live_stream_update_interval = interval;
}


void UI_OptionsDialog::calibrate_checkbox_stateChanged(int state)
{
  if(state == Qt::Checked)
  {
    spinbox2_1->setEnabled(true);
    spinbox2_2->setEnabled(true);
    ApplyButton->setEnabled(true);
    mainwindow->auto_dpi = 0;
    label2_2->setEnabled(true);
  }
  else
  {
    spinbox2_1->setEnabled(false);
    spinbox2_2->setEnabled(false);
    ApplyButton->setEnabled(false);
    mainwindow->auto_dpi = 1;
    mainwindow->y_pixelsizefactor = 2.54 / mainwindow->dpiy;
    mainwindow->x_pixelsizefactor = 2.54 / mainwindow->dpix;
    label2_2->setEnabled(false);

    mainwindow->maincurve->drawCurve_stage_1();
  }
}


void UI_OptionsDialog::ApplyButtonClicked()
{
  int i;

  mainwindow->y_pixelsizefactor = spinbox2_1->value() / 4450.0;
  mainwindow->x_pixelsizefactor = spinbox2_2->value() / 3550.0;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->sensitivity = mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].bitvalue
     / ((double)mainwindow->signalcomp[i]->voltpercm
     * mainwindow->y_pixelsizefactor);
  }

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::labelEdited(const QString  &)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::dspinBox3_2ValueChanged(double)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::radioButton5Toggled(bool)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::spinBoxValueChanged(double)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::ApplyButton2Clicked()
{
  int i, row;

  char str1_256[256]={""};

  for(row = 1; row < MAXSPECTRUMMARKERS; row++)
  {
    if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
    {
      if(((QDoubleSpinBox *)(colorBarTable->cellWidget(row - 1, 1)))->value() >= ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value())
      {
        snprintf(str1_256, 256, "Row %i must have a higher frequency than row %i", row + 1, row);

        QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_256);
        messagewindow.exec();

        return;
      }
    }
    else
    {
      break;
    }
  }

  for(row = 0; row < MAXSPECTRUMMARKERS; row++)
  {
    if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
    {
      mainwindow->spectrum_colorbar->freq[row] = ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value();
      mainwindow->spectrum_colorbar->color[row] = ((SpecialButton *)(colorBarTable->cellWidget(row, 2)))->globalColor();
      strncpy(mainwindow->spectrum_colorbar->label[row], ((QLineEdit *)(colorBarTable->cellWidget(row, 3)))->text().toLatin1().data(), 16);
      mainwindow->spectrum_colorbar->label[row][16] = 0;
    }
    else
    {
      break;
    }
  }

  mainwindow->spectrum_colorbar->items = row;

  for(; row < MAXSPECTRUMMARKERS; row++)
  {
    mainwindow->spectrum_colorbar->freq[row] = ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value();
    mainwindow->spectrum_colorbar->color[row] = ((SpecialButton *)(colorBarTable->cellWidget(row, 2)))->globalColor();
  }

  if(radiobutton5_1->isChecked()) // sum
  {
    mainwindow->spectrum_colorbar->method = 0;
  }
  else
    if(radiobutton5_2->isChecked()) // peak
    {
      mainwindow->spectrum_colorbar->method = 1;
    }
    else
      if(radiobutton5_3->isChecked()) // average
      {
        mainwindow->spectrum_colorbar->method = 2;
      }

  mainwindow->spectrum_colorbar->max_colorbar_value = dspinbox3_2->value();

  if(checkbox3_1->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_colorbar->auto_adjust = 1;
  }
  else
  {
    mainwindow->spectrum_colorbar->auto_adjust = 0;
  }

  ApplyButton2->setEnabled(false);

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(mainwindow->spectrumdock[i]->dock->isVisible())
    {
      mainwindow->spectrumdock[i]->rescan();
    }
  }
}


void UI_OptionsDialog::checkBoxChanged(int state)
{
  int i,
      row,
      lastrow=0;

  if(state == Qt::Checked)
  {
    for(row = MAXSPECTRUMMARKERS - 1; row >= 0; row--)
    {
      if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
      {
        lastrow = row;

        if(row)
        {
          for(i=row-1; i>=0; i--)
          {
            ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
          }
        }

        break;
      }
    }
  }
  else
  {
    for(row = 0; row < MAXSPECTRUMMARKERS; row++)
    {
      if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Unchecked)
      {
        lastrow = row - 1;

        for(; row < MAXSPECTRUMMARKERS; row++)
        {
          ((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->setCheckState(Qt::Unchecked);
        }

        break;
      }
    }
  }

  for(row=0; row < lastrow; row++)
  {
    if(((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value() >= ((QDoubleSpinBox *)(colorBarTable->cellWidget(row + 1, 1)))->value())
    {
      ((QDoubleSpinBox *)(colorBarTable->cellWidget(row + 1, 1)))->setValue(((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value() + 1.0);
    }
  }

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::DefaultButton2Clicked()
{
  int i;

  for(i=0; i<5; i++)
  {
    ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->clear();
  }

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(0, 1)))->setValue(4.0);
  ((SpecialButton *)(colorBarTable->cellWidget(0, 2)))->setGlobalColor(Qt::darkRed);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(1, 1)))->setValue(8.0);
  ((SpecialButton *)(colorBarTable->cellWidget(1, 2)))->setGlobalColor(Qt::darkGreen);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(2, 1)))->setValue(12.0);
  ((SpecialButton *)(colorBarTable->cellWidget(2, 2)))->setGlobalColor(Qt::darkBlue);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(3, 1)))->setValue(30.0);
  ((SpecialButton *)(colorBarTable->cellWidget(3, 2)))->setGlobalColor(Qt::darkCyan);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(4, 1)))->setValue(100.0);
  ((SpecialButton *)(colorBarTable->cellWidget(4, 2)))->setGlobalColor(Qt::darkMagenta);

  for(i=5; i<MAXSPECTRUMMARKERS; i++)
  {
    ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setValue(1.0);
    ((SpecialButton *)(colorBarTable->cellWidget(i, 2)))->setGlobalColor(Qt::white);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->clear();
  }

  radiobutton5_1->setChecked(true);

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::colorBarButtonClicked(SpecialButton *button)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  button->setGlobalColor(color);

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::checkbox1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->maincurve->blackwhite_printing = 1;
  }
  else
  {
    mainwindow->maincurve->blackwhite_printing = 0;
  }
}


void UI_OptionsDialog::checkbox2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->show_annot_markers = 1;

    mainwindow->view_markers_act->setChecked(true);
  }
  else
  {
    mainwindow->show_annot_markers = 0;

    mainwindow->view_markers_act->setChecked(false);
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox2_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annotations_show_duration = 1;
  }
  else
  {
    mainwindow->annotations_show_duration = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox2_2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annotations_duration_background_type = 1;
  }
  else
  {
    mainwindow->annotations_duration_background_type = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox2_3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->channel_linked_annotations = 1;
  }
  else
  {
    mainwindow->channel_linked_annotations = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->show_baselines = 1;
  }
  else
  {
    mainwindow->show_baselines = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox4Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->clip_to_pane = 1;
  }
  else
  {
    mainwindow->clip_to_pane = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox5Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->signal_plotorder = 1;
  }
  else
  {
    mainwindow->signal_plotorder = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox4_12Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_filter->hide_in_list_only = 1;
  }
  else
  {
    mainwindow->annot_filter->hide_in_list_only = 0;
  }
}


void UI_OptionsDialog::checkbox4_17Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annotlist_scrolltoitem_while_browsing = 1;
  }
  else
  {
    mainwindow->annotlist_scrolltoitem_while_browsing = 0;
  }
}


void UI_OptionsDialog::checkbox6Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->maincurve->crosshair_1.has_hor_line = 1;
    mainwindow->maincurve->crosshair_2.has_hor_line = 1;
  }
  else
  {
    mainwindow->maincurve->crosshair_1.has_hor_line = 0;
    mainwindow->maincurve->crosshair_2.has_hor_line = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox16Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->use_diverse_signal_colors = 1;
  }
  else
  {
    mainwindow->use_diverse_signal_colors = 0;
  }
}


void UI_OptionsDialog::checkbox3_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    dspinbox3_2->setEnabled(false);

    mainwindow->spectrum_colorbar->auto_adjust = 1;
  }
  else
  {
    dspinbox3_2->setEnabled(true);

    mainwindow->spectrum_colorbar->auto_adjust = 0;
  }

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::checkbox4_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->auto_reload_mtg = 1;
  }
  else
  {
    mainwindow->auto_reload_mtg = 0;
  }
}


void UI_OptionsDialog::checkbox4_2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->read_biosemi_status_signal = 1;
  }
  else
  {
    mainwindow->read_biosemi_status_signal = 0;
  }

  if(mainwindow->files_open)
  {
    QMessageBox::information(optionsdialog, "Information", "You need to close and re-open the file for the changes to take effect.");
  }
}


void UI_OptionsDialog::checkbox4_3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->read_nk_trigger_signal = 1;
  }
  else
  {
    mainwindow->read_nk_trigger_signal = 0;
  }

  if(mainwindow->files_open)
  {
    QMessageBox::information(optionsdialog, "Information", "You need to close and re-open the file for the changes to take effect.");
  }
}


void UI_OptionsDialog::checkbox4_4Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->use_threads = 1;
  }
  else
  {
    mainwindow->use_threads = 0;
  }
}


void UI_OptionsDialog::checkbox4_14Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->session_relative_paths = 1;
  }
  else
  {
    mainwindow->session_relative_paths = 0;
  }
}


void UI_OptionsDialog::checkbox4_11Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->edf_debug = 1;
  }
  else
  {
    mainwindow->edf_debug = 0;
  }
}


void UI_OptionsDialog::checkbox4_15Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->strip_label_types = 1;
  }
  else
  {
    mainwindow->strip_label_types = 0;
  }
}


void UI_OptionsDialog::checkbox4_16Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->mc_v_scrollarea_auto = 1;

    spinbox4_6->setEnabled(true);

    mainwindow->vert_scrollbar_act->setChecked(true);

    mainwindow->mc_v_scrollbar->setVisible(true);
  }
  else
  {
    mainwindow->mc_v_scrollarea_auto = 0;

    spinbox4_6->setEnabled(false);

    mainwindow->vert_scrollbar_act->setChecked(false);

    mainwindow->mc_v_scrollbar->setVisible(false);
  }

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::checkbox4_18Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->default_invert_eeg_signals = 1;
  }
  else
  {
    mainwindow->default_invert_eeg_signals = 0;
  }
}


void UI_OptionsDialog::checkbox4_13Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->rc_host_server_public = 1;

    QMessageBox::warning(optionsdialog, "Warning",
                         "Making the remote control port publicly accessible to your network is not recommended and\n"
                         "can be a potential security issue.\n"
                         "\nYou need to restart EDFbrowser for the changes to take effect.");
  }
  else
  {
    mainwindow->rc_host_server_public = 0;

    QMessageBox::information(optionsdialog, "Information",
                             "The remote control port can still be activated when EDFbrowser is started with the option --rc-host-port but\n"
                             "it will only be accessible from this system.\n"
                             "\nYou need to restart EDFbrowser for the changes to take effect.");
  }
}


void UI_OptionsDialog::checkbox4_7Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->display_pagetime_mmsec = 1;
  }
  else
  {
    mainwindow->display_pagetime_mmsec = 0;
  }

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::checkbox4_5Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->check_for_updates = 1;
  }
  else
  {
    mainwindow->check_for_updates = 0;
  }
}


void UI_OptionsDialog::checkbox4_6Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->linear_interpol = 1;

    mainwindow->linear_interpol_act->setChecked(true);
  }
  else
  {
    mainwindow->linear_interpol = 0;

    mainwindow->linear_interpol_act->setChecked(false);
  }

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::checkbox7_2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->auto_update_annot_onset = 1;
  }
  else
  {
    mainwindow->auto_update_annot_onset = 0;
  }
}


void UI_OptionsDialog::checkbox7_3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_user_button_update_annot_onset = 1;
  }
  else
  {
    mainwindow->annot_editor_user_button_update_annot_onset = 0;
  }
}


void UI_OptionsDialog::checkbox7_4Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_user_button_update_annot_duration = 1;
  }
  else
  {
    mainwindow->annot_editor_user_button_update_annot_duration = 0;
  }
}


void UI_OptionsDialog::checkbox7_5Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_user_button_update_annot_description = 1;
  }
  else
  {
    mainwindow->annot_editor_user_button_update_annot_description = 0;
  }
}


void UI_OptionsDialog::checkbox7_6Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_user_button_jump_to_next_page = 1;

    checkbox7_7->setEnabled(true);

    spinbox7_1->setEnabled(true);

    spinbox7_2->setEnabled(true);
  }
  else
  {
    mainwindow->annot_editor_user_button_jump_to_next_page = 0;

    checkbox7_7->setEnabled(false);

    spinbox7_1->setEnabled(false);

    spinbox7_2->setEnabled(false);
  }
}


void UI_OptionsDialog::checkbox7_7Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_user_button_stay_on_epoch_boundary = 1;
  }
  else
  {
    mainwindow->annot_editor_user_button_stay_on_epoch_boundary = 0;
  }
}


void UI_OptionsDialog::checkbox7_9Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_editor_highres = 1;

    if(mainwindow->annotationEditDock != NULL)
    {
      mainwindow->annotationEditDock->set_high_resolution(1);
    }
  }
  else
  {
    mainwindow->annot_editor_highres = 0;

    if(mainwindow->annotationEditDock != NULL)
    {
      mainwindow->annotationEditDock->set_high_resolution(0);
    }
  }

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::spinBox7_1ValueChanged(int val)
{
  mainwindow->annot_editor_user_button_epoch_len = val * TIME_FIXP_SCALING;
}


void UI_OptionsDialog::spinBox7_2ValueChanged(int val)
{
  mainwindow->annot_editor_user_button_page_len = val * TIME_FIXP_SCALING;
}


void UI_OptionsDialog::checkbox4_8Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->use_signallabel_in_annot_descr = 1;
  }
  else
  {
    mainwindow->use_signallabel_in_annot_descr = 0;
  }
}


void UI_OptionsDialog::checkbox4_9Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->maincurve->floating_ruler_use_var_width = 1;
  }
  else
  {
    mainwindow->maincurve->floating_ruler_use_var_width = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox4_10Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_onset_at_start_of_page_on_jump = 1;
  }
  else
  {
    mainwindow->annot_onset_at_start_of_page_on_jump = 0;
  }
}


void UI_OptionsDialog::BgColorButtonClicked(SpecialButton *)
{
  int i;

  QColor temp;

  QPalette palette;

  temp = QColorDialog::getColor(mainwindow->maincurve->backgroundcolor, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->backgroundcolor = temp;

    BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

    for(i=0; i<mainwindow->files_open; i++)
    {
      if(mainwindow->annotations_dock[i])
      {
        mainwindow->annotations_dock[i]->list->setPalette(palette);
        mainwindow->annotations_dock[i]->list->update();
        mainwindow->annotations_dock[i]->updateList(0);
      }
    }

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::SrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->small_ruler_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->small_ruler_color = temp;

    SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::BrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->big_ruler_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->big_ruler_color = temp;

    BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::MrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->mouse_rect_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->mouse_rect_color = temp;

    MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::TxtColorButtonClicked(SpecialButton *)
{
  int i;

  QColor temp;

  QPalette palette;

  temp = QColorDialog::getColor(mainwindow->maincurve->text_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->text_color = temp;

    TxtColorButton->setColor(mainwindow->maincurve->text_color);

    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

    for(i=0; i<mainwindow->files_open; i++)
    {
      if(edfplus_annotation_size(&mainwindow->edfheaderlist[i]->annot_list) > 0)
      {
        mainwindow->annotations_dock[i]->list->setPalette(palette);
        mainwindow->annotations_dock[i]->list->update();
        mainwindow->annotations_dock[i]->updateList(0);
      }
    }

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::SigColorButtonClicked(SpecialButton *)
{
  int i, color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  SigColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->signal_color = color;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = color;
  }

  mainwindow->maincurve->update();
}



void UI_OptionsDialog::BaseColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->baseline_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->baseline_color = temp;

    BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::Crh1ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  Crh1ColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->crosshair_1.color = color;

  mainwindow->maincurve->update();
}



void UI_OptionsDialog::Crh2ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  Crh2ColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->crosshair_2.color = color;

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::FrColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  FrColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->floating_ruler_color = color;

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::AnnotMkrButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_marker_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_marker_color = temp;

    AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotMkrSelButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_marker_selected_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_marker_selected_color = temp;

    AnnotMkrSelButton->setColor(mainwindow->maincurve->annot_marker_selected_color);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_duration_color, tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_duration_color = temp;

    AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationSelectedButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_duration_color_selected, tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_duration_color_selected = temp;

    AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::annotlistdock_edited_txt_color_button_clicked(SpecialButton *)
{
  int i;

  QColor temp;

  temp = QColorDialog::getColor(mainwindow->annot_list_edited_txt_color, tab1, "Select Color");

  if(temp.isValid())
  {
    mainwindow->annot_list_edited_txt_color = temp;

    annotlistdock_edited_txt_color_button->setColor(mainwindow->annot_list_edited_txt_color);

    for(i=0; i<MAXFILES; i++)
    {
      if(mainwindow->annotations_dock[i] != NULL)
      {
        mainwindow->annotations_dock[i]->updateList(0);
      }
    }
  }
}


void UI_OptionsDialog::dspinbox4_4ValueChanged(double val)
{
  mainwindow->default_amplitude = val;
}


void UI_OptionsDialog::saveColorSchemaButtonClicked()
{
  int i;

  char path[MAX_PATH_LENGTH]={""};
//       str[1024]={""};

  FILE *colorfile=NULL;

  strlcpy(path, mainwindow->recent_colordir, MAX_PATH_LENGTH);
  strlcat(path, "/my_colorschema.color", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Save colorschema", QString::fromLocal8Bit(path), "Colorschema files (*.color *.COLOR)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  if(strlen(path) > 4)
  {
    if(strcmp(path + strlen(path) - 6, ".color"))
    {
      strlcat(path, ".color", MAX_PATH_LENGTH);
    }
  }

  get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

  colorfile = fopen(path, "wb");
  if(colorfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Cannot open file for writing.");
    messagewindow.exec();

    return;
  }

  fprintf(colorfile, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_colorschema>\n");

  fprintf(colorfile, " <backgroundcolor>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </backgroundcolor>\n",
                  mainwindow->maincurve->backgroundcolor.red(),
                  mainwindow->maincurve->backgroundcolor.green(),
                  mainwindow->maincurve->backgroundcolor.blue());

  fprintf(colorfile, " <small_ruler_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </small_ruler_color>\n",
                  mainwindow->maincurve->small_ruler_color.red(),
                  mainwindow->maincurve->small_ruler_color.green(),
                  mainwindow->maincurve->small_ruler_color.blue());

  fprintf(colorfile, " <big_ruler_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </big_ruler_color>\n",
                  mainwindow->maincurve->big_ruler_color.red(),
                  mainwindow->maincurve->big_ruler_color.green(),
                  mainwindow->maincurve->big_ruler_color.blue());

  fprintf(colorfile, " <mouse_rect_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </mouse_rect_color>\n",
                  mainwindow->maincurve->mouse_rect_color.red(),
                  mainwindow->maincurve->mouse_rect_color.green(),
                  mainwindow->maincurve->mouse_rect_color.blue());

  fprintf(colorfile, " <text_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </text_color>\n",
                  mainwindow->maincurve->text_color.red(),
                  mainwindow->maincurve->text_color.green(),
                  mainwindow->maincurve->text_color.blue());

  fprintf(colorfile, " <baseline_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </baseline_color>\n",
                  mainwindow->maincurve->baseline_color.red(),
                  mainwindow->maincurve->baseline_color.green(),
                  mainwindow->maincurve->baseline_color.blue());

  fprintf(colorfile, " <annot_marker_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </annot_marker_color>\n",
                  mainwindow->maincurve->annot_marker_color.red(),
                  mainwindow->maincurve->annot_marker_color.green(),
                  mainwindow->maincurve->annot_marker_color.blue());

  fprintf(colorfile, " <annot_marker_selected_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </annot_marker_selected_color>\n",
                  mainwindow->maincurve->annot_marker_selected_color.red(),
                  mainwindow->maincurve->annot_marker_selected_color.green(),
                  mainwindow->maincurve->annot_marker_selected_color.blue());

  fprintf(colorfile, " <annot_duration_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  "  <alpha>%i</alpha>\n"
                  " </annot_duration_color>\n",
                  mainwindow->maincurve->annot_duration_color.red(),
                  mainwindow->maincurve->annot_duration_color.green(),
                  mainwindow->maincurve->annot_duration_color.blue(),
                  mainwindow->maincurve->annot_duration_color.alpha());

  fprintf(colorfile, " <annot_duration_color_selected>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  "  <alpha>%i</alpha>\n"
                  " </annot_duration_color_selected>\n",
                  mainwindow->maincurve->annot_duration_color_selected.red(),
                  mainwindow->maincurve->annot_duration_color_selected.green(),
                  mainwindow->maincurve->annot_duration_color_selected.blue(),
                  mainwindow->maincurve->annot_duration_color_selected.alpha());

  fprintf(colorfile, " <annot_ov_predefined_block>\n");

  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    fprintf(colorfile, "  <mc_annot_ov_color_predefined>\n"
                     "   <red>%i</red>\n"
                     "   <green>%i</green>\n"
                     "   <blue>%i</blue>\n"
                     "   <alpha>%i</alpha>\n"
                     "  </mc_annot_ov_color_predefined>\n",
                     mainwindow->mc_annot_ov_color_predefined[i].red(),
                     mainwindow->mc_annot_ov_color_predefined[i].green(),
                     mainwindow->mc_annot_ov_color_predefined[i].blue(),
                     mainwindow->mc_annot_ov_color_predefined[i].alpha());
  }

//   for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
//   {
//     xml_strlcpy_encode_entity(str1_512, mainwindow->mc_annot_ov_name_predefined[i], 1024);
//
//     fprintf(colorfile, "  <mc_annot_ov_name_predefined>%s</mc_annot_ov_name_predefined>\n", str);
//   }

  fprintf(colorfile, " </annot_ov_predefined_block>\n");

  fprintf(colorfile, " <signal_color>%i</signal_color>\n",
                  mainwindow->maincurve->signal_color);

  fprintf(colorfile, " <crosshair_1_color>%i</crosshair_1_color>\n",
                  mainwindow->maincurve->crosshair_1.color);

  fprintf(colorfile, " <crosshair_2_color>%i</crosshair_2_color>\n",
                  mainwindow->maincurve->crosshair_2.color);

  fprintf(colorfile, " <crosshair_1_has_hor_line>%i</crosshair_1_has_hor_line>\n",
                  mainwindow->maincurve->crosshair_1.has_hor_line);

  fprintf(colorfile, " <crosshair_2_has_hor_line>%i</crosshair_2_has_hor_line>\n",
                  mainwindow->maincurve->crosshair_2.has_hor_line);

  fprintf(colorfile, " <crosshair_1_dot_sz>%i</crosshair_1_dot_sz>\n",
                  mainwindow->maincurve->crosshair_1.dot_sz);

  fprintf(colorfile, " <crosshair_2_dot_sz>%i</crosshair_2_dot_sz>\n",
                  mainwindow->maincurve->crosshair_2.dot_sz);

  fprintf(colorfile, " <floating_ruler_color>%i</floating_ruler_color>\n",
                  mainwindow->maincurve->floating_ruler_color);

  fprintf(colorfile, " <blackwhite_printing>%i</blackwhite_printing>\n",
                  mainwindow->maincurve->blackwhite_printing);

  fprintf(colorfile, " <show_annot_markers>%i</show_annot_markers>\n",
                  mainwindow->show_annot_markers);

  fprintf(colorfile, " <show_baselines>%i</show_baselines>\n",
                  mainwindow->show_baselines);

  fprintf(colorfile, " <clip_to_pane>%i</clip_to_pane>\n",
                  mainwindow->clip_to_pane);

  fprintf(colorfile, " <ecg_view_mode>%i</ecg_view_mode>\n",
                  mainwindow->ecg_view_mode);

  fprintf(colorfile, "</" PROGRAM_NAME "_colorschema>\n");

  fclose(colorfile);
}


void UI_OptionsDialog::loadColorSchemaButtonClicked()
{
  int i;

  char path[MAX_PATH_LENGTH]={""},
       scratchpad_2048[2048]={""},
       result[XML_STRBUFLEN]={""};

  xml_hdl_t *xml_hdl=NULL;


  strlcpy(path, QFileDialog::getOpenFileName(0, "Load colorschema", QString::fromLocal8Bit(mainwindow->recent_colordir), "Montage files (*.color *.COLOR)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);
  if(xml_hdl==NULL)
  {
    snprintf(scratchpad_2048, 2048, "Cannot open colorschema:\n%s", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad_2048));
    messagewindow.exec();
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_colorschema"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this colorschema.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return;
  }

  mainwindow->get_rgbcolor_settings(xml_hdl, "backgroundcolor", 0, &mainwindow->maincurve->backgroundcolor);

  mainwindow->get_rgbcolor_settings(xml_hdl, "small_ruler_color", 0, &mainwindow->maincurve->small_ruler_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "big_ruler_color", 0, &mainwindow->maincurve->big_ruler_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "mouse_rect_color", 0, &mainwindow->maincurve->mouse_rect_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "text_color", 0, &mainwindow->maincurve->text_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "baseline_color", 0, &mainwindow->maincurve->baseline_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_marker_color", 0, &mainwindow->maincurve->annot_marker_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_marker_selected_color", 0, &mainwindow->maincurve->annot_marker_selected_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_duration_color", 0, &mainwindow->maincurve->annot_duration_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_duration_color_selected", 0, &mainwindow->maincurve->annot_duration_color_selected);

  if(!xml_goto_nth_element_inside(xml_hdl, "annot_ov_predefined_block", 0))
  {
    for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
    {
      mainwindow->get_rgbcolor_settings(xml_hdl, "mc_annot_ov_color_predefined", i, &mainwindow->mc_annot_ov_color_predefined[i]);
    }

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "signal_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->signal_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "floating_ruler_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->floating_ruler_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "blackwhite_printing", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->blackwhite_printing = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_annot_markers", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->show_annot_markers = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_baselines", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->show_baselines = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "clip_to_pane", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->clip_to_pane = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "ecg_view_mode", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    mainwindow->ecg_view_mode = atoi(result);
    if(mainwindow->ecg_view_mode != 1)
    {
      mainwindow->ecg_view_mode = 0;
    }

    xml_go_up(xml_hdl);
  }
  else
  {
    mainwindow->ecg_view_mode = 0;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_1_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->crosshair_1.color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_2_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->crosshair_2.color = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_1_has_hor_line", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    mainwindow->maincurve->crosshair_1.has_hor_line = atoi(result);
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_2_has_hor_line", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    mainwindow->maincurve->crosshair_2.has_hor_line = atoi(result);
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_1_dot_sz", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    mainwindow->maincurve->crosshair_1.dot_sz = atoi(result);
    if(mainwindow->maincurve->crosshair_1.dot_sz < 0)
    {
      mainwindow->maincurve->crosshair_1.dot_sz = 0;
    }
    if(mainwindow->maincurve->crosshair_1.dot_sz > 32)
    {
      mainwindow->maincurve->crosshair_1.dot_sz = 32;
    }
  }

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "crosshair_2_dot_sz", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    mainwindow->maincurve->crosshair_2.dot_sz = atoi(result);
    if(mainwindow->maincurve->crosshair_2.dot_sz < 0)
    {
      mainwindow->maincurve->crosshair_2.dot_sz = 0;
    }
    if(mainwindow->maincurve->crosshair_2.dot_sz > 32)
    {
      mainwindow->maincurve->crosshair_2.dot_sz = 32;
    }
  }

  xml_go_up(xml_hdl);

  xml_close(xml_hdl);

  update_interface();
}


void UI_OptionsDialog::update_interface(void)
{
  int i,
      default_color_idx=0,
      default_color_list[DEFAULT_COLOR_LIST_SZ];

  double value = 500;

  QPalette palette;

  default_color_list[0] = Qt::yellow;
  default_color_list[1] = Qt::green;
  default_color_list[2] = Qt::red;
  default_color_list[3] = Qt::cyan;
  default_color_list[4] = Qt::magenta;
  default_color_list[5] = Qt::blue;

  BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

  SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

  BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

  MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

  TxtColorButton->setColor(mainwindow->maincurve->text_color);

  SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);

  if(mainwindow->show_baselines)
  {
    checkbox3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox3->setCheckState(Qt::Unchecked);
  }

  BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

  Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);

  Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);

  annotlistdock_edited_txt_color_button->setColor(mainwindow->annot_list_edited_txt_color);

  if(mainwindow->maincurve->crosshair_1.has_hor_line)
  {
    checkbox6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox6->setCheckState(Qt::Unchecked);
  }

  spinbox1_1->setValue(mainwindow->maincurve->crosshair_1.dot_sz);

  FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);

  if(mainwindow->show_annot_markers)
  {
    checkbox2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2->setCheckState(Qt::Unchecked);
  }

  AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

  AnnotMkrSelButton->setColor(mainwindow->maincurve->annot_marker_selected_color);

  AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);

  AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);

  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    AnnotDurationPredefinedButton[i]->setColor(mainwindow->mc_annot_ov_color_predefined[i]);
  }

  if(mainwindow->maincurve->blackwhite_printing)
  {
    checkbox1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox1->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->clip_to_pane)
  {
    checkbox4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->use_diverse_signal_colors)
  {
    checkbox16->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox16->setCheckState(Qt::Unchecked);
  }

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i] != NULL)
    {
      mainwindow->annotations_dock[i]->list->setPalette(palette);
      mainwindow->annotations_dock[i]->list->update();
      mainwindow->annotations_dock[i]->updateList(0);
    }
  }

  if(mainwindow->use_diverse_signal_colors)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->color = default_color_list[default_color_idx++];
      default_color_idx %= DEFAULT_COLOR_LIST_SZ;
    }
  }
  else
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      mainwindow->signalcomp[i]->color = mainwindow->maincurve->signal_color;
    }
  }

  spinbox1_1->setValue(mainwindow->maincurve->crosshair_1.dot_sz);

  if(mainwindow->maincurve->crosshair_1.has_hor_line)
  {
    checkbox6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox6->setCheckState(Qt::Unchecked);
  }

  grid_radio_group->blockSignals(true);
  if(mainwindow->ecg_view_mode)
  {
    grid_ecg_radiobutton->setChecked(true);
    grid_normal_radiobutton->setChecked(false);
  }
  else
  {
    grid_normal_radiobutton->setChecked(true);
    grid_ecg_radiobutton->setChecked(false);
  }
  grid_radio_group->blockSignals(false);

  if(mainwindow->ecg_view_mode)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      if(!strcmp(mainwindow->signalcomp[i]->physdimension, "uV"))
      {
        value = 500.0;
      }
      else if(!strcmp(mainwindow->signalcomp[i]->physdimension, "mV"))
        {
          value = 0.5;
        }
        else if(!strcmp(mainwindow->signalcomp[i]->physdimension, "V"))
          {
            value = 0.0005;
          }
          else
          {
            value = 500.0;
          }

      if(mainwindow->signalcomp[i]->edfparam_0->bitvalue < 0.0)
      {
        value *= -1.0;
      }

      mainwindow->signalcomp[i]->sensitivity = (mainwindow->signalcomp[i]->edfparam_0->bitvalue / value) / mainwindow->y_pixelsizefactor;

      mainwindow->signalcomp[i]->screen_offset_pix *= (mainwindow->signalcomp[i]->voltpercm / value);

      mainwindow->signalcomp[i]->voltpercm = value;

      mainwindow->signalcomp[i]->screen_offset_unit = -mainwindow->signalcomp[i]->screen_offset_pix * mainwindow->y_pixelsizefactor * mainwindow->signalcomp[i]->voltpercm;
    }

    mainwindow->pagetime = mainwindow->maincurve->width() * mainwindow->x_pixelsizefactor * TIME_FIXP_SCALING / 5.0;

    mainwindow->setup_viewbuf();
  }
  else
  {
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::loadColorSchema_NK()
{
  mainwindow->maincurve->backgroundcolor.setRed(255);
  mainwindow->maincurve->backgroundcolor.setGreen(255);
  mainwindow->maincurve->backgroundcolor.setBlue(255);

  mainwindow->maincurve->small_ruler_color.setRed(0);
  mainwindow->maincurve->small_ruler_color.setGreen(0);
  mainwindow->maincurve->small_ruler_color.setBlue(0);

  mainwindow->maincurve->big_ruler_color.setRed(255);
  mainwindow->maincurve->big_ruler_color.setGreen(255);
  mainwindow->maincurve->big_ruler_color.setBlue(0);

  mainwindow->maincurve->mouse_rect_color.setRed(0);
  mainwindow->maincurve->mouse_rect_color.setGreen(0);
  mainwindow->maincurve->mouse_rect_color.setBlue(0);

  mainwindow->maincurve->text_color.setRed(0);
  mainwindow->maincurve->text_color.setGreen(0);
  mainwindow->maincurve->text_color.setBlue(0);

  mainwindow->maincurve->baseline_color.setRed(128);
  mainwindow->maincurve->baseline_color.setGreen(128);
  mainwindow->maincurve->baseline_color.setBlue(128);

  mainwindow->maincurve->annot_marker_color.setRed(0);
  mainwindow->maincurve->annot_marker_color.setGreen(0);
  mainwindow->maincurve->annot_marker_color.setBlue(0);

  mainwindow->maincurve->annot_marker_selected_color.setRed(128);
  mainwindow->maincurve->annot_marker_selected_color.setGreen(0);
  mainwindow->maincurve->annot_marker_selected_color.setBlue(128);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->signal_color = Qt::black;

  mainwindow->maincurve->floating_ruler_color = Qt::red;

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->show_annot_markers = 1;

  mainwindow->show_baselines = 1;

  mainwindow->maincurve->crosshair_1.color = Qt::red;

  mainwindow->maincurve->crosshair_2.color = Qt::blue;

  mainwindow->annot_list_edited_txt_color = Qt::red;

  mainwindow->clip_to_pane = 0;

  mainwindow->use_diverse_signal_colors = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;
  mainwindow->maincurve->crosshair_2.dot_sz = 4;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;
  mainwindow->maincurve->crosshair_2.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;

  mainwindow->mc_annot_ov_color_predefined[0] = QColor(255, 0, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[1] = QColor(0, 0, 128, 32);
  mainwindow->mc_annot_ov_color_predefined[2] = QColor(128, 128, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[3] = QColor(255, 85, 255, 32);
  mainwindow->mc_annot_ov_color_predefined[4] = QColor(255, 192, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[5] = QColor(128, 192, 64, 32);
  mainwindow->mc_annot_ov_color_predefined[6] = QColor(64, 0, 192, 32);
  mainwindow->mc_annot_ov_color_predefined[7] = QColor(0, 255, 255, 32);

  mainwindow->ecg_view_mode = 0;

  update_interface();
}


void UI_OptionsDialog::loadColorSchema_Dark()
{
  mainwindow->maincurve->backgroundcolor.setRed(64);
  mainwindow->maincurve->backgroundcolor.setGreen(64);
  mainwindow->maincurve->backgroundcolor.setBlue(64);

  mainwindow->maincurve->small_ruler_color.setRed(255);
  mainwindow->maincurve->small_ruler_color.setGreen(255);
  mainwindow->maincurve->small_ruler_color.setBlue(255);

  mainwindow->maincurve->big_ruler_color.setRed(128);
  mainwindow->maincurve->big_ruler_color.setGreen(128);
  mainwindow->maincurve->big_ruler_color.setBlue(128);

  mainwindow->maincurve->mouse_rect_color.setRed(255);
  mainwindow->maincurve->mouse_rect_color.setGreen(255);
  mainwindow->maincurve->mouse_rect_color.setBlue(255);

  mainwindow->maincurve->text_color.setRed(255);
  mainwindow->maincurve->text_color.setGreen(255);
  mainwindow->maincurve->text_color.setBlue(255);

  mainwindow->maincurve->baseline_color.setRed(128);
  mainwindow->maincurve->baseline_color.setGreen(128);
  mainwindow->maincurve->baseline_color.setBlue(128);
  mainwindow->show_baselines = 1;

  mainwindow->maincurve->annot_marker_color = Qt::white;
  mainwindow->show_annot_markers = 1;

  mainwindow->maincurve->annot_marker_selected_color.setRed(255);
  mainwindow->maincurve->annot_marker_selected_color.setGreen(228);
  mainwindow->maincurve->annot_marker_selected_color.setBlue(0);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->signal_color = Qt::yellow;

  mainwindow->maincurve->crosshair_1.color = Qt::yellow;

  mainwindow->maincurve->crosshair_2.color = Qt::cyan;

  mainwindow->maincurve->floating_ruler_color = Qt::cyan;

  mainwindow->annot_list_edited_txt_color.setRed(0);
  mainwindow->annot_list_edited_txt_color.setGreen(255);
  mainwindow->annot_list_edited_txt_color.setBlue(170);

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->clip_to_pane = 0;

  mainwindow->use_diverse_signal_colors = 1;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;
  mainwindow->maincurve->crosshair_2.dot_sz = 4;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;
  mainwindow->maincurve->crosshair_2.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;

  mainwindow->mc_annot_ov_color_predefined[0] = QColor(255, 0, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[1] = QColor(0, 0, 128, 32);
  mainwindow->mc_annot_ov_color_predefined[2] = QColor(128, 128, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[3] = QColor(255, 85, 255, 32);
  mainwindow->mc_annot_ov_color_predefined[4] = QColor(255, 192, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[5] = QColor(128, 192, 64, 32);
  mainwindow->mc_annot_ov_color_predefined[6] = QColor(64, 0, 192, 32);
  mainwindow->mc_annot_ov_color_predefined[7] = QColor(0, 255, 255, 32);

  mainwindow->ecg_view_mode = 0;

  update_interface();
}


void UI_OptionsDialog::loadColorSchema_blue_gray()
{
  mainwindow->maincurve->backgroundcolor = Qt::gray;

  mainwindow->maincurve->small_ruler_color = Qt::black;

  mainwindow->maincurve->big_ruler_color = Qt::darkGray;

  mainwindow->maincurve->mouse_rect_color = Qt::black;

  mainwindow->maincurve->text_color = Qt::black;

  mainwindow->maincurve->signal_color = Qt::blue;

  mainwindow->maincurve->baseline_color = Qt::darkGray;
  mainwindow->show_baselines = 1;

  mainwindow->maincurve->crosshair_1.color = Qt::red;

  mainwindow->maincurve->crosshair_2.color = Qt::cyan;

  mainwindow->maincurve->floating_ruler_color = Qt::red;

  mainwindow->maincurve->annot_marker_color = Qt::white;
  mainwindow->show_annot_markers = 1;

  mainwindow->maincurve->annot_marker_selected_color = Qt::yellow;

  mainwindow->annot_list_edited_txt_color = Qt::red;

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->clip_to_pane = 0;

  mainwindow->use_diverse_signal_colors = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;
  mainwindow->maincurve->crosshair_2.dot_sz = 4;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;
  mainwindow->maincurve->crosshair_2.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;

  mainwindow->mc_annot_ov_color_predefined[0] = QColor(255, 0, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[1] = QColor(0, 0, 128, 32);
  mainwindow->mc_annot_ov_color_predefined[2] = QColor(128, 128, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[3] = QColor(255, 85, 255, 32);
  mainwindow->mc_annot_ov_color_predefined[4] = QColor(255, 192, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[5] = QColor(128, 192, 64, 32);
  mainwindow->mc_annot_ov_color_predefined[6] = QColor(64, 0, 192, 32);
  mainwindow->mc_annot_ov_color_predefined[7] = QColor(0, 255, 255, 32);

  mainwindow->ecg_view_mode = 0;

  update_interface();
}


void UI_OptionsDialog::loadColorSchema_ECG()
{
  int i;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(strcmp(mainwindow->signalcomp[i]->physdimension, "uV") &&
       strcmp(mainwindow->signalcomp[i]->physdimension, "mV") &&
       strcmp(mainwindow->signalcomp[i]->physdimension, "V"))
    {
      QMessageBox::warning(optionsdialog, "Warning",
                           "The physical dimension (unit) of one or more signals on the screen does not equal to uV, mV or V.\n"
                           "As a result, the grid's dimensions for these signals cannot be calculated correctly.");
      break;
    }
  }

  mainwindow->maincurve->backgroundcolor.setRed(255);
  mainwindow->maincurve->backgroundcolor.setGreen(255);
  mainwindow->maincurve->backgroundcolor.setBlue(255);

  mainwindow->maincurve->small_ruler_color.setRed(0xfe);
  mainwindow->maincurve->small_ruler_color.setGreen(0xe6);
  mainwindow->maincurve->small_ruler_color.setBlue(0xe6);

  mainwindow->maincurve->big_ruler_color.setRed(255);
  mainwindow->maincurve->big_ruler_color.setGreen(0);
  mainwindow->maincurve->big_ruler_color.setBlue(0);

  mainwindow->maincurve->mouse_rect_color.setRed(0);
  mainwindow->maincurve->mouse_rect_color.setGreen(0);
  mainwindow->maincurve->mouse_rect_color.setBlue(0);

  mainwindow->maincurve->text_color.setRed(0);
  mainwindow->maincurve->text_color.setGreen(0);
  mainwindow->maincurve->text_color.setBlue(0);

  mainwindow->maincurve->baseline_color.setRed(255);
  mainwindow->maincurve->baseline_color.setGreen(0);
  mainwindow->maincurve->baseline_color.setBlue(0);

  mainwindow->maincurve->annot_marker_color.setRed(0);
  mainwindow->maincurve->annot_marker_color.setGreen(0);
  mainwindow->maincurve->annot_marker_color.setBlue(0);

  mainwindow->maincurve->annot_marker_selected_color.setRed(128);
  mainwindow->maincurve->annot_marker_selected_color.setGreen(0);
  mainwindow->maincurve->annot_marker_selected_color.setBlue(128);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->signal_color = Qt::black;

  mainwindow->maincurve->floating_ruler_color = Qt::black;

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->show_annot_markers = 1;

  mainwindow->show_baselines = 1;

  mainwindow->maincurve->crosshair_1.color = Qt::darkGreen;

  mainwindow->maincurve->crosshair_2.color = Qt::blue;

  mainwindow->annot_list_edited_txt_color = Qt::red;

  mainwindow->clip_to_pane = 0;

  mainwindow->use_diverse_signal_colors = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;
  mainwindow->maincurve->crosshair_2.dot_sz = 4;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;
  mainwindow->maincurve->crosshair_2.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.has_hor_line = 0;

  mainwindow->maincurve->crosshair_1.dot_sz = 4;

  mainwindow->mc_annot_ov_color_predefined[0] = QColor(255, 0, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[1] = QColor(0, 0, 128, 32);
  mainwindow->mc_annot_ov_color_predefined[2] = QColor(128, 128, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[3] = QColor(255, 85, 255, 32);
  mainwindow->mc_annot_ov_color_predefined[4] = QColor(255, 192, 0, 32);
  mainwindow->mc_annot_ov_color_predefined[5] = QColor(128, 192, 64, 32);
  mainwindow->mc_annot_ov_color_predefined[6] = QColor(64, 0, 192, 32);
  mainwindow->mc_annot_ov_color_predefined[7] = QColor(0, 255, 255, 32);

  mainwindow->ecg_view_mode = 1;

  update_interface();
}


void UI_OptionsDialog::lineedit4_1_changed(const QString qstr)
{
  char str1_32[32];

  strlcpy(str1_32, qstr.toUtf8().data(), 32);
  str_replace_ctrl_chars(str1_32, '!');
  utf8_set_byte_len(str1_32 ,31);
  trim_spaces(str1_32);

  if(strlen(mainwindow->ecg_qrs_rpeak_descr))
  {
    strlcpy(mainwindow->ecg_qrs_rpeak_descr, str1_32, 32);
  }
  else
  {
    strlcpy(mainwindow->ecg_qrs_rpeak_descr, "R-peak", 32);
  }
}


void UI_OptionsDialog::ApplyButton5Clicked()
{
  mainwindow->font_size = spinbox5_1->value();
  mainwindow->monofont_size = spinbox5_2->value();

  mainwindow->monofont->setPointSize(mainwindow->monofont_size);
  mainwindow->maincurve->setFont(*mainwindow->monofont);

  mainwindow->set_font_metrics(0);
  mainwindow->maincurve->update();

  QMessageBox::information(optionsdialog, "Font size changed", "You need to restart the application for the changes to take effect.");

  ApplyButton5->setEnabled(false);
}


void UI_OptionsDialog::spinBox5_1ValueChanged(int val)
{
  QFont normfont = *mainwindow->normfont;
  normfont.setPointSize(val);
  textEdit5_1->setFont(normfont);
  textEdit5_1->setPlainText(font_sz_example_txt);

  ApplyButton5->setEnabled(true);
}


void UI_OptionsDialog::spinBox5_2ValueChanged(int val)
{
  QFont monofont = *mainwindow->monofont;
  monofont.setPointSize(val);
  textEdit5_2->setFont(monofont);
  textEdit5_2->setPlainText(font_sz_example_txt);

  ApplyButton5->setEnabled(true);
}


void UI_OptionsDialog::DefaultButton5Clicked()
{
  spinbox5_1->setValue(mainwindow->sys_font_size);

  spinbox5_2->setValue(mainwindow->sys_monofont_size);
}


void UI_OptionsDialog::tabholder_idx_changed(int idx)
{
  mainwindow->options_dialog_idx = idx;
}


void UI_OptionsDialog::tab7_settings_changed()
{
  int i;

  for(i=0; i<8; i++)
  {
    if(checkbox7_1[i]->checkState() == Qt::Checked)
    {
      lineedit7_1[i]->setEnabled(true);
      checkbox7_8[i]->setEnabled(true);
      mainwindow->annot_edit_user_button_enabled[i] = 1;
    }
    else
    {
      lineedit7_1[i]->setEnabled(false);
      checkbox7_8[i]->setEnabled(false);
      mainwindow->annot_edit_user_button_enabled[i] = 0;
    }

    if(checkbox7_8[i]->checkState() == Qt::Checked)
    {
      mainwindow->annot_editor_user_button_onset_on_page_middle[i] = 1;
    }
    else
    {
      mainwindow->annot_editor_user_button_onset_on_page_middle[i] = 0;
    }

    strlcpy(mainwindow->annot_edit_user_button_name[i], lineedit7_1[i]->text().toUtf8().data(), 32);

    str_replace_ctrl_chars(mainwindow->annot_edit_user_button_name[i], '!');

    utf8_set_byte_len(mainwindow->annot_edit_user_button_name[i] ,31);

    trim_spaces(mainwindow->annot_edit_user_button_name[i]);
  }

  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    strlcpy(mainwindow->annot_by_rect_draw_description[i], ((QLineEdit *)annot_sidemenu_table->cellWidget(i, 0))->text().toUtf8().data(), 32);

    str_replace_ctrl_chars(mainwindow->annot_by_rect_draw_description[i], '!');

    utf8_set_byte_len(mainwindow->annot_by_rect_draw_description[i] ,31);

    trim_spaces(mainwindow->annot_by_rect_draw_description[i]);
  }

  if((mainwindow->annot_editor_active) && (mainwindow->annotationEditDock != NULL))
  {
    for(i=0; i<8; i++)
    {
      if(checkbox7_1[i]->checkState() == Qt::Checked)
      {
        mainwindow->annotationEditDock->user_button[i]->setVisible(true);
      }
      else
      {
        mainwindow->annotationEditDock->user_button[i]->setVisible(false);
      }

      mainwindow->annotationEditDock->user_button[i]->setText(lineedit7_1[i]->text());
    }

    mainwindow->annotationEditDock->annot_by_rect_draw_menu->clear();

    if(strlen(mainwindow->annot_by_rect_draw_description[0]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[0]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_0_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[1]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[1]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_1_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[2]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[2]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_2_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[3]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[3]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_3_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[4]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[4]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_4_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[5]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[5]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_5_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[6]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[6]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_6_clicked()));
    }
    if(strlen(mainwindow->annot_by_rect_draw_description[7]))
    {
      mainwindow->annotationEditDock->annot_by_rect_draw_menu->addAction(QString::fromUtf8(mainwindow->annot_by_rect_draw_description[7]), mainwindow->annotationEditDock, SLOT(annot_by_rect_draw_side_menu_7_clicked()));
    }
  }
}


void UI_OptionsDialog::grid_radio_group_clicked(int id)
{
  if(id == 0)
  {
    mainwindow->ecg_view_mode = 0;
  }
  else
  {
    mainwindow->ecg_view_mode = 1;

    for(int i=0; i<mainwindow->signalcomps; i++)
    {
      if(strcmp(mainwindow->signalcomp[i]->physdimension, "uV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "mV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "V"))
      {
        QMessageBox::warning(optionsdialog, "Warning",
                             "The physical dimension (unit) of one or more signals on the screen does not equal to uV, mV or V.\n"
                             "As a result, the grid's dimensions for these signals cannot be calculated correctly.");
        break;
      }
    }
  }

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::grid_radio_group_clicked(QAbstractButton *b)
{
  if(grid_radio_group->id(b) == 0)
  {
    mainwindow->ecg_view_mode = 0;
  }
  else
  {
    mainwindow->ecg_view_mode = 1;

    for(int i=0; i<mainwindow->signalcomps; i++)
    {
      if(strcmp(mainwindow->signalcomp[i]->physdimension, "uV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "mV") &&
         strcmp(mainwindow->signalcomp[i]->physdimension, "V"))
      {
        QMessageBox::warning(optionsdialog, "Warning",
                             "The physical dimension (unit) of one or more signals on the screen does not equal to uV, mV or V.\n"
                             "As a result, the grid's dimensions for these signals cannot be calculated correctly.");
        break;
      }
    }
  }

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::def_amp_radio_group_clicked(int id)
{
  if(id == 2)
  {
    dspinbox4_4->setEnabled(false);
    spinbox4_5->setEnabled(false);
    label_4_1->setEnabled(true);
    mainwindow->default_amplitude_use_physmax_div = 0;
    mainwindow->default_fit_signals_to_pane = 1;
  }
  else if(id == 1)
    {
      dspinbox4_4->setEnabled(false);
      label_4_1->setEnabled(false);
      spinbox4_5->setEnabled(true);
      mainwindow->default_amplitude_use_physmax_div = 1;
      mainwindow->default_fit_signals_to_pane = 0;
    }
    else
    {
      spinbox4_5->setEnabled(false);
      label_4_1->setEnabled(false);
      dspinbox4_4->setEnabled(true);
      mainwindow->default_amplitude_use_physmax_div = 0;
      mainwindow->default_fit_signals_to_pane = 0;
    }
}


void UI_OptionsDialog::def_amp_radio_group_clicked(QAbstractButton *b)
{
  if(def_amp_radio_group->id(b) == 1)
  {
    dspinbox4_4->setEnabled(false);
    spinbox4_5->setEnabled(true);
    mainwindow->default_amplitude_use_physmax_div = 1;
  }
  else
  {
    spinbox4_5->setEnabled(false);
    dspinbox4_4->setEnabled(true);
    mainwindow->default_amplitude_use_physmax_div = 0;
  }
}


void UI_OptionsDialog::spinbox4_5ValueChanged(int val)
{
  mainwindow->default_amplitude_physmax_div = val;
}


void UI_OptionsDialog::spinbox4_6ValueChanged(int val)
{
  mainwindow->mc_v_scrollarea_max_signals = val;

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::spinbox4_7ValueChanged(int val)
{
  mainwindow->default_time_scale = val;
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_1(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[0], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[0] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_2(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[1], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[1] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_3(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[2], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[2] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_4(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[3], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[3] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_5(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[4], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[4] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_6(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[5], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[5] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_7(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[6], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[6] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedButtonClicked_8(SpecialButton *b)
{
  QColor temp = QColorDialog::getColor(mainwindow->mc_annot_ov_color_predefined[7], tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    b->setColor(temp);
    mainwindow->mc_annot_ov_color_predefined[7] = temp;
    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationPredefinedLineEdit_changed(void)
{
  int i;

  for(i=0; i<MAX_MC_ANNOT_OV_COLORS; i++)
  {
    strlcpy(mainwindow->mc_annot_ov_name_predefined[i], AnnotDurationPredefinedLineEdit[i]->text().toUtf8().data(), 32);

    utf8_set_byte_len(mainwindow->mc_annot_ov_name_predefined[i], 31);

    str_replace_ctrl_chars(mainwindow->mc_annot_ov_name_predefined[i], '!');

    trim_spaces(mainwindow->mc_annot_ov_name_predefined[i]);
  }
}











