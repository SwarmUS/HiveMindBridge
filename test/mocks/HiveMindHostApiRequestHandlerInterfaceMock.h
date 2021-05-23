#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLERINTERFACEMOCK_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLERINTERFACEMOCK_H

#include "hivemind-bridge/IHiveMindHostApiRequestHandler.h"
#include <gmock/gmock.h>

class HiveMindHostApiRequestHandlerInterfaceMock : public IHiveMindHostRequestHandler {
  public:
    ~HiveMindHostApiRequestHandlerInterfaceMock() = default;

    MOCK_METHOD(void,
                handleMessage,
                (const MessageDTO& message, const HiveMindHostApiRequestDTO& hmRequest));

    MOCK_METHOD(bool,
                onBytesReceived,
                (std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback));
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIREQUESTHANDLERINTERFACEMOCK_H
