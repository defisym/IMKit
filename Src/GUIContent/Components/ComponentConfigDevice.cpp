#include "ComponentConfigDevice.h"

#include <_DeLib/GeneralDefinition.h>

#include "../../IMGuiEx/DisableHelper.h"

void DeleteDevice(Ctx* pCtx) {
	const auto disableDelete = DisableHelper(!pCtx->deviceHandler.bInit);

	ImGui::SameLine();
	if (ImGui::Button("Delete Device")) {
		const auto err = pCtx->deviceHandler.StopDevice();
		Context_Delete(&pCtx->deviceHandler.hContext);
		pCtx->deviceHandler.DeleteDevice();
	}
}
void CreateDeviceFailedPopUp(Ctx* pCtx) {
	// model should out of the button
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
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
	const auto deviceNames = pCtx->dllHandler.deviceNames;

	static size_t device_current_idx = 0;
	const char* device_preview_value = deviceNames[device_current_idx].c_str();

	if (ImGui::BeginCombo("Config/Device", device_preview_value)) {
		for (size_t idx = 0; idx < deviceNames.size(); idx++) {
			const bool is_selected = device_current_idx == idx;
			if (ImGui::Selectable(deviceNames[idx].c_str(), is_selected)) {
				device_current_idx = idx;
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Create Device")) {
		const bool bRet = pCtx->deviceHandler.CreateDevice(
			to_wide_string(deviceNames[device_current_idx]).c_str());

		if (!bRet) {
			ImGui::OpenPopup("Create Device Failed");
		}
	}

	DeleteDevice(pCtx);
	CreateDeviceFailedPopUp(pCtx);
}