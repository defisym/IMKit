#include "DataInterface.h"

#include <Compress/zlibInterface.h>

#include "GUIContext/Param/Param.h"

std::size_t std::hash<DataInterfaceConfig>::operator()(DataInterfaceConfig const& s) const noexcept {
    return GetParamHash(s);
}

bool Compress(const std::string& in, std::string& out) {
    return ZLIBI_Compress(in, out);
}

bool UnCompress(const std::string& in, std::string& out) {
    return ZLIBI_UnCompress(in, out);
}
