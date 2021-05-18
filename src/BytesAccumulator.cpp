#include "hivemind-bridge/BytesAccumulator.h"
#include <cstring>

bool BytesAccumulator::appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber) {
    if (packetNumber != m_lastPacketNumber + 1) {
        // A packet was skipped so we stop accumulating
        return false;
    }

    m_lastPacketNumber = packetNumber;

    for (int i = 0; i < length; i++) {
        m_bytes.push_back(bytes[i]);
    }

    return true;
}

std::vector<uint8_t> BytesAccumulator::getBytes() { return m_bytes; }

uint32_t BytesAccumulator::getLastPacketNumber() const { return m_lastPacketNumber; }
