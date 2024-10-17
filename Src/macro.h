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
// always show device config even if no device created
#define CONFIG_ALWAYS_SHOW_DEVICE_PARAM
#endif

// ------------------------------------
// General
// ------------------------------------
// try to read saved config even if the struct size changed
#define CONFIG_TRY_READ_CONFIG

// skip if process takes too long
#define READER_SKIP_FRAME

// only show the result
#define VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
#define WAVEFORM_RESTORE_ONLY_SHOW_RESULT

// show logger threshold
#define VIBRATION_LOCALIZATION_SHOW_LOGGER_THRESHOLD