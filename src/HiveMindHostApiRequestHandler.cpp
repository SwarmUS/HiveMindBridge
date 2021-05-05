#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"

HiveMindHostApiRequestHandler::HiveMindHostApiRequestHandler(ILogger &logger) : m_logger(logger) {}

void HiveMindHostApiRequestHandler::handleMessage(const MessageDTO &message,
                                                  const HiveMindHostApiRequestDTO &hmRequest) {
    // TODO should the callback take a ref to the bytes as a param?
}

bool HiveMindHostApiRequestHandler::onBytesReceived(std::function<void()> callback) {
    bool retVal = false;
    if (m_bytesReceivedCallback) { retVal = true; };

    m_bytesReceivedCallback = callback;

    return retVal;
}

