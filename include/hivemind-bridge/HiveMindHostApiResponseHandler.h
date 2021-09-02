#ifndef HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include "pheromones/GetNeighborResponseDTO.h"
#include "pheromones/GetNeighborsListResponseDTO.h"
#include "pheromones/PheromonesSettings.h"
#include <cpp-common/ILogger.h>
#include <functional>

class HiveMindHostApiResponseHandler : public IHiveMindHostApiResponseHandler {

  public:
    HiveMindHostApiResponseHandler(ILogger& logger);

    void handleMessage(const MessageDTO& message,
                       const HiveMindHostApiResponseDTO& hmResponse) override;

    bool onNeighborListUpdated(std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>,
                                                  uint64_t bytesLength)> callback);

    bool onNeighborUpdated(
        std::function<void(uint16_t neighborId, std::optional<Position> position)> callback);

  private:
    ILogger& m_logger;

    std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)>
        m_neighborListUpdateCallback;
    std::function<void(uint16_t neighborId, std::optional<Position> position)>
        m_neighborUpdateCallback;

    void handleNeighborList(const MessageDTO& message, const GetNeighborsListResponseDTO& list);
    void handleNeighbor(const MessageDTO& message, const GetNeighborResponseDTO& neighbor);
};

#endif // HIVEMINDBRIDGE_HIVEMINDHOSTAPIRESPONSEHANDLER_H
