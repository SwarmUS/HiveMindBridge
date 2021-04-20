#ifndef HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H
#define HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H

#include "hivemind-bridge/IMessageHandler.h"
#include "hivemind-bridge/MessageUtils.h"
#include "hivemind-bridge/user-call/IUserCallRequestManager.h"
#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <cpp-common/ILogger.h>

class UserCallRequestHandler : public IUserCallRequestManager {
  public:
    UserCallRequestHandler(ILogger& logger, IUserCallbackMap& callbackMap);

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message, UserCallRequestDTO ucRequest);

  private:
    ILogger& m_logger;
    IUserCallbackMap& m_callbackMap;

    MessageDTO handleFunctionListLengthRequest(MessageDTO message, UserCallRequestDTO ucRequest);
    MessageDTO handleFunctionDescriptionRequest(MessageDTO message, UserCallRequestDTO ucRequest);
    void handleFunctionCallRequest(MessageDTO message,
                                   UserCallRequestDTO ucRequest,
                                   InboundRequestHandle* result);
    MessageDTO handleUnknownUserCallRequest(MessageDTO message, UserCallRequestDTO ucRequest);
};

#endif // HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H
