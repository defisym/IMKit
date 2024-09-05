#include "ComponentWavefroms.h"

#include "ComponentWavefromsProcess.h"

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWavefroms(Ctx* pCtx) {
	if (!ImGui::CollapsingHeader("Wavefroms", ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	do {
		if (!pCtx->deviceHandler.bStart) {
			ImGui::TextUnformatted("Device not started");
			break;
		}

		pCtx->deviceHandler.bContextUpdated = false;
		[[maybe_unused]] const auto err
			= Reader_ReadOnce(pCtx->deviceHandler.hDevice,
			[](short* pBuffer,
			const size_t bufferSz,
			const size_t bufferStride,
			void* pUserData) {
				// info
				auto pCtx = static_cast<Ctx*>(pUserData);
				auto& hContext = pCtx->deviceHandler.hContext;

				// init context
				if (hContext == nullptr) { hContext = Context_Create(); }

				// update context
				pCtx->deviceHandler.bufferInfo =
					BufferInfo{ pBuffer,
						bufferSz / bufferStride, bufferStride,
						static_cast<size_t>(pCtx->deviceParams.processFrameCount),
						static_cast<size_t>(pCtx->deviceParams.pointNumPerScan) };
				pCtx->deviceHandler.bContextUpdated = true;

				return 0;
			},
			pCtx);

		const auto hContext = pCtx->deviceHandler.hContext;
		if (hContext == nullptr) {
			ImGui::TextUnformatted("Context not created");
			break;
		}

		ComponentWavefromsProcess wavefromsProcess = { pCtx,hContext };

		wavefromsProcess.StartProcess();
		wavefromsProcess.WaveformTab();
		wavefromsProcess.EndProcess();
	} while (false);
}
