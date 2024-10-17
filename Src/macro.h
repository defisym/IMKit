#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// ------------------------------------
// Debug only
// ------------------------------------
#ifdef _DEBUG
// always show device config even if no device created
#define CONFIG_ALWAYS_SHOW_DEVICE_PARAM
#endif

// ------------------------------------
// General
// ------------------------------------
// only show the result
#define VIBRATION_LOCALIZATION_ONLY_SHOW_RESULT
#define WAVEFORM_RESTORE_ONLY_SHOW_RESULT

// try to read saved config even if the struct size changed
#define CONFIG_TRY_READ_CONFIG