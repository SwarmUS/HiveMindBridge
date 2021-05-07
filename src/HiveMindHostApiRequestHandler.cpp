#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"

HiveMindHostApiRequestHandler::HiveMindHostApiRequestHandler(ILogger& logger) : m_logger(logger) {}

void HiveMindHostApiRequestHandler::handleMessage(const MessageDTO& message,
                                                  const HiveMindHostApiRequestDTO& hmRequest) {

    if (const auto* bytesRequest = std::get_if<BytesDTO>(&hmRequest.getRequest())) {
        handleBytes(message, *bytesRequest);
    } else {
        m_logger.log(LogLevel::Warn, "Unsupported HiveMindHostApi request.");
    }
}

bool HiveMindHostApiRequestHandler::onBytesReceived(
    std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) {
    bool retVal = false;
    if (m_bytesReceivedCallback) {
        retVal = true;
    };

    m_bytesReceivedCallback = callback;

    return retVal;
}

void HiveMindHostApiRequestHandler::handleBytes(const MessageDTO& message, const BytesDTO& bytes) {
    if (!m_bytesReceivedCallback) {
        m_logger.log(LogLevel::Error, "No callback was defined to handle incoming bytes.");
        return;
    }

    std::array<uint8_t, BytesDTO::PAYLOAD_MAX_SIZE> payload = bytes.getPayload();
    uint16_t payloadLength = bytes.getPayloadLength();

    m_bytesReceivedCallback(payload.data(), payloadLength);
}