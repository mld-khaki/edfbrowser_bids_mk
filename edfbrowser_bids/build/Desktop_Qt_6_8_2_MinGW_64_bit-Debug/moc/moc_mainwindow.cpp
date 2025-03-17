/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13UI_MainwindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13UI_MainwindowE = QtMocHelpers::stringData(
    "UI_Mainwindow",
    "file_position_changed",
    "",
    "annot_docklist_changed",
    "remove_all_signals",
    "edfplus_remove_duplicate_annotations",
    "video_player_seek",
    "video_player_toggle_pause",
    "shift_page_left",
    "shift_page_right",
    "shift_page_up",
    "shift_page_down",
    "next_page",
    "next_epoch",
    "dig_min_max_overflow_timer_func",
    "set_viewtime",
    "fit_signals_to_pane",
    "n",
    "fit_signals_dc_offset",
    "set_dc_offset_to_zero",
    "signalcomp_invert",
    "set_timesync_reference",
    "set_timesync",
    "shift_vert_scrollbar_up_act_triggered",
    "shift_vert_scrollbar_down_act_triggered",
    "open_manual",
    "const char*",
    "set_page_div2",
    "set_page_mult2",
    "open_new_file",
    "show_file_info",
    "close_file_action_func",
    "QAction*",
    "close_all_files",
    "exit_program",
    "signalproperties_dialog",
    "filterproperties_dialog",
    "add_signals_dialog",
    "show_about_dialog",
    "set_display_time",
    "set_display_time_whole_rec",
    "set_amplitude",
    "set_amplitude_div2",
    "set_amplitude_mult2",
    "set_vert_scrollbar",
    "set_view_markers",
    "set_linear_interpol",
    "former_page",
    "playback_file",
    "stop_playback",
    "zoomback",
    "forward",
    "show_splashscreen",
    "export_to_ascii",
    "check_edf_compatibility",
    "add_new_filter",
    "add_plif_ecg_filter",
    "add_plif_eeg_filter",
    "add_fir_filter",
    "add_spike_filter",
    "remove_all_filters",
    "remove_all_plif_ecg_filters",
    "remove_all_plif_eeg_filters",
    "remove_all_fir_filters",
    "remove_all_spike_filters",
    "add_new_math_func",
    "remove_all_math_funcs",
    "jump_to_dialog",
    "jump_to_start",
    "jump_to_end",
    "jump_to_time_millisec",
    "show_annotations",
    "show_options_dialog",
    "get_long_time",
    "char*",
    "nk2edf_converter",
    "recent_file_action_func",
    "recent_session_action_func",
    "sync_by_crosshairs",
    "save_montage",
    "load_montage",
    "view_montage",
    "view_session",
    "show_this_montage",
    "show_help",
    "show_kb_shortcuts",
    "print_to_edf",
    "set_user_defined_display_time",
    "set_user_defined_amplitude",
    "print_to_bdf",
    "print_to_img_640x480",
    "print_to_img_1920x1080",
    "print_to_img_2560x1440",
    "print_to_img_3840x2160",
    "print_to_img_800x600",
    "print_to_img_1024x768",
    "print_to_img_1280x1024",
    "print_to_img_1600x1200",
    "convert_ascii_to_edf",
    "convert_fino_to_edf",
    "convert_wave_to_edf",
    "convert_fm_audio_to_edf",
    "convert_mortara_to_edf",
    "convert_ishne_to_edf",
    "convert_cardiotrak_to_edf",
    "convert_nexfin_to_edf",
    "edfd_converter",
    "slider_moved",
    "convert_emsa_to_edf",
    "bdf2edf_converter",
    "annotation_editor",
    "save_file",
    "save_session",
    "load_session",
    "unisens2edf_converter",
    "BI98002edf_converter",
    "export_annotations",
    "unify_resolution",
    "load_predefined_mtg",
    "edit_predefined_montages",
    "show_spectrum_dock",
    "show_cdsa_dock",
    "show_aeeg_dock",
    "show_hypnogram",
    "page_10mmmin",
    "page_15mmmin",
    "page_30mmmin",
    "page_1mmsec",
    "page_2mmsec",
    "page_5mmsec",
    "page_10mmsec",
    "page_15mmsec",
    "page_25mmsec",
    "page_30mmsec",
    "page_50mmsec",
    "page_60mmsec",
    "page_100mmsec",
    "page_120mmsec",
    "page_250mmsec",
    "page_500mmsec",
    "reduce_signals",
    "edit_header",
    "biosemi2bdfplus_converter",
    "import_annotations",
    "open_stream",
    "start_stop_video",
    "stop_video_generic",
    "live_stream_timer_func",
    "video_poll_timer_func",
    "playback_realtime_timer_func",
    "organize_signals",
    "Escape_fun",
    "export_ecg_rr_interval_to_ascii",
    "qrs_detector",
    "convert_binary_to_edf",
    "convert_manscan_to_edf",
    "convert_scpecg_to_edf",
    "convert_mit_to_edf",
    "convert_biox_to_edf",
    "video_process_error",
    "QProcess::ProcessError",
    "vlc_sock_error",
    "QAbstractSocket::SocketError",
    "export_filtered_signals",
    "video_player_faster",
    "video_player_slower",
    "create_annot_by_rect_draw",
    "rc_host_server_new_connection",
    "rc_host_sock_rxdata_handler",
    "rc_host_sock_disconnected_handler",
    "desktop_resized",
    "mc_v_scrollbar_changed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13UI_MainwindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
     170,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1, 1034,    2, 0x06,    1 /* Public */,
       3,    0, 1037,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0, 1038,    2, 0x0a,    4 /* Public */,
       5,    0, 1039,    2, 0x0a,    5 /* Public */,
       6,    1, 1040,    2, 0x0a,    6 /* Public */,
       7,    0, 1043,    2, 0x0a,    8 /* Public */,
       8,    0, 1044,    2, 0x0a,    9 /* Public */,
       9,    0, 1045,    2, 0x0a,   10 /* Public */,
      10,    0, 1046,    2, 0x0a,   11 /* Public */,
      11,    0, 1047,    2, 0x0a,   12 /* Public */,
      12,    0, 1048,    2, 0x0a,   13 /* Public */,
      13,    1, 1049,    2, 0x0a,   14 /* Public */,
      14,    0, 1052,    2, 0x0a,   16 /* Public */,
      15,    1, 1053,    2, 0x0a,   17 /* Public */,
      16,    1, 1056,    2, 0x0a,   19 /* Public */,
      16,    0, 1059,    2, 0x2a,   21 /* Public | MethodCloned */,
      18,    1, 1060,    2, 0x0a,   22 /* Public */,
      18,    0, 1063,    2, 0x2a,   24 /* Public | MethodCloned */,
      19,    1, 1064,    2, 0x0a,   25 /* Public */,
      19,    0, 1067,    2, 0x2a,   27 /* Public | MethodCloned */,
      20,    2, 1068,    2, 0x0a,   28 /* Public */,
      20,    1, 1073,    2, 0x2a,   31 /* Public | MethodCloned */,
      21,    1, 1076,    2, 0x0a,   33 /* Public */,
      22,    1, 1079,    2, 0x0a,   35 /* Public */,
      23,    0, 1082,    2, 0x0a,   37 /* Public */,
      24,    0, 1083,    2, 0x0a,   38 /* Public */,
      25,    1, 1084,    2, 0x0a,   39 /* Public */,
      27,    0, 1087,    2, 0x0a,   41 /* Public */,
      28,    0, 1088,    2, 0x0a,   42 /* Public */,
      29,    0, 1089,    2, 0x08,   43 /* Private */,
      30,    0, 1090,    2, 0x08,   44 /* Private */,
      31,    1, 1091,    2, 0x08,   45 /* Private */,
      33,    0, 1094,    2, 0x08,   47 /* Private */,
      34,    0, 1095,    2, 0x08,   48 /* Private */,
      35,    0, 1096,    2, 0x08,   49 /* Private */,
      36,    0, 1097,    2, 0x08,   50 /* Private */,
      37,    0, 1098,    2, 0x08,   51 /* Private */,
      38,    0, 1099,    2, 0x08,   52 /* Private */,
      39,    1, 1100,    2, 0x08,   53 /* Private */,
      40,    0, 1103,    2, 0x08,   55 /* Private */,
      41,    1, 1104,    2, 0x08,   56 /* Private */,
      42,    0, 1107,    2, 0x08,   58 /* Private */,
      43,    0, 1108,    2, 0x08,   59 /* Private */,
      44,    1, 1109,    2, 0x08,   60 /* Private */,
      45,    1, 1112,    2, 0x08,   62 /* Private */,
      46,    1, 1115,    2, 0x08,   64 /* Private */,
      47,    0, 1118,    2, 0x08,   66 /* Private */,
      48,    0, 1119,    2, 0x08,   67 /* Private */,
      49,    0, 1120,    2, 0x08,   68 /* Private */,
      50,    0, 1121,    2, 0x08,   69 /* Private */,
      51,    0, 1122,    2, 0x08,   70 /* Private */,
      52,    0, 1123,    2, 0x08,   71 /* Private */,
      53,    0, 1124,    2, 0x08,   72 /* Private */,
      54,    0, 1125,    2, 0x08,   73 /* Private */,
      55,    0, 1126,    2, 0x08,   74 /* Private */,
      56,    0, 1127,    2, 0x08,   75 /* Private */,
      57,    0, 1128,    2, 0x08,   76 /* Private */,
      58,    0, 1129,    2, 0x08,   77 /* Private */,
      59,    0, 1130,    2, 0x08,   78 /* Private */,
      60,    0, 1131,    2, 0x08,   79 /* Private */,
      61,    0, 1132,    2, 0x08,   80 /* Private */,
      62,    0, 1133,    2, 0x08,   81 /* Private */,
      63,    0, 1134,    2, 0x08,   82 /* Private */,
      64,    0, 1135,    2, 0x08,   83 /* Private */,
      65,    0, 1136,    2, 0x08,   84 /* Private */,
      66,    0, 1137,    2, 0x08,   85 /* Private */,
      67,    0, 1138,    2, 0x08,   86 /* Private */,
      68,    0, 1139,    2, 0x08,   87 /* Private */,
      69,    0, 1140,    2, 0x08,   88 /* Private */,
      70,    1, 1141,    2, 0x08,   89 /* Private */,
      71,    0, 1144,    2, 0x08,   91 /* Private */,
      72,    0, 1145,    2, 0x08,   92 /* Private */,
      73,    1, 1146,    2, 0x08,   93 /* Private */,
      75,    0, 1149,    2, 0x08,   95 /* Private */,
      22,    1, 1150,    2, 0x08,   96 /* Private */,
      21,    1, 1153,    2, 0x08,   98 /* Private */,
      76,    1, 1156,    2, 0x08,  100 /* Private */,
      77,    1, 1159,    2, 0x08,  102 /* Private */,
      78,    0, 1162,    2, 0x08,  104 /* Private */,
      79,    0, 1163,    2, 0x08,  105 /* Private */,
      80,    0, 1164,    2, 0x08,  106 /* Private */,
      81,    0, 1165,    2, 0x08,  107 /* Private */,
      82,    0, 1166,    2, 0x08,  108 /* Private */,
      83,    0, 1167,    2, 0x08,  109 /* Private */,
      84,    0, 1168,    2, 0x08,  110 /* Private */,
      85,    0, 1169,    2, 0x08,  111 /* Private */,
      86,    0, 1170,    2, 0x08,  112 /* Private */,
      87,    0, 1171,    2, 0x08,  113 /* Private */,
      88,    0, 1172,    2, 0x08,  114 /* Private */,
      89,    0, 1173,    2, 0x08,  115 /* Private */,
      90,    0, 1174,    2, 0x08,  116 /* Private */,
      91,    0, 1175,    2, 0x08,  117 /* Private */,
      92,    0, 1176,    2, 0x08,  118 /* Private */,
      93,    0, 1177,    2, 0x08,  119 /* Private */,
      94,    0, 1178,    2, 0x08,  120 /* Private */,
      95,    0, 1179,    2, 0x08,  121 /* Private */,
      96,    0, 1180,    2, 0x08,  122 /* Private */,
      97,    0, 1181,    2, 0x08,  123 /* Private */,
      98,    0, 1182,    2, 0x08,  124 /* Private */,
      99,    0, 1183,    2, 0x08,  125 /* Private */,
     100,    0, 1184,    2, 0x08,  126 /* Private */,
     101,    0, 1185,    2, 0x08,  127 /* Private */,
     102,    0, 1186,    2, 0x08,  128 /* Private */,
     103,    0, 1187,    2, 0x08,  129 /* Private */,
     104,    0, 1188,    2, 0x08,  130 /* Private */,
     105,    0, 1189,    2, 0x08,  131 /* Private */,
     106,    0, 1190,    2, 0x08,  132 /* Private */,
     107,    1, 1191,    2, 0x08,  133 /* Private */,
     108,    0, 1194,    2, 0x08,  135 /* Private */,
     109,    0, 1195,    2, 0x08,  136 /* Private */,
     110,    0, 1196,    2, 0x08,  137 /* Private */,
     111,    0, 1197,    2, 0x08,  138 /* Private */,
     112,    0, 1198,    2, 0x08,  139 /* Private */,
     113,    0, 1199,    2, 0x08,  140 /* Private */,
     114,    0, 1200,    2, 0x08,  141 /* Private */,
     115,    0, 1201,    2, 0x08,  142 /* Private */,
     116,    0, 1202,    2, 0x08,  143 /* Private */,
     117,    0, 1203,    2, 0x08,  144 /* Private */,
     118,    1, 1204,    2, 0x08,  145 /* Private */,
     119,    0, 1207,    2, 0x08,  147 /* Private */,
     120,    0, 1208,    2, 0x08,  148 /* Private */,
     121,    0, 1209,    2, 0x08,  149 /* Private */,
     122,    0, 1210,    2, 0x08,  150 /* Private */,
     123,    0, 1211,    2, 0x08,  151 /* Private */,
     124,    0, 1212,    2, 0x08,  152 /* Private */,
     125,    0, 1213,    2, 0x08,  153 /* Private */,
     126,    0, 1214,    2, 0x08,  154 /* Private */,
     127,    0, 1215,    2, 0x08,  155 /* Private */,
     128,    0, 1216,    2, 0x08,  156 /* Private */,
     129,    0, 1217,    2, 0x08,  157 /* Private */,
     130,    0, 1218,    2, 0x08,  158 /* Private */,
     131,    0, 1219,    2, 0x08,  159 /* Private */,
     132,    0, 1220,    2, 0x08,  160 /* Private */,
     133,    0, 1221,    2, 0x08,  161 /* Private */,
     134,    0, 1222,    2, 0x08,  162 /* Private */,
     135,    0, 1223,    2, 0x08,  163 /* Private */,
     136,    0, 1224,    2, 0x08,  164 /* Private */,
     137,    0, 1225,    2, 0x08,  165 /* Private */,
     138,    0, 1226,    2, 0x08,  166 /* Private */,
     139,    0, 1227,    2, 0x08,  167 /* Private */,
     140,    0, 1228,    2, 0x08,  168 /* Private */,
     141,    0, 1229,    2, 0x08,  169 /* Private */,
     142,    0, 1230,    2, 0x08,  170 /* Private */,
     143,    0, 1231,    2, 0x08,  171 /* Private */,
     144,    0, 1232,    2, 0x08,  172 /* Private */,
     145,    0, 1233,    2, 0x08,  173 /* Private */,
     146,    1, 1234,    2, 0x08,  174 /* Private */,
     147,    0, 1237,    2, 0x08,  176 /* Private */,
     148,    0, 1238,    2, 0x08,  177 /* Private */,
     149,    0, 1239,    2, 0x08,  178 /* Private */,
     150,    0, 1240,    2, 0x08,  179 /* Private */,
     151,    0, 1241,    2, 0x08,  180 /* Private */,
     152,    0, 1242,    2, 0x08,  181 /* Private */,
     153,    0, 1243,    2, 0x08,  182 /* Private */,
     154,    0, 1244,    2, 0x08,  183 /* Private */,
     155,    0, 1245,    2, 0x08,  184 /* Private */,
     156,    0, 1246,    2, 0x08,  185 /* Private */,
     157,    0, 1247,    2, 0x08,  186 /* Private */,
     158,    0, 1248,    2, 0x08,  187 /* Private */,
     159,    1, 1249,    2, 0x08,  188 /* Private */,
     161,    1, 1252,    2, 0x08,  190 /* Private */,
     163,    0, 1255,    2, 0x08,  192 /* Private */,
     164,    0, 1256,    2, 0x08,  193 /* Private */,
     165,    0, 1257,    2, 0x08,  194 /* Private */,
     166,    0, 1258,    2, 0x08,  195 /* Private */,
     167,    0, 1259,    2, 0x08,  196 /* Private */,
     168,    0, 1260,    2, 0x08,  197 /* Private */,
     169,    0, 1261,    2, 0x08,  198 /* Private */,
     170,    0, 1262,    2, 0x08,  199 /* Private */,
     171,    1, 1263,    2, 0x08,  200 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,   17,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::LongLong, 0x80000000 | 74,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 160,    2,
    QMetaType::Void, 0x80000000 | 162,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_Mainwindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN13UI_MainwindowE.offsetsAndSizes,
    qt_meta_data_ZN13UI_MainwindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13UI_MainwindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_Mainwindow, std::true_type>,
        // method 'file_position_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<long long, std::false_type>,
        // method 'annot_docklist_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'edfplus_remove_duplicate_annotations'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'video_player_seek'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'video_player_toggle_pause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_left'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_right'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_up'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_down'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'next_page'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'next_epoch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<long long, std::false_type>,
        // method 'dig_min_max_overflow_timer_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_viewtime'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<long long, std::false_type>,
        // method 'fit_signals_to_pane'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fit_signals_to_pane'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fit_signals_dc_offset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fit_signals_dc_offset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_dc_offset_to_zero'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_dc_offset_to_zero'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'signalcomp_invert'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'signalcomp_invert'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_timesync_reference'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_timesync'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'shift_vert_scrollbar_up_act_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_vert_scrollbar_down_act_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_manual'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const char *, std::false_type>,
        // method 'set_page_div2'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_page_mult2'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_new_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_file_info'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'close_file_action_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'close_all_files'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exit_program'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'signalproperties_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'filterproperties_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_signals_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_about_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_display_time'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'set_display_time_whole_rec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_amplitude'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'set_amplitude_div2'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_amplitude_mult2'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_vert_scrollbar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_view_markers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_linear_interpol'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'former_page'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'playback_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop_playback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'zoomback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'forward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_splashscreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'export_to_ascii'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'check_edf_compatibility'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_new_filter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_plif_ecg_filter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_plif_eeg_filter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_fir_filter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_spike_filter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_filters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_plif_ecg_filters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_plif_eeg_filters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_fir_filters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_spike_filters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_new_math_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove_all_math_funcs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'jump_to_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'jump_to_start'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'jump_to_end'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'jump_to_time_millisec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<long long, std::false_type>,
        // method 'show_annotations'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_options_dialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'get_long_time'
        QtPrivate::TypeAndForceComplete<long long, std::false_type>,
        QtPrivate::TypeAndForceComplete<char *, std::false_type>,
        // method 'nk2edf_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_timesync'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'set_timesync_reference'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'recent_file_action_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'recent_session_action_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'sync_by_crosshairs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_montage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'load_montage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'view_montage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'view_session'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_this_montage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_help'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_kb_shortcuts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_user_defined_display_time'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_user_defined_amplitude'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_bdf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_640x480'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_1920x1080'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_2560x1440'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_3840x2160'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_800x600'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_1024x768'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_1280x1024'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'print_to_img_1600x1200'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_ascii_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_fino_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_wave_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_fm_audio_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_mortara_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_ishne_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_cardiotrak_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_nexfin_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'edfd_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slider_moved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'convert_emsa_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'bdf2edf_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'annotation_editor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_session'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'load_session'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unisens2edf_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'BI98002edf_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'export_annotations'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unify_resolution'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'load_predefined_mtg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'edit_predefined_montages'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_spectrum_dock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_cdsa_dock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_aeeg_dock'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_hypnogram'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_10mmmin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_15mmmin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_30mmmin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_1mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_2mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_5mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_10mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_15mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_25mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_30mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_50mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_60mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_100mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_120mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_250mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'page_500mmsec'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reduce_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'edit_header'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'biosemi2bdfplus_converter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'import_annotations'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_stream'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'start_stop_video'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop_video_generic'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'live_stream_timer_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'video_poll_timer_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'playback_realtime_timer_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'organize_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Escape_fun'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'export_ecg_rr_interval_to_ascii'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'qrs_detector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_binary_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_manscan_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_scpecg_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_mit_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'convert_biox_to_edf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'video_process_error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>,
        // method 'vlc_sock_error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'export_filtered_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'video_player_faster'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'video_player_slower'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'create_annot_by_rect_draw'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rc_host_server_new_connection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rc_host_sock_rxdata_handler'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rc_host_sock_disconnected_handler'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'desktop_resized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mc_v_scrollbar_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void UI_Mainwindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_Mainwindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->file_position_changed((*reinterpret_cast< std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 1: _t->annot_docklist_changed(); break;
        case 2: _t->remove_all_signals(); break;
        case 3: _t->edfplus_remove_duplicate_annotations(); break;
        case 4: _t->video_player_seek((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->video_player_toggle_pause(); break;
        case 6: _t->shift_page_left(); break;
        case 7: _t->shift_page_right(); break;
        case 8: _t->shift_page_up(); break;
        case 9: _t->shift_page_down(); break;
        case 10: _t->next_page(); break;
        case 11: _t->next_epoch((*reinterpret_cast< std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 12: _t->dig_min_max_overflow_timer_func(); break;
        case 13: _t->set_viewtime((*reinterpret_cast< std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 14: _t->fit_signals_to_pane((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->fit_signals_to_pane(); break;
        case 16: _t->fit_signals_dc_offset((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->fit_signals_dc_offset(); break;
        case 18: _t->set_dc_offset_to_zero((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->set_dc_offset_to_zero(); break;
        case 20: _t->signalcomp_invert((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 21: _t->signalcomp_invert((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 22: _t->set_timesync_reference((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->set_timesync((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->shift_vert_scrollbar_up_act_triggered(); break;
        case 25: _t->shift_vert_scrollbar_down_act_triggered(); break;
        case 26: _t->open_manual((*reinterpret_cast< std::add_pointer_t<const char*>>(_a[1]))); break;
        case 27: _t->set_page_div2(); break;
        case 28: _t->set_page_mult2(); break;
        case 29: _t->open_new_file(); break;
        case 30: _t->show_file_info(); break;
        case 31: _t->close_file_action_func((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 32: _t->close_all_files(); break;
        case 33: _t->exit_program(); break;
        case 34: _t->signalproperties_dialog(); break;
        case 35: _t->filterproperties_dialog(); break;
        case 36: _t->add_signals_dialog(); break;
        case 37: _t->show_about_dialog(); break;
        case 38: _t->set_display_time((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 39: _t->set_display_time_whole_rec(); break;
        case 40: _t->set_amplitude((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 41: _t->set_amplitude_div2(); break;
        case 42: _t->set_amplitude_mult2(); break;
        case 43: _t->set_vert_scrollbar((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 44: _t->set_view_markers((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 45: _t->set_linear_interpol((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 46: _t->former_page(); break;
        case 47: _t->playback_file(); break;
        case 48: _t->stop_playback(); break;
        case 49: _t->zoomback(); break;
        case 50: _t->forward(); break;
        case 51: _t->show_splashscreen(); break;
        case 52: _t->export_to_ascii(); break;
        case 53: _t->check_edf_compatibility(); break;
        case 54: _t->add_new_filter(); break;
        case 55: _t->add_plif_ecg_filter(); break;
        case 56: _t->add_plif_eeg_filter(); break;
        case 57: _t->add_fir_filter(); break;
        case 58: _t->add_spike_filter(); break;
        case 59: _t->remove_all_filters(); break;
        case 60: _t->remove_all_plif_ecg_filters(); break;
        case 61: _t->remove_all_plif_eeg_filters(); break;
        case 62: _t->remove_all_fir_filters(); break;
        case 63: _t->remove_all_spike_filters(); break;
        case 64: _t->add_new_math_func(); break;
        case 65: _t->remove_all_math_funcs(); break;
        case 66: _t->jump_to_dialog(); break;
        case 67: _t->jump_to_start(); break;
        case 68: _t->jump_to_end(); break;
        case 69: _t->jump_to_time_millisec((*reinterpret_cast< std::add_pointer_t<qlonglong>>(_a[1]))); break;
        case 70: _t->show_annotations(); break;
        case 71: _t->show_options_dialog(); break;
        case 72: { qlonglong _r = _t->get_long_time((*reinterpret_cast< std::add_pointer_t<char*>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qlonglong*>(_a[0]) = std::move(_r); }  break;
        case 73: _t->nk2edf_converter(); break;
        case 74: _t->set_timesync((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 75: _t->set_timesync_reference((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 76: _t->recent_file_action_func((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 77: _t->recent_session_action_func((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 78: _t->sync_by_crosshairs(); break;
        case 79: _t->save_montage(); break;
        case 80: _t->load_montage(); break;
        case 81: _t->view_montage(); break;
        case 82: _t->view_session(); break;
        case 83: _t->show_this_montage(); break;
        case 84: _t->show_help(); break;
        case 85: _t->show_kb_shortcuts(); break;
        case 86: _t->print_to_edf(); break;
        case 87: _t->set_user_defined_display_time(); break;
        case 88: _t->set_user_defined_amplitude(); break;
        case 89: _t->print_to_bdf(); break;
        case 90: _t->print_to_img_640x480(); break;
        case 91: _t->print_to_img_1920x1080(); break;
        case 92: _t->print_to_img_2560x1440(); break;
        case 93: _t->print_to_img_3840x2160(); break;
        case 94: _t->print_to_img_800x600(); break;
        case 95: _t->print_to_img_1024x768(); break;
        case 96: _t->print_to_img_1280x1024(); break;
        case 97: _t->print_to_img_1600x1200(); break;
        case 98: _t->convert_ascii_to_edf(); break;
        case 99: _t->convert_fino_to_edf(); break;
        case 100: _t->convert_wave_to_edf(); break;
        case 101: _t->convert_fm_audio_to_edf(); break;
        case 102: _t->convert_mortara_to_edf(); break;
        case 103: _t->convert_ishne_to_edf(); break;
        case 104: _t->convert_cardiotrak_to_edf(); break;
        case 105: _t->convert_nexfin_to_edf(); break;
        case 106: _t->edfd_converter(); break;
        case 107: _t->slider_moved((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 108: _t->convert_emsa_to_edf(); break;
        case 109: _t->bdf2edf_converter(); break;
        case 110: _t->annotation_editor(); break;
        case 111: _t->save_file(); break;
        case 112: _t->save_session(); break;
        case 113: _t->load_session(); break;
        case 114: _t->unisens2edf_converter(); break;
        case 115: _t->BI98002edf_converter(); break;
        case 116: _t->export_annotations(); break;
        case 117: _t->unify_resolution(); break;
        case 118: _t->load_predefined_mtg((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 119: _t->edit_predefined_montages(); break;
        case 120: _t->show_spectrum_dock(); break;
        case 121: _t->show_cdsa_dock(); break;
        case 122: _t->show_aeeg_dock(); break;
        case 123: _t->show_hypnogram(); break;
        case 124: _t->page_10mmmin(); break;
        case 125: _t->page_15mmmin(); break;
        case 126: _t->page_30mmmin(); break;
        case 127: _t->page_1mmsec(); break;
        case 128: _t->page_2mmsec(); break;
        case 129: _t->page_5mmsec(); break;
        case 130: _t->page_10mmsec(); break;
        case 131: _t->page_15mmsec(); break;
        case 132: _t->page_25mmsec(); break;
        case 133: _t->page_30mmsec(); break;
        case 134: _t->page_50mmsec(); break;
        case 135: _t->page_60mmsec(); break;
        case 136: _t->page_100mmsec(); break;
        case 137: _t->page_120mmsec(); break;
        case 138: _t->page_250mmsec(); break;
        case 139: _t->page_500mmsec(); break;
        case 140: _t->reduce_signals(); break;
        case 141: _t->edit_header(); break;
        case 142: _t->biosemi2bdfplus_converter(); break;
        case 143: _t->import_annotations(); break;
        case 144: _t->open_stream(); break;
        case 145: _t->start_stop_video(); break;
        case 146: _t->stop_video_generic((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 147: _t->live_stream_timer_func(); break;
        case 148: _t->video_poll_timer_func(); break;
        case 149: _t->playback_realtime_timer_func(); break;
        case 150: _t->organize_signals(); break;
        case 151: _t->Escape_fun(); break;
        case 152: _t->export_ecg_rr_interval_to_ascii(); break;
        case 153: _t->qrs_detector(); break;
        case 154: _t->convert_binary_to_edf(); break;
        case 155: _t->convert_manscan_to_edf(); break;
        case 156: _t->convert_scpecg_to_edf(); break;
        case 157: _t->convert_mit_to_edf(); break;
        case 158: _t->convert_biox_to_edf(); break;
        case 159: _t->video_process_error((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 160: _t->vlc_sock_error((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 161: _t->export_filtered_signals(); break;
        case 162: _t->video_player_faster(); break;
        case 163: _t->video_player_slower(); break;
        case 164: _t->create_annot_by_rect_draw(); break;
        case 165: _t->rc_host_server_new_connection(); break;
        case 166: _t->rc_host_sock_rxdata_handler(); break;
        case 167: _t->rc_host_sock_disconnected_handler(); break;
        case 168: _t->desktop_resized(); break;
        case 169: _t->mc_v_scrollbar_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 38:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 40:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 74:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 75:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 76:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 77:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 118:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 160:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (UI_Mainwindow::*)(long long );
            if (_q_method_type _q_method = &UI_Mainwindow::file_position_changed; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (UI_Mainwindow::*)();
            if (_q_method_type _q_method = &UI_Mainwindow::annot_docklist_changed; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *UI_Mainwindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_Mainwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN13UI_MainwindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int UI_Mainwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 170)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 170;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 170)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 170;
    }
    return _id;
}

// SIGNAL 0
void UI_Mainwindow::file_position_changed(long long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UI_Mainwindow::annot_docklist_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
