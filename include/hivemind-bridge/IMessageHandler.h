#ifndef HIVEMIND_BRIDGE_IMESSAGEHANDLER_H
#define HIVEMIND_BRIDGE_IMESSAGEHANDLER_H

#include "InboundRequestHandle.h"
#include "hivemind-bridge/InboundResponseHandle.h"
#include "hivemind-bridge/UserCallbackFunctionWrapper.h"
#include <optional>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <pheromones/FunctionCallResponseDTO.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <variant>

typedef std::unordered_map<std::string, UserCallbackFunctionWrapper> CallbackMap;

class IMessageHandler {
  public:
    virtual ~IMessageHandler() = default;

    /**
     * Parse a message and execute the appropriate callback or action.
     * @param message the message to parse.
     * @return A message containing the appropriate acknowlege (with appropriate errors if
     * necessary)
     */
    virtual std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> handleMessage(
        MessageDTO message) = 0;

    /**
     * Parse a greet message and return the contained swarmAgentId.
     * @param greetMessage The message to parse.
     * @return The contained swarmAgentId if the operation succeded.
     */
    virtual std::optional<uint32_t> handleGreet(MessageDTO greetMessage) = 0;
};

#endif // HIVEMIND_BRIDGE_IMESSAGEHANDLER_H
