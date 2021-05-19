#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"

HiveMindHostApiRequestHandler::HiveMindHostApiRequestHandler(ILogger& logger,
                                                             IBytesAccumulator& accumulator) :
    m_logger(logger), m_bytesAccumulator(accumulator) {}

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
    (void)message;

    if (!m_bytesReceivedCallback) {
        m_logger.log(LogLevel::Error, "No callback was defined to handle incoming bytes.");
        return;
    }

    // TODO hold an array of bytesAccumulator to manage more than one bytes req at a time.
    // Create and delete the bytesAccumulators when the reception of bytes was fulfilled.
    if (!m_bytesAccumulator.appendBytes(const_cast<uint8_t*>(bytes.getPayload().data()),
                                        bytes.getPayloadLength(), bytes.getPacketNumber())) {
        m_logger.log(LogLevel::Error, "A packet was skipped.");
        return;
    }

    if (bytes.isLastPacket()) {
        m_bytesReceivedCallback(m_bytesAccumulator.getBytes().data(),
                                m_bytesAccumulator.getBytes().size());
        m_bytesAccumulator.reset();
    }
}