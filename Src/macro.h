// ReSharper disable CppClangTidyClangDiagnosticInvalidUtf8
#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// ------------------------------------
// Debug only
// ------------------------------------
#ifdef _DEBUG
// show imgui demo window
//#define SHOW_DEMO_WINDOW

// show font debug
#define FONT_SHOW_FONT_DEBUG

// always show device config even if no device created
#define CONFIG_ALWAYS_SHOW_DEVICE_PARAM
#endif

// ------------------------------------
// General
// ------------------------------------
// load all fonts
//#define FONT_LOAD_ALL_FONTS

// only load simplified chinese 2500 regularly used characters
// speed up font texture generate, good for debug
// for imgui, japanese katakana will be included also
// but some characters like �D (not regularly used) and �~ (kokuji)
// will not be displayed properly
// https://en.wikipedia.org/wiki/Kokuji
#define FONT_SIMPLIFIED_CHINESE_COMMON_ONLY

// try to read saved config even if the struct size changed
#define CONFIG_TRY_READ_CONFIG

// skip if process takes too long
#define READER_SKIP_FRAME
// output debug string when skip triggered
#define READER_OUTPUT_DEBUG_STRING_WHEN_SKIP_FRAME

// only show the result
#define VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
#define WAVEFORM_RESTORE_ONLY_SHOW_RESULT

// show logger threshold
#define VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD