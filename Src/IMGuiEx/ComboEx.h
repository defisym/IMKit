#pragma once

#include <vector>
#include <string>

#include "imgui.h"

struct ComboContext {
    bool bInit = false;
    size_t currentIdx = 0;
};

struct ComboInfoBase {
    const char* label = nullptr;
    ImGuiComboFlags flags = 0;
    ComboContext* pCtx = nullptr;

    ComboInfoBase(const char* label, ComboContext* pCtx, ImGuiComboFlags flags = 0) {
        this->label = label;
        this->flags = flags;
        this->pCtx = pCtx;
    }
};

struct ComboInfo :ComboInfoBase {
    size_t defaultIndex = 0u;

    ComboInfo(const char* label, ComboContext* pCtx,
        size_t defaultIndex = 0, ImGuiComboFlags flags = 0)
        :ComboInfoBase(label, pCtx, flags) {
        this->defaultIndex = defaultIndex;
    }
};

template<typename T>
struct ComboInfoEx :ComboInfoBase {
    T defaultValue = {};

    ComboInfoEx(const char* label, ComboContext* pCtx,
        T defaultValue, ImGuiComboFlags flags = 0)
        :ComboInfoBase(label, pCtx, flags) {
        this->defaultValue = defaultValue;
    }
};

template<typename T>
struct ComboItem {
    std::string displayName = "Undefined";
    T value = {};
};

namespace ComboExImpl {
    template<typename ReturnType, typename ItemType>
    ReturnType ComboEx(const ComboInfo& comboInfo, const std::vector<ItemType>& comboItems) {
        if (!comboInfo.pCtx->bInit) [[unlikely]] {
            assert(comboInfo.defaultIndex < comboItems.size());
            comboInfo.pCtx->currentIdx = comboInfo.defaultIndex;
            comboInfo.pCtx->bInit = true;
        }

        auto GetName = [&] (size_t index)->const char* {
            if constexpr (std::is_same_v<ItemType, ComboItem<ReturnType>>) {
                return comboItems[index].displayName.c_str();
            }
            else {
                using Type = std::remove_cvref<ItemType>;

                if constexpr (std::is_same_v<Type, std::remove_cvref<const char*>>) {
                    return comboItems[index];
                }
                else if constexpr (std::is_same_v<Type, std::remove_cvref<std::string>>) {
                    return comboItems[index].c_str();
                }
                else {
                    static_assert(false, "Error Combo Type");
                    return {};
                }
            }
            };
        auto GetValue = [&] (size_t index)-> ReturnType {
            if constexpr (std::is_same_v<ItemType, ComboItem<ReturnType>>) {
                return comboItems[index].value;
            }
            else {
                using Type = std::remove_cvref<ItemType>;

                if constexpr (std::is_same_v<Type, std::remove_cvref<const char*>>) {
                    return comboItems[index];
                }
                else if constexpr (std::is_same_v<Type, std::remove_cvref<std::string>>) {
                    return comboItems[index].c_str();
                }
                else {
                    static_assert(false, "Error Combo Type");
                    return {};
                }
            }
            };

        auto& currentIdx = comboInfo.pCtx->currentIdx;

        if (ImGui::BeginCombo(comboInfo.label,
            GetName(currentIdx),
            comboInfo.flags)) {
            for (size_t idx = 0; idx < comboItems.size(); idx++) {
                const bool bSelected = currentIdx == idx;
                if (ImGui::Selectable(GetName(idx), bSelected)) {
                    currentIdx = idx;
                }

                if (bSelected) { ImGui::SetItemDefaultFocus(); }
            }

            ImGui::EndCombo();
        }

        return GetValue(currentIdx);
    }
}

inline const char* ComboEx(const ComboInfo& comboInfo, const std::vector<const char*>& comboItems) {
    return ComboExImpl::ComboEx<const char*, const char*>(comboInfo, comboItems);
}

inline const char* ComboEx(const ComboInfo& comboInfo, const std::vector<std::string>& comboItems) {
    return ComboExImpl::ComboEx<const char*, std::string>(comboInfo, comboItems);
}

template<typename T>
inline T ComboEx(const ComboInfo& comboInfo, const std::vector<ComboItem<T>>& comboItems) {
    return ComboExImpl::ComboEx<T, ComboItem<T>>(comboInfo, comboItems);
}

template<typename T>
inline T ComboEx(const ComboInfoEx<T>& comboInfoEx, const std::vector<ComboItem<T>>& comboItems) {
    size_t defaultIndex = 0;
    
    for (size_t index = 0; index < comboItems.size(); index++) {
        if (comboItems[index].value == comboInfoEx.defaultValue) {
            defaultIndex = index;
            break;
        }
    }

    const ComboInfo comboInfo = { comboInfoEx.label,comboInfoEx.pCtx,defaultIndex,comboInfoEx.flags };

    return ComboExImpl::ComboEx<T, ComboItem<T>>(comboInfo, comboItems);
}