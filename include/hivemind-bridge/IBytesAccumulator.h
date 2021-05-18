#ifndef HIVEMINDBRIDGE_IBYTESACCUMULATOR_H
#define HIVEMINDBRIDGE_IBYTESACCUMULATOR_H

class IBytesAccumulator {
public:
    virtual bool appendBytes(uint8_t* bytes, uint16_t length, uint32_t packetNumber) = 0;

    virtual std::vector<uint8_t> getBytes() = 0;

    virtual uint32_t getLastPacketNumber() const = 0;
};

#endif //HIVEMINDBRIDGE_IBYTESACCUMULATOR_H
