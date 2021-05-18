#ifndef HIVEMINDBRIDGE_BYTESACCUMULATOR_H
#define HIVEMINDBRIDGE_BYTESACCUMULATOR_H

#include <vector>
#include <cstdint>
#include "hivemind-bridge/IBytesAccumulator.h"

class BytesAccumulator : public IBytesAccumulator {
public:

    bool appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber) override;

    std::vector<uint8_t> getBytes() override;

    uint32_t getLastPacketNumber() const override;

private:
    std::vector<uint8_t> m_bytes;
    uint32_t m_lastPacketNumber = - 1;

};

#endif //HIVEMINDBRIDGE_BYTESACCUMULATOR_H
