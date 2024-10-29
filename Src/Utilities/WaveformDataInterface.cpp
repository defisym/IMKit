#include "WaveformDataInterface.h"

const std::string& WaveformRestoreContextStringify::ToString(const WaveformRestoreContext& data, const bool bBinary) {
    result = std::format("Waveform at : {}\n", data.opt.shakeStart + data.opt.unwrap2DStart);

    const auto restore = OTDRData{ data.restore.restore.data(),data.restore.restore.size() };
    result += std::format("Waveform : {}\n", stringify.ToString(restore));
    const auto fft = OTDRData{ data.restore.fft.data(),data.restore.fftElement };
    result += std::format("Waveform : {}\n", stringify.ToString(fft));

    return result;
}

WaveformDataInterface::WaveformDataInterface(const LogDataConfig& conf) :LogDataInterface(conf) {}

const std::string& WaveformDataInterface::ToString() {
    return Compress(stringify.ToString(*logData, config.bBinary));
}
