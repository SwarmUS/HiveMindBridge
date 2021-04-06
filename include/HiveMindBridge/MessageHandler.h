#ifndef HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
#define HIVE_MIND_BRIDGE_MESSAGEHANDLER_H

#include "Callback.h"
#include "IMessageHandler.h"
#include "HiveMindBridge/MessageUtils.h"
#include <cpp-common/ILogger.h>

class MessageHandler : public IMessageHandler {
  public:
    MessageHandler(ILogger& logger);
    ~MessageHandler();

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message) override;

    std::optional<uint32_t> handleGreet(MessageDTO greetMessage) override;

    bool registerCallback(std::string name, CallbackFunction callback) override;

    bool registerCallback(std::string name,
                          CallbackFunction callback,
                          CallbackArgsManifest manifest) override;

    std::optional<CallbackFunction> getCallback(const std::string& name) override;

  private:
    ILogger& m_logger;
    CallbackMap m_callbacks;
    std::vector<std::string> m_callbackNames; // Association between callbacks' names and their id

    MessageDTO handleFunctionListLengthRequest(uint32_t requestId,
                                               uint32_t msgDestinationId,
                                               uint32_t msgSourceId,
                                               UserCallTargetDTO sourceModule);

    MessageDTO handleFunctionDescriptionRequest(
        uint32_t requestId,
        uint32_t msgDestinationId,
        uint32_t msgSourceId,
        UserCallTargetDTO sourceModule,
        FunctionDescriptionRequestDTO functionDescriptionRequest);
};

#endif // HIVE_MIND_BRIDGE_MESSAGEHANDLER_H