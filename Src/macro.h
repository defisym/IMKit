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
// multi-thread
#define MULTITHREAD

// set priority to ABOVE_NORMAL_PRIORITY_CLASS
#define PROCESS_SET_PRIORITY

// bind to CPU core 0
#define PROCESS_SET_AFFINITY

// json crahes in profile mode
// enable this macro to measure performance
//#define NO_I18N

// disable vsync
#ifndef MULTITHREAD
// UI uses a spin lock to protect data
// disable it as processor may cannot get the lock
#define DISPLAY_NO_SYNC
#endif

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

#ifdef READER_SKIP_FRAME
// do not skip frame until receive processFrameCount frames
// if internal frames are dropped, there will be a quick change of waveform
// and create other frequency in FFT result
// especially on low-end PCs
// 
// NOTE: 1. disable context has the same effect
//          but also have side effect in 3.
//       2. if the macro is defined, and context mode enabled
//          then the vibration result will be affected, as
//          internal data are ignored but context mode assume
//          all data are consecutive
//       3. drain may cost a long time as buffer may have accumulated too many data
//          E.g., 1. sample 75000 points, uses 300KB, as one frame 
//                2. average by 2560 frames, uses 768MB, as one bunch
//                3. assume PC needs to skip two bunches, uses about 1.5GB
//                   and PCIe speed of DAQ is 1.5GB/s, skip will take 1000ms
//                   which causes more delay and more data needs to be skipped
//          so it's a choice, drop interval frames will affect the result
//          but won't take long when drain data (E.g., 200 frames, only takes 40ms)
// Advice: 1. Enable context
//         2. By a decent PC
//#define READER_SKIP_FRAME_WHEN_FILLED

// output debug string when skip triggered
#define READER_SKIP_FRAME_OUTPUT_DEBUG_STRING
#endif

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

// TODO Range should be passed in `WaveformRestoreParams`
// current it will be force enabled and cause crash
// that's why this macro is disabled for now

// use moving diff
//#define WAVEFORM_RESTORE_MOVING_DIFF
#ifdef WAVEFORM_RESTORE_MOVING_DIFF
#define WAVEFORM_RESTORE_MOVING_DIFF_DEFAULTRANGE 2
#endif

// use rising edge instead of peak
//#define WAVEFORM_RESTORE_USING_RISING_EDGE
#ifdef WAVEFORM_RESTORE_USING_RISING_EDGE
// default percent
// if shake starts at 0, peak at 10, the point for unwrap 2D is 5
// aka start + (peak - start) * WAVEFORM_RESTORE_USING_RISING_EDGE_PERCENT
// set this to 1 is the same as disable this macro
#define WAVEFORM_RESTORE_USING_RISING_EDGE_PERCENT 0.5
#endif

// log viberation waveform of the point
// that exceed the threshold
#define WAVEFORM_RESTORE_LOG_PEAK_WAVEFORM

#ifdef WAVEFORM_RESTORE_LOG_PEAK_WAVEFORM
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

// multithread
//#define WAVEFORM_RESTORE_LOG_PEAK_MULTITHREAD

#endif

// show logger threshold
#define VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD

// convert result
#define VIBRATION_LOCALIZATION_USE_METER
#define WAVEFORM_RESTORE_USE_MILLISECOND

// display in subplot
#define WAVEFORM_RESTORE_DISPLAY_WAVE_AND_FFT_IN_SUBPLOT