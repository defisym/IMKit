#include "FilterHandler.h"

#include "Utilities/Param.h"

// ------------------------------------
// Hasher
// ------------------------------------

std::size_t std::hash<FilterParamBase>::operator()(
    FilterParamBase const& s) const noexcept {
    return GetParamHash(s);
}

std::size_t std::hash<LowPassFilterParam>::operator()(
    LowPassFilterParam const& s) const noexcept {
    auto hash = GetParamHash<FilterParamBase>({ s.bEnable,s.bUpdate });
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<int>{}(s.filterStopFrequency);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

std::size_t std::hash<HighPassFilterParam>::operator()(
    HighPassFilterParam const& s) const noexcept {
    auto hash = GetParamHash<FilterParamBase>({ s.bEnable,s.bUpdate });
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<int>{}(s.filterStopFrequency);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

std::size_t std::hash<MeanFilterParam>::operator()(
    MeanFilterParam const& s) const noexcept {
    auto hash = GetParamHash<FilterParamBase>({ s.bEnable,s.bUpdate });
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<int>{}(s.radius);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}
