#ifndef HIVEMIND_BRIDGE_MESSAGEHANDLERINTERFACEMOCK_H
#define HIVEMIND_BRIDGE_MESSAGEHANDLERINTERFACEMOCK_H

#include "hivemind-bridge/IMessageHandler.h"
#include <gmock/gmock.h>
#include <pheromones/MessageDTO.h>
#include <variant>

class MessageHandlerInterfaceMock : public IMessageHandler {
  public:
    ~MessageHandlerInterfaceMock() = default;

    MOCK_METHOD((std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle>),
                handleMessage,
                (MessageDTO message),
                (override));

    MOCK_METHOD(std::optional<uint32_t>, handleGreet, (MessageDTO message), (override));
};

#endif // HIVEMIND_BRIDGE_MESSAGEHANDLERINTERFACEMOCK_H
