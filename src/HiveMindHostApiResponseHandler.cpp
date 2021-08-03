#include "hivemind-bridge/HiveMindHostApiResponseHandler.h"

HiveMindHostApiResponseHandler::HiveMindHostApiResponseHandler(ILogger& logger) : m_logger(logger) {}

void HiveMindHostApiResponseHandler::handleMessage(const MessageDTO& message, const HiveMindHostApiResponseDTO& hmResponse) {
    // TODO
}