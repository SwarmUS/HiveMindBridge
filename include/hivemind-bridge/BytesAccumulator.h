#ifndef HIVEMINDBRIDGE_BYTESACCUMULATOR_H
#define HIVEMINDBRIDGE_BYTESACCUMULATOR_H

#include <cstdint>
#include <vector>

class BytesAccumulator {
  public:
    bool appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber);

    std::vector<uint8_t> getBytes();

    uint32_t getLastPacketNumber() const;

    void reset();

  private:
    std::vector<uint8_t> m_bytes;
    uint32_t m_lastPacketNumber = -1;
};

#endif // HIVEMINDBRIDGE_BYTESACCUMULATOR_H
