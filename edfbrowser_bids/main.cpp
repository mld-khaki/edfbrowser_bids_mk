/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013 - 2024 Teunis van Beelen
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


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include <QApplication>
#include <QObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QEventLoop>
#include <QMessageBox>
#include <QFileInfo>
#include <QString>


#include "mainwindow.h"


int main(int argc, char *argv[])
{
#if defined(_MSC_VER) || defined(_MSC_FULL_VER) || defined(_MSC_BUILD)
#error "Wrong compiler or platform!"
#endif

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "system is not little endian!"
#endif

  union
  {
    char four[4];
    int one;
  } byte_order_test_var;

  /* avoid surprises! */
  if((sizeof(char)      != 1) ||
     (sizeof(short)     != 2) ||
     (sizeof(int)       != 4) ||
     (sizeof(long long) != 8) ||
     (sizeof(float)     != 4) ||
     (sizeof(double)    != 8))
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }

  /* check endianness! */
  byte_order_test_var.one = 0x03020100;
  if((byte_order_test_var.four[0] != 0) ||
     (byte_order_test_var.four[1] != 1) ||
     (byte_order_test_var.four[2] != 2) ||
     (byte_order_test_var.four[3] != 3))
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }

#if defined(__LP64__)
  if(sizeof(long) != 8)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#else
  if(sizeof(long) != 4)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#endif

#if defined(__MINGW64__)
  if(sizeof(long) != 4)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#endif

#if (QT_VERSION >= 0x050600) && (QT_VERSION < 0x060000)
  QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#endif
#if (QT_VERSION >= 0x060000)
#ifdef Q_OS_WIN32
  qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
#endif
#endif
  QApplication app(argc, argv);

  QPixmap pixmap(":/images/splash.png");

  QPainter p(&pixmap);
  QFont sansFont("Noto Sans", 10);
  p.setFont(sansFont);
  p.setPen(Qt::black);
  if(!strcmp(PROGRAM_BETA_SUFFIX, ""))
  {
    p.drawText(250, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "    " THIS_APP_BITS_W);
  }
  else
  {
    p.drawText(50, 240, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "  " THIS_APP_BITS_W);
    p.drawText(50, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, PROGRAM_BETA_SUFFIX);
  }

  QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

  QTimer t1;
  t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1.setTimerType(Qt::PreciseTimer);
#endif
  QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));

  if(QCoreApplication::arguments().size()<2)
  {
    splash.show();

    t1.start(3000);

    QEventLoop evlp;
    QTimer::singleShot(100, &evlp, SLOT(quit()));
    evlp.exec();
  }

  qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

  char str1_512[512]="";

  UI_Mainwindow *MainWindow = new UI_Mainwindow;
  if(MainWindow == NULL)
  {
    splash.close();

    snprintf(str1_512, 512, "Malloc error.\nFile: %s  line: %i", __FILE__, __LINE__);

    fprintf(stderr, "%s\n", str1_512);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(str1_512);
    msgBox.exec();

    return EXIT_FAILURE;
  }

  // Process command line arguments to open EDF file
  QStringList args = QCoreApplication::arguments();
  if(args.size() > 1)
  {
    QString filePath = args.at(1);
    QFileInfo fileInfo(filePath);
    
    // Check if file exists
    if(fileInfo.exists())
    {
      // Check file extension to determine if it's an EDF file
      QString suffix = fileInfo.suffix().toLower();
      if(suffix == "edf" || suffix == "bdf" || suffix == "rec")
      {
        // We need to use the public open_new_file() method
        // The MainWindow has path variable and open_new_file() method to handle this
        
        // First, we need to make this file path available in MainWindow class
        // We can do this by setting the window property
        MainWindow->setProperty("pending_open_file", filePath);
        
        // Connect to a signal that will be emitted when the main window is shown
        QObject::connect(MainWindow, &UI_Mainwindow::shown, 
                        [MainWindow]() {
                            QString pendingFile = MainWindow->property("pending_open_file").toString();
                            if (!pendingFile.isEmpty()) {
                                // Call the appropriate public method
                                MainWindow->open_edf_file(pendingFile);
                            }
                        });
      }
      else
      {
        fprintf(stderr, "Warning: File %s is not an EDF/BDF/REC file.\n", 
                filePath.toLocal8Bit().constData());
      }
    }
    else
    {
      fprintf(stderr, "Error: File %s does not exist.\n", filePath.toLocal8Bit().constData());
    }
  }

  int ret = app.exec();

  delete MainWindow;

  return ret;
}

void UI_Mainwindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    emit shown();
}

/**
 * Opens an EDF file programmatically
 * This is used for command line opening of files
 */
void UI_Mainwindow::open_edf_file(const QString &file_path)
{
    if(file_path.isEmpty()) return;

    // Convert QString to char array
    QByteArray ba = file_path.toLocal8Bit();
    const char *file_path_str = ba.constData();

    // Check if we need to close any open files if annotations edited
    if(files_open && annotations_edited)
    {
        QMessageBox messagewindow;
        messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
        messagewindow.setIcon(QMessageBox::Question);
        messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
        messagewindow.setDefaultButton(QMessageBox::Cancel);
        if(messagewindow.exec() == QMessageBox::Cancel) {
            return;
        }
    }

    // If streaming is active, don't allow opening a file
    if(live_stream_active)
    {
        QMessageBox::critical(this, "Error", "You cannot open a file while a streaming file is open.\nClose the streaming file first.");
        return;
    }

    // Check if max files already open
    if(files_open >= MAXFILES)
    {
        QMessageBox::critical(this, "Error", "There are too many files opened.");
        return;
    }

    // Set path for opening
    strlcpy(path, file_path_str, MAX_PATH_LENGTH);

    // Set directory
    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

    // Use the existing command line mechanism
    cmdlineargument = 1;

    // Open the file
    open_new_file();
}


