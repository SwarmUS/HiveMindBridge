#ifndef HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H
#define HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H

#include "hivemind-bridge/IUserCallRequestHandler.h"
#include <gmock/gmock.h>

class UserCallRequestManagerInterfaceMock : public IUserCallRequestHandler {
  public:
    ~UserCallRequestManagerInterfaceMock() = default;

    MOCK_METHOD((std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle>),
                handleMessage,
                (const MessageDTO& message, const UserCallRequestDTO& ucRequest));
};

#endif // HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H
