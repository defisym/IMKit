#include "DataInterface.h"

#include <Compress/zlibInterface.h>

#include "GUIContext/Param/Param.h"

std::size_t std::hash<DataInterfaceConfig>::operator()(DataInterfaceConfig const& s) const noexcept {
    return GetParamHash(s);
}

bool Compress(const std::string& in, std::string& out) {
    const auto sz = compressBound(static_cast<uLong>(in.size()));
    out.clear();
    out.resize(sz, '\0');

    uLong compressedSize = sz;

    const auto ret = compress(reinterpret_cast<Bytef*>(out.data()),
        &compressedSize,
        reinterpret_cast<const Bytef*>(in.data()),
        static_cast<uLong>(in.size()));

    if (ret != Z_OK) { compressedSize = 0; }
    out.resize(compressedSize);

    return ret == Z_OK;
}
