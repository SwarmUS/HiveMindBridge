#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H

#include "hivemind-bridge/BytesAccumulator.h"
#include "hivemind-bridge/IHiveMindHostApiRequestHandler.h"
#include <cpp-common/ILogger.h>
#include <map>
#include <memory>

class HiveMindHostApiRequestHandler : public IHiveMindHostRequestHandler {
  public:
    HiveMindHostApiRequestHandler(ILogger& logger);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiRequestDTO& hmRequest) override;

    bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) override;

  private:
    ILogger& m_logger;
    std::map<uint32_t, BytesAccumulator> m_bytesAccumulatorMap;

    std::function<void(uint8_t* bytes, uint64_t bytesLength)> m_bytesReceivedCallback;

    void handleBytes(const MessageDTO& message, const BytesDTO& bytes);
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
