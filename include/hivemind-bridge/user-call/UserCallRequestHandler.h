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

    FunctionListLengthResponseDTO handleFunctionListLengthRequest(
        MessageDTO message, FunctionListLengthRequestDTO fcRequest);
    FunctionDescriptionResponseDTO handleFunctionDescriptionRequest(
        MessageDTO message, FunctionDescriptionRequestDTO fcRequest);
    FunctionCallResponseDTO handleFunctionCallRequest(MessageDTO message,
                                                      FunctionCallRequestDTO fcRequest,
                                                      InboundRequestHandle* result);
};

#endif // HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H
