#include "ComponentConfigParam.h"

#include "../../IMGuiEx/DisableHelper.h"

// TODO config items changes according to the device
bool UpdateParam(Ctx* pCtx) {
    // ------------------------
    // Basic info
    // ------------------------
	if (pCtx->deviceHandler.hParam == nullptr) { return false; }

	const auto hParam = pCtx->deviceHandler.hParam;
	bool bEnable = pCtx->deviceHandler.bInit;

	static Param param;
	auto& fiberParams = pCtx->fiberParams;
	auto& deviceParams = pCtx->deviceParams;

	// ------------------------
	// Frame
	// ------------------------

	ImGui::Checkbox("Use context", &deviceParams.bUseCountext);
	ManualDisableHelper frameHelper;
	frameHelper.Disable(!deviceParams.bUseCountext);
	ImGui::InputInt("Frame to update", &deviceParams.updateFrameCount, 64, 256);
	frameHelper.Enable();
	ImGui::InputInt("Frame to process", &deviceParams.processFrameCount, 64, 256);

	const auto frameToRead = deviceParams.bUseCountext
		? deviceParams.updateFrameCount
		: deviceParams.processFrameCount;

	ImGui::SameLine();
	ImGui::TextUnformatted([] (const double frame, const double scanRate) {
		const bool bFrameGreaterThanScanRate = frame >= scanRate;
		const auto updateCount =
			bFrameGreaterThanScanRate
			? static_cast<int>(floor(frame / scanRate))
			: static_cast<int>(floor(scanRate / frame));
		const auto frameLabel =
			bFrameGreaterThanScanRate
			? std::format("(Update every {} seconds)", updateCount)
			: std::format("(Update {} times per second)", updateCount);

		return frameLabel;
	}(frameToRead,
		deviceParams.scanRate).c_str());

	// ------------------------
	// Point num per scan
	// ------------------------
	ImGui::InputInt("Point num per scan",
					&deviceParams.pointNumPerScan, 256, 1024);

	ImGui::SameLine();

	Param_Get(hParam, L"resolution", &param);
	deviceParams.resolution = param.doubleParam;
	const auto maxPoint = static_cast<int>(floor(fiberParams.length / deviceParams.resolution));
	ImGui::TextUnformatted(std::format("(Max {})", maxPoint).c_str());

	param.uint32_tParam = deviceParams.pointNumPerScan;
	Param_Set(hParam, L"point_num_per_scan", param);

	deviceParams.pointNumToRead = frameToRead * deviceParams.pointNumPerScan;
	param.uint32_tParam = deviceParams.pointNumToRead;
	Param_Set(hParam, L"pointNumToRead", param);

	// ------------------------
	// Scan rate
	// ------------------------
	ImGui::InputDouble("Scan rate",
					   &deviceParams.scanRate, 0.01f, 1.0f, "%.8f");

	ImGui::SameLine();
	const auto maxRate = fiberParams.lightSpeed / (2 * fiberParams.length);
	ImGui::TextUnformatted(std::format("(Max {})", maxRate).c_str());

	param.doubleParam = deviceParams.scanRate;
	Param_Set(hParam, L"scan_rate", param);

	// ------------------------
	// Pulse high width
	// ------------------------
	ImGui::InputInt("Pulse high width ns", &deviceParams.pulseHighWidth);
	param.uint32_tParam = deviceParams.pulseHighWidth;
	Param_Set(hParam, L"pulse_high_width_ns", param);

	// ------------------------
	// Center frequency
	// ------------------------
	ImGui::InputInt("Center frequency (Hz)", &deviceParams.centerFrequency);
	param.uint32_tParam = deviceParams.centerFrequency;
	Param_Set(hParam, L"center_freq_hz", param);

	//static int dataSrcSel = 0;
	//ImGui::Combo("Data Src", &dataSrcSel, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

	param.uint32_tParam = deviceParams.dataSrcSel;
	Param_Set(hParam, L"data_src_sel", param);

	param.uint32_tParam = deviceParams.demCHQuantity;
	Param_Set(hParam, L"demodulation_ch_quantity", param);

	param.uint32_tParam = deviceParams.uploadRateSel;
	Param_Set(hParam, L"upload_rate_sel", param);

	bEnable = bEnable && (Param_Valid(hParam) == 0);

	return bEnable;
}

void StartStopDevice(Ctx* pCtx, const bool bEnable) {
	const auto disableStart = DisableHelper(!bEnable);

	if (ImGui::Button("Start Device")) {
		pCtx->deviceHandler.StartDevice();
	}

	ImGui::SameLine();
	if (ImGui::Button("Stop Device")) {
        [[maybe_unused]] const auto err = pCtx->deviceHandler.StopDevice();
		Context_Delete(&pCtx->deviceHandler.hContext);
	}
}

void ComponentConfigParam(Ctx* pCtx) {	
	const auto bEnable = UpdateParam(pCtx);
	StartStopDevice(pCtx, bEnable);
}