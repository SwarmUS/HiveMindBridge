#ifndef HIVEMINDBRIDGE_USERCALLREQUESTMANAGER_H
#define HIVEMINDBRIDGE_USERCALLREQUESTMANAGER_H

#include "hivemind-bridge/IMessageHandler.h"
#include "hivemind-bridge/MessageUtils.h"
#include "hivemind-bridge/user-call/IUserCallRequestManager.h"
#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <cpp-common/ILogger.h>

class UserCallRequestManager : public IUserCallRequestManager {
  public:
    UserCallRequestManager(ILogger& logger, IUserCallbackMap& callbackMap);

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message, UserCallRequestDTO ucRequest);

  private:
    ILogger& m_logger;
    IUserCallbackMap& m_callbackMap;

    MessageDTO handleFunctionListLengthRequest(MessageDTO message, UserCallRequestDTO ucRequest);
    MessageDTO handleFunctionDescriptionRequest(MessageDTO message, UserCallRequestDTO ucRequest);
    MessageDTO handleFunctionCallRequest(MessageDTO message,
                                         UserCallRequestDTO ucRequest,
                                         InboundRequestHandle* result);
};

#endif // HIVEMINDBRIDGE_USERCALLREQUESTMANAGER_H
