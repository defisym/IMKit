#pragma once

#include "macro.h"

#include <cstdio>
#include <concepts>

#include "Param.h"

namespace ConfigHandler {
    struct FileHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
        FILE* fp = nullptr;
        ~FileHelper() { if (fp) { [[maybe_unused]] const auto err = fclose(fp); } }
    };

    template<typename T>
    concept CanRead = requires(T t) {
        { t.Read((FILE*)nullptr) } -> std::same_as<bool>;
    };

    template<typename T>
    inline bool ReadHelper(FILE* fp, T* pDst) {
        if (!std::is_trivially_copyable_v<T> && !CanRead<T>) { return false; }

        auto readFunc = [&] (void* pBuf, const size_t readSz) {
            const auto read = fread(pBuf, readSz, 1, fp);
            if (read == 0) { return false; }

            return true;
            };

        size_t sz = 0;
        if (!readFunc(&sz, sizeof(sz))) { return false; }
        size_t version = 0;
        if (!readFunc(&version, sizeof(version))) { return false; }

        // type changed
#ifdef CONFIG_VERSION_BY_SIZE
        if (sz != sizeof(T)) {
#else
        if (version != GetParamHash<T>()) {
#endif

#ifdef CONFIG_TRY_READ_CONFIG
            return readFunc(pDst, sz);
#else
            // skip
            return fseek(fp, static_cast<long>(sz), SEEK_CUR) == 0;
#endif
        }

        if constexpr (CanRead<T>) {
            return pDst->Read(fp);
        }
        else {
            return readFunc(pDst, sizeof(T));
        }
    }

    template <class Type>
    inline bool LoadConfig(FILE* fp, Type& config) {
        return ReadHelper(fp, &config);
    }

    template <class Type, class... Types>
    inline bool LoadConfig(FILE* fp, Type& config, Types&&... configs) {
        if (!ReadHelper(fp, &config)) { return false; }
        return LoadConfig(fp, std::forward<Types>(configs)...);
    }

    template<typename T>
    concept CanWrite = requires(T t) {
        { t.Write((FILE*)nullptr) } -> std::same_as<size_t>;
    };

    template<typename T>
    inline bool WriteHelper(FILE * fp, T * pDst) {
        if (!std::is_trivially_copyable_v<T> && !CanWrite<T>) { return false; }

        const size_t sz = sizeof(T);
        size_t write = fwrite(&sz, sizeof(sz), 1, fp);
        if (write == 0) { return false; }

        const size_t version = GetParamHash<T>();
        write = fwrite(&version, sizeof(version), 1, fp);
        if (write == 0) { return false; }

        if constexpr (CanWrite<T>) {
            write = pDst->Write(fp);
        }
        else {
            write = fwrite(pDst, sizeof(T), 1, fp);
        }
        if (write == 0) { return false; }

        return true;
    }

    template <class Type>
    inline bool SaveConfig(FILE* fp, Type& config) {
        return WriteHelper(fp, &config);
    }

    template <class Type, class... Types>
    inline bool SaveConfig(FILE* fp, Type& config, Types&&... configs) {
        if (!WriteHelper(fp, &config)) { return false; }
        return SaveConfig(fp, std::forward<Types>(configs)...);
    }
};

template <class... Types>
inline bool LoadConfig(char const* pFileName, Types&&... configs) {
    FILE* fp = nullptr;

    const auto err = fopen_s(&fp, pFileName, "rb");
    if (err != 0 || fp == nullptr) { return false; }

    auto helper = ConfigHandler::FileHelper{ fp };

    return ConfigHandler::LoadConfig(fp, std::forward<Types>(configs)...);
}

template <class... Types>
inline bool SaveConfig(char const* pFileName, Types&&... configs) {
    FILE* fp = nullptr;

    const auto err = fopen_s(&fp, pFileName, "wb");
    if (err != 0 || fp == nullptr) { return false; }

    auto helper = ConfigHandler::FileHelper{ fp };

    return ConfigHandler::SaveConfig(fp, std::forward<Types>(configs)...);
}