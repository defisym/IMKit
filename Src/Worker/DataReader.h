#pragma once

#include "ThreadBase.h"

struct Ctx;

struct DataReader :ThreadBase {
    Ctx* pCtx = nullptr;

    DataReader(Ctx* p);
    int Worker();
};