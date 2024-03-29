#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H

#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include <functional>
#include <gmock/gmock.h>

class HiveMindHostApiResponseHandlerInterfaceMock : public IHiveMindHostApiResponseHandler {
  public:
    ~HiveMindHostApiResponseHandlerInterfaceMock() = default;

    MOCK_METHOD(void,
                handleMessage,
                (const MessageDTO& message, const HiveMindHostApiResponseDTO& hmResponse));
    MOCK_METHOD(
        bool,
        onNeighborListUpdated,
        (std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)>));
    MOCK_METHOD(
        bool,
        onNeighborUpdated,
        (std::function<void(uint16_t neighborId, std::optional<Position> position)> callback));
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLERINTERFACEMOCK_H
