#include "DataReader.h"

#include "GUIContext/GUIContext.h"

DataReader::DataReader(Ctx* p) :pCtx(p) {}

int DataReader::Worker() {
    pCtx->deviceHandler.ReadData(&pCtx->processHandler);

    return 0;
}
