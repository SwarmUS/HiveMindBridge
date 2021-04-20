#ifndef HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H
#define HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H

#include "hivemind-bridge/user-call/IUserCallRequestHandler.h"
#include <gmock/gmock.h>

class UserCallRequestManagerInterfaceMock : public IUserCallRequestHandler {
  public:
    ~UserCallRequestManagerInterfaceMock() = default;

    MOCK_METHOD((std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle>),
                handleMessage,
                (MessageDTO message, UserCallRequestDTO ucRequest));
};

#endif // HIVEMINDBRIDGE_USERCALLREQUESTMANAGERINTERFACEMOCK_H
