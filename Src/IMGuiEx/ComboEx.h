#pragma once

#include <vector>
#include <string>

#include "imgui.h"

struct ComboInfo {
    const char* label = nullptr;
    size_t defaultIndex = 0;
    ImGuiComboFlags flags = 0;

    ComboInfo(const char* label, size_t defaultIndex = 0, ImGuiComboFlags flags = 0) {
        this->label = label;
        this->defaultIndex = defaultIndex;
        this->flags = flags;
    }
};

template<typename T>
struct ComboItem {
    const char* displayName = "Undefined";
    T value = T{};
};

namespace ComboExImpl {
    template<typename ReturnType, typename ItemType>
    ReturnType ComboEx(const ComboInfo& comboInfo, const std::vector<ItemType>& comboItems) {
        static bool bInit = false;
        static size_t currentIdx = 0;

        if (!bInit) [[unlikely]] {
            assert(comboInfo.defaultIndex < comboItems.size());
            currentIdx = comboInfo.defaultIndex;
            bInit = true;
        }

        auto GetName = [&] (size_t index)->const char* {
            if constexpr (std::is_same_v<ItemType, ComboItem<ReturnType>>) {
                return comboItems[index].displayName;
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

        if (ImGui::BeginCombo(comboInfo.label,
            GetName(currentIdx),
            comboInfo.flags)) {
            for (size_t idx = 0; idx < comboItems.size(); idx++) {
                const bool bSelected = currentIdx == idx;
                if (ImGui::Selectable(GetName(currentIdx), bSelected)) {
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