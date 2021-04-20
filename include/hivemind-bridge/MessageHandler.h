#ifndef HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
#define HIVE_MIND_BRIDGE_MESSAGEHANDLER_H

#include "IMessageHandler.h"
#include "hivemind-bridge/MessageUtils.h"
#include "hivemind-bridge/user-call/Callback.h"
#include "hivemind-bridge/user-call/IUserCallRequestManager.h"
#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <cpp-common/ILogger.h>

class MessageHandler : public IMessageHandler {
  public:
    MessageHandler(ILogger& logger, IUserCallRequestManager& userCallRequestManager);
    ~MessageHandler();

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message) override;

    std::optional<uint32_t> handleGreet(MessageDTO greetMessage) override;

  private:
    ILogger& m_logger;
    IUserCallRequestManager& m_userCallRequestHandler;
};

#endif // HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
