#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H

#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include <gmock/gmock.h>

class HiveMindHostApiResponseHandlerInterfaceMock : public IHiveMindHostApiResponseHandler {
  public:
    ~HiveMindHostApiResponseHandlerInterfaceMock() = default;

    MOCK_METHOD(void, handleMessage, (const MessageDTO& message, const HiveMindHostApiResponseDTO& hmResponse));
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H
