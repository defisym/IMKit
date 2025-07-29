#pragma once

#include <string>

#include "IMGuiEx/I18NInterface.h"

// ------------------------------------------------
// DataInterfaceBase: Handle compress & uncompress
// ------------------------------------------------

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

// ------------------------------------------------
// DataStringifyInterface
// ------------------------------------------------

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
struct is_data_stringify_interface : std::false_type {};

template <typename DataStringify, typename DataType>
struct is_data_stringify_interface<DataStringifyInterface<DataStringify, DataType>> : std::true_type {};

template <typename T>
concept ValidDataStringifyEx = is_data_stringify_interface<T>::value;

// ------------------------------------------------
// DataParseInterface
// ------------------------------------------------

template<typename DataParse, typename DataType>
concept ValidDataDataParse = requires(DataParse dataParse) {
    { dataParse.Parse(std::string_view{},
        typename DataParse::ParamType{}) } -> std::same_as<DataType>;
};

template<typename DataParse, typename DataType>
    requires ValidDataDataParse<DataParse, DataType>
class DataParseInterface :public DataInterfaceBase {  // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    //Parse return the RValue object, no cache needed

public:
    DataParseInterface(const DataInterfaceConfig& conf = {}) :DataInterfaceBase(conf) {}
    ~DataParseInterface() = default;

    [[nodiscard]] DataType Parse(const std::string_view& string,
        const DataParse::ParamType& param = {}) {
        return UnCompress(DataParse{}.Parse(string, param));
    }
};