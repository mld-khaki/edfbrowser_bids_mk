/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013 - 2025 Teunis van Beelen
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
#include <strings.h>
#include <time.h>
#include <chrono>

#include <QApplication>
#include <QCoreApplication>
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
#include <QDateTime>
#include <QDir>


#include "mainwindow.h"


#include "reduce_signals.h"
#include "edf_compat.h"

FILE* g_output_file = NULL;

#ifdef _WIN32
#include <windows.h>

// Global file pointer for output
void attach_console_if_needed() {
    // If no console exists, create one
    if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) {
        AllocConsole();
    }

    // Redirect stdout to console
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$", "r", stdin);
}
#else
// Empty implementation for non-Windows platforms
void attach_console_if_needed() {}
#endif

// Function to initialize the output file
bool init_output_file(const char* edf_file_path)
{    // Extract base name and directory
    QFileInfo file_info(edf_file_path);
    QString base_name = file_info.completeBaseName();
    QString dir_path = file_info.absolutePath();
    
    // Create temporary output file name (will be renamed later based on pass/fail)
    QString out_filename = base_name + ".edf_temp";
    QString out_fullpath = QDir(dir_path).filePath(out_filename);
    
    // Get current timestamp
    QDateTime current_time = QDateTime::currentDateTime();
    QString timestamp = current_time.toString("yyyy-MM-dd HH:mm:ss");
    
    // Open file for writing
    g_output_file = fopen(out_fullpath.toLocal8Bit().constData(), "w");
    if (g_output_file == NULL) {
        qCritical() << "Failed to create output file:" << out_fullpath;
        return false;
    }
    
    // Write header to file
    fprintf(g_output_file, "Timestamp: %s\n", timestamp.toLocal8Bit().constData());
    fprintf(g_output_file, "File: %s\n", edf_file_path);
    fprintf(g_output_file, "\nOutput:\n");
    
    return true;
}

// Function to finalize the output file and rename it based on the result
void finalize_output_file(const char* edf_file_path, bool passed, double elapsed_time) {
    if (g_output_file == NULL) {
        qCritical() << "Output file not initialized";
        return;
    }
    
    // Write elapsed time at the end
    fprintf(g_output_file, "\nElapsed Time: %.2f seconds\n", elapsed_time);
    fclose(g_output_file);
    g_output_file = NULL;
    
    // Rename file with appropriate extension
    QFileInfo file_info(edf_file_path);
    QString base_name = file_info.completeBaseName();
    QString dir_path = file_info.absolutePath();
    
    QString temp_filename = base_name + ".edf_temp";
    QString result_ext = passed ? ".edf_pass" : ".edf_fail";
    QString final_filename = base_name + result_ext;
    
    QString temp_fullpath = QDir(dir_path).filePath(temp_filename);
    QString final_fullpath = QDir(dir_path).filePath(final_filename);

    // Remove existing file if it exists
    if (QFile::exists(final_fullpath)) {
        QFile::remove(final_fullpath);
    }
    
    // Rename temp file to final result file
    if (QFile::rename(temp_fullpath, final_fullpath)) {
        qDebug() << "Result saved to:" << final_fullpath;
    } else {
        qCritical() << "Failed to rename temporary file to:" << final_fullpath;
    }
}

// Replace tee_printf with direct file writing
void file_printf(const char* format, ...) {
    if (g_output_file == NULL) {
        // If output file isn't open, just print to console
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        return;
    }
    
    // Print to both console and file
    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);
    
    // Print to console
    vprintf(format, args1);
    
    // Print to file
    vfprintf(g_output_file, format, args2);
    fflush(g_output_file);  // Ensure it's written immediately
    
    va_end(args1);
    va_end(args2);
}

char buf[512];

// Updated main function with direct file output changes
int main(int argc, char *argv[]) {
#if defined(_MSC_VER) || defined(_MSC_FULL_VER) || defined(_MSC_BUILD)
#error "Wrong compiler or platform!"
#endif

    attach_console_if_needed();

    /* -------------------------
       Global CLI help/version (ADDED)
       ------------------------- */
    for(int i = 1; i < argc; i++)
    {
        if((!strcmp(argv[i], "--help")) || (!strcmp(argv[i], "-h")))
        {
            printf("\nEDFBrowser, cli updated by Dr. Milad Khaki 2026-03-03 LHSC Extension %s\n\n", PROGRAM_VERSION);
            printf("Usage:\n");
            printf("  edfbrowser [file.edf]\n");
            printf("  edfbrowser --check-compatibility <file.edf>\n");
            printf("  edfbrowser --reduce-signals [options]\n\n");

            printf("General options:\n");
            printf("  --help, -h                 Show this help\n");
            printf("  --version                  Show version\n\n");

            printf("Reduce Signals options:\n");
            printf("  --input <file>\n");
            printf("  --output <file>\n");
            printf("  --from \"YYYY-MM-DD HH:MM:SS\"\n");
            printf("  --to   \"YYYY-MM-DD HH:MM:SS\"\n");
            printf("  --mask-signals <pattern>\n");
            printf("  --signals <csv>\n");
            printf("  --sr-divider <int>\n");
            printf("  --aa-order <int>\n");
            printf("  --overwrite\n");
            printf("  --no-annotations\n\n");

            return EXIT_SUCCESS;
        }

        if(!strcmp(argv[i], "--version"))
        {
            printf("EDFBrowser version %s\n", PROGRAM_VERSION);
            return EXIT_SUCCESS;
        }
    }

    /* -------------------------
       Headless CLI: reduce signals / duration / samplerate
       ------------------------- */
    bool reduce_cli = false;
    for(int i=1; i<argc; i++)
    {
      if((!strcmp(argv[i], "--reduce-signal")) || (!strcmp(argv[i], "--reduce-signals")))
      {
        reduce_cli = true;
        break;
      }
    }

    if(reduce_cli)
    {
      QApplication app(argc, argv);

      reduce_signals_cli_options_t opt;
      memset(&opt, 0, sizeof(opt));
      opt.from_datarecord = 1;
      opt.to_datarecord = 0;
      opt.pre_offset_minutes = 0;
      opt.post_offset_minutes = 0;
      opt.aa_filter_order = 0;
      opt.read_annotations = 1;
      opt.overwrite_existing = 0;
      opt.global_samplerate_divider = 1;

      const char *in_path = NULL;
      const char *out_path = NULL;

      auto read_arg = [&](int &idx) -> const char *
      {
        if(idx + 1 >= argc) return NULL;
        idx++;
        return argv[idx];
      };

      for(int i=1; i<argc; i++)
      {
        const char *a = argv[i];

        if((!strcmp(a, "--reduce-signal")) || (!strcmp(a, "--reduce-signals")))
        {
          continue;
        }
        else if((!strcmp(a, "--input")) || (!strcmp(a, "--in")))
        {
          in_path = read_arg(i);
        }
        else if((!strcmp(a, "--output")) || (!strcmp(a, "--out")))
        {
          out_path = read_arg(i);
        }
        else if(!strcmp(a, "--from"))
        {
          const char *v = read_arg(i);
          if(v) strlcpy(opt.from_timestamp, v, sizeof(opt.from_timestamp));
        }
        else if(!strcmp(a, "--to"))
        {
          const char *v = read_arg(i);
          if(v) strlcpy(opt.to_timestamp, v, sizeof(opt.to_timestamp));
        }
        else if(!strcmp(a, "--pre-offset-min"))
        {
          const char *v = read_arg(i);
          if(v) opt.pre_offset_minutes = atoi(v);
        }
        else if(!strcmp(a, "--post-offset-min"))
        {
          const char *v = read_arg(i);
          if(v) opt.post_offset_minutes = atoi(v);
        }
        else if((!strcmp(a, "--signals")) || (!strcmp(a, "--reduce-signals-list")))
        {
          const char *v = read_arg(i);
          if(v) strlcpy(opt.signals_csv, v, sizeof(opt.signals_csv));
        }
        else if((!strcmp(a, "--mask-signals")) || (!strcmp(a, "--mask")))
        {
          const char *v = read_arg(i);
          if(v) strlcpy(opt.mask_signals, v, sizeof(opt.mask_signals));
        }
        else if((!strcmp(a, "--sr-divider")) || (!strcmp(a, "--samplerate-divider")))
        {
          const char *v = read_arg(i);
          if(v) opt.global_samplerate_divider = atoi(v);
        }
        else if((!strcmp(a, "--aa-order")) || (!strcmp(a, "--aa")))
        {
          const char *v = read_arg(i);
          if(v) opt.aa_filter_order = atoi(v);
        }
        else if(!strcmp(a, "--json"))
        {
          const char *v = read_arg(i);
          if(v) strlcpy(opt.json_config_path, v, sizeof(opt.json_config_path));
        }
        else if(!strcmp(a, "--overwrite"))
        {
          opt.overwrite_existing = 1;
        }
        else if(!strcmp(a, "--no-annotations"))
        {
          opt.read_annotations = 0;
        }
      }

      char errbuf[4096] = {0};
      int rc = reduce_signals_cli(in_path, out_path, &opt, errbuf, sizeof(errbuf));
      if(rc != 0)
      {
        if(errbuf[0] != 0)
        {
          fprintf(stderr, "[reduce-signals] Error: %s\n", errbuf);
        }
        else
        {
          fprintf(stderr, "[reduce-signals] Error: failed with code %d\n", rc);
        }
      }
      return rc;
    }

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
        return EXIT_FAILURE;
    }

    /* check endianness! */
    byte_order_test_var.one = 0x03020100;
    if ((byte_order_test_var.four[0] != 0) ||
        (byte_order_test_var.four[1] != 1) ||
        (byte_order_test_var.four[2] != 2) ||
        (byte_order_test_var.four[3] != 3)) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        return EXIT_FAILURE;
    }

#if defined(__LP64__)
    if (sizeof(long) != 8) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical() << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }
#else
    if (sizeof(long) != 4) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical() << "Wrong compiler or platform!\n";
        return EXIT_FAILURE;
    }
#endif

#if defined(__MINGW64__)
    if (sizeof(long) != 4) {
        fprintf(stderr, "Wrong compiler or platform!\n");
        qCritical() << "Wrong compiler or platform!\n";
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

    if (args.size() > 1) {
        qDebug() << "argument mode";
        if (strcasecmp(argv[1], "--check-compatibility") == 0)
        {
            qWarning() << "Found the argument";
            if(argc < 3)
            {
                fprintf(stderr, "Error: No filename specified for compatibility check\n");
                qCritical() << "Error: No filename specified for compatibility check";
                return EXIT_FAILURE;
            }

            // Check if file exists before proceeding
            QFileInfo check_file(argv[2]);
            if (!check_file.exists() || !check_file.isFile()) {
                fprintf(stderr, "Error: File does not exist or is not a regular file: %s\n", argv[2]);
                qCritical() << "Error: File does not exist or is not a regular file:" << argv[2];
                return EXIT_FAILURE;
            }

            // Skip processing if already done
            QString base_name = check_file.completeBaseName();
            QString dir_path = check_file.absolutePath();
            QString pass_file = QDir(dir_path).filePath(base_name + ".edf_pass");
            QString fail_file = QDir(dir_path).filePath(base_name + ".edf_fail");
            
            if (QFileInfo::exists(pass_file) || QFileInfo::exists(fail_file)) {                
                qDebug() << "Skipping (already processed):" << argv[2];
                return EXIT_SUCCESS;
            }

            // Start timing
            auto start_time = std::chrono::high_resolution_clock::now();

            // Initialize output file
            if (!init_output_file(argv[2])) {
                return EXIT_FAILURE;
            }

            FILE *inputfile;
            char txt_string[2048];
            edfhdrblck_t *edfhdr;

            file_printf("Opening input file: %s\n", argv[2]);
            qDebug() << "Opening input file:" << argv[2];
            
            inputfile = fopen(argv[2], "rb");
            if(inputfile == NULL)
            {
                file_printf("Error: Cannot open file '%s'\n", argv[2]);
                qCritical() << "Error: Cannot open file:" << argv[2];
                finalize_output_file(argv[2], false, 0.0);
                return EXIT_FAILURE;
            }
            
            txt_string[0] = 0;
            char str1_2048[2048]={""};

            file_printf("Starting EDF compatibility check for: %s\n", argv[2]);

            edfhdr = check_edf_file(inputfile, str1_2048, 2048, 0, 0);  // Parse header

            if (edfhdr == NULL) {
                fclose(inputfile);
                file_printf("Error: %s\n", str1_2048);
                qCritical() << "Error:" << str1_2048;
                
                // Calculate elapsed time
                auto end_time = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(end_time - start_time).count();
                
                // Finalize and rename output file
                finalize_output_file(argv[2], false, elapsed);
                
                return EXIT_FAILURE;
            }
            else
            {
                strcpy(buf,"EDF Header loaded correctly\r\n");
                file_printf(buf);
                qCritical() << buf;
            }

            char compat_err[4096] = {0};
            int result = check_edf_compatibility(edfhdr, inputfile, compat_err, sizeof(compat_err));

            file_printf(compat_err);

            // Determine if compatibility check passed
            bool check_passed = (result == 0);
            
            // Write detailed results directly to the file
            if (check_passed) {
                file_printf("Compatibility check passed: File is a valid %s%s file.\n",
                        edfhdr->edf ? "EDF" : "BDF", edfhdr->edfplus || edfhdr->bdfplus ? "+" : "");

                file_printf("Number of signals: %i\n", edfhdr->edfsignals);

                file_printf("Data record duration: %f\n", edfhdr->data_record_duration);
                file_printf("Recording length in Sec: %lli\n", edfhdr->recording_duration_hr);
                file_printf("Number of data records: %lli\n", edfhdr->datarecords);

                file_printf("Discontinuous state of file: <%d>\n", edfhdr->discontinuous);

                file_printf("Recording duration: %d seconds\n", edfhdr->recording_len_sec);
            } else {
                file_printf("Compatibility check failed: File is NOT a valid EDF or BDF file.\n");
                file_printf("Error: %s\n", compat_err);
            }
            
            // Calculate elapsed time
            auto end_time = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end_time - start_time).count();
            
            // Finalize and rename output file
            finalize_output_file(argv[2], check_passed, elapsed);

            // Clean up
            free(edfhdr->edfparam);
            free(edfhdr);
            fclose(inputfile);
            
            qDebug() << "Check completed!";
            return check_passed ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        

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
                    //MainWindow->setProperty("pending_open_file", filePath);

                    // Connect to a signal that will be emitted when the main window is
                    // shown
                    /* QObject::connect(MainWindow, &UI_Mainwindow::shown, [MainWindow]() {
                        QString pendingFile =
                            MainWindow->property("pending_open_file").toString();
                        if (!pendingFile.isEmpty()) {
                            // Call the appropriate public method
                            MainWindow->open_edf_file(pendingFile);
                        }
                    });*/
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
    } // <-- This is the correct closing brace for the if (args.size() > 1) statement

    QPixmap pixmap(":/images/splash.png");
  QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

    if (!(args.contains("--check-compatibility")))
    {

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

  char str1_512[512]="";

    UI_Mainwindow *MainWindow = new UI_Mainwindow;
    if (MainWindow == NULL) {
    splash.close();

    snprintf(str1_512, 512, "Malloc error.\nFile: %s  line: %i", __FILE__, __LINE__);

    fprintf(stderr, "%s\n", str1_512);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(str1_512);
    msgBox.exec();

        return EXIT_FAILURE;
    }

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


    int ret = app.exec();

    delete MainWindow;

    qDebug() << "Checked successfully!";
    return ret;
}

void UI_Mainwindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    emit shown();
}
