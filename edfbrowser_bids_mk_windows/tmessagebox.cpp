/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2024 Teunis van Beelen
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

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QFrame>
#include <QIcon>
#include <QStyle>
#include <QFontMetrics>
#include <QTextEdit>

#include "tmessagebox.h"


tmessagebox::tmessagebox(QMessageBox::Icon icon, const char *title_txt, const char *info_txt, const char *detailed_txt, QWidget *w_parent)
{
  QDialog *dlg = new QDialog(w_parent);
  dlg->setWindowTitle(title_txt);
  dlg->setSizeGripEnabled(true);
  dlg->setModal(true);
  dlg->setAttribute(Qt::WA_DeleteOnClose, true);

  QFontMetrics fm(dlg->font());
  double h_scaling = fm.height() / 18.0;
  double w_scaling = fm.tightBoundingRect("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").width() / 260.0;

  dlg->setMinimumSize(400 * w_scaling, 300 * h_scaling);

  int pxmap_h = 40 * w_scaling,
      pxmap_w = 40 * w_scaling;

  QLabel *info_txt_label = new QLabel;
  info_txt_label->setText(info_txt);

  QLabel *icon_label = new QLabel;
  if(icon == QMessageBox::Critical)
  {
    icon_label->setPixmap(dlg->style()->standardIcon(QStyle::SP_MessageBoxCritical).pixmap(pxmap_h, pxmap_w));
  }
  else if(icon == QMessageBox::Question)
    {
      icon_label->setPixmap(dlg->style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(pxmap_h, pxmap_w));
    }
    else if(icon == QMessageBox::Information)
      {
        icon_label->setPixmap(dlg->style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(pxmap_h, pxmap_w));
      }
      else if(icon == QMessageBox::Warning)
        {
          icon_label->setPixmap(dlg->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(pxmap_h, pxmap_w));
        }

  QTextEdit *text_edit = new QTextEdit;
  text_edit->setReadOnly(true);
  text_edit->setLineWrapMode(QTextEdit::NoWrap);
  text_edit->append(detailed_txt);
  int w = text_edit->document()->idealWidth() +
          text_edit->document()->documentMargin() +
          text_edit->verticalScrollBar()->width() + 10;
  if(w > 900) w = 900;
  text_edit->setMinimumWidth(w);
  if((w + 40) > (400 * w_scaling))
  {
    dlg->setMinimumWidth(w + 40);
  }
  int h = text_edit->document()->lineCount() * 25;
  if(h > 700)  h = 700;
  if(h > (200 * h_scaling))
  {
    dlg->setMinimumHeight(h);
  }
  text_edit->moveCursor(QTextCursor::Start);
  text_edit->verticalScrollBar()->setValue(text_edit->verticalScrollBar()->minimum());

  QPushButton *close_button = new QPushButton;
  close_button->setText("Close");

  QFrame *frame = new QFrame;
  frame->setFrameStyle(QFrame::HLine | QFrame::Plain);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(icon_label);
  hlayout1->addStretch(150);
  hlayout1->addWidget(info_txt_label);
  hlayout1->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addStretch(1000);
  hlayout2->addWidget(close_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);
  vlayout1->addWidget(frame);
  vlayout1->addWidget(text_edit, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout2);

  dlg->setLayout(vlayout1);

  QObject::connect(close_button, SIGNAL(clicked()), dlg, SLOT(close()));

  dlg->exec();
};







