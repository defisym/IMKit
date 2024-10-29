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

// always show device config even if no device created
#define CONFIG_ALWAYS_SHOW_DEVICE_PARAM
#endif

// ------------------------------------
// General
// ------------------------------------
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