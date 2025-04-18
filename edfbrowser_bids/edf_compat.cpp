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



int check_edf_compatibility(edfhdrblck_t *hdr, FILE *inputfile, char *errmsg, int errmsg_len)
{

    union {
        unsigned int one;
        signed int one_signed;
        unsigned short two[2];
        signed short two_signed[2];
        unsigned char four[4];
    } var;

    double elapsedtime,
        time_tmp=0;

    char *cnv_buf=NULL;

    int i, j, k,
        r = 0,
        n, q, p2,
        len,
        edfplus,
        discontinuous,
        bdf,
        bdfplus,
        max,
        onset,
        duration,
        duration_start,
        zero,
        temp,
        annots_in_tal,
        annots_in_record,
        error, samplesize = hdr->bdf ? 3 : 2;

    const int recordsize = hdr->recordsize;
    const int edfsignals = hdr->edfsignals;
    const int nr_annot_chns = hdr->nr_annot_chns;
    const double data_record_duration = hdr->data_record_duration;
    const int datarecords = hdr->datarecords;
    const int *annot_ch = hdr->annot_ch;
    edfpar_t *edfparam = hdr->edfparam;
    if (!hdr || !inputfile || !errmsg || errmsg_len <= 0) {
        snprintf(errmsg, errmsg_len, "Invalid parameters passed to check_edf_compatibility().");
        return -1;
    }


    long long offset = (long long)((edfsignals + 1) * 256);
    if (fseeko(inputfile, offset, SEEK_SET)) {
        snprintf(errmsg, errmsg_len, "Failed to seek to data records.");
        return -2;
    }

    char *buf = (char *)calloc(1, recordsize);
    if (!buf) {
        snprintf(errmsg, errmsg_len, "Memory allocation failed for record buffer.");
        return -3;
    }

    for (i = 0; i < datarecords; ++i) {

        if (int(i*10000.0f/datarecords) == 0 || i == datarecords - 1) {
            float pct = (float)(i + 1) / datarecords * 100.0f;
            printf("\rChecking datarecord %d/%d (%.1f%%)...", i + 1, datarecords, pct);
            fflush(stdout);
        }

        if (fread(buf, recordsize, 1, inputfile) != 1) {
            snprintf(errmsg, errmsg_len, "Read error at datarecord %d.", i);
            free(buf);
            return -4;
        }

        for (j = 0; j < edfsignals; ++j) {
            for (k = 0; k < edfparam[j].smp_per_record; ++k) {
                int value;
                if (samplesize == 3) {
                    unsigned char b0 = buf[edfparam[j].datrec_offset + k * 3];
                    unsigned char b1 = buf[edfparam[j].datrec_offset + k * 3 + 1];
                    char b2 = buf[edfparam[j].datrec_offset + k * 3 + 2];
                    value = (int)(b0 | (b1 << 8) | (b2 << 16));
                    if (b2 & 0x80) value |= 0xFF000000;
                } else {
                    short *p = (short *)(buf + edfparam[j].datrec_offset + k * 2);
                    value = *p;
                }

                if (value > edfparam[j].dig_max || value < edfparam[j].dig_min) {
                    snprintf(errmsg, errmsg_len,
                             "Datarecord %d, signal %d, sample %d out of bounds. Value: %d, Range: [%d, %d]",
                             i + 1, j + 1, k + 1, value, edfparam[j].dig_min, edfparam[j].dig_max);
                    free(buf);
                    return -5;
                }
            }
        }
    }

    free(buf);
    return 0;  // No errors found


    if(hdr->bdf)  samplesize = 3;
    else  samplesize = 2;

    char *scratchpad=NULL,
        str1_2048[2048]={""};

    cnv_buf = (char *)calloc(1, recordsize);
    if(cnv_buf==NULL)
    {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error. (cnv_buf).");
        messagewindow.exec();
        return -3;
    }

    int max_tal_ln = 0;

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
        return -3;
    }

    if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
    {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile.");
        messagewindow.exec();
        free(cnv_buf);
        free(scratchpad);
        return -4;
    }

    /***************** check the maximum and minimum of samples of all signals ******************************/

    QProgressDialog progress("Checking...", "Abort", 0, datarecords );
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(200);

    int progress_steps = datarecords / 100;
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
                return -1;
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
            return -4;
        }

        for(j=0; j<edfsignals; j++)
        {
            for(k=0; k<edfparam[j].smp_per_record; k++)
            {
                if(hdr->bdf)
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
                    return -5;
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
                    return -6;
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

            p2 = edfparam[annot_ch[r]].datrec_offset;
            max = edfparam[annot_ch[r]].smp_per_record * samplesize;

            /************** process one annotation signal ****************/

            if(cnv_buf[p2 + max - 1]!=0)
            {
                error = 5;
                goto END;
            }

            if(!r)  /* if it's the first annotation signal, then check */
            {       /* the timekeeping annotation */
                error = 1;

                for(k=0; k<(max-2); k++)
                {
                    scratchpad[k] = cnv_buf[p2 + k];

                    if(scratchpad[k]==20)
                    {
                        if(cnv_buf[p2 + k + 1]!=20)
                        {
                            error = 6;
                            goto END;
                        }
                        scratchpad[k] = 0;
                        if(is_onset_number_free(scratchpad))
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
                scratchpad[n] = cnv_buf[p2 + k];

                if(!scratchpad[n])
                {
                    if(!zero)
                    {
                        if(k)
                        {
                            if(cnv_buf[p2 + k - 1]!=20)
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
                        if(is_onset_number_free(scratchpad))
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
                        if(is_duration_number_free(scratchpad))
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
                p2 = (i * recordsize) + (edfsignals * 256) + 256 + (edfparam[annot_ch[r]].datrec_offset);
                snprintf(str1_2048 + len, 2048 - len, "Offset from start of file: 0x%X\n\n"
                                                      "Stopped at the first error, additional errors may be present.",
                         p2);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str1_2048);
                messagewindow.exec();
                free(cnv_buf);
                free(scratchpad);
                return -4;
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
    return 0;
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




