#include "WaveformDataInterface.h"

const std::string& ShakeInfoStringify::ToString(const ShakeInfo& opt, const bool bBinary) {
    result = std::format("Waveform start at: {}m, Peak at: {}m, "
        "Max value: {}, Length: {}m\n",
        opt.shakeStart / opt.resolution,
        (opt.shakeStart + opt.unwrap2DStart) / opt.resolution,
        opt.maxValue, opt.shakeRange / opt.resolution);

    return result;
}

const std::string& WaveformRestoreContextStringify::ToString(const WaveformRestoreContext& data, const bool bBinary) {
    result = shakeInfoStringify.ToString(data.opt);
    const auto restore = OTDRData{ .pData = data.restore.restore.data(),
        .sz = data.restore.restore.size() };
    result += std::format("Waveform: \n{}\n", stringify.ToString(restore, bBinary));
    const auto fft = OTDRData{ .pData = data.restore.fft.data(),
        .sz = data.restore.fftElement };
    result += std::format("Waveform FFT: \n{}\n", stringify.ToString(fft, bBinary));

    return result;
}

const std::string& PeakWaveformRestoreStringify::ToString(const PeakWaveformRestoreHandler& data, const bool bBinary) {
    // reset
    result = {};

    // vibration
    const auto vlResult = OTDRData{ .pData = data.GetVibrationLocalizationData(),
        .sz = data.GetVibrationLocalizationSize() };
    result += std::format("Restore base (Vibration Localization Result): \n{}\n",
        stringify.ToString(vlResult, bBinary));

    const auto vlFilteredResult = OTDRData{ .pData = data.GetVibrationLocalizationFilteredData(),
        .sz = data.GetVibrationLocalizationFilteredSize() };
    result += std::format("Process base (Filtered): \n{}\n",
        stringify.ToString(vlFilteredResult, bBinary));

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

const char* WaveformDataInterface::DataTypeInfo() {
    return "Waveform";
}
