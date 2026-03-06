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



#include "edf_compat.h"
#include "mainwindow.h"
#include <string>
#include <cstdarg>
#include <vector>
#include <algorithm>

// Append printf-formatted text to a std::string (portable)
static void appendf(std::string &out, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // First pass: compute required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed > 0) {
        std::vector<char> buf(static_cast<size_t>(needed) + 1);
        vsnprintf(buf.data(), buf.size(), fmt, args);
        out.append(buf.data());
    }

    va_end(args);
}

// Copy std::string into errmsg safely (null-terminated)
static void copy_to_errmsg(const std::string &src, char *errmsg, int errmsg_len)
{
    if (!errmsg || errmsg_len <= 0) return;
    size_t n = std::min(src.size(), static_cast<size_t>(errmsg_len - 1));
    memcpy(errmsg, src.data(), n);
    errmsg[n] = '\0';
}


UI_EDFCompatwindow::UI_EDFCompatwindow(QWidget *w_parent)
{
    int i;

    mainwindow = (UI_Mainwindow *)w_parent;

    EDFCompatDialog = new QDialog;

    EDFCompatDialog->setMinimumSize(600 * mainwindow->w_scaling, 180 * mainwindow->h_scaling);
    EDFCompatDialog->setWindowTitle("Check EDF(+) / BDF(+) compatibility");
    EDFCompatDialog->setModal(true);
    EDFCompatDialog->setAttribute(Qt::WA_DeleteOnClose, true);

    filelist = new QListWidget;
    filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
    filelist->setSelectionMode(QAbstractItemView::SingleSelection);
    for(i=0; i<mainwindow->files_open; i++)
    {
        if((mainwindow->edfheaderlist[i]->edf)||(mainwindow->edfheaderlist[i]->bdf))
        {
            new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
        }
    }

    CheckButton = new QPushButton;
    CheckButton->setText("Check");

    CloseButton = new QPushButton;
    CloseButton->setText("Close");

    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(CheckButton);
    hlayout1->addStretch(1000);
    hlayout1->addWidget(CloseButton);

    QVBoxLayout *vlayout1 = new QVBoxLayout;
    vlayout1->addWidget(filelist, 1000);
    vlayout1->addSpacing(20);
    vlayout1->addLayout(hlayout1);

    EDFCompatDialog->setLayout(vlayout1);

    QObject::connect(CloseButton,  SIGNAL(clicked()), EDFCompatDialog, SLOT(close()));
    QObject::connect(CheckButton,  SIGNAL(clicked()), this,            SLOT(CheckButtonClicked()));

    filelist->setCurrentRow(mainwindow->files_open - 1);

    EDFCompatDialog->exec();
}





void UI_EDFCompatwindow::CheckButtonClicked_prv()
{
    int i, j, k, p, r=0, n, q,
        len,
        edfsignals,
        datarecords,
        recordsize,
        edfplus,
        discontinuous,
        bdf,
        bdfplus,
            *annot_ch=NULL,
        nr_annot_chns,
        max,
        onset,
        duration,
        duration_start,
        zero,
        max_tal_ln,
        error,
        temp,
        samplesize,
        annots_in_tal,
        progress_steps,
        annots_in_record;

    char *scratchpad=NULL,
        *cnv_buf=NULL,
        str1_2048[2048]={""};

    double data_record_duration,
        elapsedtime,
        time_tmp=0;

    union {
        unsigned int one;
        signed int one_signed;
        unsigned short two[2];
        signed short two_signed[2];
        unsigned char four[4];
    } var;

    FILE *inputfile=NULL;

    edfpar_t *edfparam=NULL;


    if(!mainwindow->files_open)
    {
        EDFCompatDialog->close();
        return;
    }

    CheckButton->setEnabled(false);
    CloseButton->setEnabled(false);

    for(i=0; i<mainwindow->files_open; i++)
    {
        if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
        {
            break;
        }
    }

    if(i==mainwindow->files_open)  return;

    n = i;

    inputfile = mainwindow->edfheaderlist[n]->file_hdl;

    edfsignals = mainwindow->edfheaderlist[n]->edfsignals;
    recordsize = mainwindow->edfheaderlist[n]->recordsize;
    edfparam = mainwindow->edfheaderlist[n]->edfparam;
    nr_annot_chns = mainwindow->edfheaderlist[n]->nr_annot_chns;
    datarecords = mainwindow->edfheaderlist[n]->datarecords;
    data_record_duration = mainwindow->edfheaderlist[n]->data_record_duration;
    edfplus = mainwindow->edfheaderlist[n]->edfplus;
    bdfplus = mainwindow->edfheaderlist[n]->bdfplus;
    discontinuous = mainwindow->edfheaderlist[n]->discontinuous;
    annot_ch = mainwindow->edfheaderlist[n]->annot_ch;
    bdf = mainwindow->edfheaderlist[n]->bdf;

    if(bdf)  samplesize = 3;
    else  samplesize = 2;

    cnv_buf = (char *)calloc(1, recordsize);
    if(cnv_buf==NULL)
    {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error. (cnv_buf).");
        messagewindow.exec();
        EDFCompatDialog->close();
        return;
    }

    max_tal_ln = 0;

    for(i=0; i<nr_annot_chns; i++)
    {
        if(max_tal_ln<edfparam[annot_ch[i]].smp_per_record * samplesize)  max_tal_ln = edfparam[annot_ch[i]].smp_per_record * samplesize;
    }

    if(max_tal_ln<128)  max_tal_ln = 128;

    scratchpad = (char *)calloc(1, max_tal_ln + 3);
    if(scratchpad==NULL)
    {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "malloc error. (scratchpad).");
        messagewindow.exec();
        free(cnv_buf);
        EDFCompatDialog->close();
        return;
    }

    if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
    {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile.");
        messagewindow.exec();
        free(cnv_buf);
        free(scratchpad);
        EDFCompatDialog->close();
        return;
    }

    /***************** check the maximum and minimum of samples of all signals ******************************/

    QProgressDialog progress("Checking...", "Abort", 0, datarecords, EDFCompatDialog);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(200);

    progress_steps = datarecords / 100;
    if(progress_steps < 1)
    {
        progress_steps = 1;
    }

    elapsedtime = 0.0;

    for(i=0; i<datarecords; i++)
    {
        if(!(i%progress_steps))
        {
            progress.setValue(i);

            qApp->processEvents();

            if(progress.wasCanceled() == true)
            {
                free(cnv_buf);
                free(scratchpad);
                EDFCompatDialog->close();
                return;
            }
        }

        if(fread(cnv_buf, recordsize, 1, inputfile)!=1)
        {
            progress.reset();
            snprintf(str1_2048, 2048, "An error occurred while reading inputfile at datarecord %i.", i + 1);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
            messagewindow.exec();
            free(cnv_buf);
            free(scratchpad);
            EDFCompatDialog->close();
            return;
        }

        for(j=0; j<edfsignals; j++)
        {
            for(k=0; k<edfparam[j].smp_per_record; k++)
            {
                if(bdf)
                {
                    var.two[0] = *((unsigned short *)(cnv_buf + edfparam[j].datrec_offset + (k * 3)));
                    var.four[2] = *(cnv_buf + edfparam[j].datrec_offset + (k * 3) + 2);

                    if(var.four[2]&0x80)
                    {
                        var.four[3] = 0xff;
                    }
                    else
                    {
                        var.four[3] = 0x00;
                    }

                    temp = var.one_signed;
                }
                else
                {
                    temp = *((signed short *)(cnv_buf + edfparam[j].datrec_offset + (k * 2)));
                }

                if(temp > edfparam[j].dig_max)
                {
                    progress.reset();
                    snprintf(str1_2048, 2048, "Error.\n\nIn datarecord %i -> signal %i -> sample %i is more than digital maximum.\n"
                                              "Digital maximum for this signal as written in header is %i but sample is %i.\n"
                                              "Offset from start of file: 0x%X\n",
                             i + 1,
                             j + 1,
                             k + 1,
                             edfparam[j].dig_max,
                             temp,
                             (i * recordsize) + edfparam[j].datrec_offset + (k * samplesize) + (edfsignals * 256) + 256);
                    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
                    messagewindow.exec();
                    free(scratchpad);
                    free(cnv_buf);
                    EDFCompatDialog->close();
                    return;
                }

                if(temp < edfparam[j].dig_min)
                {
                    progress.reset();
                    snprintf(str1_2048, 2048, "Error.\n\nIn datarecord %i -> signal %i -> sample %i is less than digital minimum.\n"
                                              "Digital minimum for this signal as written in header is %i but sample is %i.\n"
                                              "Offset from start of file: 0x%X\n",
                             i + 1,
                             j + 1,
                             k + 1,
                             edfparam[j].dig_min,
                             temp,
                             (i * recordsize) + edfparam[j].datrec_offset + (k * samplesize) + (edfsignals * 256) + 256);
                    QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
                    messagewindow.exec();
                    free(cnv_buf);
                    free(scratchpad);
                    EDFCompatDialog->close();
                    return;
                }
            }
        }

        /************** process annotationsignals (if any) **************/

        error = 0;

        for(r=0; r<nr_annot_chns; r++)
        {
            n = 0;
            zero = 0;
            onset = 0;
            duration = 0;
            duration_start = 0;
            scratchpad[0] = 0;
            annots_in_tal = 0;
            annots_in_record = 0;

            p = edfparam[annot_ch[r]].datrec_offset;
            max = edfparam[annot_ch[r]].smp_per_record * samplesize;

            /************** process one annotation signal ****************/

            if(cnv_buf[p + max - 1]!=0)
            {
                error = 5;
                goto END;
            }

            if(!r)  /* if it's the first annotation signal, then check */
            {       /* the timekeeping annotation */
                error = 1;

                for(k=0; k<(max-2); k++)
                {
                    scratchpad[k] = cnv_buf[p + k];

                    if(scratchpad[k]==20)
                    {
                        if(cnv_buf[p + k + 1]!=20)
                        {
                            error = 6;
                            goto END;
                        }
                        scratchpad[k] = 0;
                        if(is_onset_number(scratchpad))
                        {
                            error = 36;
                            goto END;
                        }
                        else
                        {
                            time_tmp = atof(scratchpad);
                            if(i)
                            {
                                if(discontinuous)
                                {
                                    if(time_tmp<elapsedtime)
                                    {
                                        error = 4;
                                        goto END;
                                    }
                                }
                                else
                                {
                                    if(((time_tmp-elapsedtime)>(data_record_duration+0.00000001))||((time_tmp-elapsedtime)<(data_record_duration-0.00000001)))
                                    {
                                        error = 3;
                                        goto END;
                                    }
                                }
                            }
                            else
                            {
                                if(time_tmp>=1.0)
                                {
                                    error = 2;
                                    goto END;
                                }
                            }
                            elapsedtime = time_tmp;
                            error = 0;
                            break;
                        }
                    }
                }
            }

            for(k=0; k<max; k++)
            {
                scratchpad[n] = cnv_buf[p + k];

                if(!scratchpad[n])
                {
                    if(!zero)
                    {
                        if(k)
                        {
                            if(cnv_buf[p + k - 1]!=20)
                            {
                                error = 33;
                                goto END;
                            }
                        }
                        else
                        {
                            zero++;
                        }
                        n = 0;
                        onset = 0;
                        duration = 0;
                        duration_start = 0;
                        scratchpad[0] = 0;
                        annots_in_tal = 0;
                    }
                    zero++;
                    continue;
                }
                if(zero>1)
                {
                    error = 34;
                    goto END;
                }
                zero = 0;

                if((scratchpad[n]==20)||(scratchpad[n]==21))
                {
                    if(scratchpad[n]==21)
                    {
                        if(duration||duration_start||onset||annots_in_tal)
                        {               /* it's not allowed to have multiple duration fields */
                            error = 35;   /* in one TAL or to have a duration field which is   */
                            goto END;     /* not immediately behind the onsetfield             */
                        }
                        duration_start = 1;
                    }

                    if((scratchpad[n]==20)&&onset&&(!duration_start))
                    {
                        if(r||annots_in_record)
                        {
                            if(n >= 0)
                            {
                                for(q=0; q<n; q++)
                                {
                                    if(q==MAX_ANNOTATION_LEN)  break;
                                    if((((unsigned char *)scratchpad)[q] < 32) || (((unsigned char *)scratchpad)[q] == 127))
                                    {
                                        if((scratchpad[q] != '\t') &&
                                            (scratchpad[q] != '\n') &&
                                            (scratchpad[q] != '\r'))
                                        {
                                            error = 38;  /* non-printable characters in description string */
                                            goto END;
                                        }
                                    }
                                }
                            }
                        }
                        annots_in_tal++;
                        annots_in_record++;
                        n = 0;
                        continue;
                    }

                    if(!onset)
                    {
                        scratchpad[n] = 0;
                        if(is_onset_number(scratchpad))
                        {
                            error = 36;
                            goto END;
                        }
                        onset = 1;
                        n = 0;
                        continue;
                    }

                    if(duration_start)
                    {
                        scratchpad[n] = 0;
                        if(is_duration_number(scratchpad))
                        {
                            error = 37;
                            goto END;
                        }
                        duration = 1;
                        duration_start = 0;
                        n = 0;
                        continue;
                    }
                }

                n++;
            }

        END:

            /****************** end ************************/

            if(error)
            {
                progress.reset();

                str1_2048[0] = 0;

                if(error==1)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i.\n",
                             i + 1);
                }
                if(error==2)
                {
                    snprintf(str1_2048, 1700, "Error.\n\n"
                                              "Time keeping annotation in the first datarecord is %.12f\n"
                                              "expected <1.0\n",
                             time_tmp);
                }
                if(error==3)
                {
                    snprintf(str1_2048, 1700, "Error.\n\n"
                                              "Datarecord duration is %.12f but timestep between\n"
                                              "datarecord %i and preceding datarecord is %.12f.\n",
                             data_record_duration,
                             i + 1,
                             time_tmp - elapsedtime);
                }
                if(error==4)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nDatarecords are not in chronological order,\n"
                                              "datarecord %i has timestamp %.12f and datarecord %i has timestamp %.12f.\n",
                             i,
                             elapsedtime,
                             i + 1,
                             time_tmp);
                }
                if(error==5)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "last byte is not a null-byte.\n",
                             i + 1);
                }
                if(error==6)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "timekeeping TAL (the first annotation in the first annotations signal)\n"
                                              "should have two consecutive bytes with values 0x14 immediately after\n"
                                              "the number.\n",
                             i + 1);
                }
                if(error==33)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "byte before the first null-byte should be equal to 0x14.\n",
                             i + 1);
                }
                if(error==34)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "after two consecutive null-bytes all of the remaining bytes should be null-bytes as well.\n"
                                              "(it's not allowed to have two or more null-bytes between two TAL's)\n",
                             i + 1);
                }
                if(error==35)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "it's not allowed to have multiple duration fields in one TAL or\n"
                                              "to have a duration field which is not immediately behind the onsetfield.\n",
                             i + 1);
                }
                if(error==36)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "onset has an invalid number.\n",
                             i + 1);
                }
                if(error==37)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "duration has an invalid number.\n",
                             i + 1);
                }
                if(error==38)
                {
                    snprintf(str1_2048, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                                              "description has a non-printable character.\n",
                             i + 1);
                }
                len = strlen(str1_2048);
                p = (i * recordsize) + (edfsignals * 256) + 256 + (edfparam[annot_ch[r]].datrec_offset);
                snprintf(str1_2048 + len, 2048 - len, "Offset from start of file: 0x%X\n\n"
                                                      "Stopped at the first error, additional errors may be present.",
                         p);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
                messagewindow.exec();
                free(cnv_buf);
                free(scratchpad);
                EDFCompatDialog->close();
                return;
            }
        }
    }

    progress.reset();

    if(bdf)
    {
        if(bdfplus)
        {
            QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is BDF+ compliant.");
            messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
            messagewindow.exec();
        }
        else
        {
            QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is BDF compliant.");
            messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
            messagewindow.exec();
        }
    }
    else
    {
        if(edfplus)
        {
            QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is EDF+ compliant.");
            messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
            messagewindow.exec();
        }
        else
        {
            QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is EDF compliant.");
            messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
            messagewindow.exec();
        }
    }

    free(cnv_buf);
    free(scratchpad);
    EDFCompatDialog->close();
}

// edf_compat_helper.cpp

void attach_console_if_needed_compat() {
#ifdef _WIN32
    // If no console exists, create one
    if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) {
        AllocConsole();
    }

    // Redirect stdout to console
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$", "r", stdin);
#endif
}

    StdoutCapture::StdoutCapture() {
        original_stdout = stdout;
#ifdef _WIN32
        // Create a temporary file
        tmpnam_s(temp_filename, sizeof(temp_filename));
        // Don't redirect stdout yet, just open the capture file
        fopen_s(&capture_file, temp_filename, "w");
#else
        capture_file = tmpfile();
#endif
    }

    StdoutCapture::~StdoutCapture() {
        if (capture_file) {
            fclose(capture_file);
        }
    }

    // Record a line to the capture file without redirecting stdout
    void StdoutCapture::record(const char* format, ...) {
        if (!capture_file) return;

        va_list args;
        va_start(args, format);
        vfprintf(capture_file, format, args);
        fflush(capture_file);
        va_end(args);
    }

    std::string StdoutCapture::get_output() {
        std::string output;
        if (!capture_file) return output;

#ifdef _WIN32
        // Close the current file and reopen for reading
        fclose(capture_file);
        capture_file = NULL;

        FILE* read_file;
        fopen_s(&read_file, temp_filename, "r");

        if (read_file) {
            char buffer[4096];
            while (fgets(buffer, sizeof(buffer), read_file) != NULL) {
                output += buffer;
            }
            fclose(read_file);

            // Clean up temp file
            remove(temp_filename);
        }
#else
        // Save current position
        long pos = ftell(capture_file);
        // Go to beginning of file
        fseek(capture_file, 0, SEEK_SET);

        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), capture_file) != NULL) {
            output += buffer;
        }

        // Reset position
        fseek(capture_file, pos, SEEK_SET);
#endif

        return output;
    }



int check_edf_compatibility(edfhdrblck_t *hdr, FILE *inputfile, char *errmsg, int errmsg_len)
{
    if (!hdr || !inputfile || !errmsg || errmsg_len <= 0) {
        snprintf(errmsg, errmsg_len, "Invalid parameters passed to check_edf_compatibility().");
        return -1;
    }

    char summary[16384];

    attach_console_if_needed_compat();
    int error_count = 0;
    char *scratchpad = NULL;
    char *cnv_buf = NULL;
    int max_tal_ln = 0;
    char str_tmp[4096]; // Increased buffer size for error details

    // Start building detailed summary
    snprintf(summary, 16384, "== EDF/BDF Compatibility Check Summary ==\n");
    snprintf(str_tmp, sizeof(str_tmp), "File type: %s%s\n",
             hdr->edf ? "EDF" : "BDF",
             hdr->edfplus || hdr->bdfplus ? "+" : "");
    strcat(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Number of signals: %i\n", hdr->edfsignals);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Number of data records: %lli\n", hdr->datarecords);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Data record duration: %.4f seconds\n", hdr->data_record_duration);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Recording duration: %d seconds\n", hdr->recording_len_sec);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Number of annotation channels: %d\n", hdr->nr_annot_chns);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "File is %s\n", hdr->discontinuous ? "discontinuous" : "continuous");
    strcat_s(summary, str_tmp);

    const int recordsize = hdr->recordsize;
    const int edfsignals = hdr->edfsignals;
    const int nr_annot_chns = hdr->nr_annot_chns;
    const int datarecords = hdr->datarecords;
    const int *annot_ch = hdr->annot_ch;
    edfpar_t *edfparam = hdr->edfparam;
    int samplesize = hdr->bdf ? 3 : 2;

    // Additional header information
    strcat_s(summary, "\n== Signal Information ==\n");
    for(int i = 0; i < edfsignals; i++) {
        snprintf(str_tmp, sizeof(str_tmp), "Signal %d: %s, %d samples/record, min: %d, max: %d\n",
                 i+1, edfparam[i].label, edfparam[i].smp_per_record,
                 edfparam[i].dig_min, edfparam[i].dig_max);
        strcat_s(summary, str_tmp);
    }

    // Allocate buffers for processing
    cnv_buf = (char *)calloc(1, recordsize);
    if (cnv_buf == NULL) {
        snprintf(str_tmp, sizeof(str_tmp), "ERROR: Memory allocation failed for record buffer.\n");
        strcat_s(summary, str_tmp);

        strncpy(errmsg, summary, errmsg_len - 1);
        errmsg[errmsg_len - 1] = '\0';
        return -2;
    }

    for (int i = 0; i < nr_annot_chns; i++) {
        if (max_tal_ln < edfparam[annot_ch[i]].smp_per_record * samplesize)
            max_tal_ln = edfparam[annot_ch[i]].smp_per_record * samplesize;
    }

    if (max_tal_ln < 128) max_tal_ln = 128;

    scratchpad = (char *)calloc(1, max_tal_ln + 3);
    if (scratchpad == NULL) {
        snprintf(str_tmp, sizeof(str_tmp), "ERROR: Memory allocation failed for scratchpad buffer.\n");
        strcat_s(summary, str_tmp);

        free(cnv_buf);
        strncpy(errmsg, summary, errmsg_len - 1);
        errmsg[errmsg_len - 1] = '\0';
        return -2;
    }

    // Seek to beginning of data records
    if (fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET)) {
        snprintf(str_tmp, sizeof(str_tmp), "ERROR: Failed to seek to data records in the file.\n");
        strcat_s(summary, str_tmp);

        free(cnv_buf);
        free(scratchpad);
        strncpy(errmsg, summary, errmsg_len - 1);
        errmsg[errmsg_len - 1] = '\0';
        return -3;
    }

    // Statistics for summary
    long long total_samples = 0;
    long long samples_checked = 0;
    long long samples_out_of_range = 0;
    double elapsedtime = 0.0;
    int annotation_errors = 0;
    int sample_errors = 0;
    int timing_errors = 0;

    strcat_s(summary, "\n== Data Validation ==\n");
    strcat_s(summary, "Checking all samples against digital min/max values...\n");

    // Union for byte manipulation
    union {
        unsigned int one;
        signed int one_signed;
        unsigned short two[2];
        signed short two_signed[2];
        unsigned char four[4];
    } var;

    // Create an error section in the summary
    strcat_s(summary, "\n== Errors Detected ==\n");

    // Flag to keep track of major errors that should abort processing
    int abort_processing = 0;

    // Check all data records
    for (int i = 0; i < datarecords && !abort_processing; i++) {
        // Print progress indicators
        if (i % (datarecords / 100 + 1) == 0 || i == datarecords - 1) {
            float pct = (float)(i + 1) / datarecords * 100.0f;
            fprintf(stdout, "Checking datarecord %d/%d (%.0f%%)\n", i + 1, datarecords, pct);
            fflush(stdout);
        }

        // Read record
        if (fread(cnv_buf, recordsize, 1, inputfile) != 1) {
            snprintf(str_tmp, sizeof(str_tmp), "ERROR: Read error at datarecord %d. Cannot continue checking.\n", i + 1);
            strcat_s(summary, str_tmp);
            error_count++;
            abort_processing = 1;
            break;
        }

        // Check all signals in this record
        for (int j = 0; j < edfsignals; j++) {
            for (int k = 0; k < edfparam[j].smp_per_record; k++) {
                total_samples++;
                samples_checked++;

                int temp;

                // Extract sample value based on file type (EDF or BDF)
                if (hdr->bdf) {
                    var.two[0] = *((unsigned short *)(cnv_buf + edfparam[j].datrec_offset + (k * 3)));
                    var.four[2] = *(cnv_buf + edfparam[j].datrec_offset + (k * 3) + 2);

                    if (var.four[2] & 0x80) {
                        var.four[3] = 0xff;
                    } else {
                        var.four[3] = 0x00;
                    }

                    temp = var.one_signed;
                } else {
                    temp = *((signed short *)(cnv_buf + edfparam[j].datrec_offset + (k * 2)));
                }

                // Check if the sample is within digital min/max range
                if (temp > edfparam[j].dig_max) {
                    samples_out_of_range++;
                    sample_errors++;
                    error_count++;
                    snprintf(str_tmp, sizeof(str_tmp),
                             "ERROR %d: Datarecord %d, signal %d, sample %d exceeds digital maximum. "
                             "Value: %d, Max: %d, Offset: 0x%X\n",
                             error_count, i + 1, j + 1, k + 1, temp, edfparam[j].dig_max,
                             (i * recordsize) + edfparam[j].datrec_offset + (k * samplesize) + (edfsignals * 256) + 256);
                    strcat_s(summary, str_tmp);

                    // Limit the number of sample errors reported to avoid excessive output
                    if (sample_errors >= 10) {
                        snprintf(str_tmp, sizeof(str_tmp),
                                 "Note: Additional sample range errors may exist but won't be individually listed.\n");
                        strcat_s(summary, str_tmp);
                        break;  // Break from the innermost loop only
                    }
                }

                if (temp < edfparam[j].dig_min) {
                    samples_out_of_range++;
                    sample_errors++;
                    error_count++;
                    snprintf(str_tmp, sizeof(str_tmp),
                             "ERROR %d: Datarecord %d, signal %d, sample %d below digital minimum. "
                             "Value: %d, Min: %d, Offset: 0x%X\n",
                             error_count, i + 1, j + 1, k + 1, temp, edfparam[j].dig_min,
                             (i * recordsize) + edfparam[j].datrec_offset + (k * samplesize) + (edfsignals * 256) + 256);
                    strcat_s(summary, str_tmp);

                    // Limit the number of sample errors reported to avoid excessive output
                    if (sample_errors >= 10) {
                        snprintf(str_tmp, sizeof(str_tmp),
                                 "Note: Additional sample range errors may exist but won't be individually listed.\n");
                        strcat_s(summary, str_tmp);
                        break;  // Break from the innermost loop only
                    }
                }
            }

            // Break from outer loop if we've reached the sample error reporting limit
            if (sample_errors >= 10) {
                break;
            }
        }

        // Process annotation channels if present
        if (nr_annot_chns > 0) {
            int local_error = 0;

            for (int r = 0; r < nr_annot_chns; r++) {
                int n = 0;
                int zero = 0;
                int onset = 0;
                int duration = 0;
                int duration_start = 0;
                int annots_in_tal = 0;
                int annots_in_record = 0;
                scratchpad[0] = 0;

                int p = edfparam[annot_ch[r]].datrec_offset;
                int max = edfparam[annot_ch[r]].smp_per_record * samplesize;

                // Check if last byte is null as required
                if (cnv_buf[p + max - 1] != 0) {
                    annotation_errors++;
                    error_count++;
                    snprintf(str_tmp, sizeof(str_tmp),
                             "ERROR %d: Invalid annotation in datarecord %d, last byte is not a null-byte. "
                             "Offset: 0x%X\n",
                             error_count, i + 1,
                             (i * recordsize) + p + max - 1 + (edfsignals * 256) + 256);
                    strcat_s(summary, str_tmp);
                    local_error = 1;
                    // Continue checking other aspects
                }

                // For first annotation channel, check timekeeping annotation
                if (!r) {
                    local_error = 1;  // Assume error until we find valid timekeeping

                    for (int k = 0; k < (max - 2); k++) {
                        scratchpad[k] = cnv_buf[p + k];

                        if (scratchpad[k] == 20) {
                            if (cnv_buf[p + k + 1] != 20) {
                                annotation_errors++;
                                error_count++;
                                snprintf(str_tmp, sizeof(str_tmp),
                                         "ERROR %d: Invalid annotation in datarecord %d, timekeeping TAL should have two consecutive "
                                         "bytes with values 0x14. Offset: 0x%X\n",
                                         error_count, i + 1,
                                         (i * recordsize) + p + k + (edfsignals * 256) + 256);
                                strcat_s(summary, str_tmp);
                                break;  // Continue to next step
                            }

                            scratchpad[k] = 0;
                            if (is_onset_number_free(scratchpad)) {
                                annotation_errors++;
                                error_count++;
                                snprintf(str_tmp, sizeof(str_tmp),
                                         "ERROR %d: Invalid annotation in datarecord %d, onset has an invalid number format. "
                                         "Value: \"%s\" Offset: 0x%X\n",
                                         error_count, i + 1, scratchpad,
                                         (i * recordsize) + p + (edfsignals * 256) + 256);
                                strcat_s(summary, str_tmp);
                                break;  // Continue to next step
                            } else {
                                double time_tmp = atof(scratchpad);

                                // First datarecord checks
                                if (i == 0) {
                                    if (time_tmp >= 1.0) {
                                        timing_errors++;
                                        error_count++;
                                        snprintf(str_tmp, sizeof(str_tmp),
                                                 "ERROR %d: Time keeping annotation in the first datarecord is %.12f, "
                                                 "expected <1.0. Offset: 0x%X\n",
                                                 error_count, time_tmp,
                                                 (i * recordsize) + p + (edfsignals * 256) + 256);
                                        strcat_s(summary, str_tmp);
                                        // Don't exit, continue checking
                                    }
                                }
                                // Other datarecords - check for timing consistency
                                else {
                                    if (hdr->discontinuous) {
                                        if (time_tmp < elapsedtime) {
                                            timing_errors++;
                                            error_count++;
                                            snprintf(str_tmp, sizeof(str_tmp),
                                                     "ERROR %d: Datarecords are not in chronological order, datarecord %d has timestamp %.12f "
                                                     "and datarecord %d has timestamp %.12f. Offset: 0x%X\n",
                                                     error_count, i, elapsedtime, i + 1, time_tmp,
                                                     (i * recordsize) + p + (edfsignals * 256) + 256);
                                            strcat_s(summary, str_tmp);
                                            // Don't exit, continue checking
                                        }
                                    } else {
                                        if (((time_tmp - elapsedtime) > (hdr->data_record_duration + 0.00000001)) ||
                                            ((time_tmp - elapsedtime) < (hdr->data_record_duration - 0.00000001))) {
                                            timing_errors++;
                                            error_count++;
                                            snprintf(str_tmp, sizeof(str_tmp),
                                                     "ERROR %d: Datarecord duration is %.12f but timestep between datarecord %d and preceding "
                                                     "datarecord is %.12f. Offset: 0x%X\n",
                                                     error_count, hdr->data_record_duration, i + 1, time_tmp - elapsedtime,
                                                     (i * recordsize) + p + (edfsignals * 256) + 256);
                                            strcat_s(summary, str_tmp);
                                            // Don't exit, continue checking
                                        }
                                    }
                                }
                                elapsedtime = time_tmp;
                                local_error = 0;  // Found valid timekeeping
                                break;
                            }
                        }
                    }

                    // If we didn't find a timekeeping annotation
                    if (local_error) {
                        annotation_errors++;
                        error_count++;
                        snprintf(str_tmp, sizeof(str_tmp),
                                 "ERROR %d: Invalid annotation in datarecord %d, could not find timekeeping annotation. "
                                 "This is required for EDF+/BDF+ files. Offset: 0x%X\n",
                                 error_count, i + 1,
                                 (i * recordsize) + p + (edfsignals * 256) + 256);
                        strcat_s(summary, str_tmp);
                        // Continue checking other aspects
                    }
                }

                // Process annotations
                for (int k = 0; k < max; k++) {
                    scratchpad[n] = cnv_buf[p + k];

                    // Handle null bytes
                    if (!scratchpad[n]) {
                        if (!zero) {
                            if (k) {
                                if (cnv_buf[p + k - 1] != 20) {
                                    annotation_errors++;
                                    error_count++;
                                    snprintf(str_tmp, sizeof(str_tmp),
                                             "ERROR %d: Invalid annotation in datarecord %d, byte before the first null-byte should be 0x14. "
                                             "Offset: 0x%X\n",
                                             error_count, i + 1,
                                             (i * recordsize) + p + k - 1 + (edfsignals * 256) + 256);
                                    strcat_s(summary, str_tmp);
                                    // Continue checking other aspects
                                }
                            } else {
                                zero++;
                            }
                            n = 0;
                            onset = 0;
                            duration = 0;
                            duration_start = 0;
                            scratchpad[0] = 0;
                            annots_in_tal = 0;
                        }
                        zero++;
                        continue;
                    }

                    // Check for invalid consecutive nulls
                    if (zero > 1) {
                        annotation_errors++;
                        error_count++;
                        snprintf(str_tmp, sizeof(str_tmp),
                                 "ERROR %d: Invalid annotation in datarecord %d, after two consecutive null-bytes "
                                 "all remaining bytes should be null-bytes as well. Offset: 0x%X\n",
                                 error_count, i + 1,
                                 (i * recordsize) + p + k + (edfsignals * 256) + 256);
                        strcat_s(summary, str_tmp);
                        break;  // This is a severe issue, move to next channel
                    }
                    zero = 0;

                    // Process TAL fields
                    if ((scratchpad[n] == 20) || (scratchpad[n] == 21)) {
                        // Duration field
                        if (scratchpad[n] == 21) {
                            if (duration || duration_start || onset || annots_in_tal) {
                                annotation_errors++;
                                error_count++;
                                snprintf(str_tmp, sizeof(str_tmp),
                                         "ERROR %d: Invalid annotation in datarecord %d, multiple duration fields in one TAL "
                                         "or duration field not immediately after onset. Offset: 0x%X\n",
                                         error_count, i + 1,
                                         (i * recordsize) + p + k + (edfsignals * 256) + 256);
                                strcat_s(summary, str_tmp);
                                // Continue checking other aspects
                            }
                            duration_start = 1;
                        }

                        // Handle annotation text
                        if ((scratchpad[n] == 20) && onset && (!duration_start)) {
                            if (r || annots_in_record) {
                                if (n >= 0) {
                                    for (int q = 0; q < n; q++) {
                                        if (q == MAX_ANNOTATION_LEN) break;
                                        if ((((unsigned char *)scratchpad)[q] < 32) || (((unsigned char *)scratchpad)[q] == 127)) {
                                            if ((scratchpad[q] != '\t') && (scratchpad[q] != '\n') && (scratchpad[q] != '\r')) {
                                                annotation_errors++;
                                                error_count++;
                                                snprintf(str_tmp, sizeof(str_tmp),
                                                         "ERROR %d: Invalid annotation in datarecord %d, description has a non-printable character. "
                                                         "Offset: 0x%X\n",
                                                         error_count, i + 1,
                                                         (i * recordsize) + p + k - n + q + (edfsignals * 256) + 256);
                                                strcat_s(summary, str_tmp);
                                                break;  // Continue checking other aspects
                                            }
                                        }
                                    }
                                }
                            }
                            annots_in_tal++;
                            annots_in_record++;
                            n = 0;
                            continue;
                        }

                        // Onset field
                        if (!onset) {
                            scratchpad[n] = 0;
                            if (is_onset_number_free(scratchpad)) {
                                annotation_errors++;
                                error_count++;
                                snprintf(str_tmp, sizeof(str_tmp),
                                         "ERROR %d: Invalid annotation in datarecord %d, onset has an invalid number format. "
                                         "Value: \"%s\" Offset: 0x%X\n",
                                         error_count, i + 1, scratchpad,
                                         (i * recordsize) + p + k - n + (edfsignals * 256) + 256);
                                strcat_s(summary, str_tmp);
                                // Continue checking other aspects
                            }
                            onset = 1;
                            n = 0;
                            continue;
                        }

                        // Duration field data
                        if (duration_start) {
                            scratchpad[n] = 0;
                            if (is_duration_number_free(scratchpad)) {
                                annotation_errors++;
                                error_count++;
                                snprintf(str_tmp, sizeof(str_tmp),
                                         "ERROR %d: Invalid annotation in datarecord %d, duration has an invalid number format. "
                                         "Value: \"%s\" Offset: 0x%X\n",
                                         error_count, i + 1, scratchpad,
                                         (i * recordsize) + p + k - n + (edfsignals * 256) + 256);
                                strcat_s(summary, str_tmp);
                                // Continue checking other aspects
                            }
                            duration = 1;
                            duration_start = 0;
                            n = 0;
                            continue;
                        }
                    }

                    n++;

                    // Avoid overflow - make sure n never exceeds the scratchpad size
                    if (n >= max_tal_ln) {
                        annotation_errors++;
                        error_count++;
                        snprintf(str_tmp, sizeof(str_tmp),
                                 "ERROR %d: Annotation text too long in datarecord %d. Annotation processing aborted. "
                                 "Offset: 0x%X\n",
                                 error_count, i + 1,
                                 (i * recordsize) + p + k + (edfsignals * 256) + 256);
                        strcat_s(summary, str_tmp);
                        break;  // Stop processing this channel
                    }
                }

                // Limit the number of annotation errors reported to avoid excessive output
                if (annotation_errors >= 20) {
                    snprintf(str_tmp, sizeof(str_tmp),
                             "Note: Additional annotation errors may exist but won't be individually listed.\n");
                    strcat_s(summary, str_tmp);
                    break;  // Break from the annotation channel loop
                }
            }
        }

        // If we've found too many errors, provide a warning and stop detailed reporting
        if (error_count > 50) {
            snprintf(str_tmp, sizeof(str_tmp),
                     "WARNING: More than 50 errors detected. Detailed error reporting stopped to avoid excessive output.\n"
                     "The file has serious compatibility issues and does not comply with the EDF/BDF specification.\n");
            strcat_s(summary, str_tmp);
            break;  // Exit the main checking loop
        }
    }

    // Update summary with final statistics
    strcat_s(summary, "\n== Statistics ==\n");

    snprintf(str_tmp, sizeof(str_tmp), "Total samples checked: %lld\n", samples_checked);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Samples out of range: %lld\n", samples_out_of_range);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Sample errors detected: %d\n", sample_errors);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Annotation errors detected: %d\n", annotation_errors);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Timing errors detected: %d\n", timing_errors);
    strcat_s(summary, str_tmp);

    snprintf(str_tmp, sizeof(str_tmp), "Total error count: %d\n", error_count);
    strcat_s(summary, str_tmp);

    strcat_s(summary, "\n== Result ==\n");

    if (error_count == 0) {
        snprintf(str_tmp, sizeof(str_tmp), "Compatibility check passed. File is a valid %s%s file.\n",
                 hdr->edf ? "EDF" : "BDF",
                 hdr->edfplus || hdr->bdfplus ? "+" : "");
    } else {
        snprintf(str_tmp, sizeof(str_tmp),
                 "Compatibility check FAILED. File does not comply with the %s%s specification.\n"
                 "See detailed errors above for specific issues.\n",
                 hdr->edf ? "EDF" : "BDF",
                 hdr->edfplus || hdr->bdfplus ? "+" : "");
    }
    strcat_s(summary, str_tmp);

    // Copy the summary to the error message buffer (which will be used for the report)
    strncpy(errmsg, summary, errmsg_len - 1);
    errmsg[errmsg_len - 1] = '\0';

    // Clean up
    free(cnv_buf);
    free(scratchpad);

    return error_count > 0 ? -1 : 0;  // Return negative if errors were found
}


// Replacement for UI_EDFCompatwindow::CheckButtonClicked

void UI_EDFCompatwindow::CheckButtonClicked()
{
    if(!mainwindow->files_open)
    {
        EDFCompatDialog->close();
        return;
    }

    CheckButton->setEnabled(false);
    CloseButton->setEnabled(false);

    int n = filelist->currentRow();
    if (n < 0 || n >= mainwindow->files_open)
    {
        QMessageBox::critical(EDFCompatDialog, "Error", "Invalid file index selected.");
        EDFCompatDialog->close();
        return;
    }

    FILE *inputfile = mainwindow->edfheaderlist[n]->file_hdl;
    edfhdrblck_t *hdr = mainwindow->edfheaderlist[n];

    char errmsg[2048] = {0};

    QProgressDialog progress("Checking EDF/BDF Compatibility...", "Abort", 0, 0, EDFCompatDialog);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(200);
    progress.show();
    qApp->processEvents();

    int result = check_edf_compatibility(hdr, inputfile, errmsg, sizeof(errmsg));

    progress.close();

    if (result != 0) {
        QMessageBox::critical(EDFCompatDialog, "Compatibility Error", errmsg);
        EDFCompatDialog->close();
        return;
    }

    QString result_text;
    if (hdr->bdf)
        result_text = hdr->bdfplus ? "No errors found, file is BDF+ compliant." : "No errors found, file is BDF compliant.";
    else
        result_text = hdr->edfplus ? "No errors found, file is EDF+ compliant." : "No errors found, file is EDF compliant.";

    QMessageBox messagewindow(QMessageBox::Information, "Result", result_text);
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    EDFCompatDialog->close();
}


int is_duration_number_free(char *str)
{
    int i, l, hasdot = 0;

    l = strlen(str);

    if(!l)  return 1;

    if((str[0] == '.')||(str[l-1] == '.'))  return 1;

    for(i=0; i<l; i++)
    {
        if(str[i]=='.')
        {
            if(hasdot)  return 1;
            hasdot++;
        }
        else
        {
            if((str[i]<48)||(str[i]>57))  return 1;
        }
    }

    return 0;
}



int UI_EDFCompatwindow::is_duration_number(char *str)
{
    int i, l, hasdot = 0;

    l = strlen(str);

    if(!l)  return 1;

    if((str[0] == '.')||(str[l-1] == '.'))  return 1;

    for(i=0; i<l; i++)
    {
        if(str[i]=='.')
        {
            if(hasdot)  return 1;
            hasdot++;
        }
        else
        {
            if((str[i]<48)||(str[i]>57))  return 1;
        }
    }

    return 0;
}



int is_onset_number_free(char *str)
{
    int i, l, hasdot = 0;

    l = strlen(str);

    if(l<2)  return 1;

    if((str[0]!='+')&&(str[0]!='-'))  return 1;

    if((str[1] == '.')||(str[l-1] == '.'))  return 1;

    for(i=1; i<l; i++)
    {
        if(str[i]=='.')
        {
            if(hasdot)  return 1;
            hasdot++;
        }
        else
        {
            if((str[i]<48)||(str[i]>57))  return 1;
        }
    }

    return 0;
}

int UI_EDFCompatwindow::is_onset_number(char *str)
{
    int i, l, hasdot = 0;

    l = strlen(str);

    if(l<2)  return 1;

    if((str[0]!='+')&&(str[0]!='-'))  return 1;

    if((str[1] == '.')||(str[l-1] == '.'))  return 1;

    for(i=1; i<l; i++)
    {
        if(str[i]=='.')
        {
            if(hasdot)  return 1;
            hasdot++;
        }
        else
        {
            if((str[i]<48)||(str[i]>57))  return 1;
        }
    }

    return 0;
}



int UI_EDFCompatwindow::is_integer_number(char *str)
{
    int i=0, l, hasspace = 0, hassign=0, digit=0;

    l = strlen(str);

    if(!l)  return 1;

    if((str[0]=='+')||(str[0]=='-'))
    {
        hassign++;
        i++;
    }

    for(; i<l; i++)
    {
        if(str[i]==' ')
        {
            if(!digit)
            {
                return 1;
            }
            hasspace++;
        }
        else
        {
            if((str[i]<48)||(str[i]>57))
            {
                return 1;
            }
            else
            {
                if(hasspace)
                {
                    return 1;
                }
                digit++;
            }
        }
    }

    if(digit)  return 0;
    else  return 1;
}



int UI_EDFCompatwindow::is_number(char *str)
{
    int i=0, l, hasspace = 0, hassign=0, digit=0, hasdot=0;

    l = strlen(str);

    if(!l)  return 1;

    if((str[0]=='+')||(str[0]=='-'))
    {
        hassign++;
        i++;
    }

    if(str[l-1]=='.')
    {
        return 1;
    }

    for(; i<l; i++)
    {
        if(str[i]==' ')
        {
            if(!digit)
            {
                return 1;
            }
            if(str[i-1]=='.')
            {
                return 1;
            }
            hasspace++;
        }
        else
        {
            if(((str[i]<48)||(str[i]>57))&&str[i]!='.')
            {
                return 1;
            }
            else
            {
                if(hasspace)
                {
                    return 1;
                }
                if(str[i]=='.')
                {
                    if(hasdot||!i)  return 1;
                    if(hassign&&(i<2))  return 1;
                    hasdot++;
                }
                else
                {
                    digit++;
                }
            }
        }
    }

    if(digit)  return 0;
    else  return 1;
}




