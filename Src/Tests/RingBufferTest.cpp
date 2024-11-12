#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ThreadSafeRingBuffer.h>

namespace RingBufferTest {
    [[noreturn]] void RingBufferReadTest() {
        ThreadSafeRingBuffer<uint8_t> rb(10);

        AllocConsole();
        FILE* pStream;
        [[maybe_unused]] const auto err = freopen_s(&pStream, "CONOUT$", "a+", stdout);

        for (size_t index = 0; index < rb.bufferSz; index++) {
            rb.pBuffer[index] = static_cast<uint8_t>(static_cast<size_t>(rand() % 10));
            std::cout << static_cast<size_t>(rb.pBuffer[index]) << ", ";
        }
        std::cout << "\n";

        constexpr size_t sz = 3;
        const auto buf = new uint8_t[sz];

        for (;;) {
            rb.ReadData(buf, sz);

            for (size_t index = 0; index < sz; index++) {
                std::cout << static_cast<size_t>(buf[index]) << ", ";
            }
            std::cout << "\n";
        }
	}

    [[noreturn]] void RingBufferWriteTest() {
        ThreadSafeRingBuffer<uint8_t> rb(10);

        AllocConsole();
        FILE* pStream;
        [[maybe_unused]] const auto err = freopen_s(&pStream, "CONOUT$", "a+", stdout);

        for (size_t index = 0; index < rb.bufferSz; index++) {
            rb.pBuffer[index] = static_cast<uint8_t>(static_cast<size_t>(rand() % 10));
            std::cout << static_cast<size_t>(rb.pBuffer[index]) << ", ";
        }
        std::cout << "\n";

        constexpr size_t sz = 3;
        const auto buf = new uint8_t[sz];

        for (;;) {
            buf[0] = static_cast<uint8_t>(static_cast<size_t>(rand() % 10));
            buf[1] = static_cast<uint8_t>(static_cast<size_t>(rand() % 10));
            buf[2] = static_cast<uint8_t>(static_cast<size_t>(rand() % 10));

            for (size_t index = 0; index < sz; index++) {
                std::cout << static_cast<size_t>(buf[index]) << ", ";
            }
            std::cout << "\n";

            rb.WriteData(buf, sz);

            for (size_t index = 0; index < rb.bufferSz; index++) {
                std::cout << static_cast<size_t>(rb.pBuffer[index]) << ", ";
            }
            std::cout << "\n";
        }
    }
}
