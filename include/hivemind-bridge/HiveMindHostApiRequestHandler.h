#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H

#include "hivemind-bridge/IHiveMindHostApiRequestHandler.h"
#include "hivemind-bridge/IBytesAccumulator.h"
#include <cpp-common/ILogger.h>
#include <memory>

class HiveMindHostApiRequestHandler : public IHiveMindHostRequestHandler {
  public:
    HiveMindHostApiRequestHandler(ILogger& logger, IBytesAccumulator& accumulator);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiRequestDTO& hmRequest) override;

    bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) override;

  private:
    ILogger& m_logger;
    IBytesAccumulator& m_bytesAccumulator;
    std::function<void(uint8_t* bytes, uint64_t bytesLength)> m_bytesReceivedCallback;

    void handleBytes(const MessageDTO& message, const BytesDTO& bytes);
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
