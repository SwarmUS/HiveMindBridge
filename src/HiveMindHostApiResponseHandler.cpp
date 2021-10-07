#include "hivemind-bridge/HiveMindHostApiResponseHandler.h"

HiveMindHostApiResponseHandler::HiveMindHostApiResponseHandler(ILogger& logger) :
    m_logger(logger) {}

void HiveMindHostApiResponseHandler::handleMessage(const MessageDTO& message,
                                                   const HiveMindHostApiResponseDTO& hmResponse) {
    if (const auto* neighborList =
            std::get_if<GetNeighborsListResponseDTO>(&hmResponse.getResponse())) {
        handleNeighborList(message, *neighborList);
    } else if (const auto* neighbor =
                   std::get_if<GetNeighborResponseDTO>(&hmResponse.getResponse())) {
        handleNeighbor(message, *neighbor);
    } else {
        m_logger.log(LogLevel::Warn, "Unsupported HiveMindHostApi response.");
    }
}

bool HiveMindHostApiResponseHandler::onNeighborListUpdated(
    std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback) {
    bool retVal = static_cast<bool>(m_neighborListUpdateCallback);

    m_neighborListUpdateCallback = callback;

    return retVal;
}

bool HiveMindHostApiResponseHandler::onNeighborUpdated(
    std::function<void(uint16_t neighborId, std::optional<Position> position)> callback) {
    bool retVal = static_cast<bool>(m_neighborUpdateCallback);

    m_neighborUpdateCallback = callback;

    return retVal;
}

void HiveMindHostApiResponseHandler::handleNeighborList(const MessageDTO& message,
                                                        const GetNeighborsListResponseDTO& list) {
    (void)message;

    if (!m_neighborListUpdateCallback) {
        m_logger.log(LogLevel::Warn,
                     "No callback was defined to handle incoming GetNeighborsListResponse.");
        return;
    }

    m_neighborListUpdateCallback(list.getNeighbors(), list.getNeighborsLength());
}

void HiveMindHostApiResponseHandler::handleNeighbor(const MessageDTO& message,
                                                    const GetNeighborResponseDTO& neighbor) {
    (void)message;

    if (!m_neighborUpdateCallback) {
        m_logger.log(LogLevel::Warn,
                     "No callback was defined to handle incoming GetNeighborResponse.");
        return;
    }

    if (neighbor.getNeighborPosition()) {
        Position pos;
        pos.distance = neighbor.getNeighborPosition()->getDistance();
        pos.azimuth = neighbor.getNeighborPosition()->getAzimuth();
        pos.inLOS = neighbor.getNeighborPosition()->inLOS();

        m_neighborUpdateCallback(neighbor.getNeighborId(), pos);
    } else {
        m_neighborUpdateCallback(neighbor.getNeighborId(), {});
    }
}