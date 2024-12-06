#include "DataReader.h"

#include "GUIContext/GUIContext.h"

void DataQueue::UpdateQueue(const size_t count, const size_t size) {
    phaseBuffer.ExtendBuffer(count * size);
    amplitudeBuffer.ExtendBuffer(count * size);
    frameSize = size;
}

void DataQueue::AddData(const OTDRContextHandle handle) {
    Context_UpdateToBuffer(handle, phaseBuffer.WriteData(frameSize), 0);
    Context_UpdateToBuffer(handle, amplitudeBuffer.WriteData(frameSize), 1);
}

OTDRProcessValueType* DataQueue::GetPhaseData() {
    return phaseBuffer.ReadData(frameSize);
}

OTDRProcessValueType* DataQueue::GetAmplitudeData() {
    return amplitudeBuffer.ReadData(frameSize);
}

DataReader::DataReader(Ctx* p, const ExecuteCallbackType& cb) :WorkerBase(p, cb) {}

void DataReader::WakeCallback() {
    // call this before re-start thread as info may be changed by user
    // make sure the queue can hold all data
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    queue.UpdateQueue(deviceParams.processFrameCount, deviceParams.pointNumPerScan);
}

int DataReader::LoopBody() {
    auto deviceHandler = pCtx->deviceHandler;
    deviceHandler.ReadDataInternal([&] {
        Context_UpdateInfo(deviceHandler.hContext, &deviceHandler.bufferInfo);
        queue.AddData(deviceHandler.hContext);
    });

    return 0;
}
