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

#endif

// ------------------------------------
// General
// ------------------------------------
// multi-thread
#define MULTITHREAD

#ifdef MULTITHREAD
// process thread will sleep when no data for an estimated delay
// can reduce CPU usage
#define MULTITHREAD_SLEEP_WHEN_NODATA
#endif

// set priority to ABOVE_NORMAL_PRIORITY_CLASS
#define PROCESS_SET_PRIORITY

// bind process to given CPU core instead of 
// let operarte system schedule
#define PROCESS_SET_AFFINITY

// use high performance GPU
// if not set, the first adaptor will be used
//#define USE_HIGEPERFORMANCE_GPU

#ifdef USE_HIGEPERFORMANCE_GPU
// use NVIDIA
// ref: https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
#define USE_NVIDIA_GPU

#ifndef USE_NVIDIA_GPU
// use AMD
// ref: https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
// #define USE_AMD_GPU

#ifndef USE_AMD_GPU
// use intel Arc Graphics
// not implemented
#define USE_INTEL_GPU
#endif
#endif
#endif

// indent inside tab
//#define INDENT_INSIDE_TAB

// json crashes in profile mode
// enable this macro to measure performance
//#define NO_I18N

// audio thread will always run and mess the profile result
//#define NO_AUDIO

// auto set when profiling
//#define PROFILE

#ifdef PROFILE
#ifndef NO_I18N
#define NO_I18N
#endif

#ifndef NO_AUDIO
#define NO_AUDIO
#endif
#endif

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

// check the version by size of param struct
// disable will use the hash of default param instead
//#define CONFIG_VERSION_BY_SIZE

#ifdef CONFIG_VERSION_BY_SIZE
// try to read saved config even if the struct size changed
#define CONFIG_TRY_READ_CONFIG
#endif

// optimize plot display by using threshold, by skip interval points
// the shape of plot will be changed if the original data fluctuates dramatically
// and affect zoom in
// disable it if your want the exactly result, or the target PC is powerful enough
//#define OPTIMIZE_PLOT_DISPLAY

#ifdef OPTIMIZE_PLOT_DISPLAY
#define OPTIMIZE_PLOT_DISPLAY_THRESHOLD 512
#else
#include <limits.h>
#define OPTIMIZE_PLOT_DISPLAY_THRESHOLD INT_MAX
#endif
