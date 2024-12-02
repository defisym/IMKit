#pragma once

#include <cassert>

#include "./../Src/Module/General/Definition.h"
#include "./../Src/Utilities/Buffer.h"

template<typename Type = OTDRProcessValueType>
struct RingBuffer :Buffer<Type> {
    size_t readIndex = 0;
    size_t writeIndex = 0;

    RingBuffer() = default;
    RingBuffer(const size_t sz) :Buffer<Type>(sz) {}

    // ------------------------
    // Read & Write logic are almost the same
    // but allow some spaghetti code here
    // ------------------------

    // copy pData to buffer
    virtual void WriteData(const Type* pData, const size_t dataSize) {
        const auto remain = this->_sz - writeIndex;
        const auto write = (std::min)(dataSize, remain);
        const auto left = dataSize - write;

        const auto pStart = this->_pBuf + writeIndex;
        memcpy(pStart, pData, sizeof(Type) * write);

        writeIndex += write;
        if (writeIndex == this->_sz) { writeIndex = 0; }

        if (left == 0) { return; }
        RingBuffer::WriteData(pData + write, left);
    }

    // get buffer at current index and forward sz
    // caller should grantee that pointer and offset are valid
    virtual Type* WriteData(const size_t sz) {
        assert(sz + writeIndex <= this->_sz);
        auto pStart = this->_pBuf + writeIndex;

        writeIndex += sz;
        if (writeIndex == this->_sz) { writeIndex = 0; }

        return pStart;
    }

    // copy buffer to pBuf
    virtual void ReadData(Type* pBuf, const size_t sz) {
        const auto remain = this->_sz - readIndex;
        const auto read = (std::min)(sz, remain);
        const auto left = sz - read;

        const auto pStart = this->_pBuf + readIndex;
        memcpy(pBuf, pStart, sizeof(Type) * read);

        readIndex += read;
        if (readIndex == this->_sz) { readIndex = 0; }

        if (left == 0) { return; }
        RingBuffer::ReadData(pBuf + read, left);
    }

    // get buffer at current index and forward sz
    // caller should grantee that pointer and offset are valid
    virtual Type* ReadData(const size_t sz) {
        assert(sz + readIndex <= this->_sz);
        auto pStart = this->_pBuf + readIndex;

        readIndex += sz;
        if (readIndex == this->_sz) { readIndex = 0; }

        return pStart;
    }

    virtual void ResetIndex() { writeIndex = 0; readIndex = 0; }

    virtual void DiscardUnreadBuffer() { readIndex = writeIndex; }
};
