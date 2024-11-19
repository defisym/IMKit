#pragma once

#include <SDL.h>
#include <SDL_thread.h>

#include <_DeLib/LockHelper.h>

struct SpinLock {
    SDL_SpinLock lock = 0;
};