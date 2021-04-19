#ifndef HIVEMINDBRIDGE_IUSERCALLREQUESTMANAGER_H
#define HIVEMINDBRIDGE_IUSERCALLREQUESTMANAGER_H

#include "hivemind-bridge/InboundRequestHandle.h"
#include "hivemind-bridge/InboundResponseHandle.h"

class IUserCallRequestManager {
  public:
    virtual std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message, UserCallRequestDTO ucRequest) = 0;
};

#endif // HIVEMINDBRIDGE_IUSERCALLREQUESTMANAGER_H
