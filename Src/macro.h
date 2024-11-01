// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// ------------------------------------
// Note
// ------------------------------------
// For build performance when changing macro
// you shouldn't include in a header file

// ------------------------------------
// Debug only
// ------------------------------------
#ifdef _DEBUG
// show imgui demo window
//#define SHOW_DEMO_WINDOW

// show font debug
//#define FONT_SHOW_FONT_DEBUG

// always show device config even if device not created
#define CONFIG_ALWAYS_SHOW_DEVICE_PARAM

// always show calibrate config even if device not started
#define CONFIG_ALWAYS_SHOW_CALIBRATE_PARAM
#endif

// ------------------------------------
// General
// ------------------------------------
// set priority to ABOVE_NORMAL_PRIORITY_CLASS
#define PROCESS_SET_PRIORITY

// bind to CPU core 0
#define PROCESS_SET_AFFINITY

// load all fonts in fonts folder, disabled will use 
//#define FONT_LOAD_ALL_FONTS

// only load simplified chinese 2500 regularly used characters
// speed up font texture generate, good for debug
// for imgui, japanese katakana will be included also
// but some characters like †D (not regularly used) and ë~ (kokuji)
// will not be displayed properly
// https://en.wikipedia.org/wiki/Kokuji
#ifndef NDEBUG
// in release mode should be fast enough
#define FONT_SIMPLIFIED_CHINESE_COMMON_ONLY
#endif

#ifndef FONT_SIMPLIFIED_CHINESE_COMMON_ONLY
#define FONT_SIMPLIFIED_CHINESE_ONLY
#endif

// chech the version by size of param struct
// disable will use the hash of default param instead
//#define CONFIG_VERSION_BY_SIZE

#ifdef CONFIG_VERSION_BY_SIZE
// try to read saved config even if the struct size changed
#define CONFIG_TRY_READ_CONFIG
#endif

// allow custom mode in easy mode
#define CONFIG_ALLOW_CUSTOM_IN_EASYMODE

// auto create device, if failed, show the divice select part
#define CONFIG_AUTO_CREATE_DEVICE

// force enable in debug mode
#ifndef NDEBUG
#ifndef CONFIG_ALLOW_CUSTOM_IN_EASYMODE
#define CONFIG_ALLOW_CUSTOM_IN_EASYMODE
#endif
#endif

// skip internal points in optical module
// as it's too complicated to change the process algorithm
// and comparing to the total length the internal part can be ignored
// so process those data won't consume much performance
// so it's just a UI change and nothing modified internally
#define SKIP_INTERNAL_POINTS

// skip if process takes too long
#define READER_SKIP_FRAME
// output debug string when skip triggered
#define READER_OUTPUT_DEBUG_STRING_WHEN_SKIP_FRAME

// optimize plot display by using threshold, by skip interval points
// the shape of plot will be changed if the orginal data fluctates dramatically
// and affect zoom in
// disable it if your want the exactly result, or the target PC is powerful enough
//#define OPTIMIZE_PLOT_DSIPLAY

#ifdef OPTIMIZE_PLOT_DSIPLAY
#define OPTIMIZE_PLOT_DSIPLAY_THRESHOLD 512
#else
#include <limits.h>
#define OPTIMIZE_PLOT_DSIPLAY_THRESHOLD INT_MAX
#endif

// using context to calibrate, more accurate
// note: peak wave restore will process when wave buffer filled
//       disable it to have the same behaviour
//#define CALIBRATE_USING_CONTEXT

// normalize result by frame count
#define VIBRATION_LOCALIZATION_NORMALIZE_RESULT_BY_FRAMECOUNT

// only show the result
#define VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
#define WAVEFORM_RESTORE_ONLY_SHOW_RESULT

// always update
#ifdef VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
#define VIBRATION_LOCALIZATION_ALWAYS_UPDATE
#endif

#ifdef WAVEFORM_RESTORE_ONLY_SHOW_RESULT
#define WAVEFORM_RESTORE_ALWAYS_UPDATE
#endif

// log viberation waveform
// only work when vibration localization is always update
#ifdef VIBRATION_LOCALIZATION_ALWAYS_UPDATE

// log viberation waveform of the point
// that exceed the threshold
#define WAVEFORM_RESTORE_LOG_PEAK_WAVEFORM

#ifdef WAVEFORM_RESTORE_LOG_PEAK_WAVEFORM
// this shares the wave raw data, so wavefrom always update
// must be enabled
#ifndef WAVEFORM_RESTORE_ALWAYS_UPDATE
#define WAVEFORM_RESTORE_ALWAYS_UPDATE
#endif

// don't filter raw data
//#define WAVEFORM_RESTORE_LOG_PEAK_NO_FILTER

#ifndef WAVEFORM_RESTORE_LOG_PEAK_NO_FILTER
#define WAVEFORM_RESTORE_LOG_PEAK_FILTER_RADIUS 1
#endif

// don't remove peak that is too small
//#define WAVEFORM_RESTORE_LOG_PEAK_ALL_PEAK

#ifndef WAVEFORM_RESTORE_LOG_PEAK_ALL_PEAK
#define WAVEFORM_RESTORE_LOG_PEAK_ALL_PEAK_RANGE 3
#endif

// display logged waveforms of each peak
#define WAVEFORM_RESTORE_LOG_PEAK_SHOW_LOGGED_WAVEFORM

#endif
#endif

// show logger threshold
#define VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD

// convert result
#define VIBRATION_LOCALIZATION_USE_METER
#define WAVEFORM_RESTORE_USE_MILLISECOND

// display in subplot
#define WAVEFORM_RESTORE_DISPLAY_WAVE_AND_FFT_IN_SUBPLOT