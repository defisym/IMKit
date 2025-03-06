#include "WaveformDataInterface.h"

#include "IMGuiEx/I18NInterface.h"

const std::string& ShakeInfoStringify::ToString(const ShakeInfo& opt, const bool bBinary) {
    result = I18NFMT("Waveform start at: {}m, Peak at: {}m, "
        "Max value: {}, Length: {}m\n",
        opt.shakeStart * opt.resolution,
        (opt.shakeStart + opt.unwrap2DStart) * opt.resolution,
        opt.maxValue, opt.shakeRange * opt.resolution).c_str();

    return result;
}

ShakeInfoInterface::ShakeInfoInterface(const LogDataConfig& conf)
    :LogDataInterface(conf) {
}

const std::string& ShakeInfoInterface::ToString() {
    return Compress(stringify.ToString(*logData, config.bBinary));
}

StringResult ShakeInfoInterface::DataTypeInfo() {
    return I18N("ShakeInfo");
}

const std::string& WaveformRestoreContextStringify::ToString(const WaveformRestoreContext& data, const bool bBinary) {
    result = shakeInfoStringify.ToString(data.opt);
    const auto restore = OTDRData{ .pData = data.restore.restore.data(),
        .sz = data.restore.restore.size() };
    result += I18NFMT("Waveform: \n{}\n",
        stringify.ToString(restore, bBinary)).c_str();
    const auto fft = OTDRData{ .pData = data.restore.fft.data(),
        .sz = data.restore.fftElement };
    result += I18NFMT("Waveform FFT: \n{}\n",
        stringify.ToString(fft, bBinary)).c_str();

    return result;
}

const std::string& PeakWaveformRestoreStringify::ToString(const PeakWaveformRestoreHandler& data, const bool bBinary) {
    // reset
    result = {};

    // vibration
    const auto vlResult = OTDRData{ .pData = data.GetVibrationLocalizationData(),
        .sz = data.GetVibrationLocalizationSize() };
    result += I18NFMT("Restore base (Vibration Localization Result): \n{}\n",
        stringify.ToString(vlResult, bBinary)).c_str();

    const auto vlFilteredResult = OTDRData{ .pData = data.GetVibrationLocalizationFilteredData(),
        .sz = data.GetVibrationLocalizationFilteredSize() };
    result += I18NFMT("Process base (Filtered): \n{}\n",
        stringify.ToString(vlFilteredResult, bBinary)).c_str();

    // waveform
    const auto& results = data.GetPeakWaveformRestoreResult();
    for (const auto& it : results) {
        result += waveformStringify.ToString(it, bBinary);
    }

    return result;
}


WaveformDataInterface::WaveformDataInterface(const LogDataConfig& conf)
    :LogDataInterface(conf) {
}

const std::string& WaveformDataInterface::ToString() {
    return Compress(stringify.ToString(*logData, config.bBinary));
}

StringResult WaveformDataInterface::DataTypeInfo() {
    return I18N("Waveform");
}
