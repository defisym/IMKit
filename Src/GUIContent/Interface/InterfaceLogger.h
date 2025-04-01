#pragma once

struct Logger;
struct LoggerConfig;
struct ImVec2;
void InterfaceLogger(const char* pID,
    Logger* pLogger, LoggerConfig* pConfig,
    const ImVec2* pSize = nullptr);
