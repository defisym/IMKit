#include "VibrationDataInterface.h"


VibrationDataInterface::VibrationDataInterface(const LogDataConfig& conf) :LogDataInterface(conf) {}

void VibrationDataInterface::UpdateData(const DataType& data) {
    this->logData = data;
}

const std::string& VibrationDataInterface::ToString() {
    return Compress(stringify.ToString(logData, config.bBinary));    
}

const char* VibrationDataInterface::DataTypeInfo() {
    return "Vibration";
}
