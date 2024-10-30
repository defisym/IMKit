#pragma once

#include <vector>
#include <string>

#include "imgui.h"

struct ComboContext {
    bool bInit = false;
    size_t currentIdx = 0;
};

enum class ComboInfoFlags :std::uint8_t {
    Input_None = 0,
    Input_Literal = 1 << 0,
};

struct ComboInfoBase {
    const char* label = nullptr;
    ComboContext* pCtx = nullptr;
    ImGuiComboFlags flags = ImGuiComboFlags_None;
    ComboInfoFlags infoFlags = ComboInfoFlags::Input_None;

    ComboInfoBase(const char* label, ComboContext* pCtx,
                  const ImGuiComboFlags flags = 0, const ComboInfoFlags infoFlags = ComboInfoFlags::Input_None) {
        this->label = label;
        this->pCtx = pCtx;

        this->flags = flags;
        this->infoFlags = infoFlags;
    }
};

struct ComboInfo :ComboInfoBase {
    size_t defaultIndex = 0u;

    ComboInfo(const char* pLabel, ComboContext* p, size_t defaultIndex = 0,
              const ImGuiComboFlags cflags = 0, const ComboInfoFlags iFlags = ComboInfoFlags::Input_None)
        :ComboInfoBase(pLabel, p, cflags, iFlags) {
        this->defaultIndex = defaultIndex;
    }
};

template<typename T>
struct ComboInfoEx :ComboInfoBase {
    T defaultValue = {};

    ComboInfoEx(const char* pLabel, ComboContext* p, T defaultValue,
                const ImGuiComboFlags cflags = 0, const ComboInfoFlags iFlags = ComboInfoFlags::Input_None)
        :ComboInfoBase(pLabel, p, cflags, iFlags) {
        this->defaultValue = defaultValue;
    }
};

template<typename T>
struct ComboItem {
    std::string displayName = "Undefined";
    T value = {};
};

namespace ComboExImpl {
    template<typename T>
    bool Compare(const T& l, const T& r) {
        return l == r;
    }
    template<>
    inline bool Compare(const char* const& l, const char* const& r) {
        return strcmp(l, r) == 0;
    }

    template<typename ReturnType, typename ItemType>
    const char* GetName(const ItemType& item) {
        if constexpr (std::is_same_v<ItemType, ComboItem<ReturnType>>) {
            return item.displayName.c_str();
        }
        else {
            using Type = std::remove_cvref<ItemType>;

            if constexpr (std::is_same_v<Type, std::remove_cvref<const char*>>) {
                return item;
            }
            else if constexpr (std::is_same_v<Type, std::remove_cvref<std::string>>) {
                return item.c_str();
            }
            else {
                static_assert(false, "Combo Type Error");
                return {};
            }
        }
    }
    template<typename ReturnType, typename ItemType>
    ReturnType GetValue(const ItemType& item) {
        if constexpr (std::is_same_v<ItemType, ComboItem<ReturnType>>) {
            return item.value;
        }
        else {
            using Type = std::remove_cvref<ItemType>;

            if constexpr (std::is_same_v<Type, std::remove_cvref<const char*>>) {
                return item;
            }
            else if constexpr (std::is_same_v<Type, std::remove_cvref<std::string>>) {
                return item.c_str();
            }
            else {
                static_assert(false, "Combo Type Error");
                return {};
            }
        }
    }

    template<typename ReturnType, typename ItemType>
    ReturnType ComboEx(const ComboInfo& comboInfo, const std::vector<ItemType>& comboItems) {
        if (!comboInfo.pCtx->bInit) [[unlikely]] {
            assert(comboInfo.defaultIndex < comboItems.size());
            comboInfo.pCtx->currentIdx = comboInfo.defaultIndex;
            comboInfo.pCtx->bInit = true;
        }
  
        auto& currentIdx = comboInfo.pCtx->currentIdx;

        // open combo
        if (ImGui::BeginCombo(comboInfo.label, GetName<ReturnType, ItemType>(comboItems[currentIdx]), comboInfo.flags)) {
            for (size_t idx = 0; idx < comboItems.size(); idx++) {
                const bool bSelected = currentIdx == idx;
                if (ImGui::Selectable(GetName<ReturnType, ItemType>(comboItems[idx]), bSelected)) {
                    currentIdx = idx;
                }

                if (bSelected) { ImGui::SetItemDefaultFocus(); }
            }

            ImGui::EndCombo();
        }
        // combo not opened, update
        else if (!(static_cast<std::uint8_t>(comboInfo.infoFlags) & static_cast<std::uint8_t>(ComboInfoFlags::Input_Literal))) {
            currentIdx = comboInfo.defaultIndex;
        }

        return GetValue<ReturnType, ItemType>(comboItems[currentIdx]);
    }

    template<typename ReturnType, typename ItemType>
    inline bool GetComboItemIndex(const ReturnType& valueToSearch, const std::vector<ItemType>& comboItems,
        size_t& index) {
        const auto it = std::ranges::find_if(comboItems, [&] (const ItemType& element) {
            return ComboExImpl::Compare(valueToSearch, GetValue<ReturnType, ItemType>(element));
        });

        if (it == comboItems.end()) { return false; }

        index = std::distance(comboItems.begin(), it);
        return true;
    }

    template<typename ReturnType, typename ItemType>
    inline bool GetComboItemIndex(const ItemType& item, const std::vector<ItemType>& comboItems,
        size_t& index) {
        return GetComboItemIndex(GetValue<ReturnType, ItemType>(item), comboItems, index);
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

    ComboExImpl::GetComboItemIndex<T, ComboItem<T>>(comboInfoEx.defaultValue, comboItems, defaultIndex);

    const ComboInfo comboInfo = { comboInfoEx.label,comboInfoEx.pCtx,defaultIndex,comboInfoEx.flags };

    return ComboExImpl::ComboEx<T, ComboItem<T>>(comboInfo, comboItems);
}

inline bool GetComboItemIndex(const char* valueToSearch, const std::vector<const char*>& comboItems,
    size_t& index) {
    return ComboExImpl::GetComboItemIndex<const char*, const char*>(valueToSearch, comboItems, index);
}

inline bool GetComboItemIndex(const std::string& valueToSearch, const std::vector<std::string>& comboItems,
    size_t& index) {
    return ComboExImpl::GetComboItemIndex<const char*, std::string>(valueToSearch, comboItems, index);
}

template<typename T>
inline bool GetComboItemIndex(const T& valueToSearch, const std::vector<ComboItem<T>>& comboItems,
    size_t& index) {
    return ComboExImpl::GetComboItemIndex<T, ComboItem<T>>(valueToSearch, comboItems, index);
}