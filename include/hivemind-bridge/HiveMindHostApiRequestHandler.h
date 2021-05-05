#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H

#include "hivemind-bridge/IHiveMindHostApiRequestHandler.h"
#include <cpp-common/ILogger.h>
#include <memory>

class HiveMindHostApiRequestHandler : public IHiveMindHostRequestHandler {
  public:
    HiveMindHostApiRequestHandler(ILogger& logger);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiRequestDTO& hmRequest) override;

    bool onBytesReceived(std::function<void()> callback) override;

private:
    ILogger& m_logger;
    std::function<void()> m_bytesReceivedCallback;
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLER_H
