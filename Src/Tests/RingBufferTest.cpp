#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <_DeLib/ThreadSafeRingBuffer.h>

namespace RingBufferTest {
    [[noreturn]] void RingBufferReadTest() {
        ThreadSafeRingBuffer<uint8_t> rb(10);

        AllocConsole();
        FILE* pStream;
        freopen_s(&pStream, "CONOUT$", "a+", stdout);

        for (size_t index = 0; index < rb.bufferSz; index++) {
            rb.pBuffer[index] = (uint8_t)((size_t)(rand() % 10));
            std::cout << (size_t)rb.pBuffer[index] << ", ";
        }
        std::cout << "\n";

        auto sz = 3;
        auto buf = new uint8_t[sz];

        for (;;) {
            rb.ReadData(buf, sz);

            for (size_t index = 0; index < sz; index++) {
                std::cout << (size_t)buf[index] << ", ";
            }
            std::cout << "\n";
        }
	}

    [[noreturn]] void RingBufferWriteTest() {
        ThreadSafeRingBuffer<uint8_t> rb(10);

        AllocConsole();
        FILE* pStream;
        freopen_s(&pStream, "CONOUT$", "a+", stdout);

        for (size_t index = 0; index < rb.bufferSz; index++) {
            rb.pBuffer[index] = (uint8_t)((size_t)(rand() % 10));
            std::cout << (size_t)rb.pBuffer[index] << ", ";
        }
        std::cout << "\n";

        auto sz = 3;
        auto buf = new uint8_t[sz];

        for (;;) {
            buf[0] = (uint8_t)((size_t)(rand() % 10));
            buf[1] = (uint8_t)((size_t)(rand() % 10));
            buf[2] = (uint8_t)((size_t)(rand() % 10));

            for (size_t index = 0; index < sz; index++) {
                std::cout << (size_t)buf[index] << ", ";
            }
            std::cout << "\n";

            rb.WriteData(buf, sz);

            for (size_t index = 0; index < rb.bufferSz; index++) {
                std::cout << (size_t)rb.pBuffer[index] << ", ";
            }
            std::cout << "\n";
        }
    }
}
