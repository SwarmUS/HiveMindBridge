#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include "pheromones/PheromonesSettings.h"
#include <functional>
#include <cpp-common/ILogger.h>
#include "pheromones/GetNeighborResponseDTO.h"
#include "pheromones/GetNeighborsListResponseDTO.h"

class HiveMindHostApiResponseHandler : public IHiveMindHostApiResponseHandler {

  public:

    HiveMindHostApiResponseHandler(ILogger& logger);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiResponseDTO& hmResponse) override;

    bool onNeighborListUpdated(
        std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback);

    bool onNeighborUpdated(
        std::function<void(uint16_t neighborId, float distance, float relativeOrientation, bool inLOS)> callback);

  private:
    ILogger& m_logger;

    std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> m_neighborListUpdatesCallback;
    std::function<void(uint16_t neighborId, float distance, float relativeOrientation, bool inLOS)> m_neighborUpdateCallback;

    void handleNeighborList(const MessageDTO& message, const GetNeighborsListResponseDTO& list);
    void handleNeighbor(const MessageDTO& message, const GetNeighborResponseDTO& neighbor);
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
