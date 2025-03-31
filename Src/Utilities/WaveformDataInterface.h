#pragma once

#include "Logger.h"
#include "OTDRDataInterface.h"

#include "Processor/Waveform/WaveformRestoreHandler.h"

struct ShakeInfoStringify {
    std::string result;
    [[nodiscard]] const std::string& ToString(const ShakeInfo& opt, const bool bBinary = false);
};

class ShakeInfoInterface final :public LogDataInterface {
    using DataType = const  ShakeInfo*;
    DataType logData = nullptr;
    ShakeInfoStringify stringify;

public:
    ShakeInfoInterface(const LogDataConfig& conf = {});

    void UpdateData(const DataType& data) { logData = data; }
    [[nodiscard]] const DataType& GetData() const { return logData; }
    [[nodiscard]] const std::string& ToString() override;
    [[nodiscard]] StringResult DataTypeInfo() override;
};

static_assert(ValidDataInterface<ShakeInfoInterface>);

struct WaveformRestoreContextStringify {
    std::string result;
    OTDRDataStringify stringify = {};
    ShakeInfoStringify shakeInfoStringify = {};
    [[nodiscard]] const std::string& ToString(const WaveformRestoreContext& data, const bool bBinary = false);
};

struct PeakWaveformRestoreStringify {
    std::string result;
    OTDRDataStringify stringify = {};
    WaveformRestoreContextStringify waveformStringify = {};
    [[nodiscard]] const std::string& ToString(const PeakWaveformRestoreHandler& data, const bool bBinary = false);
};

class WaveformDataInterface final :public LogDataInterface {
    using DataType = const PeakWaveformRestoreHandler*;
    DataType logData = nullptr;
    PeakWaveformRestoreStringify stringify;

public:
    WaveformDataInterface(const LogDataConfig& conf = {});

    void UpdateData(const DataType& data) { logData = data; }
    [[nodiscard]] const DataType& GetData() const { return logData; }
    [[nodiscard]] const std::string& ToString() override;
    [[nodiscard]] StringResult DataTypeInfo() override;
};

static_assert(ValidDataInterface<WaveformDataInterface>);
