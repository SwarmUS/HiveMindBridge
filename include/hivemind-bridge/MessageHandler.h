#ifndef HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
#define HIVE_MIND_BRIDGE_MESSAGEHANDLER_H

#include "Callback.h"
#include "hivemind-bridge/IHiveMindHostApiRequestHandler.h"
#include "hivemind-bridge/IMessageHandler.h"
#include "hivemind-bridge/IUserCallRequestHandler.h"
#include "hivemind-bridge/IUserCallbackMap.h"
#include "hivemind-bridge/MessageUtils.h"
#include <cpp-common/ILogger.h>

class MessageHandler : public IMessageHandler {
  public:
    MessageHandler(ILogger& logger,
                   IUserCallRequestHandler& userCallRequestManager,
                   IHiveMindHostRequestHandler& hmRequestHandler);
    ~MessageHandler() = default;

    std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message) override;

    std::optional<uint32_t> handleGreet(MessageDTO greetMessage) override;

  private:
    ILogger& m_logger;
    IHiveMindHostRequestHandler& m_hmRequestHandler;
    IUserCallRequestHandler& m_userCallRequestHandler;
};

#endif // HIVE_MIND_BRIDGE_MESSAGEHANDLER_H
