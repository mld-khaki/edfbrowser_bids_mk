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




#include "colordialog.h"



UI_ColorMenuDialog::UI_ColorMenuDialog(int *newColor, QWidget *w_parent, int show_var_colors_button)
{
  colormenu_dialog = new QDialog(w_parent);

  colormenu_dialog->setMinimumSize(200, 265);
  colormenu_dialog->setWindowTitle("Color");
  colormenu_dialog->setModal(true);
  colormenu_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  color = newColor;

  *color = -1;

  show_var_colors = show_var_colors_button;

  ColorButton1 = new SpecialButton;
  ColorButton1->setMinimumSize(30, 30);
  ColorButton1->setColor(Qt::black);

  ColorButton2 = new SpecialButton;
  ColorButton2->setMinimumSize(30, 30);
  ColorButton2->setColor(Qt::red);

  ColorButton3 = new SpecialButton;
  ColorButton3->setMinimumSize(30, 30);
  ColorButton3->setColor(Qt::darkRed);

  ColorButton4 = new SpecialButton;
  ColorButton4->setMinimumSize(30, 30);
  ColorButton4->setColor(Qt::green);

  ColorButton5 = new SpecialButton;
  ColorButton5->setMinimumSize(30, 30);
  ColorButton5->setColor(Qt::darkGreen);

  ColorButton6 = new SpecialButton;
  ColorButton6->setMinimumSize(30, 30);
  ColorButton6->setColor(Qt::blue);

  ColorButton7 = new SpecialButton;
  ColorButton7->setMinimumSize(30, 30);
  ColorButton7->setColor(Qt::darkBlue);

  ColorButton8 = new SpecialButton;
  ColorButton8->setMinimumSize(30, 30);
  ColorButton8->setColor(Qt::cyan);

  ColorButton9 = new SpecialButton;
  ColorButton9->setMinimumSize(30, 30);
  ColorButton9->setColor(Qt::darkCyan);

  ColorButton10 = new SpecialButton;
  ColorButton10->setMinimumSize(30, 30);
  ColorButton10->setColor(Qt::magenta);

  ColorButton11 = new SpecialButton;
  ColorButton11->setMinimumSize(30, 30);
  ColorButton11->setColor(Qt::darkMagenta);

  ColorButton12 = new SpecialButton;
  ColorButton12->setMinimumSize(30, 30);
  ColorButton12->setColor(Qt::yellow);

  ColorButton13 = new SpecialButton;
  ColorButton13->setMinimumSize(30, 30);
  ColorButton13->setColor(Qt::darkYellow);

  ColorButton14 = new SpecialButton;
  ColorButton14->setMinimumSize(30, 30);
  ColorButton14->setColor(Qt::gray);

  ColorButton15 = new SpecialButton;
  ColorButton15->setMinimumSize(30, 30);
  ColorButton15->setColor(Qt::darkGray);

  ColorButton16 = new SpecialButton;
  ColorButton16->setMinimumSize(30, 30);
  ColorButton16->setColor(Qt::white);

  QGridLayout *gr = new QGridLayout;
  gr->addWidget(ColorButton1, 0, 0);
  gr->addWidget(ColorButton2, 0, 1);
  gr->addWidget(ColorButton3, 0, 2);
  gr->addWidget(ColorButton4, 0, 3);
  gr->addWidget(ColorButton5, 1, 0);
  gr->addWidget(ColorButton6, 1, 1);
  gr->addWidget(ColorButton7, 1, 2);
  gr->addWidget(ColorButton8, 1, 3);
  gr->addWidget(ColorButton9, 2, 0);
  gr->addWidget(ColorButton10, 2, 1);
  gr->addWidget(ColorButton11, 2, 2);
  gr->addWidget(ColorButton12, 2, 3);
  gr->addWidget(ColorButton13, 3, 0);
  gr->addWidget(ColorButton14, 3, 1);
  gr->addWidget(ColorButton15, 3, 2);
  gr->addWidget(ColorButton16, 3, 3);
  gr->setColumnStretch(4, 1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  if(show_var_colors)
  {
    varyColorButton = new QPushButton;
    varyColorButton->setText("Varying colors");
    varyColorButton->setToolTip("Use different colors for every signal");
    hlayout1->addWidget(varyColorButton);
    hlayout1->addStretch(1000);
  }

  CloseButton = new QPushButton;
  CloseButton->setText("Close");
  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addStretch(1000);
  hlayout2->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(gr);
  if(show_var_colors)
  {
    vlayout1->addSpacing(10);
    vlayout1->addLayout(hlayout1);
    vlayout1->addSpacing(10);
  }
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout2);
  vlayout1->addSpacing(10);

  colormenu_dialog->setLayout(vlayout1);

  QObject::connect(CloseButton,     SIGNAL(clicked()),                colormenu_dialog, SLOT(close()));
  QObject::connect(ColorButton1,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton1_pressed(SpecialButton *)));
  QObject::connect(ColorButton2,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton2_pressed(SpecialButton *)));
  QObject::connect(ColorButton3,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton3_pressed(SpecialButton *)));
  QObject::connect(ColorButton4,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton4_pressed(SpecialButton *)));
  QObject::connect(ColorButton5,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton5_pressed(SpecialButton *)));
  QObject::connect(ColorButton6,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton6_pressed(SpecialButton *)));
  QObject::connect(ColorButton7,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton7_pressed(SpecialButton *)));
  QObject::connect(ColorButton8,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton8_pressed(SpecialButton *)));
  QObject::connect(ColorButton9,    SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton9_pressed(SpecialButton *)));
  QObject::connect(ColorButton10,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton10_pressed(SpecialButton *)));
  QObject::connect(ColorButton11,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton11_pressed(SpecialButton *)));
  QObject::connect(ColorButton12,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton12_pressed(SpecialButton *)));
  QObject::connect(ColorButton13,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton13_pressed(SpecialButton *)));
  QObject::connect(ColorButton14,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton14_pressed(SpecialButton *)));
  QObject::connect(ColorButton15,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton15_pressed(SpecialButton *)));
  QObject::connect(ColorButton16,   SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton16_pressed(SpecialButton *)));
  if(show_var_colors)
  {
    QObject::connect(varyColorButton, SIGNAL(clicked()), this, SLOT(varyColorButtonClicked()));
  }

  colormenu_dialog->exec();
}


void UI_ColorMenuDialog::varyColorButtonClicked()
{
  *color = 127;

  colormenu_dialog->close();
}


void UI_ColorMenuDialog::ColorButton1_pressed(SpecialButton *)
{
  *color = Qt::black;

  colormenu_dialog->close();
}


void UI_ColorMenuDialog::ColorButton2_pressed(SpecialButton *)
{
  *color = Qt::red;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton3_pressed(SpecialButton *)
{
  *color = Qt::darkRed;

  colormenu_dialog->close();
}


void UI_ColorMenuDialog::ColorButton4_pressed(SpecialButton *)
{
  *color = Qt::green;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton5_pressed(SpecialButton *)
{
  *color = Qt::darkGreen;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton6_pressed(SpecialButton *)
{
  *color = Qt::blue;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton7_pressed(SpecialButton *)
{
  *color = Qt::darkBlue;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton8_pressed(SpecialButton *)
{
  *color = Qt::cyan;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton9_pressed(SpecialButton *)
{
  *color = Qt::darkCyan;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton10_pressed(SpecialButton *)
{
  *color = Qt::magenta;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton11_pressed(SpecialButton *)
{
  *color = Qt::darkMagenta;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton12_pressed(SpecialButton *)
{
  *color = Qt::yellow;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton13_pressed(SpecialButton *)
{
  *color = Qt::darkYellow;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton14_pressed(SpecialButton *)
{
  *color = Qt::gray;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton15_pressed(SpecialButton *)
{
  *color = Qt::darkGray;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton16_pressed(SpecialButton *)
{
  *color = Qt::white;

  colormenu_dialog->close();
}









