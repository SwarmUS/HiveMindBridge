#ifndef HIVEMINDBRIDGE_BYTESACCUMULATOR_H
#define HIVEMINDBRIDGE_BYTESACCUMULATOR_H

#include <vector>
#include <cstdint>

class BytesAccumulator {
public:

    bool appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber);

    std::vector<uint8_t> getBytes();

private:
    std::vector<uint8_t> m_bytes;
    uint32_t m_lastPacketNumber;
};

#endif //HIVEMINDBRIDGE_BYTESACCUMULATOR_H
