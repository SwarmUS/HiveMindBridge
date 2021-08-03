#include "hivemind-bridge/HiveMindHostApiResponseHandler.h"

HiveMindHostApiResponseHandler::HiveMindHostApiResponseHandler(ILogger& logger) : m_logger(logger) {}

void HiveMindHostApiResponseHandler::handleMessage(const MessageDTO& message, const HiveMindHostApiResponseDTO& hmResponse) {
    // TODO
}

bool HiveMindHostApiResponseHandler::onNeighborListUpdated(
    std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback) {
    // TODO
}

bool HiveMindHostApiResponseHandler::onNeighborUpdated(
    std::function<void(uint16_t neighborId, float distance, float relativeOrientation, bool inLOS)> callback) {
    // TODO
}

void handleNeighborList(const MessageDTO& message, const GetNeighborsListResponseDTO& list) {
    // TODO
}

void handleNeighbor(const MessageDTO& message, const GetNeighborResponseDTO& neighbor) {
    // TODO
}