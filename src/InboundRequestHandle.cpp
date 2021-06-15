#include "hivemind-bridge/InboundRequestHandle.h"

void InboundRequestHandle::setResponse(MessageDTO message) { m_responseMessage = message; }

void InboundRequestHandle::setCallbackReturnContext(
    std::shared_future<std::optional<CallbackReturn>> future) {
    m_callbackReturnContext = std::move(future);
}

MessageDTO InboundRequestHandle::getResponse() { return m_responseMessage; }

std::shared_future<std::optional<CallbackReturn>> InboundRequestHandle::getCallbackReturnContext() {
    return m_callbackReturnContext;
}

void InboundRequestHandle::setCallbackName(std::string name) { m_callbackName = std::move(name); }

void InboundRequestHandle::setMessageSourceId(uint32_t id) { m_msgSourceId = id; }

void InboundRequestHandle::setMessageDestinationId(uint32_t id) { m_msgDestinationId = id; }

void InboundRequestHandle::setSourceModule(UserCallTargetDTO target) { m_sourceModule = target; }

uint32_t InboundRequestHandle::getMessageSourceId() const { return m_msgSourceId; }

uint32_t InboundRequestHandle::getMessageDestinationId() const { return m_msgDestinationId; }

UserCallTargetDTO InboundRequestHandle::getSourceModule() const { return m_sourceModule; }
