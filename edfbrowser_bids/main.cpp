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

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <QApplication>
#include <QColor>
#include <QEventLoop>
#include <QFileInfo>
#include <QFont>
#include <QMessageBox>
#include <QObject>
#include <QPen>
#include <QPixmap>
#include <QSplashScreen>
#include <QString>
#include <QTimer>
#include <QDebug>


#include "mainwindow.h"
#include "edf_compat.h"

int main(int argc, char *argv[]) {
#if defined(_MSC_VER) || defined(_MSC_FULL_VER) || defined(_MSC_BUILD)
#error "Wrong compiler or platform!"
#endif

#ifdef QT_DEBUG
    qDebug() << "Running in Debug mode";
#else
    qDebug() << "Running in Release mode";
#endif

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "system is not little endian!"
#endif

    union {
        char four[4];
        int one;
    } byte_order_test_var;

    /* avoid surprises! */
    if ((sizeof(char) != 1) || (sizeof(short) != 2) || (sizeof(int) != 4) ||
        (sizeof(long long) != 8) || (sizeof(float) != 4) ||
        (sizeof(double) != 8)) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical() << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }

    /* check endianness! */
    byte_order_test_var.one = 0x03020100;
    if ((byte_order_test_var.four[0] != 0) ||
        (byte_order_test_var.four[1] != 1) ||
        (byte_order_test_var.four[2] != 2) ||
        (byte_order_test_var.four[3] != 3)) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical() << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }

#if defined(__LP64__)
    if (sizeof(long) != 8) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical () << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }
#else
    if (sizeof(long) != 4) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical () << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }
#endif

#if defined(__MINGW64__)
    if (sizeof(long) != 4) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical () << "Wrong compiler or platform!\n";
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

    //========
    // Process command line arguments to open EDF file
    QStringList args = QCoreApplication::arguments();
    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

    if (!(args.contains("--check-compatibility"))) {


        QPainter p(&pixmap);
        QFont sansFont("Noto Sans", 10);
        p.setFont(sansFont);
        p.setPen(Qt::black);
        if (!strcmp(PROGRAM_BETA_SUFFIX, "")) {
            p.drawText(250, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine,
                       "version " PROGRAM_VERSION "    " THIS_APP_BITS_W);
        } else {
            p.drawText(50, 240, 300, 30, Qt::AlignLeft | Qt::TextSingleLine,
                       "version " PROGRAM_VERSION "  " THIS_APP_BITS_W);
            p.drawText(50, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine,
                       PROGRAM_BETA_SUFFIX);
        }



        if (args.size() < 2) {
            splash.show();
            QTimer t1;
            t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
            t1.setTimerType(Qt::PreciseTimer);
#endif
            QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));
            t1.start(3000);
            QEventLoop evlp;
            QTimer::singleShot(100, &evlp, SLOT(quit()));
            evlp.exec();
        }

        qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

        UI_Mainwindow *MainWindow = new UI_Mainwindow;
        if (MainWindow == NULL) {
            splash.close();
            // ... error handling as before
        }

        // Attach MainWindow logic here

        int ret = app.exec();
        delete MainWindow;
        return ret;
    }

    //========

    QTimer t1;
    t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
    t1.setTimerType(Qt::PreciseTimer);
#endif
    QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));

    if (QCoreApplication::arguments().size() < 2) {
        splash.show();

        t1.start(3000);

        QEventLoop evlp;
        QTimer::singleShot(100, &evlp, SLOT(quit()));
        evlp.exec();
    }

    qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

    char str1_512[512] = "";

    UI_Mainwindow *MainWindow = new UI_Mainwindow;
    if (MainWindow == NULL) {
        splash.close();

        snprintf(str1_512, 512, "Malloc error.\nFile: %s  line: %i", __FILE__,
                 __LINE__);

        fprintf(stderr, "%s\n", str1_512);

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(str1_512);
        msgBox.exec();
        qCritical() << "Malloc error.\nFile: " <<  __FILE__ << "line: " << __LINE__;
        return EXIT_FAILURE;
    }

    if (args.size() > 1) {
        qDebug() << "argument mode";
        if (strcmpi(argv[1], "--check-compatibility") == 0)
        {
            qWarning() << "Found the argument";
            if(argc < 3)
            {
                qCritical() << "Error: No filename specified for compatibility check\n";
                qCritical() << "Usage: edfbrowser --check-compatibility filename.edf\n";
                return EXIT_FAILURE;
            }

            FILE *inputfile;
            char txt_string[2048];
            edfhdrblck_t *edfhdr;

            qDebug() << "Opening input file: " << argv[2];
            inputfile = fopen(argv[2], "rb");
            if(inputfile == NULL)
            {
                qCritical() << "Error: Cannot open file '"<<  argv[2] << "'\r\n";
                return EXIT_FAILURE;
            }
            txt_string[0] = 0;
            char str1_2048[2048]={""};

            edfhdr = check_edf_file(inputfile, str1_2048, 2048, 0, 0);  // You still need this to parse header

            if (edfhdr == NULL) {
                fclose(inputfile);
                qCritical() << "Error: " << str1_2048 << "\r\n";
                return EXIT_FAILURE;
            }

            char compat_err[2048] = {0};
            int result = check_edf_compatibility(edfhdr, inputfile, compat_err, sizeof(compat_err));

            if (result != 0) {
                qCritical() << "Compatibility check failed:\n" << compat_err;
                fclose(inputfile);
                free(edfhdr->edfparam);
                free(edfhdr);
                fprintf(stdout, "Compatibility check failed: File is a NOT a valid EDF or BDF file.\n");
                return EXIT_FAILURE;
            }

            fprintf(stdout, "Compatibility check passed: File is a valid %s%s file.\n",
                    edfhdr->edf ? "EDF" : "BDF", edfhdr->edfplus || edfhdr->bdfplus ? "+" : "");
            fprintf(stdout, "Number of signals: %i\n", edfhdr->edfsignals);
            fprintf(stdout, "Number of data records: %lli\n", edfhdr->datarecords);
            fprintf(stdout, "Recording duration: %d seconds\n", edfhdr->recording_len_sec);

            free(edfhdr->edfparam);
            free(edfhdr);
            fclose(inputfile);
            qDebug() << "Checked successfully!";
            return EXIT_SUCCESS;

            if(edfhdr == NULL)
            {
                fclose(inputfile);
                qCritical() << "Error: " << txt_string << "\r\n";
                return EXIT_FAILURE;
            }

            fprintf(stdout, "File is a valid %s file.\n", edfhdr->edf ? "EDF" : "BDF");
            fprintf(stdout, "Number of signals: %i\n", edfhdr->edfsignals);
            fprintf(stdout, "Number of data records: %lli\n", edfhdr->datarecords);
            fprintf(stdout, "Recording duration: %d seconds\n", edfhdr->recording_len_sec);

            free(edfhdr->edfparam);
            free(edfhdr);
            fclose(inputfile);
            qDebug() << "Checked successfully!";
            return EXIT_SUCCESS;
        }
        else
        {
            qDebug("Proceed to opeining the file if it exists");

            QString filePath = args.at(1);
            QFileInfo fileInfo(filePath);

            // Check if file exists
            if(fileInfo.exists())
            {
                // Check file extension to determine if it's an EDF file
                QString suffix = fileInfo.suffix().toLower();
                if (suffix == "edf" || suffix == "bdf" || suffix == "rec") {
                    // We need to use the public open_new_file() method
                    // The MainWindow has path variable and open_new_file() method to handle
                    // this

                    // First, we need to make this file path available in MainWindow class
                    // We can do this by setting the window property
                    MainWindow->setProperty("pending_open_file", filePath);

                    // Connect to a signal that will be emitted when the main window is
                    // shown
                    QObject::connect(MainWindow, &UI_Mainwindow::shown, [MainWindow]() {
                        QString pendingFile =
                            MainWindow->property("pending_open_file").toString();
                        if (!pendingFile.isEmpty()) {
                            // Call the appropriate public method
                            MainWindow->open_edf_file(pendingFile);
                        }
                    });
                } else {
                    fprintf(stderr, "Warning: File %s is not an EDF/BDF/REC file.\n",
                            filePath.toLocal8Bit().constData());
                }
            }
            else
            {
                fprintf(stderr, "Error: File %s does not exist.\n",
                        filePath.toLocal8Bit().constData());
            }
        }
    }

    int ret = app.exec();

    delete MainWindow;

    qDebug() << "Checked successfully!";
    return ret;
}

void UI_Mainwindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    emit shown();
}
