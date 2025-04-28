#pragma once

#include <string>

#include "IMGuiEx/I18NInterface.h"

struct DataInterfaceConfig {
    // save file in binary, otherwise is human-readable
    bool bBinary = false;
    // compress data, only works in binary mode
    bool bCompress = false;
};

template<>
struct std::hash<DataInterfaceConfig> {
    std::size_t operator()(DataInterfaceConfig const& s) const noexcept;
};

bool Compress(const std::string& in, std::string& out);

template<typename DataStringify, typename DataType>
concept ValidDataStringify = requires(DataStringify dataStringify) {
    { dataStringify.DataTypeInfo() } -> std::same_as<const char*>;
    { dataStringify.ToString(DataType{}, bool{}) } -> std::same_as<const std::string&>;
};

template<typename DataStringify, typename DataType>
    requires ValidDataStringify<DataStringify, DataType>
class DataInterface {  // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    DataInterfaceConfig config = {};
    DataStringify stringify = {};
    std::string compressed;

public:
    DataInterface(const DataInterfaceConfig& conf = {}) { UpdateConfig(conf); }
    ~DataInterface() = default;

    void UpdateConfig(const DataInterfaceConfig& conf = {}) { this->config = conf; }
    [[nodiscard]] const std::string& Compress(const std::string& str) {
        if (!config.bCompress) { return str; }
        ::Compress(str, compressed); return compressed;
    }
    [[nodiscard]] const std::string& ToString(const DataType& data) { 
        return Compress(stringify.ToString(data, config.bBinary));
    }
    [[nodiscard]] StringResult DataTypeInfo() {
        return I18N(stringify.DataTypeInfo());
    }
};

template <typename T>
struct is_data_interface : std::false_type {};

template <typename DataStringify, typename DataType>
struct is_data_interface<DataInterface<DataStringify, DataType>> : std::true_type {};

template <typename T>
concept ValidDataInterfaceEx = is_data_interface<T>::value;