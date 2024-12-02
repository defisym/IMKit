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

DataReader::DataReader(Ctx* p) :pCtx(p) {}

void DataReader::UpdateQueue() {
    const auto& deviceParams = pCtx->deviceHandler.deviceParams;
    queue.UpdateQueue(deviceParams.processFrameCount, deviceParams.pointNumPerScan);
}

int DataReader::LoopBody() {
    pCtx->deviceHandler.ReadData(&pCtx->processHandler);

    return 0;
}
