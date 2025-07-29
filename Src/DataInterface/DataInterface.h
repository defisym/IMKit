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
bool UnCompress(const std::string& in, std::string& out);

class DataInterfaceBase {
protected:
    DataInterfaceConfig config = {};
    std::string compress;

public:
    DataInterfaceBase(const DataInterfaceConfig& conf = {}) { UpdateConfig(conf); }
    virtual ~DataInterfaceBase() = default;

    void UpdateConfig(const DataInterfaceConfig& conf = {}) { this->config = conf; }
    [[nodiscard]] const std::string& Compress(const std::string& str) {
        if (!config.bCompress) { return str; }
        ::Compress(str, compress); return compress;
    }
    [[nodiscard]] const std::string& UnCompress(const std::string& str) {
        if (!config.bCompress) { return str; }
        ::UnCompress(str, compress); return compress;
    }
};

template<typename DataStringify, typename DataType>
concept ValidDataStringify = requires(DataStringify dataStringify) {
    { dataStringify.DataTypeInfo() } -> std::same_as<const char*>;
    { dataStringify.ToString(DataType{}, bool{}) } -> std::same_as<const std::string&>;
};

template<typename DataStringify, typename DataType>
    requires ValidDataStringify<DataStringify, DataType>
class DataStringifyInterface :public DataInterfaceBase {  // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    DataStringify stringify = {};

public:
    DataStringifyInterface(const DataInterfaceConfig& conf = {}) :DataInterfaceBase(conf) {}
    ~DataStringifyInterface() = default;

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
struct is_data_interface<DataStringifyInterface<DataStringify, DataType>> : std::true_type {};

template <typename T>
concept ValidDataStringifyEx = is_data_interface<T>::value;


template<typename DataParse, typename DataType, typename ParseParam>
concept ValidDataDataParse = requires(DataParse dataStringify) {
    { dataStringify.ToString(DataType{}, ParseParam{}) } -> std::same_as<DataType>;
};