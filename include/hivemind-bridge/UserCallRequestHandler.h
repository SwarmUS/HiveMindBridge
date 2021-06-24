#ifndef HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H
#define HIVEMINDBRIDGE_USERCALLREQUESTHANDLER_H

#include "IUserCallRequestHandler.h"
#include "IUserCallbackMap.h"
#include "hivemind-bridge/IMessageHandler.h"
#include "hivemind-bridge/MessageUtils.h"
#include <cpp-common/ILogger.h>

class UserCallRequestHandler : public IUserCallRequestHandler {
  public:
    UserCallRequestHandler(ILogger& logger, IUserCallbackMap& callbackMap);

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        const MessageDTO& message, const UserCallRequestDTO& ucRequest);

  private:
    std::optional<CallbackReturn> callbackWrapper(const CallbackArgs& args,
                                                  uint16_t argsLength,
                                                  const std::string& functionName);

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
