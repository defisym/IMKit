#include "WaveformDataInterface.h"

const std::string& WaveformRestoreContextStringify::ToString(const WaveformRestoreContext& data, const bool bBinary) {
    result = std::format("Waveform at: {}\n", data.opt.shakeStart + data.opt.unwrap2DStart);

    const auto restore = OTDRData{ data.restore.restore.data(),data.restore.restore.size() };
    result += std::format("Waveform: \n{}", stringify.ToString(restore, bBinary));
    const auto fft = OTDRData{ data.restore.fft.data(),data.restore.fftElement };
    result += std::format("Waveform FFT: \n{}", stringify.ToString(fft, bBinary));

    return result;
}

const std::string& PeakWaveformRestoreStringify::ToString(const PeakWaveformRestoreHandler& data, const bool bBinary) {
    // reset
    result = {};

    // vibration
    const auto vlResult = OTDRData{ data.GetVibrationLocalizationData(), data.GetVibrationLocalizationSize() };
    result += std::format("Restore base (Vibration Localization Result): {}\n", stringify.ToString(vlResult, bBinary));

    const auto vlFilteredResult = OTDRData{ data.GetVibrationLocalizationFilteredData(), data.GetVibrationLocalizationFilteredSize() };
    result += std::format("Process base (Filtered): {}\n", stringify.ToString(vlFilteredResult, bBinary));

    // waveform
    const auto results = data.GetPeakWaveformRestoreResult();
    for (const auto& it : results) {
        result += waveformStringify.ToString(it, bBinary);
    }

    return result;
}


WaveformDataInterface::WaveformDataInterface(const LogDataConfig& conf) :LogDataInterface(conf) {}

const std::string& WaveformDataInterface::ToString() {
    return Compress(stringify.ToString(*logData, config.bBinary));
}
