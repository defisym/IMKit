#include "ComponentConfigDevice.h"

#include <_DeLib/GeneralDefinition.h>

#include "../../IMGuiEx/ComboEx.h"
#include "../../IMGuiEx/DisableHelper.h"
#include "../../GUIContent/Components/ComponentWaveformsProcess.h"

void CreateDevice(Ctx* pCtx, const wchar_t* pDeviceName) {
    const auto disable = DisableHelper(pCtx->deviceHandler.bInit);

    if (ImGui::Button("Create Device")) {
        if (pCtx->deviceHandler.CreateDevice(pDeviceName)) { return; }
        ImGui::OpenPopup("Create Device Failed");
    }
}
void DeleteDevice(Ctx* pCtx) {
	const auto disable = DisableHelper(!pCtx->deviceHandler.bInit);

	if (ImGui::Button("Delete Device")) {
        [[maybe_unused]] const auto err = pCtx->deviceHandler.StopDevice();
		pCtx->deviceHandler.DeleteDevice();
	}
}
void CreateDeviceFailedPopUp(Ctx* pCtx) {
	// model should out of the button
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(150.0f, 75.0f));

	constexpr auto modelFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	if (ImGui::BeginPopupModal("Create Device Failed", nullptr, modelFlags)) {
		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();

		ImGui::EndPopup();
	}
}

void ComponentConfigDevice(Ctx* pCtx) {
	const auto& deviceNames = pCtx->deviceHandler.deviceNames;

	static ComboContext deviceComboCtx = {};
	const auto pDeviceName = ComboEx({ "Config/Device", &deviceComboCtx }, deviceNames);

    CreateDevice(pCtx, to_wide_string(pDeviceName).c_str());
    ImGui::SameLine();
	DeleteDevice(pCtx);

	CreateDeviceFailedPopUp(pCtx);
}