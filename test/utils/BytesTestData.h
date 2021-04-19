#ifndef HIVEMINDBRIDGE_BYTESTESTDATA_H
#define HIVEMINDBRIDGE_BYTESTESTDATA_H

#include <cstdint>

template <int size>
struct ByteArray {
    uint8_t arr[size];

    constexpr ByteArray() : arr() {
        for (int i = 0; i < size; i++) // will cycle through values from 0 to 255 since uint8
            arr[i] = i;
    }
};

constexpr int SMALL_BYTE_ARRAY_SIZE = 8;
constexpr ByteArray SMALL_BYTE_ARRAY = ByteArray<SMALL_BYTE_ARRAY_SIZE>();

constexpr int LONG_BYTE_ARRAY_SIZE = 1024;
constexpr ByteArray LONG_BYTE_ARRAY = ByteArray<LONG_BYTE_ARRAY_SIZE>();

#endif // HIVEMINDBRIDGE_BYTESTESTDATA_H
