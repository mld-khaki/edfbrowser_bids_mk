
contains(QT_MAJOR_VERSION, 4) {

LIST = 0 1 2 3 4 5 6 7
for(a, LIST):contains(QT_MINOR_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")

contains(QT_MINOR_VERSION, 8) {
  LIST = 0 1 2 3 4 5 6
  for(a, LIST):contains(QT_PATCH_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")
}
}


contains(QT_MAJOR_VERSION, 5) {

LIST = 0 1 2 3 4 5 6 7 8 9 10 11
for(a, LIST):contains(QT_MINOR_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")

contains(QT_MINOR_VERSION, 12) {
  LIST = 0 1 2 3 4 5
  for(a, LIST):contains(QT_PATCH_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")
}
}


contains(QT_MAJOR_VERSION, 6) {

LIST = 0 1 2 3
for(a, LIST):contains(QT_MINOR_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")

contains(QT_MINOR_VERSION, 4) {
  LIST = 0 1
  for(a, LIST):contains(QT_PATCH_VERSION, $$a):error("This project needs Qt4 version >= 4.8.7 or Qt5 version >= 5.12.6 or Qt6 version >= 6.4.2")
}
}


TEMPLATE = app
TARGET = edfbrowser
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt
CONFIG += warn_on
CONFIG += release
CONFIG += static
CONFIG += largefile
QT += network

contains(QT_MAJOR_VERSION, 6) {
QT += widgets
QT += printsupport
}

contains(QT_MAJOR_VERSION, 5) {
QT += widgets
QT += printsupport

win32 {
    QTPLUGIN += windowsprintersupport
}
}


QMAKE_CFLAGS += -Wall -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits -Wfatal-errors -Wstringop-overflow=2 -Wdeprecated-declarations -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -U_FORTIFY_SOURCE

QMAKE_CXXFLAGS += -Wextra -Wshadow -Wformat -Wformat-nonliteral -Wformat-security -Wtype-limits -Wfatal-errors -Wstringop-overflow=2 -Wdeprecated-declarations -U_FORTIFY_SOURCE


OBJECTS_DIR = ./objects
MOC_DIR = ./moc

HEADERS += global.h
HEADERS += mainwindow.h
HEADERS += viewcurve.h
HEADERS += tmessagebox.h
HEADERS += check_edf_file.h
HEADERS += show_edf_hdr.h
HEADERS += signals_dialog.h
HEADERS += signal_chooser.h
HEADERS += edf_compat.h
HEADERS += colordialog.h
HEADERS += filter_dialog.h
HEADERS += jump_dialog.h
HEADERS += about_dialog.h
HEADERS += edf_annotations.h
HEADERS += annotations_dock.h
HEADERS += options_dialog.h
HEADERS += xml.h
HEADERS += save_montage_dialog.h
HEADERS += load_montage_dialog.h
HEADERS += view_montage_dialog.h
HEADERS += view_session_dialog.h
HEADERS += show_actual_montage_dialog.h
HEADERS += print_to_edf.h
HEADERS += pagetime_dialog.h
HEADERS += amplitude_dialog.h
HEADERS += print_to_bdf.h
HEADERS += utc_date_time.h
HEADERS += special_button.h
HEADERS += edit_annotation_dock.h
HEADERS += save_annots.h
HEADERS += filtercurve.h
HEADERS += utils.h
HEADERS += signalcurve.h
HEADERS += spectrumanalyzer.h
HEADERS += edflib.h
HEADERS += export_annotations.h
HEADERS += edit_predefined_mtg.h
HEADERS += spectrum_dock.h
HEADERS += edf_annot_list.h
HEADERS += reduce_signals.h
HEADERS += active_file_chooser.h
HEADERS += header_editor.h
HEADERS += bdf_triggers.h
HEADERS += adjustfiltersettings.h
HEADERS += import_annotations.h
HEADERS += averager_dialog.h
HEADERS += averager_curve_wnd.h
HEADERS += ecg_export.h
HEADERS += statistics_dialog.h
HEADERS += filtered_block_read.h
HEADERS += flywheel.h
HEADERS += z_score_dialog.h
HEADERS += check_for_updates.h
HEADERS += date_time_stamp_parser.h
HEADERS += spike_filter_dialog.h
HEADERS += edf_helper.h
HEADERS += plif_ecg_subtract_filter_dialog.h
HEADERS += plif_eeg_subtract_filter_dialog.h
HEADERS += annotlist_filter_dialog.h
HEADERS += export_filtered_signals.h
HEADERS += ecg_statistics.h
HEADERS += fir_filter_dialog.h
HEADERS += cdsa_dialog.h
HEADERS += cdsa_dock.h
HEADERS += aeeg_dialog.h
HEADERS += aeeg_dock.h
HEADERS += hypnogram_dialog.h
HEADERS += hypnogram_dock.h
HEADERS += run_qrs_detector.h
HEADERS += hrv_dock.h
HEADERS += unify_resolution.h
HEADERS += rename_annots_dialog.h
HEADERS += math_f_dialog.h

HEADERS += cnvs/ascii_export.h
HEADERS += cnvs/ascii2edf.h
HEADERS += cnvs/bdf2edf.h
HEADERS += cnvs/biosemi2bdfplus.h
HEADERS += cnvs/biox2edf.h
HEADERS += cnvs/bi9800.h
HEADERS += cnvs/edfplusd_cnv.h
HEADERS += cnvs/emsa2edf.h
HEADERS += cnvs/fino2edf.h
HEADERS += cnvs/fma_ecg2edf.h
HEADERS += cnvs/ishne2edf.h
HEADERS += cnvs/manscan2edf.h
HEADERS += cnvs/mit2edf.h
HEADERS += cnvs/mortara2edf.h
HEADERS += cnvs/nexfin2edf.h
HEADERS += cnvs/nk2edf.h
HEADERS += cnvs/raw2edf.h
HEADERS += cnvs/scp_ecg2edf.h
HEADERS += cnvs/unisens2edf.h
HEADERS += cnvs/wav2edf.h
HEADERS += cnvs/cardiotrak2edf.h

HEADERS += filt/ecg_filter.h
HEADERS += filt/fft_wrap.h
HEADERS += filt/filter.h
HEADERS += filt/fir_filter.h
HEADERS += filt/plif_ecg_subtract_filter.h
HEADERS += filt/plif_eeg_subtract_filter.h
HEADERS += filt/pt_qrs.h
HEADERS += filt/ravg_filter.h
HEADERS += filt/spike_filter.h
HEADERS += filt/z_ratio_filter.h
HEADERS += filt/math_func.h
HEADERS += filt/remez.h
HEADERS += filt/wind_sinc.h

HEADERS += third_party/fidlib/fidlib.h
HEADERS += third_party/fidlib/fidmkf.h
HEADERS += third_party/fidlib/fidrf_cmdlist.h

HEADERS += third_party/kiss_fft/kiss_fft.h
HEADERS += third_party/kiss_fft/_kiss_fft_guts.h
HEADERS += third_party/kiss_fft/kiss_fftr.h

SOURCES += main.cpp
SOURCES += mainwindow_constr.cpp
SOURCES += mainwindow.cpp
SOURCES += tmessagebox.cpp
SOURCES += rc_host.cpp
SOURCES += viewbuf.cpp
SOURCES += load_session.cpp
SOURCES += save_session.cpp
SOURCES += videoplayer.cpp
SOURCES += read_write_settings.cpp
SOURCES += viewcurve.cpp
SOURCES += vc_painter.cpp
SOURCES += vc_signal_props.cpp
SOURCES += check_edf_file.c
SOURCES += show_edf_hdr.cpp
SOURCES += signals_dialog.cpp
SOURCES += signal_chooser.cpp
SOURCES += edf_compat.cpp
SOURCES += colordialog.cpp
SOURCES += filter_dialog.cpp
SOURCES += jump_dialog.cpp
SOURCES += about_dialog.cpp
SOURCES += edf_annotations.cpp
SOURCES += annotations_dock.cpp
SOURCES += options_dialog.cpp
SOURCES += xml.c
SOURCES += save_montage_dialog.cpp
SOURCES += load_montage_dialog.cpp
SOURCES += view_montage_dialog.cpp
SOURCES += view_session_dialog.cpp
SOURCES += show_actual_montage_dialog.cpp
SOURCES += print_to_edf.cpp
SOURCES += pagetime_dialog.cpp
SOURCES += amplitude_dialog.cpp
SOURCES += print_to_bdf.cpp
SOURCES += utc_date_time.c
SOURCES += special_button.cpp
SOURCES += edit_annotation_dock.cpp
SOURCES += save_annots.cpp
SOURCES += filtercurve.cpp
SOURCES += utils.c
SOURCES += signalcurve.cpp
SOURCES += spectrumanalyzer.cpp
SOURCES += edflib.c
SOURCES += export_annotations.cpp
SOURCES += edit_predefined_mtg.cpp
SOURCES += spectrum_dock.cpp
SOURCES += edf_annot_list.c
SOURCES += reduce_signals.cpp
SOURCES += active_file_chooser.cpp
SOURCES += header_editor.cpp
SOURCES += bdf_triggers.cpp
SOURCES += adjustfiltersettings.cpp
SOURCES += import_annotations.cpp
SOURCES += averager_dialog.cpp
SOURCES += averager_curve_wnd.cpp
SOURCES += ecg_export.cpp
SOURCES += statistics_dialog.cpp
SOURCES += filtered_block_read.c
SOURCES += flywheel.cpp
SOURCES += z_score_dialog.cpp
SOURCES += check_for_updates.cpp
SOURCES += date_time_stamp_parser.c
SOURCES += spike_filter_dialog.cpp
SOURCES += edf_helper.c
SOURCES += plif_ecg_subtract_filter_dialog.cpp
SOURCES += plif_eeg_subtract_filter_dialog.cpp
SOURCES += annotlist_filter_dialog.cpp
SOURCES += export_filtered_signals.cpp
SOURCES += ecg_statistics.c
SOURCES += fir_filter_dialog.cpp
SOURCES += cdsa_dialog.cpp
SOURCES += cdsa_dock.cpp
SOURCES += aeeg_dialog.cpp
SOURCES += aeeg_dock.cpp
SOURCES += hypnogram_dialog.cpp
SOURCES += hypnogram_dock.cpp
SOURCES += run_qrs_detector.cpp
SOURCES += hrv_dock.cpp
SOURCES += unify_resolution.cpp
SOURCES += rename_annots_dialog.cpp
SOURCES += math_f_dialog.cpp

SOURCES += cnvs/ascii_export.cpp
SOURCES += cnvs/ascii2edf.cpp
SOURCES += cnvs/bdf2edf.cpp
SOURCES += cnvs/biosemi2bdfplus.cpp
SOURCES += cnvs/biox2edf.cpp
SOURCES += cnvs/bi9800.cpp
SOURCES += cnvs/edfplusd_cnv.cpp
SOURCES += cnvs/emsa2edf.cpp
SOURCES += cnvs/fino2edf.cpp
SOURCES += cnvs/fma_ecg2edf.cpp
SOURCES += cnvs/ishne2edf.cpp
SOURCES += cnvs/manscan2edf.cpp
SOURCES += cnvs/mit2edf.cpp
SOURCES += cnvs/mortara2edf.cpp
SOURCES += cnvs/nexfin2edf.cpp
SOURCES += cnvs/nk2edf.cpp
SOURCES += cnvs/raw2edf.cpp
SOURCES += cnvs/scp_ecg2edf.cpp
SOURCES += cnvs/unisens2edf.cpp
SOURCES += cnvs/wav2edf.cpp
SOURCES += cnvs/cardiotrak2edf.cpp

SOURCES += filt/ecg_filter.c
SOURCES += filt/fft_wrap.c
SOURCES += filt/filter.c
SOURCES += filt/fir_filter.c
SOURCES += filt/plif_ecg_subtract_filter.c
SOURCES += filt/plif_eeg_subtract_filter.c
SOURCES += filt/pt_qrs.c
SOURCES += filt/ravg_filter.c
SOURCES += filt/spike_filter.c
SOURCES += filt/z_ratio_filter.c
SOURCES += filt/math_func.c
SOURCES += filt/remez.c
SOURCES += filt/wind_sinc.c

SOURCES += third_party/fidlib/fidlib.c

SOURCES += third_party/kiss_fft/kiss_fft.c
SOURCES += third_party/kiss_fft/kiss_fftr.c


RESOURCES = images.qrc

win32 {
 RC_FILE = edfbrowser.rc
}

unix {
target.path = /usr/bin
target.files = edfbrowser
INSTALLS += target

documentation.path = /usr/share/doc/edfbrowser
documentation.files = doc/*
INSTALLS += documentation

icon_a.path = /usr/share/icons
icon_a.files = images/edf.png
INSTALLS += icon_a

icon_b.path += /usr/share/pixmaps
icon_b.files = images/edf.png
INSTALLS += icon_b

icon_c.path += /usr/share/icons/hicolor/48x48/apps
icon_c.files = images/edf.png
INSTALLS += icon_c

icon_d.path += /usr/share/icons/hicolor/48x48/mimetypes
icon_d.files = images/edf.png
INSTALLS += icon_d

desktop_link.path += /usr/share/applications
desktop_link.files += install/edfbrowser.desktop
INSTALLS += desktop_link

mime.path += /usr/share/mime/packages
mime.files += install/edfbrowser.xml
INSTALLS += mime
}

mac {
 TARGET = EDFbrowser
 QMAKE_BUNDLE = EDFbrowser
 QMAKE_APPLICATION_BUNDLE_NAME = EDFbrowser
 QMAKE_TARGET_BUNDLE_PREFIX = net.teuniz
 icns.target = edf.icns
 icns.commands = /usr/bin/env bash $$_PRO_FILE_PWD_/images/macos-icns-create.command "$$(PWD)"
 QMAKE_EXTRA_TARGETS += icns
 ICON = $$(PWD)/edf.icns
 QMAKE_POST_LINK = /usr/bin/env bash $$_PRO_FILE_PWD_/install/macos-dmg-create.command "$$(PWD)"
 QMAKE_CLEAN += edf.iconset EDFbrowser-*-temp.dmg
 QMAKE_DISTCLEAN += edf.icns EDFbrowser-*.dmg
}
