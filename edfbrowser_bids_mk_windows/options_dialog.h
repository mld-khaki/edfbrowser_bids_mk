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




#ifndef OPTIONSFORM1_H
#define OPTIONSFORM1_H



#include "qt_headers.h"

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "special_button.h"
#include "signalcurve.h"
#include "edf_annot_list.h"
#include "utils.h"


class UI_Mainwindow;



class UI_OptionsDialog : public QObject
{
  Q_OBJECT

public:
  UI_OptionsDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *optionsdialog;

QPushButton    *CloseButton,
               *colorSchema_Blue_on_Gray_Button,
               *colorSchema_NK_Button,
               *colorSchema_ECG_Button,
               *colorSchema_Dark_Button,
               *ApplyButton,
               *DefaultButton2,
               *ApplyButton2,
               *ApplyButton5,
               *DefaultButton5,
               *saveColorSchemaButton,
               *loadColorSchemaButton;

SpecialButton  *BgColorButton,
               *SrColorButton,
               *BrColorButton,
               *MrColorButton,
               *TxtColorButton,
               *SigColorButton,
               *BaseColorButton,
               *Crh1ColorButton,
               *Crh2ColorButton,
               *FrColorButton,
               *AnnotMkrButton,
               *AnnotMkrSelButton,
               *AnnotDurationButton,
               *AnnotDurationSelectedButton,
               *AnnotDurationPredefinedButton[MAX_MC_ANNOT_OV_COLORS],
               *slabel2_1,
               *slabel2_3,
               *annotlistdock_edited_txt_color_button;

QLabel         *label_4_1,
               *label2_2;

QSpinBox       *spinbox1_1,
               *spinbox2_1,
               *spinbox2_2,
               *spinbox4_1,
               *spinbox4_2,
               *spinbox4_3,
               *spinbox4_5,
               *spinbox4_6,
               *spinbox4_7,
               *spinbox5_1,
               *spinbox5_2,
               *spinbox7_1,
               *spinbox7_2;

QDoubleSpinBox *dspinbox3_2,
               *dspinbox4_4;

QComboBox      *combobox4_1,
               *combobox4_2,
               *combobox4_3;

QCheckBox      *checkbox1,
               *checkbox2,
               *checkbox3,
               *checkbox3_1,
               *checkbox4,
               *checkbox5,
               *checkbox6,
               *checkbox16,
               *checkbox4_1,
               *checkbox4_2,
               *checkbox4_3,
               *checkbox4_4,
               *checkbox4_5,
               *checkbox4_6,
               *checkbox4_7,
               *checkbox4_8,
               *checkbox4_9,
               *checkbox4_10,
               *checkbox4_11,
               *checkbox4_12,
               *checkbox4_13,
               *checkbox4_14,
               *checkbox4_15,
               *checkbox4_16,
               *checkbox4_17,
               *checkbox4_18,
               *checkbox2_1,
               *checkbox2_2,
               *checkbox2_3,
               *checkbox7_1[8],
               *checkbox7_8[8],
               *checkbox7_2,
               *checkbox7_3,
               *checkbox7_4,
               *checkbox7_5,
               *checkbox7_6,
               *checkbox7_7,
               *checkbox7_9;

QTableWidget   *colorBarTable,
               *annot_sidemenu_table;

QRadioButton   *radiobutton5_1,
               *radiobutton5_2,
               *radiobutton5_3,
               *grid_normal_radiobutton,
               *grid_ecg_radiobutton;

QLineEdit      *lineEdit3_1,
               *lineedit4_1,
               *lineedit7_1[8],
               *AnnotDurationPredefinedLineEdit[MAX_MC_ANNOT_OV_COLORS];

QGroupBox      *groupbox1,
               *groupbox2;

QTextEdit      *textEdit5_1,
               *textEdit5_2;

QTabWidget     *tabholder;

QWidget        *tab1,
               *tab2,
               *tab3,
               *tab4,
               *tab5,
               *tab6,
               *tab7;

QButtonGroup   *def_amp_radio_group,
               *grid_radio_group;

void update_interface(void);

private slots:

void grid_radio_group_clicked(int);
void grid_radio_group_clicked(QAbstractButton *);
void def_amp_radio_group_clicked(int);
void def_amp_radio_group_clicked(QAbstractButton *);
void BgColorButtonClicked(SpecialButton *);
void SrColorButtonClicked(SpecialButton *);
void BrColorButtonClicked(SpecialButton *);
void MrColorButtonClicked(SpecialButton *);
void TxtColorButtonClicked(SpecialButton *);
void SigColorButtonClicked(SpecialButton *);
void BaseColorButtonClicked(SpecialButton *);
void Crh1ColorButtonClicked(SpecialButton *);
void Crh2ColorButtonClicked(SpecialButton *);
void FrColorButtonClicked(SpecialButton *);
void AnnotMkrButtonClicked(SpecialButton *);
void AnnotMkrSelButtonClicked(SpecialButton *);
void AnnotDurationButtonClicked(SpecialButton *);
void AnnotDurationSelectedButtonClicked(SpecialButton *);
void checkbox1Clicked(int);
void checkbox2Clicked(int);
void checkbox2_1Clicked(int);
void checkbox2_2Clicked(int);
void checkbox2_3Clicked(int);
void checkbox3Clicked(int);
void checkbox4Clicked(int);
void checkbox5Clicked(int);
void annotlistdock_edited_txt_color_button_clicked(SpecialButton *);
void checkbox6Clicked(int);
void checkbox16Clicked(int);
void checkbox3_1Clicked(int);
void checkbox4_1Clicked(int);
void checkbox4_2Clicked(int);
void checkbox4_3Clicked(int);
void checkbox4_4Clicked(int);
void checkbox4_5Clicked(int);
void checkbox4_6Clicked(int);
void checkbox4_7Clicked(int);
void checkbox4_8Clicked(int);
void checkbox4_9Clicked(int);
void checkbox4_10Clicked(int);
void checkbox4_11Clicked(int);
void checkbox4_12Clicked(int);
void checkbox4_13Clicked(int);
void checkbox4_14Clicked(int);
void checkbox4_15Clicked(int);
void checkbox4_16Clicked(int);
void checkbox4_17Clicked(int);
void checkbox4_18Clicked(int);
void checkbox7_2Clicked(int);
void checkbox7_3Clicked(int);
void checkbox7_4Clicked(int);
void checkbox7_5Clicked(int);
void checkbox7_6Clicked(int);
void checkbox7_7Clicked(int);
void checkbox7_9Clicked(int);
void ApplyButtonClicked();
void colorBarButtonClicked(SpecialButton *);
void DefaultButton2Clicked();
void checkBoxChanged(int);
void ApplyButton2Clicked();
void radioButton5Toggled(bool);
void spinBox1_1ValueChanged(int);
void spinBoxValueChanged(double);
void dspinBox3_2ValueChanged(double);
void labelEdited(const QString  &);
void calibrate_checkbox_stateChanged(int);
void spinBox4_1ValueChanged(int);
void combobox4_1IndexChanged(int);
void combobox4_2IndexChanged(int);
void combobox4_3IndexChanged(int);
void spinBox4_2ValueChanged(int);
void spinBox4_3ValueChanged(int);
void spinbox4_5ValueChanged(int);
void spinbox4_6ValueChanged(int);
void spinbox4_7ValueChanged(int);
void saveColorSchemaButtonClicked();
void loadColorSchemaButtonClicked();
void loadColorSchema_NK();
void loadColorSchema_Dark();
void loadColorSchema_blue_gray();
void loadColorSchema_ECG();
void dspinbox4_4ValueChanged(double);
void lineedit4_1_changed(const QString);
void spinBox5_1ValueChanged(int);
void spinBox5_2ValueChanged(int);
void ApplyButton5Clicked();
void DefaultButton5Clicked();
void spinBox7_1ValueChanged(int);
void spinBox7_2ValueChanged(int);
void tab7_settings_changed();
void tabholder_idx_changed(int);
void AnnotDurationPredefinedButtonClicked_1(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_2(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_3(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_4(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_5(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_6(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_7(SpecialButton *);
void AnnotDurationPredefinedButtonClicked_8(SpecialButton *);
void AnnotDurationPredefinedLineEdit_changed(void);

};



#endif // OPTIONSFORM1_H


