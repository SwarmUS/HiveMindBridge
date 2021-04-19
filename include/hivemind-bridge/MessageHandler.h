#ifndef HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
#define HIVE_MIND_BRIDGE_MESSAGEHANDLER_H

#include "Callback.h"
#include "IMessageHandler.h"
#include "hivemind-bridge/MessageUtils.h"
#include "hivemind-bridge/user-call/IUserCallRequestManager.h"
#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <cpp-common/ILogger.h>

class MessageHandler : public IMessageHandler {
  public:
    MessageHandler(ILogger& logger,
                   IUserCallRequestManager& userCallRequestManager,
                   IUserCallbackMap& userCallbackMap);
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
    IUserCallRequestManager& m_userCallRequestManager;
    IUserCallbackMap& m_userCallbackMap;

};

#endif // HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
