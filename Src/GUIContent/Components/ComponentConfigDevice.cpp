#include "ComponentConfigDevice.h"

#include <_DeLib/GeneralDefinition.h>

#include "../../IMGuiEx/DisableHelper.h"
#include "../../GUIContent/Components/ComponentWaveformsProcess.h"

void CreateDevice(Ctx* pCtx, const wchar_t* pDeviceName) {
    const auto disable = DisableHelper(pCtx->deviceHandler.bInit);

    if (ImGui::Button("Create Device")) {
        const bool bRet = pCtx->deviceHandler.CreateDevice(pDeviceName);

        if (!bRet) {
            ImGui::OpenPopup("Create Device Failed");
			return;
        }

		auto& pWaveformsProcess = pCtx->processHandler.pWaveformsProcess;

		delete pWaveformsProcess;
		pWaveformsProcess = new ComponentWaveformsProcess{ pCtx };
    }
}
void DeleteDevice(Ctx* pCtx) {
	const auto disable = DisableHelper(!pCtx->deviceHandler.bInit);

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

	static size_t deviceCurrentIdx = 0;
	if (ImGui::BeginCombo("Config/Device", deviceNames[deviceCurrentIdx].c_str())) {
		for (size_t idx = 0; idx < deviceNames.size(); idx++) {
			const bool bSelected = deviceCurrentIdx == idx;
			if (ImGui::Selectable(deviceNames[idx].c_str(), bSelected)) {
				deviceCurrentIdx = idx;
			}

            if (bSelected) { ImGui::SetItemDefaultFocus(); }
		}

		ImGui::EndCombo();
	}

    CreateDevice(pCtx, to_wide_string(deviceNames[deviceCurrentIdx]).c_str());
    ImGui::SameLine();
	DeleteDevice(pCtx);

	CreateDeviceFailedPopUp(pCtx);
}