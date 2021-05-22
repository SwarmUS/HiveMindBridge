#ifndef HIVEMINDBRIDGE_BYTESACCUMULATOR_H
#define HIVEMINDBRIDGE_BYTESACCUMULATOR_H

#include <cstdint>
#include <vector>

/**
 * An accumulator of bytes to be used for the reception of an arbitrary bytes payload.
 */
class BytesAccumulator {
  public:
    /**
     * Append some bytes to reconstruct the payload from many packets
     * @param bytes The bytes from a packet
     * @param length The length of the packet's payload
     * @param packetNumber The number of the packet number
     * @return false if the packet number does not follow strictly the last packet to have been
     * appended. Returns true if the operation succeded.
     */
    bool appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber);

    /**
     * Returns the assembled bytes
     */
    std::vector<uint8_t> getBytes();

    /**
     * Returns the number of the last packet that was appended to the accumulator.
     */
    uint32_t getLastPacketNumber() const;

  private:
    std::vector<uint8_t> m_bytes;
    uint32_t m_lastPacketNumber = -1; // We start at -1 since the first packet number should be 0
};

#endif // HIVEMINDBRIDGE_BYTESACCUMULATOR_H
