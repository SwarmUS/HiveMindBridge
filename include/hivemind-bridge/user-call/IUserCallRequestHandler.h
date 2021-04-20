#ifndef HIVEMINDBRIDGE_IUSERCALLREQUESTHANDLER_H
#define HIVEMINDBRIDGE_IUSERCALLREQUESTHANDLER_H

#include "hivemind-bridge/InboundRequestHandle.h"
#include "hivemind-bridge/InboundResponseHandle.h"

class IUserCallRequestHandler {
  public:
    virtual std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        const MessageDTO& message, const UserCallRequestDTO& ucRequest) = 0;
};

#endif // HIVEMINDBRIDGE_IUSERCALLREQUESTHANDLER_H
