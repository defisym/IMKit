#include "ComponentWaveforms.h"

#include "ComponentWaveformsProcess.h"

// Note: ImGui call this each frame
//       if move to other UI lib should change the read logic
void ComponentWaveforms(Ctx* pCtx) {
	if (!ImGui::CollapsingHeader("Waveforms", ImGuiTreeNodeFlags_DefaultOpen)) {
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
				pCtx->deviceHandler.bContextInit = true;

				// update context
				pCtx->deviceHandler.bufferInfo =
					BufferInfo{ pBuffer,
						bufferSz / bufferStride, bufferStride,
						static_cast<size_t>(pCtx->deviceParams.updateFrameCount),
						static_cast<size_t>(pCtx->deviceParams.pointNumPerScan) };
				pCtx->deviceHandler.bContextUpdated = true;

				return 0;
			},
			pCtx);

		if (!pCtx->deviceHandler.bContextInit) {
			ImGui::TextUnformatted("Context not created");
			break;
		}

	    Context_Update(pCtx->deviceHandler.hContext, &pCtx->deviceHandler.bufferInfo);
		pCtx->processHandler.pWaveformsProcess->WaveformTab();
	} while (false);
}
