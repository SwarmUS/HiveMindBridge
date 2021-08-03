#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include <cpp-common/ILogger.h>

class HiveMindHostApiResponseHandler : public IHiveMindHostApiResponseHandler {

  public:
    HiveMindHostApiResponseHandler(ILogger& logger);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiResponseDTO& hmResponse);

  private:
    ILogger& m_logger;
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
